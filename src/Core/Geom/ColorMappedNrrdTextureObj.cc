/*
   For more information, please see: http://software.sci.utah.edu

   The MIT License

   Copyright (c) 2004 Scientific Computing and Imaging Institute,
   University of Utah.

   License for the specific language governing rights and limitations under
   Permission is hereby granted, free of charge, to any person obtaining a
   copy of this software and associated documentation files (the "Software"),
   to deal in the Software without restriction, including without limitation
   the rights to use, copy, modify, merge, publish, distribute, sublicense,
   and/or sell copies of the Software, and to permit persons to whom the
   Software is furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
   OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.
*/


/*
 *  ColorMappedNrrdTextureObj.cc
 *
 *  Written by:
 *   McKay Davis
 *   School of Computing
 *   University of Utah
 *   November, 2005
 *
 *  Copyright (C) 2005 SCI Group
 */

#include <sci_gl.h>
#include <sci_glu.h>
#include <sci_glx.h>
#include <Core/Datatypes/NrrdData.h>
#include <Core/Geom/ShaderProgramARB.h>
#include <Core/Geom/ColorMappedNrrdTextureObj.h>
#include <Core/Math/MiscMath.h>
#include <Core/Containers/StringUtil.h>

#include <teem/air.h>
#include <limits.h>

namespace SCIRun {

#define NRRD_EXEC(__nrrd_command__) \
  if (__nrrd_command__) { \
    char *err = biffGetDone(NRRD); \
    cerr << string("Error on line #")+to_string(__LINE__) + \
	    string(" executing nrrd command: \n")+ \
            string(#__nrrd_command__)+string("\n")+ \
            string("Message: ")+string(err); \
    free(err); \
    return; \
  }


ColorMappedNrrdTextureObj::ColorMappedNrrdTextureObj(NrrdDataHandle &nin, 
                               int axis, 
                               int min_slice, int max_slice,
                               int time) :
  colormap_(0),
  nrrd_(0),
  nrrd_dirty_(true),
  dirty_(4,true),
  dirty_region_(),
  texture_id_(4,0),
  xdiv_(2),
  ydiv_(2),
  clut_min_(0.0), 
  clut_max_(1.0),
  data_(0),
  own_data_(false)
{
  if (!nin.get_rep() || !nin->nrrd)// || nin->nrrd->dim != 3) 
    throw "ColorMappedNrrdTextureObj::ColorMappedNrrdTextureObj(nrrd)"
      "nrrd not valid";
  nrrd_ = new NrrdData;

  if (min_slice != max_slice) {
    int *min = new int[nin->nrrd->dim];
    int *max = new int[nin->nrrd->dim];
    for (int i = 0; i < nin->nrrd->dim; i++) {
      min[i] = 0;
      max[i] = nin->nrrd->axis[i].size-1;
    }
    min[axis] = Min(min_slice, max_slice);
    max[axis] = Max(min_slice, max_slice);
    NrrdDataHandle tmp1 = new NrrdData;
    NRRD_EXEC(nrrdCrop(tmp1->nrrd, nin->nrrd, min, max));
    NRRD_EXEC(nrrdProject(nrrd_->nrrd, tmp1->nrrd, axis, 
                          nrrdMeasureMax, nrrdTypeDefault));
  } else {
    NRRD_EXEC(nrrdSlice(nrrd_->nrrd, nin->nrrd, axis, min_slice));
  }
  nrrd_dirty_ = true;
}




ColorMappedNrrdTextureObj::~ColorMappedNrrdTextureObj()
{
  if (own_data_ && data_) delete[] data_;
  for (unsigned int t = 0; t < texture_id_.size(); ++t) 
    if (glIsTexture(texture_id_[t]))
        glDeleteTextures(1,&texture_id_[t]);
}

void
ColorMappedNrrdTextureObj::set_clut_minmax(float min, float max)
{
  clut_min_ = Min(min,max);
  clut_max_ = Max(min,max);
  nrrd_dirty_ = true;
}

void
ColorMappedNrrdTextureObj::set_colormap(ColorMapHandle &cmap)
{
  colormap_ = cmap;
  nrrd_dirty_ = true;
}


// void
// ColorMappedNrrdTextureObj::recalculate(int x1, int y1, int x2, int y2)
// {
  

  



template <class T>
void
ColorMappedNrrdTextureObj::apply_colormap_to_raw_data(float *dst, 
                                                      T *src, 
                                                      int num, 
                                                      const float *rgba, 
                                                      int ncolors,
                                                      float scale, 
                                                      float bias)
{
  const int sizeof4floats = 4*sizeof(float);
  const int maxcolor = ncolors-1;
  for (int pos = 0; pos < num; ++pos) {
    float val = float(src[pos])*scale+bias;
    int color = Round(Clamp(val, 0.0f, 1.0f)*maxcolor);
    memcpy(dst + pos * 4, rgba + color * 4, sizeof4floats);
  }
}


float *
ColorMappedNrrdTextureObj::apply_colormap()
{
  const int num = nrrd_->nrrd->axis[1].size*nrrd_->nrrd->axis[2].size;
  float *data = new float[4*num];
  if (!data) return 0;

  const float range = (clut_max_ - clut_min_);
  const float scale = (range > 0.00001) ? (1.0 / range) : 1.0;
  const float bias =  (range > 0.00001) ? -clut_min_*scale : 0.0;

  int ncolors;  
  const float *rgba;
  if (!colormap_.get_rep()) {
    ncolors = 256;
    float *nrgba = new float[256*4];
    for (int c = 0; c < 256*4; ++c) 
      nrgba[c] = (c % 4 == 3) ? 1.0 : (c/4)/255.0;
    nrgba[3] = 0.0;
    rgba = nrgba;
  } else {
    ncolors = colormap_->resolution();
    rgba = colormap_->get_rgba();
  }

  switch (nrrd_->nrrd->type) {
  case nrrdTypeChar: {
    apply_colormap_to_raw_data(data, (char *)nrrd_->nrrd->data,
                               num, rgba, ncolors, scale, bias);
  } break;
  case nrrdTypeUChar: {
    apply_colormap_to_raw_data(data, (unsigned char *)nrrd_->nrrd->data,
                               num, rgba, ncolors, scale, bias);
  } break;
  case nrrdTypeShort: {
    apply_colormap_to_raw_data(data, (short *)nrrd_->nrrd->data,
                               num, rgba, ncolors, scale, bias);
  } break;
  case nrrdTypeUShort: {
    apply_colormap_to_raw_data(data, (unsigned short *)nrrd_->nrrd->data,
                               num, rgba, ncolors, scale, bias);
  } break;
  case nrrdTypeInt: {
    apply_colormap_to_raw_data(data, (int *)nrrd_->nrrd->data,
                               num, rgba, ncolors, scale, bias);
  } break;
  case nrrdTypeUInt: {
    apply_colormap_to_raw_data(data, (unsigned int *)nrrd_->nrrd->data,
                               num, rgba, ncolors, scale, bias);
  } break;
  case nrrdTypeLLong: {
    apply_colormap_to_raw_data(data, (signed long long *)nrrd_->nrrd->data,
                               num, rgba, ncolors, scale, bias);
  } break;
  case nrrdTypeULLong: {
    apply_colormap_to_raw_data(data, (unsigned long long *)nrrd_->nrrd->data, 
                               num, rgba, ncolors, scale, bias);
  } break;
  case nrrdTypeFloat: {
    apply_colormap_to_raw_data(data, (float *)nrrd_->nrrd->data,
                               num, rgba, ncolors, scale, bias);
  } break;
  case nrrdTypeDouble: {
    apply_colormap_to_raw_data(data, (double *)nrrd_->nrrd->data,
                               num, rgba, ncolors, scale, bias);
  } break;
  default: throw "Unsupported data type: "+to_string(nrrd_->nrrd->type);
  }

  if (!colormap_.get_rep())
    delete[] rgba;
    
  return data;
}



bool
ColorMappedNrrdTextureObj::bind(int x, int y)
{
  if (!nrrd_.get_rep() || !nrrd_->nrrd)
    return false;

  int pos = y*2+x;
  const bool bound = glIsTexture(texture_id_[pos]);

  if (!bound) {
    glGenTextures(1, &texture_id_[pos]);
    CHECK_OPENGL_ERROR();  
  }

  glBindTexture(GL_TEXTURE_2D, texture_id_[pos]);
  CHECK_OPENGL_ERROR();  

  if (bound && !nrrd_dirty_ && !dirty_[pos])
    return true;


  if (nrrd_dirty_) {
    if (nrrd_->nrrd->axis[0].size == 1) {
      if (own_data_ && data_) {
        delete [] data_;
        data_ = 0;
        own_data_ = false;
      }
      data_ = apply_colormap();
      own_data_ = true;
    } else {
      data_ = nrrd_->nrrd->data;
      own_data_ = false;
    }
    if (!data_) 
      return false;
    nrrd_dirty_ = false;
    for (int i = 0; i < 4; ++i)
      dirty_[i] = true;

  }

  GLenum type = GL_FLOAT;
  if (nrrd_->nrrd->axis[0].size != 1) {
    switch (nrrd_->nrrd->type) {
    case nrrdTypeChar: type = GL_BYTE; break;
    case nrrdTypeUChar: type = GL_UNSIGNED_BYTE; break;
    case nrrdTypeShort: type = GL_SHORT; break;
    case nrrdTypeUShort: type = GL_UNSIGNED_SHORT; break;	
    case nrrdTypeInt: type = GL_INT; break;
    case nrrdTypeUInt: type = GL_UNSIGNED_INT; break;
    case nrrdTypeFloat: type = GL_FLOAT; break;
    default: throw "Cant bind nrrd"; break;
    }
  }


  GLint format = GL_RGBA;
  if (nrrd_->nrrd->axis[0].size == 3)
    format = GL_RGB;

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, nrrd_->nrrd->axis[1].size);  
  glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, nrrd_->nrrd->axis[2].size);  
  CHECK_OPENGL_ERROR();  

  if (x == 0) {
    xdiv_[x].first = 0;
    xdiv_[x].second = LargestPowerOf2(nrrd_->nrrd->axis[1].size);
  } else if (x == 1) {
    int wid = Pow2(nrrd_->nrrd->axis[1].size - xdiv_[0].second);
    xdiv_[1].second = nrrd_->nrrd->axis[1].size;
    xdiv_[1].first = xdiv_[1].second - wid;
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, xdiv_[1].first);
  }
  CHECK_OPENGL_ERROR();  


  if (y == 0) {
    ydiv_[y].first = 0;
    ydiv_[y].second = LargestPowerOf2(nrrd_->nrrd->axis[2].size);
  } else if (y == 1) {
    int wid = Pow2(nrrd_->nrrd->axis[2].size - ydiv_[0].second);
    ydiv_[1].second = nrrd_->nrrd->axis[2].size;
    ydiv_[1].first = ydiv_[1].second - wid;
    glPixelStorei(GL_UNPACK_SKIP_ROWS, ydiv_[1].first);
  }
  CHECK_OPENGL_ERROR();  

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  CHECK_OPENGL_ERROR();  
  

//   cerr << "X: " << x << " Y: " << y
//        << " Xdim: " << nrrd_->nrrd->axis[0].size
//        << " ydim: " << nrrd_->nrrd->axis[1].size
//        << " Xdiv first: " << xdiv_[x].first
//        << " Xdiv second: " << xdiv_[x].second
//        << " Xdiv size: " <<  xdiv_[x].second -  xdiv_[x].first 
//        << " ydiv first: " << ydiv_[y].first
//        << " ydiv second: " << ydiv_[y].second 
//        << " ydiv size: " <<  ydiv_[y].second -  ydiv_[y].first 
//        << std::endl;
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
               xdiv_[x].second - xdiv_[x].first, 
               ydiv_[y].second - ydiv_[y].first,
               0,format, type, data_);
  CHECK_OPENGL_ERROR();  

  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, 0); 
  glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
  glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0); 

  CHECK_OPENGL_ERROR();  
  dirty_[pos] = false;

  return true;
}

  
void
//ColorMappedNrrdTextureObj::draw_quad(float coords[]) 
ColorMappedNrrdTextureObj::draw_quad(Point &min, Vector &xdir, Vector &ydir) 
{
  glEnable(GL_TEXTURE_2D);

  unsigned int yoff = 0;

  for (int y = 0; y < 2; ++y) {
    unsigned int xoff = 0;
    for (int x = 0; x < 2; ++x) {
      if (!bind(x,y)) continue;
      //      glDisable(GL_TEXTURE_2D);
      //      glColor4d(1.0, 0.0, 0.3, 1.0);
      float x1 = xoff / float(nrrd_->nrrd->axis[1].size);
      float y1 = yoff / float(nrrd_->nrrd->axis[2].size);
      float x2 = xdiv_[x].second / float(nrrd_->nrrd->axis[1].size);
      float y2 = ydiv_[y].second / float(nrrd_->nrrd->axis[2].size);

      float tx = (xoff - xdiv_[x].first) / float(xdiv_[x].second - xdiv_[x].first);
      float ty = (yoff - ydiv_[y].first) / float(ydiv_[y].second - ydiv_[y].first);
      xoff = xdiv_[x].second;

      
//       cerr << " x: " << x 
//            << " y: " << y
//            << " x1: " << x1 
//            << " y1: " << y1
//            << " x2: " << x2
//            << " y2: " << y2
//            << " xoff: " << xoff
//            << " yoff: " << yoff
//            << " tx: " << tx
//            << " ty: " << ty << std::endl;

      glBegin(GL_QUADS);
      
      glTexCoord2d(tx, ty); 
      Point p = min + x1*xdir + y1*ydir;
      glVertex3f(p.x(), p.y(), p.z());
      
      glTexCoord2d(1.0, ty);
      p = min + x2*xdir + y1*ydir;
      glVertex3f(p.x(), p.y(), p.z());
      
      glTexCoord2d(1.0, 1.0);
      p = min + x2*xdir + y2*ydir;
      glVertex3f(p.x(), p.y(), p.z());
      
      glTexCoord2d(tx, 1.0);
      p = min + x1*xdir + y2*ydir;
      glVertex3f(p.x(), p.y(), p.z());
      glEnd();
      
      CHECK_OPENGL_ERROR(); 
    }
    yoff = ydiv_[y].second;
  }
  glDisable(GL_TEXTURE_2D);

}

BBoxSet::BBoxSet() :
  boxes_()
{
}

BBoxSet::BBoxSet(BBox &box) :
  boxes_()
{
  add(box);
}

BBoxSet::BBoxSet(BBoxSet &boxes) :
  boxes_()
{
  add(boxes);
}

BBoxSet::~BBoxSet() 
{
}

void
BBoxSet::add(BBox &box)
{
  boxes_.push_back(box);
}

void
BBoxSet::add(BBoxSet &boxes)
{
  BBoxes::iterator iter = boxes.boxes_.begin(), end = boxes.boxes_.end();
  while (iter != end) {
    boxes_.push_back(*iter);
    ++iter;
  }
}



void
BBoxSet::sub(BBox &box)
{
  BBoxes boxes = boxes_;
  boxes_.clear();
  BBoxes::iterator iter = boxes.begin(), end = boxes.end();
  while (iter != end) {
    BBox &box1 = *iter;
    ++iter;
    if (!box.overlaps2(box1)) continue;
    for (int x = 0; x < 3; ++x) {
      float x1, x2;
      switch (x) {
      case 0: 
        x1 = Min(box.min().x(), box1.min().x());
        x2 = Max(box.min().x(), box1.min().x());
        break;
      case 1: 
        x1 = Max(box.min().x(), box1.min().x());
        x2 = Min(box.max().x(), box1.max().x());
        break;
      default:
      case 2: 
        x1 = Min(box.max().x(), box1.max().x());
        x2 = Max(box.max().x(), box1.max().x());
        break;
      }
       
      for (int y = 0; y < 3; ++y) {
        float y1, y2;
        switch (y) {
        case 0: 
          y1 = Min(box.min().y(), box1.min().y());
          y2 = Max(box.min().y(), box1.min().y());
          break;
        case 1: 
          y1 = Max(box.min().y(), box1.min().y());
          y2 = Min(box.max().y(), box1.max().y());
          break;
        default:
        case 2: 
          y1 = Min(box.max().y(), box1.max().y());
          y2 = Max(box.max().y(), box1.max().y());
          break;
        }

        for (int z = 0; z < 3; ++z) {
          float z1, z2;
          switch (z) {
          case 0: 
            z1 = Min(box.min().z(), box1.min().z());
            z2 = Max(box.min().z(), box1.min().z());
            break;
          case 1: 
            z1 = Max(box.min().z(), box1.min().z());
            z2 = Min(box.max().z(), box1.max().z());
            break;
          default:
          case 2: 
            z1 = Min(box.max().z(), box1.max().z());
            z2 = Max(box.max().z(), box1.max().z());
            break;
          }
          Point p1(x1,y1,z1);
          Point p2(x2,y2,z2);
          BBox newbox(p1,p2);
          if (newbox.overlaps2(box1) && !newbox.overlaps(box))
            boxes_.push_back(newbox);
        }
      }
    }
  }
}


void
BBoxSet::sub(BBoxSet &boxes)
{
  BBoxes::iterator iter = boxes.boxes_.begin(), end = boxes.boxes_.end();
  while (iter != end) {
    sub(*iter);
    ++iter;
  }
}

void
BBoxSet::reset()
{
  boxes_.clear();
}


void
BBoxSet::set(BBox &box)
{
  boxes_.clear();
  add(box);
}


void
BBoxSet::set(BBoxSet &boxes)
{
  boxes_.clear();
  add(boxes);
}



BBox
BBoxSet::get()
{
  BBox box;
  BBoxes::iterator iter = boxes_.begin(), end = boxes_.end();
  while (iter != end) {
    box.extend(*iter);
    ++iter;
  }
  return box;
}

vector<BBox>
BBoxSet::get_boxes()
{
  return boxes_;
}


}
