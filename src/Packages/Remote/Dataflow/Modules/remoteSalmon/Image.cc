//////////////////////////////////////////////////////////////////////
// Image.cpp - Load and save images of many file formats.
//
// by David K. McAllister, 1997-1998.

#include <Remote/Modules/remoteSalmon/Image.h>
#include <Remote/Modules/remoteSalmon/Utils.h>
#include <Remote/Modules/remoteSalmon/Assert.h>

#include <SCICore/Math/MiscMath.h>
#include <SCICore/Math/Trig.h>
#include <SCICore/Math/Expon.h>
#include <SCICore/Util/Assert.h>

#include <tiffio.h>

extern "C" {
#include <jpeglib.h>
}

#include <fstream>
#include <iostream>

using namespace std;

#include <stdlib.h>

namespace Remote {
namespace Modules {

//////////////////////////////////////////////////////
// Sun Raster File Format Header

struct rasterfile {
	int ras_magic;		/* magic number */
	int ras_width;		/* width (pixels) of image */
	int ras_height; 	/* height (pixels) of image */
	int ras_depth;		/* depth (1, 8, or 24 bits) of pixel */
	int ras_length; 	/* length (bytes) of image */
	int ras_type;		/* type of file; see RT_* below */
	int ras_maptype;		/* type of colormap; see RMT_* below */
	int ras_maplength;		/* length (bytes) of following map */
	/* color map follows for ras_maplength bytes, followed by image */
};
#define RAS_MAGIC	0x59a66a95

/* Sun supported ras_type's */
#define RT_OLD		0	/* Raw pixrect image in 68000 byte order */
#define RT_STANDARD 1	/* Raw pixrect image in 68000 byte order */
#define RT_BYTE_ENCODED 2	/* Run-length compression of bytes */
#define RT_FORMAT_RGB	3	/* XRGB or RGB instead of XBGR or BGR */
#define RT_FORMAT_TIFF	4	/* tiff <-> standard rasterfile */
#define RT_FORMAT_IFF	5	/* iff (TAAC format) <-> standard rasterfile */
#define RT_EXPERIMENTAL 0xffff	/* Reserved for testing */

/* Sun registered ras_maptype's */
#define RMT_RAW 	2
/* Sun supported ras_maptype's */
#define RMT_NONE	0	/* ras_maplength is expected to be 0 */
#define RMT_EQUAL_RGB	1	/* red[ras_maplength/3],green[],blue[] */

/*
* NOTES:
*	Each line of the image is rounded out to a multiple of 16 bits.
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^!!!!!!!!!!!!!!
*	This corresponds to the rounding convention used by the memory pixrect
*	package (/usr/include/pixrect/memvar.h) of the SunWindows system.
*	The ras_encoding field (always set to 0 by Sun's supported software)
*	was renamed to ras_length in release 2.0.  As a result, rasterfiles
*	of type 0 generated by the old software claim to have 0 length; for
*	compatibility, code reading rasterfiles must be prepared to compute the
*	true length from the width, height, and depth fields.
*/

bool Image::LoadRas(const char *FName)
{
	// Read a Sun Raster File image.
#if defined ( SCI_MACHINE_sgi ) || defined ( SCI_MACHINE_hp )
	ifstream InFile(FName);
	if(!InFile.rdbuf()->is_open())
#else
	ifstream InFile(FName, ios::in | ios::binary);
	if(!InFile.is_open())
#endif
	{
		cerr << "Failed to open Raster file `" << FName << "'.\n";
		return true;
	}
	
	cerr << FName << " open.\n";
	
	rasterfile Hdr;
	InFile.read((char *) &Hdr, sizeof(rasterfile));
	
#ifdef SCI_LITTLE_ENDIAN
	ConvertLong((unsigned int *)&Hdr, sizeof(rasterfile) / sizeof(int));
#endif
	
	wid = Hdr.ras_width;
	hgt = Hdr.ras_height;
	chan = 3;
	size = wid * hgt;
	dsize = wid * hgt * chan;

	if(Hdr.ras_depth != 24)
	{
		cerr << "Take your " << Hdr.ras_depth << " bit image and go away!\n";
		wid = hgt = chan = size = dsize = 0;
		return true;
	}
	
	if(dsize != Hdr.ras_length)
	{
		cerr << "Size was " << dsize << ", but ras_length was " << Hdr.ras_length << ".\n";
		return true;
	}
	
	if(wid > 4096)
	{
		cerr << "Too big! " << wid << endl;
		return true;
	}
	
	Pix = new unsigned char[dsize];
	cerr << "Loading a ras image.\n";
	
	if(Hdr.ras_type == RT_FORMAT_RGB)
	{
		if(Hdr.ras_maptype == RMT_NONE)
		{
			// Now read the color values.
			for (int y = 0; y < hgt; y++)
			{
				InFile.read((char *)&Pix[y*wid*3], wid*3);
			}
		}
		else if(Hdr.ras_maptype == RMT_EQUAL_RGB)
		{
			cerr << "Reading color mapped image. Maplength = " << Hdr.ras_maplength << endl;
			
			unsigned char ColorMap[256][3];
			unsigned char Colors[4096];
			InFile.read((char *)ColorMap, Hdr.ras_maplength*3);
			
			for (int y = 0; y < hgt; y++)
			{
				InFile.read((char *)Colors, wid);
				
				for(int x=0; x<wid; )
				{
					Pix[y*wid+x++] = ColorMap[Colors[x]][0];
					Pix[y*wid+x++] = ColorMap[Colors[x]][1];
					Pix[y*wid+x++] = ColorMap[Colors[x]][2];
				}
			}
		}
		else
		{
			cerr << "Strange color map scheme.\n";
			return true;
		}
	}
	else if(Hdr.ras_type == RT_STANDARD)
	{
		cerr << "BGR Image (RT_STANDARD)\n";
		if(Hdr.ras_maptype == RMT_NONE)
		{
			// Now read the color values.
			unsigned char Colors[4096][3];
			
			int ii=0;
			for (int y = 0; y < hgt; y++)
			{
				InFile.read((char *)Colors, wid*3);

				for(int x=0; x<wid; x++)
				{
					Pix[ii++] = Colors[x][2];
					Pix[ii++] = Colors[x][1];
					Pix[ii++] = Colors[x][0];
				}
			}
		}
		else
		{
			cerr << "Strange color map scheme.\n";
			return true;
		}
	}
	else
	{
		cerr << "Strange format.\n";
		return true;
	}
	
	InFile.close();

	return false;
}


//////////////////////////////////////////////////////
// PPM File Format

// Read a PPM or PGM file.
bool Image::LoadPPM(const char *FName)
{
#if defined ( SCI_MACHINE_sgi ) || defined ( SCI_MACHINE_hp )
	ifstream InFile(FName);
	if(!InFile.rdbuf()->is_open())
#else
	ifstream InFile(FName, ios::in | ios::binary);
	if(!InFile.is_open())
#endif
	{
		cerr << "Failed to open PPM file `" << FName << "'.\n";
		return true;
	}
	
	char Magic1, Magic2;
	InFile >> Magic1 >> Magic2;
	
	if(Magic1 != 'P' || (Magic2 != '5' && Magic2 != '6'))
	{
		cerr << FName << " is not a known PPM file.\n";
		InFile.close();
		return true;
	}
	
	InFile.get();
	char c = InFile.peek();
	while(c == '#')
	{
		char line[999];
		InFile.getline(line, 1000);
		cerr << line << endl;
		c = InFile.peek();
	}
	
	int dyn_range;
	InFile >> wid >> hgt >> dyn_range;
	InFile.get();
	
	if(dyn_range != 255)
	{
		cerr << "Must be 255. Was " << dyn_range << endl;
		return true;
	}
	
	size = wid * hgt;
	
	if(Magic2 == '5') {
		cerr << "Loading a PGM image.\n";
		chan = 1;
		dsize = wid * hgt * chan;
		Pix = new unsigned char[dsize];
		
		for (int y = 0; y < hgt; y++)
			InFile.read((char *)&Pix[y*wid], wid);
	} else {
		cerr << "Loading a PPM image.\n";
		chan = 3;
		dsize = wid * hgt * chan;
		Pix = new unsigned char[dsize];
		
		for (int y = 0; y < hgt; y++)
			InFile.read((char *)&Pix[y*wid*3], wid*3);
	}
	
	InFile.close();
	return false;
}

bool Image::SavePPM(const char *FName) const
{
	if(Pix == NULL || chan < 1 || wid < 1 || hgt < 1)
	{
		cerr << "Image is not defined. Not saving.\n";
		return true;
	}

	if(chan != 1 && chan != 3)
	{
		cerr << "Can't save a " << chan << " channel image as a PPM.\n";
		return true;
	}

#if defined ( SCI_MACHINE_sgi ) || defined ( SCI_MACHINE_hp )
	ofstream OF(FName);
	if(!OF.rdbuf()->is_open())
#else
	ofstream OF(FName, ios::out | ios::binary);
	if(!OF.is_open())
#endif
	{
		cerr << "Failed to open file `" << FName << "'.\n";
		return true;
	}
	
	OF << ((chan == 1) ? "P5\n" : "P6\n");
	OF << wid << " " << hgt << endl << 255 << endl;
	OF.write((char *)Pix, dsize);
	
	OF.close();
	
	cerr << "Wrote PPM file " << FName << endl;

	return false;
}

//////////////////////////////////////////////////////
// SGI Iris RGB Images

#define RGB_MAGIC 0x01da0101

/* private typedefs */
struct rawImageRec
{
	unsigned short imagic;
	unsigned short type;
	unsigned short dim;
	unsigned short sizeX, sizeY, sizeZ;
	unsigned long min, max;
	unsigned long wasteBytes;
	char name[80];
	unsigned long colorMap;

	// Not part of image header.
	FILE *file;
	unsigned char *tmp;
	unsigned long rleEnd;
	unsigned int *rowStart;
	int *rowSize;
};

static void RawImageGetRow(rawImageRec *raw, unsigned char *buf, int y, int z)
{
	unsigned char *iPtr, *oPtr, pixel;
	int count;
	
	if ((raw->type & 0xFF00) == 0x0100) {
		fseek(raw->file, raw->rowStart[y+z*raw->sizeY], SEEK_SET);
		fread(raw->tmp, 1, (unsigned int)raw->rowSize[y+z*raw->sizeY],
			raw->file);
		
		iPtr = raw->tmp;
		oPtr = buf;
		while (1) {
			pixel = *iPtr++;
			count = (int)(pixel & 0x7F);
			if (!count) {
				return;
			}
			if (pixel & 0x80) {
				while (count--) {
					*oPtr++ = *iPtr++;
				}
			} else {
				pixel = *iPtr++;
				while (count--) {
					*oPtr++ = pixel;
				}
			}
		}
	} else {
		fseek(raw->file, 512+(y*raw->sizeX)+(z*raw->sizeX*raw->sizeY),
			SEEK_SET);
		fread(buf, 1, raw->sizeX, raw->file);
	}
}

bool Image::LoadRGB(const char *FName)
{
	rawImageRec raw;
	unsigned char *tmpR, *tmpG, *tmpB;

	// Test endian-ness
	union {
		int testWord;
		char testByte[4];
	} endianTest;
	bool swapFlag;
	
	endianTest.testWord = 1;
	if (endianTest.testByte[0] == 1) {
		swapFlag = true;
	} else {
		swapFlag = false;
	}

	// Open the file
	if ((raw.file = fopen(FName, "rb")) == NULL) {
		cerr << "LoadRGB() failed: can't open image file " << FName << endl;
		return true;
	}
	
	fread(&raw, 1, 104, raw.file);
	
	cerr << "Image name is: `" << raw.name << "'\n";

	if (swapFlag) {
		ConvertShort(&raw.imagic, 6);
	}
	
	raw.tmp = new unsigned char[raw.sizeX*256];
	tmpR = new unsigned char[raw.sizeX*256];
	tmpG = new unsigned char[raw.sizeX*256];
	tmpB = new unsigned char[raw.sizeX*256];
	if (raw.tmp == NULL || tmpR == NULL || tmpG == NULL || tmpB == NULL) {
		fclose(raw.file);
		return NULL;
	}
	
	if ((raw.type & 0xFF00) == 0x0100) {
		int x = raw.sizeY * raw.sizeZ;
		int y = x * sizeof(unsigned int);
		raw.rowStart = new unsigned int[x];
		raw.rowSize = new int[x];
		if (raw.rowStart == NULL || raw.rowSize == NULL) {
			return NULL;
		}
		raw.rleEnd = 512 + (2 * y);
		fseek(raw.file, 512, SEEK_SET);
		fread(raw.rowStart, 1, y, raw.file);
		fread(raw.rowSize, 1, y, raw.file);
		if (swapFlag) {
			ConvertLong(raw.rowStart, x);
			ConvertLong((unsigned int *)raw.rowSize, x);
		}
	}
	
	wid = raw.sizeX;
	hgt = raw.sizeY;
	chan = raw.sizeZ;
	size = wid * hgt;
	dsize = wid * hgt * chan;
	
	Pix = new unsigned char[dsize];
	if ((raw.type & 0xFF00) == 0x0100)
		cerr << "Loading an rle compressed RGB image.\n";
	else
		cerr << "Loading a raw RGB image.\n";
	
	unsigned char *ptr = Pix;
	for (int i = raw.sizeY - 1; i >= 0; i--) {
		if(chan == 1)
		{
			RawImageGetRow(&raw, ptr, i, 0);
			ptr += wid;
		}
		else
		{
			RawImageGetRow(&raw, tmpR, i, 0);
			RawImageGetRow(&raw, tmpG, i, 1);
			RawImageGetRow(&raw, tmpB, i, 2);

			// Copy into standard RGB image.
			for (int j = 0; j < raw.sizeX; j++) {
				*ptr++ = tmpR[j];
				*ptr++ = tmpG[j];
				*ptr++ = tmpB[j];
			}
			
		}
	}
	
	fclose(raw.file);
	
	delete [] raw.tmp;
	delete [] tmpR;
	delete [] tmpG;
	delete [] tmpB;
	delete [] raw.rowStart;
	delete [] raw.rowSize;

	return false;
}

//////////////////////////////////////////////////////
// GIF File Format

/*
#define GIF_MAGIC 0x47494638
bool Image::LoadGIF(const char *FName)
{
	extern int ReadGIF(const char *FName, unsigned char **pic, int *w, int *h, int *ch);

	if(ReadGIF(FName, (unsigned char **)&Pix, &wid, &hgt, &chan) == 0) {
		cerr << "LoadGIF() failed: can't read GIF image file " << FName << endl;
		return true;
	}

	size = wid * hgt;
	dsize = wid * hgt * chan;

	return false;
}

bool Image::SaveGIF(const char *FName) const
{
	if(Pix == NULL || chan < 1 || wid < 1 || hgt < 1)
	{
		cerr << "Image is not defined. Not saving.\n";
		return true;
	}

	if(chan != 1 && chan != 3)
	{
		cerr << "Can't save a " << chan << " channel image as a GIF.\n";
		return true;
	}

	extern int WriteGIF(const char *FName, int wid, int hgt,
		unsigned char *Pix, bool GrayScale, char *comment);
	
	if(WriteGIF(FName, wid, hgt, (unsigned char *)Pix, (chan == 1),
		"Written using DaveMc Tools") == 0) {
		cerr << "SaveGIF() failed: can't write GIF image file " << FName << endl;
		return true;
	}
	
	cerr << "Wrote file " << FName << endl;
	return false;
}

*/

//////////////////////////////////////////////////////
// JPEG File Format

#ifdef SCI_USE_JPEG

#define JPEG_MAGIC 0xffd8ffe0

/*
bool Image::LoadJPEG(const char *FName)
{
#define NUM_ROWS 16
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE *infile;
	unsigned int y;
	JSAMPROW row_ptr[NUM_ROWS];
	
	if((infile = fopen(FName, "rb")) == NULL)
	{
		cerr << "can't open JPEG file " << FName << endl;
		return true;
	}
	
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);
	
	jpeg_stdio_src(&cinfo, infile);
	jpeg_read_header(&cinfo, TRUE);
	
	jpeg_start_decompress(&cinfo);
	
	wid = cinfo.output_width;
	hgt = cinfo.output_height;	
	chan = cinfo.output_components;
	size = wid * hgt;
	dsize = wid * hgt * chan;
	
	Pix = new unsigned char[dsize];
	
	while(cinfo.output_scanline < cinfo.output_height)
	{
		for(y=0; y<NUM_ROWS; y++)
			row_ptr[y] = &Pix[(cinfo.output_scanline + y) * wid * chan];
		jpeg_read_scanlines(&cinfo, row_ptr, NUM_ROWS);
	}
	
	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	
	fclose(infile);

	return false;
}
*/

/*
bool Image::SaveJPEG(const char *FName) const
{
	if(Pix == NULL || chan < 1 || wid < 1 || hgt < 1)
	{
		cerr << "Image is not defined. Not saving.\n";
		return true;
	}

	if(chan != 1 && chan != 3)
	{
		cerr << "Can't save a " << chan << " channel image as a JPEG.\n";
		return true;
	}
	
	
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	FILE *outfile;
	int y;
	JSAMPROW row_ptr[1];
	
	if((outfile = fopen(FName, "wb")) == NULL)
	{
		cerr << "SaveJPEG() failed: can't write to " << FName << endl;
		return true;
	}
	
	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);
	
	jpeg_stdio_dest(&cinfo, outfile);
	
	cinfo.image_width = wid;
	cinfo.image_height = hgt;
	cinfo.input_components = chan;
	cinfo.in_color_space = (chan == 1) ? JCS_GRAYSCALE : JCS_RGB;
	
	jpeg_set_defaults(&cinfo);
	
	jpeg_start_compress(&cinfo, TRUE);
	
	for(y=0; y<hgt; y++)
	{
		row_ptr[0] = &Pix[y*wid*chan];
		jpeg_write_scanlines(&cinfo, row_ptr, 1);
	}
	
	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);
	
	fclose(outfile);

	return false;
}
*/
#endif

//////////////////////////////////////////////////////
// TIFF File Format

#ifdef SCI_USE_TIFF

// Compression mode constants
#define NONE		1 
#define LEMPELZIV	5 

// Returns false on success and true on failure.
/*
bool Image::LoadTIFF(const char *FName)
{
	TIFF* tif;		// tif file handler 
	
#ifdef SCI_DEBUG
	cerr << "Attempting to open " << FName << " as TIFF.\n";
	cerr << "TIFF version is " << TIFFGetVersion() << endl;
#endif
	tif = TIFFOpen(FName, "r");
	if (!tif)
	{
		cerr << "Could not open TIFF file '" << FName << "'.\n";
		return true;
	}

	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &wid);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &hgt);
	TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &chan);
#ifdef SCI_MACHINE_sgi
	chan = chan >> 16;
#endif

#ifdef SCI_DEBUG
	cerr << "size = " << wid <<"x"<< hgt << " TIFFTAG_SAMPLESPERPIXEL=" << chan << endl;
	int tmp = 0;
	TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &tmp);
	cerr << "TIFFTAG_BITSPERSAMPLE " << tmp << endl;
	TIFFGetField(tif, TIFFTAG_COMPRESSION, &tmp);
	cerr << "TIFFTAG_COMPRESSION " << tmp << endl;
	TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &tmp);
	cerr << "TIFFTAG_PHOTOMETRIC " << tmp << endl;
	TIFFGetField(tif, TIFFTAG_EXTRASAMPLES, &tmp);
	cerr << "TIFFTAG_EXTRASAMPLES " << tmp << endl;

	TIFFPrintDirectory(tif, stderr, 0);
#endif

	size = wid * hgt;
	dsize = wid * hgt * chan;
	
	uint32 *ABGR = (uint32*) _TIFFmalloc(wid * hgt * sizeof (uint32));		// buffer for image data
	if (ABGR == NULL)
		return true;
	if (!TIFFReadRGBAImage(tif, wid, hgt, ABGR, 0))
		return true;
	
	Pix = new unsigned char[dsize];
	unsigned char *a = (unsigned char *) ABGR;
	unsigned char *row;
	int k, x;

#ifdef SCI_LITTLE_ENDIAN
	switch(chan)
	{
	case 1:
		for (k=hgt-1; k>=0; k--)
		{
			// Just keeps the red channel.
			row = &Pix[k*wid];
			for(x=0; x<wid; x++)
				row[x] = *(a += 4);
		}
		break;
	case 2:
		for (k=hgt-1; k>=0; k--)
		{
			// Just keeps red and alpha.
			row = &Pix[k*wid*2];
			for(x=0; x<2*wid; )
			{
				row[x++] = *(a+0);
				row[x++] = *(a+3);
				a += 4;
			}
		}
		break;
	case 3:
		for (k=hgt-1; k>=0; k--)
		{
			row = &Pix[k*wid*3];
			for(x=0; x<3*wid; )
			{
				row[x++] = *(a+0);
				row[x++] = *(a+1);
				row[x++] = *(a+2);
				a += 4;
			}
		}
		break;
	case 4:
		for (k=hgt-1; k>=0; k--)
		{
			row = &Pix[k*wid*4];
			memcpy(row, a, wid*4);
			a += wid*4;
		}
		break;
	}
#else	
	switch(chan)
	{
	case 1:
		a += 3;
		for (k=hgt-1; k>=0; k--)
		{
			row = &Pix[k*wid];
			for(x=0; x<wid; x++)
				row[x] = *(a += 4);
		}
		break;
	case 2:
		for (k=hgt-1; k>=0; k--)
		{
			row = &Pix[k*wid*2];
			for(x=0; x<2*wid; )
			{
				row[x++] = *(a+3);
				row[x++] = *(a+0);
				a += 4;
			}
		}
		break;
	case 3:
		for (k=hgt-1; k>=0; k--)
		{
			row = &Pix[k*wid*3];
			for(x=0; x<3*wid; )
			{
				row[x++] = *(a+3);
				row[x++] = *(a+2);
				row[x++] = *(a+1);
				a += 4;
			}
		}
		break;
	case 4:
		for (k=hgt-1; k>=0; k--)
		{
			row = &Pix[k*wid*4];
			for(x=0; x<4*wid; )
			{
				row[x++] = *(a+3);
				row[x++] = *(a+2);
				row[x++] = *(a+1);
				row[x++] = *(a);
				a += 4;
			}
		}
		break;
	}
#endif
	
	_TIFFfree(ABGR);

#ifdef SCI_DEBUG
	int dircount = 0;
	do {
		dircount++;
	} while (TIFFReadDirectory(tif));
	if(dircount > 1)
		cerr << "**** Contains " << dircount << " directories.\n";
#endif

	TIFFClose(tif);
	return 0;
}
*/

/*
bool Image::SaveTIFF(const char *FName) const
{
	if(Pix == NULL || chan < 1 || wid < 1 || hgt < 1)
	{
		cerr << "Image is not defined. Not saving.\n";
		return true;
	}

	TIFF *tif = TIFFOpen(FName, "w");
	if (tif == NULL)
		return true;
	
	TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, wid);
	TIFFSetField(tif, TIFFTAG_IMAGELENGTH, hgt);
	TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
	TIFFSetField(tif, TIFFTAG_COMPRESSION, LEMPELZIV);
	// TIFFSetField(tif, TIFFTAG_COMPRESSION, NONE);
	TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, (chan > 2) ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK);
	TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, chan);
	TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(tif, TIFFTAG_XRESOLUTION, 96.0);
	TIFFSetField(tif, TIFFTAG_YRESOLUTION, 96.0);
	TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, 2);
	TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, 1);
	// Actually isn't associated, but the library breaks.
	uint16 extyp = EXTRASAMPLE_ASSOCALPHA;
	if(chan == 2 || chan == 4)
		TIFFSetField(tif, TIFFTAG_EXTRASAMPLES, 1, &extyp);

	unsigned char *c = Pix;
	
	// Write one row of the image at a time 
	for (int l = 0; l <  hgt; l++)
	{
		if (TIFFWriteScanline(tif, c, l, 0) < 0)
		{
			TIFFClose(tif);
			return true;
		}
		c += wid*chan;
	}
	
	// Close the file and return OK
	TIFFClose(tif);
	return false;
}
*/
#endif

Image::Image(const char *FName, const int ch)
{
	Pix = NULL;
	wid = hgt = chan = size = dsize = 0;
	
#if defined ( SCI_MACHINE_sgi ) || defined ( SCI_MACHINE_hp )
	ifstream InFile(FName);
	if(!InFile.rdbuf()->is_open())
#else
	ifstream InFile(FName, ios::in | ios::binary);
	if(!InFile.is_open())
#endif
	{
		cerr << "Failed to open file `" << FName << "'.\n";
		return;
	}

	unsigned int Magic;
	char *Mag = (char *)&Magic;
	InFile.read(Mag, 4);
	InFile.close();
	unsigned int eMagic = Magic;
	ConvertLong(&eMagic, 1);
	
	if(Magic == RAS_MAGIC || eMagic == RAS_MAGIC)
	{
		LoadRas(FName);
	}
	/*
	else if(Magic == GIF_MAGIC || eMagic == GIF_MAGIC)
	{
		LoadGIF(FName);
	}
	*/
	/*
#ifdef SCI_USE_JPEG
	else if(Magic == JPEG_MAGIC || eMagic == JPEG_MAGIC)
	{
		LoadJPEG(FName);
	}
#endif
*/
	else if(Magic == RGB_MAGIC || eMagic == RGB_MAGIC)
	{
		LoadRGB(FName);
	}
	else if((Mag[0] == 'P' && (Mag[1] == '5' || Mag[1] == '6')) ||
		(Mag[3] == 'P' && (Mag[2] == '5' || Mag[2] == '6')))
	{
		LoadPPM(FName);
	}
	else
#ifdef SCI_USE_TIFF
	  //if(LoadTIFF(FName))
#endif
	{
		cerr << "Could not determine file type of `" << FName << "'.\n";
		cerr << "Magic was " << Magic << " or "<<eMagic<<
			" or `" << Mag[0] << Mag[1] << Mag[2] << Mag[3] << "'.\n";
		return;
	}
	
	// Use a specific number of color channels, if desired.
	if(ch >= 1 && ch <= 4 && ch != chan && (Pix != NULL))
		SetChan(ch);
}

/*
// Returns false on success; true on error.
bool Image::Save(const char *fname) const
{
  ASSERTERR(fname, "NULL filename");

  char *extp = strrchr(fname, '.');

  extp++;

  if(strlen(extp) != 3)
	{
	  cerr << "Can't grok filename " << fname << endl;
	  return true;
	}

  extp[0] |= 0x20;
  extp[1] |= 0x20;
  extp[2] |= 0x20;

  if(!strcmp(extp, "gif"))
	 return SaveGIF(fname);
  else if(!strcmp(extp, "tif"))
	 return SaveTIFF(fname);
  else if(!strcmp(extp, "jpg"))
	 return SaveJPEG(fname);
  else if(!strcmp(extp, "ppm"))
	 return SavePPM(fname);
  else if(!strcmp(extp, "pgm"))
	 return SavePPM(fname);

  return true;
}
*/

// Convert the number of image channels in this image.
void Image::SetChan(const int ch)
{
	if(ch < 1 || ch > 4) return;

	unsigned char *P2 = doSetChan(ch, Pix);

	delete [] Pix;
	Pix = P2;

	chan = ch;
	size = wid * hgt;
	dsize = wid * hgt * chan;
}

// Return a new pixel buffer based on P, but with ch channels.
unsigned char *Image::doSetChan(const int ch, unsigned char *P)
{
	int i, i2;

	int dsize1 = wid * hgt * ch;

	// Change the image's parameters.
	unsigned char *P2 = new unsigned char[dsize1];
	
	if(ch == chan)
	{
		memcpy(P2, P, dsize1);
		return P2;
	}

	// Change the number of channels.
	switch(chan)
	{
	case 1:
		{
			switch(ch)
			{
			case 2:
				for(i=i2=0; i<dsize; i++)
				{
					P2[i2++] = P[i];
					P2[i2++] = 255;
				}
				break;
			case 3:
				for(i=i2=0; i<dsize; i++)
				{
					P2[i2++] = P[i];
					P2[i2++] = P[i];
					P2[i2++] = P[i];
				}
				break;
			case 4:
				for(i=i2=0; i<dsize; i++)
				{
					P2[i2++] = P[i];
					P2[i2++] = P[i];
					P2[i2++] = P[i];
					P2[i2++] = 255;
				}
				break;
			}
		}
		break;
	case 2:
		{
			switch(ch)
			{
			case 1:
				for(i=i2=0; i<dsize; i+=2)
				{
					P2[i2++] = P[i];
				}
				break;
			case 3:
				for(i=i2=0; i<dsize; i+=2)
				{
					P2[i2++] = P[i];
					P2[i2++] = P[i];
					P2[i2++] = P[i];
				}
				break;
			case 4:
				for(i=i2=0; i<dsize; i++)
				{
					P2[i2++] = P[i];
					P2[i2++] = P[i];
					P2[i2++] = P[i];
					P2[i2++] = P[++i];
				}
				break;
			}
		}
		break;
	case 3:
		{
			switch(ch)
			{
			case 1:
				for(i=i2=0; i<dsize; )
				{
					P2[i2++] = (unsigned char)((77 * int(P[i++]) + 150 * int(P[i++]) +
						29 * int(P[i++])) >> 8);
				}
				break;
			case 2:
				for(i=i2=0; i<dsize; )
				{
					P2[i2++] = (unsigned char)((77 * int(P[i++]) + 150 * int(P[i++]) +
						29 * int(P[i++])) >> 8);
					P2[i2++] = 255;
				}
				break;
			case 4:
				for(i=i2=0; i<dsize; )
				{
					P2[i2++] = P[i++];
					P2[i2++] = P[i++];
					P2[i2++] = P[i++];
					P2[i2++] = 255;
				}
				break;
			}
		}
		break;
	case 4:
		{
			switch(ch)
			{
			case 1:
				for(i=i2=0; i<dsize; i++)
				{
					P2[i2++] = (unsigned char)((77 * int(P[i++]) + 150 * int(P[i++]) +
						29 * int(P[i++])) >> 8);
				}
				break;
			case 2:
				for(i=i2=0; i<dsize; )
				{
					P2[i2++] = (unsigned char)((77 * int(P[i++]) + 150 * int(P[i++]) +
						29 * int(P[i++])) >> 8);
					P2[i2++] = P[i++];
				}
				break;
			case 3:
				for(i=i2=0; i<dsize; i++)
				{
					P2[i2++] = P[i++];
					P2[i2++] = P[i++];
					P2[i2++] = P[i++];
				}
				break;
			}
		}
		break;
	}

	return P2;
}

// Convert this image to the given parameters.
void Image::Set(const int _w, const int _h, const int _ch,
				const bool init)
{
	int w = _w, h = _h, ch = _ch;

	if(w < 0) w = wid;
	if(h < 0) h = hgt;
	if(ch < 0) ch = chan;
	
	int dsize1 = w * h * ch;

	if(Pix)
	{
		// Deal with existing picture.
		if(dsize1 <= 0)
		{
			delete [] Pix;
			Pix = NULL;
			wid = hgt = chan = size = dsize = 0;
			return;
		}
		
		// Copies the pixels to a new array of the right num. color channels.
		unsigned char *P2 = doSetChan(ch, Pix);
		delete [] Pix;
		Pix = P2;
		chan = ch;
		dsize = wid * hgt * chan;
		size = wid * hgt;

		// Rescale to the new width and height.
		Resize(w, h);
	}
	else
	{
		if(dsize1 > 0)
		{
			wid = w;
			hgt = h;
			chan = ch;
			dsize = dsize1;
			size = wid * hgt;
			Pix = new unsigned char[dsize];
			if(init)
				fill(0);
		}
		else
		{
			Pix = NULL;
			wid = hgt = chan = size = dsize = 0;
		}
	}
}

// Convert this image to the given parameters.
void Image::Set(const Image &Img, const int _w, const int _h,
				const int _ch, const bool init)
{
	int w = _w, h = _h, ch = _ch;

	if(w < 0) w = Img.wid;
	if(h < 0) h = Img.hgt;
	if(ch < 0) ch = Img.chan;
	
	int dsize1 = w * h * ch;
	
	if(Pix)
		delete [] Pix;
	
	// Deal with existing picture.
	if(dsize1 <= 0)
	{
		wid = hgt = chan = size = dsize = 0;
		Pix = NULL;
		return;
	}
	
	if(Img.Pix)
	{		
		// Copies the pixels to a new array of the right num. color channels.
		wid = Img.wid;
		hgt = Img.hgt;
		chan = Img.chan;
		size = wid * hgt;
		dsize = wid * hgt * chan;

		Pix = doSetChan(ch, Img.Pix);
		chan = ch;
		dsize = wid * hgt * chan;

		// Rescale to the new width and height.
		Resize(w, h);
	}
	else
	{
		if(dsize1 > 0)
		{
			wid = w;
			hgt = h;
			chan = ch;
			dsize = dsize1;
			size = wid * hgt;
			Pix = new unsigned char[dsize];
			if(init)
				fill(0);
		}
	}
}

Image Image::operator+(const Image &Im) const
{
	ASSERT(chan == Im.chan && wid == Im.wid && hgt == Im.hgt);
	
	Image Out(wid, hgt, chan);
	
	for(int i=0; i<dsize; i++)
	{
		unsigned short sum = Pix[i] + Im.Pix[i];
		Out.Pix[i] = (sum <= 255) ? sum : 255;
	}
	
	return Out;
}

Image &Image::operator+=(const Image &Im)
{
	ASSERT(chan == Im.chan && wid == Im.wid && hgt == Im.hgt);
	
	for(int i=0; i<dsize; i++)
	{
		unsigned short sum = Pix[i] + Im.Pix[i];
		Pix[i] = (sum <= ((unsigned char)255)) ? sum : ((unsigned char)255);
	}
	
	return *this;
}

// Copy channel number src of image img to channel dest of this image.
// Things work fine if img is *this.
void Image::SpliceChan(const Image &Im, const int src, const int dest)
{
	ASSERT(wid == Im.wid && hgt == Im.hgt);
	ASSERT(src < Im.chan && dest < chan);

	int tind = dest;
	int sind = src;
	for(; tind < dsize ; )
		Pix[tind += chan] = Im.Pix[sind += Im.chan];
}

void Image::VFlip()
{
  int lsize = wid * chan;
  unsigned char *tbuf = new unsigned char[lsize];

  for(int y=0; y<hgt/2; y++)
	{
	  memcpy(tbuf, &Pix[y*lsize], lsize);
	  memcpy(&Pix[y*lsize], &Pix[(hgt-y-1)*lsize], lsize);
	  memcpy(&Pix[(hgt-y-1)*lsize], tbuf, lsize);
	}
  delete [] tbuf;
}

// 3x3 blur with special kernel. Assumes single channel image.
void Image::FastBlur1()
{
	ASSERT(chan == 1);
	// cerr << "FastBlur1\n";
	int y;
	
	// Allocates space for image.
	unsigned char *P2 = new unsigned char[dsize];
	
	// Do corners.
	{
		unsigned short C = Pix[0] << 2;
		C += Pix[1] << 2;
		C += Pix[wid] << 2;
		C += Pix[wid+1] << 1;
		C += Pix[wid+1];
		P2[0] = (unsigned char)((C + 16) / 15);

		C = Pix[wid-1] << 2;
		C += Pix[wid-2] << 2;
		C += Pix[wid+wid-1] << 2;
		C += Pix[wid+wid-2] << 1;
		C += Pix[wid+wid-2];
		P2[wid-1] = (unsigned char)((C + 16) / 15);
		
		int ib=(hgt-1)*wid;
		C = Pix[ib] << 2;
		C += Pix[ib+1] << 2;
		C += Pix[ib-wid] << 2;
		C += Pix[ib-wid+1] << 1;
		C += Pix[ib-wid+1];
		P2[ib] = (unsigned char)((C + 16) / 15);

		C = Pix[ib+wid-1] << 2;
		C += Pix[ib+wid-2] << 2;
		C += Pix[ib-1] << 2;
		C += Pix[ib-2] << 1;
		C += Pix[ib-2];
		P2[ib+wid-1] = (unsigned char)((C + 16) / 15);
	}

	// Do top and bottom edges.
	int it=1, ib=(hgt-1)*wid+1;
	for(; it<wid-1; ib++, it++)
	{
		// Top
		unsigned short C = Pix[it] << 2;
		C += Pix[it+1] << 2;
		C += Pix[it-1] << 2;
		C += Pix[it+wid] << 2;
		C += Pix[it+wid+1];
		C += Pix[it+wid+1] << 1;
		C += Pix[it+wid-1];
		C += Pix[it+wid-1] << 1;
		P2[it] = (unsigned char)((C + 16) / 22);
		
		// Bottom
		C = Pix[ib] << 2;
		C += Pix[ib+1] << 2;
		C += Pix[ib-1] << 2;
		C += Pix[ib-wid] << 2;
		C += Pix[ib-wid+1];
		C += Pix[ib-wid+1] << 1;
		C += Pix[ib-wid-1];
		C += Pix[ib-wid-1] << 1;
		P2[ib] = (unsigned char)((C + 16) / 22);
		//P2[ib] = 255;
	}
	
	for(y=1; y<hgt-1; y++)
	{
		int il = y*wid, ir = y*wid+wid-1;
		
		// Left side
		unsigned short C = Pix[il] << 2;
		C += Pix[il+1] << 2;
		C += Pix[il+wid] << 2;
		C += Pix[il-wid] << 2;
		C += Pix[il+wid+1];
		C += Pix[il+wid+1] << 1;
		C += Pix[il-wid+1];
		C += Pix[il-wid+1] << 1;
		P2[il] = (unsigned char)((C + 16) / 22);
		
		// Right side
		C = Pix[ir] << 2;
		C += Pix[ir-1] << 2;
		C += Pix[ir+wid] << 2;
		C += Pix[ir-wid] << 2;
		C += Pix[ir+wid-1];
		C += Pix[ir+wid-1] << 1;
		C += Pix[ir-wid-1];
		C += Pix[ir-wid-1] << 1;
		P2[ir] = (unsigned char)((C + 16) / 22);
		//P2[ir] = 255;
	}
	
#ifdef SCI_USE_MP
#pragma parallel 
#pragma pfor schedtype(gss) local(y)
#endif
	for(y=1; y<hgt-1; y++)
	{
		int ind = y*wid+1;
		for(int x=1; x<wid-1; x++, ind++)
		{
			// Sum of weights: 343 444 343 = 32
			unsigned short C = Pix[ind] << 2;
			C += Pix[ind+1] << 2;
			C += Pix[ind-1] << 2;
			C += Pix[ind+wid] << 2;
			C += Pix[ind-wid] << 2;
			C += Pix[ind+wid+1];
			C += Pix[ind+wid+1] << 1;
			C += Pix[ind+wid-1];
			C += Pix[ind+wid-1] << 1;
			C += Pix[ind-wid+1];
			C += Pix[ind-wid+1] << 1;
			C += Pix[ind-wid-1];
			C += Pix[ind-wid-1] << 1;
			P2[ind] = (unsigned char)((C + 16) >> 5);
		}
	}

	// Hook the new image into this Image.
	delete[] Pix;
	Pix = P2;
}

// N is the size of ONE DIMENSION of the kernel.
// Assumes an odd kernel size. Assumes single channel image.
void Image::Filter1(const int N, const KERTYPE *kernel)
{
	ASSERT(chan == 1);

	int N2 = N/2, x, y;
	
	// Allocates space for image.
	unsigned char *P2 = new unsigned char[dsize];

	// Do top and bottom edges.
	{
	for(x=N2; x<wid-N2; x++)
	{
		for(y=0; y<N2; y++)
			P2[y*wid+x] = SampleSlow1(x, y, N, kernel);
		for(y=hgt-N2; y<hgt; y++)
			P2[y*wid+x] = SampleSlow1(x, y, N, kernel);
	}
	}

	// Do left and right edges.
	for(y=0; y<hgt; y++)
	{
		for(int x=0; x<N2; x++)
			P2[y*wid+x] = SampleSlow1(x, y, N, kernel);
		for(x=wid-N2; x<wid; x++)
			P2[y*wid+x] = SampleSlow1(x, y, N, kernel);
	}

#ifdef SCI_USE_MP
#pragma parallel 
#pragma pfor schedtype(gss) local(y)
#endif
	for(y=N2; y<hgt-N2; y++)
	{
		int y0 = y-N2;
		int y1 = y+N2;
		for(int x=N2; x<wid-N2; x++)
		{
			// Add the pixels that contribute to this one.
			int x0 = x-N2;
			int x1 = x+N2;
			
			unsigned int C = 0;
			int ker = 0;
			for(int yy=y0; yy <= y1; yy++)
			{
				for(int xx=x0; xx <= x1; xx++, ker++)
				{
					C += Pix[yy*wid+xx] * kernel[ker];
				}
			}
			P2[y*wid+x] = (unsigned char)((C + 0x8000) >> 16);
		}
	}

	// Hook the new image into this Image.
	delete[] Pix;
	Pix = P2;
}

void Image::Filter3(const int N, const KERTYPE *kernel)
{
	ASSERT(chan == 3);

	int N2 = N/2, x, y;
	
	// Allocates space for color image.
	unsigned char *P2 = new unsigned char[dsize];

	Pixel *Pp2 = (Pixel *)P2;
	// Do top and bottom edges.
	for(x=N2; x<wid-N2; x++)
	{
		for(y=0; y<N2; y++)
			Pp2[y*wid+x] = SampleSlow3(x, y, N, kernel);
		for(y=hgt-N2; y<hgt; y++)
			Pp2[y*wid+x] = SampleSlow3(x, y, N, kernel);
	}

	// Do left and right edges.
	for(y=0; y<hgt; y++)
	{
		for(x=0; x<N2; x++)
			Pp2[y*wid+x] = SampleSlow3(x, y, N, kernel);
		for(x=wid-N2; x<wid; x++)
			Pp2[y*wid+x] = SampleSlow3(x, y, N, kernel);
	}

	for(y=N2; y<hgt-N2; y++)
	{
		int y0 = y-N2;
		int y1 = y+N2;
		for(x=N2; x<wid-N2; x++)
		{
			// Add the pixels that contribute to this one.
			int x0 = x-N2;
			int x1 = x+N2;
			
			unsigned int Cr = 0, Cg = 0, Cb = 0;
			int ker = 0;
			//KERTYPE SK = 0;
			for(int yy=y0; yy <= y1; yy++)
			{
				for(int xx=x0; xx <= x1; xx++, ker++)
				{
					Cr += Pix[(yy*wid+xx)*3] * kernel[ker];
					Cg += Pix[(yy*wid+xx)*3+1] * kernel[ker];
					Cb += Pix[(yy*wid+xx)*3+2] * kernel[ker];
				}
			}
			P2[(y*wid+x)*3] = (unsigned char)((Cr + 0x8000) >> 16);
			P2[(y*wid+x)*3+1] = (unsigned char)((Cg + 0x8000) >> 16);
			P2[(y*wid+x)*3+2] = (unsigned char)((Cb + 0x8000) >> 16);
		}
	}

	// Hook the new image into this Image.
	delete[] Pix;
	Pix = P2;
}

// N is the size of ONE DIMENSION of the kernel.
// Assumes an odd kernel size.
void Image::Filter(const int N, const KERTYPE *kernel)
{
	if(chan == 1)
		Filter1(N, kernel);
	else if(chan == 3)
		Filter3(N, kernel);
	else
		cerr << "Filtering not supported on " << chan << " channel images.\n";
}

// The sqrt of 2 pi.
#ifndef SQRT2PI
#define SQRT2PI 2.506628274631000502415765284811045253006
#endif

// Symmetric gaussian centered at origin.
// No covariance matrix. Give it X and Y.
inline double Gaussian2(double x, double y, double sigma)
{
        return exp(-0.5 * (Sqr(x) + Sqr(y)) / Sqr(sigma)) / (SQRT2PI * sigma);
}


double *MakeBlurKernel(const int N, const double sigma)
{
	double *kernel = new double[N*N];
	
	int N2 = N/2, x, y;
	
	double S = 0;
	for(y= -N2; y<=N2; y++)
	{
		for(x= -N2; x<=N2; x++)
		{
			double G = Gaussian2(x, y, sigma);
			kernel[(y + N2)*N + (x+N2)] = G;
			
			S += G;
		}
	}
	
	// normalize the kernel.
	for(y = 0; y<N; y++)
		for(x = 0; x<N; x++)
			kernel[y*N + x] /= S;

	return kernel;
}

// Make a fixed point kernel from a double kernel.
// Does NOT delete the old kernel.
KERTYPE *DoubleKernelToFixed(const int N, double *dkernel)
{
	KERTYPE *ckernel = new KERTYPE[N*N];

	// I should multiply by 256 and clamp to 255 but I won't.
	double SD = 0;
	double SC = 0;
	for(int i=0; i<N*N; i++)
	{
		ckernel[i] = (KERTYPE)(65535.0 * dkernel[i] + 0.5);
		SD += dkernel[i];
		SC += ckernel[i];
	}

	//cerr << "Double kernel weight = " << SD << endl;
	//cerr << "Byte kernel weight = " << int(SC) << endl;

	return ckernel;
}

// Blur this image with a kernel of size N and st. dev. of sigma.
// sigma = 1 seems to work well for different N.
// Makes a gaussian of st. dev. sigma, samples it at the places on
// the kernel (1 pixel is 1 unit), and then normalizes the kernel
// to have unit mass.
void Image::Blur(const int N, const double sig)
{
	if(chan == 1 && sig == 0)
	{
		FastBlur1();
		return;
	}

	double sigma = sig;
	if(sig < 0)
		sigma = double(N) / 3.0;
	
	double *dkernel = MakeBlurKernel(N, sigma);
	KERTYPE *ckernel = DoubleKernelToFixed(N, dkernel);
	delete [] dkernel;

	Filter(N, ckernel);

	delete [] ckernel;
}

// Upsample linearly.
void Image::HorizFiltLinear(unsigned char *Ld, int wd,
  unsigned char *Ls, int ws)
{
  int xs = 0, xsw = 0, xdw = 0;
  for(int xd=0; xd<wd; xd++, xdw += wd)
    {
      // Sample from xs and xs+1 into xd.
      for(int c=0; c<chan; c++)
	{
	  Ld[xd] = (Ls[xs] * (xdw-xsw)) / ws +
	    (Ls[xs+1] * (xsw+ws-xdw)) / ws;
	}
		
      // Maybe step xs.
      if(xsw+ws < xdw)
	{
	  xs++;
	  xsw += ws;
	}
    }
}

// Rescales the image to the given size.
void Image::Resize(const int w, const int h)
{
#if 0
  if(w < 1 || h < 1 || chan < 1)
    {
      wid = hgt = chan = size = dsize = 0;
      if(Pix)
	delete [] Pix;
      return;
    }
  
  if(size < 1 || Pix == NULL)
    return;
  
  unsigned char *P;
  int x, y;
  
  // Scale the width first.
  int dsize1 = w * hgt * chan;
  
  if(w == wid)
    P = Pix;
  else
    {
      P = new unsigned char[dsize1];
      if(w > wid)
	{
	  // Upsample using cubic filter.
	  for(y=0; y<hgt; y++)
	    HorizFiltLinear(&P[y*w*chan], w, &Pix[y*wid*chan], wid);
	}
      else
	{
	  // Downsample using gaussian.
	  for(y=0; y<hgt; y++)
	    HorizFiltGaussian(&P[y*w*chan], w, &Pix[y*wid*chan], wid);
	}
    }
  
  if(P != Pix)
    delete [] Pix;
  Pix = P;
  wid = w;
  
  // Scale the height.
  dsize1 = wid * h * chan;
  
  if(h == hgt)
    P = Pix;
  else
    {
      P = new unsigned char[dsize1];
      if(h > hgt)
	{
	  // Upsample using cubic filter.
	  for(x=0; x<wid; x++)
	    VertFiltCubic(&P[x*chan], h, &Pix[x*chan], hgt, wid*chan);
	}
      else
	{
	  // Downsample using gaussian.
	  for(x=0; x<wid; x++)
	    VertFiltGaussian(&P[x*chan], h, &Pix[x*chan], hgt, wid*chan);
	}
    }
  
  if(P != Pix)
    delete [] Pix;
  Pix = P;
#endif
  wid = w;
  hgt = h;
  size = wid * hgt;
  dsize = wid * hgt * chan;
}


}
}
