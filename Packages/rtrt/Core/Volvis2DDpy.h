#ifndef __2DVOLVISDPY_H__
#define __2DVOLVISDPY_H__

#include <Packages/rtrt/Core/DpyBase.h>
#include <Core/Thread/Runnable.h>
#include <Packages/rtrt/Core/shape.h>
#include <Packages/rtrt/Core/texture.h>
#include <Packages/rtrt/Core/widget.h>
#include <vector>
#include <Packages/rtrt/Core/VolumeVis2D.h>


using std::vector;

namespace rtrt {

  class Volvis2DDpy : public DpyBase {
    // creates the background texture
    virtual void createBGText();
    // restores visible background texture to the clean original
    virtual void loadCleanTexture();
    // draws the background texture
    virtual void drawBackground();
    // adds a new widget to the end of the vector
    virtual void addWidget( int x, int y );
    // cycles through the possible widget types: tri->rect(ellipse)->rect(1d)->rect(rainbow)->tri...
    virtual void cycleWidgets( int type );
    // draws all widgets in widgets vector without their textures
    virtual void drawWidgets( GLenum mode );
    // paints widget textures onto the background
    virtual void bindWidgetTextures();
    // determines whether a pixel is inside of a widget
    virtual bool insideAnyWidget( int x, int y );
    // moves user-selected widget to the end of the widgets vector to be drawn last ("on top" )
    virtual void prioritizeWidgets();
    // retrieves information about picked widgets, determines which widget was picked
    virtual void processHits( GLint hits, GLuint buffer[] );
    // determines which widget the user picked
    virtual void pickShape( int x, int y );


    // Called at the start of run.
    virtual void init();
    // Called whenever the window needs to be redrawn
    virtual void display();
    // Called when the window is resized.  Note: xres and yres will not be
    // updated by the event handler.  That's what this function is for.
    virtual void resize( const int width, const int height );
    // Key is pressed/released.  Use the XK_xxx constants to determine
    // which key was pressed/released
    virtual void key_pressed( unsigned long key );
    // These handle mouse button events.  button indicates which button.
    // x and y are the location measured from the upper left corner of the
    // window.
    virtual void button_pressed( MouseButton button, const int x, const int y );
    virtual void button_released( MouseButton button, const int x, const int y );
    virtual void button_motion(MouseButton button, const int x, const int y);
    
  
  public:
    void adjustMasterAlpha( float dx );
    void lookup( Voxel2D<float> voxel, Color &color, float &alpha );
    void attach( VolumeVis2D* volume );
    void loadUIState( unsigned long key );
    void saveUIState( unsigned long key );
    void adjustRaySize( unsigned long key );
    virtual void animate(bool &changed);

    float original_t_inc;
    float t_inc;
    float master_alpha;
    vector<Widget*> widgets;           // collection of widgets to be controlled in any function
    int pickedIndex;                   // index of currently selected widget by frame
    int old_x;                         // saved most recent x-coordinate
    int old_y;                         // saved most recent y-coordinate
    float x_pixel_width;               // screenspace-to-worldspace x-dim ratio
    float y_pixel_width;               // screenspace-to-worldspace y-dim ratio
    float vmin, vmax, gmin, gmax;      // voxel minima/maxima
    GLuint bgTextName;
    GLuint transFuncTextName;
    Texture <GLfloat> *bgTextImage;    // clean background texture
    Texture <GLfloat> *transTexture1;  // collection of widget textures painted onto background
    Texture <GLfloat> *transTexture2;  // used for texture swapping to remove volume rendering "streaks"
    int whichTexture;
    vector<VolumeVis2D*> volumes;
    Volvis2DDpy( float t_inc );
  };

} // end namespace rtrt

#endif
