//
// "$Id: Fl_PNG_Image.cxx 12439 2017-09-10 11:38:23Z AlbrechtS $"
//
// Fl_PNG_Image routines.
//
// Copyright 1997-2012 by Easy Software Products.
// Image support by Matthias Melcher, Copyright 2000-2009.
//
// Copyright 2013-2017 by Bill Spitzak and others.
//
// This library is free software. Distribution and use rights are outlined in
// the file "COPYING" which should have been included with this file.  If this
// file is missing or damaged, see the license at:
//
//     http://www.fltk.org/COPYING.php
//
// Please report all bugs and problems on the following page:
//
//     http://www.fltk.org/str.php
//
// Contents:

//
//   Fl_PNG_Image::Fl_PNG_Image() - Load a PNG image file.
//

//
// Include necessary header files...
//

#include <config.h>
#include <FL/Fl.H>
#include <FL/Fl_System_Driver.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_Shared_Image.H>
#include <FL/fl_utf8.h>

#include <stdio.h>
#include <stdlib.h>

#if defined(HAVE_LIBPNG) && defined(HAVE_LIBZ)
extern "C"
{
#  include <zlib.h>
#  ifdef HAVE_PNG_H
#    include <png.h>
#  else
#    include <libpng/png.h>
#  endif // HAVE_PNG_H
}

typedef struct  {
  png_structp pp;
  const unsigned char *current;
  const unsigned char *last;
} fl_png_memory;

extern "C" {
  static void png_read_data_from_mem( png_structp png_ptr, //pointer to our data
				      png_bytep data,  // where to copy the image data for libpng computing
				      png_size_t length) // length of data to copy
  {
    fl_png_memory *png_mem_data = (fl_png_memory*)png_get_io_ptr(png_ptr); // get the pointer to our struct
    if (png_mem_data->current + length > png_mem_data->last) {
      png_error(png_mem_data->pp, "Invalid attempt to read row data");
      return;
    }
    /* copy data from image buffer */
    memcpy (data, png_mem_data->current, length);
    /* advance in the memory data */
    png_mem_data->current += length;
  }
} // extern "C"
#endif // HAVE_LIBPNG && HAVE_LIBZ


/**
 The constructor loads the named PNG image from the given png filename.

 The destructor frees all memory and server resources that are used by
 the image.

 Use Fl_Image::fail() to check if Fl_PNG_Image failed to load. fail() returns
 ERR_FILE_ACCESS if the file could not be opened or read, ERR_FORMAT if the
 PNG format could not be decoded, and ERR_NO_IMAGE if the image could not
 be loaded for another reason.

 \param[in] filename	Name of PNG file to read
 */
Fl_PNG_Image::Fl_PNG_Image (const char *filename): Fl_RGB_Image(0,0,0)
{
  load_png_(filename, NULL, 0);
}


/**
 \brief Constructor that reads a PNG image from memory.

 Construct an image from a block of memory inside the application. Fluid offers
 "binary Data" chunks as a great way to add image data into the C++ source code.
 name_png can be NULL. If a name is given, the image is added to the list of
 shared images (see: Fl_Shared_Image) and will be available by that name.

 \param name_png  A name given to this image or NULL
 \param buffer	  Pointer to the start of the PNG image in memory
 \param maxsize   Size in bytes of the memory buffer containing the PNG image
 */
Fl_PNG_Image::Fl_PNG_Image (
      const char *name_png, const unsigned char *buffer, int maxsize): Fl_RGB_Image(0,0,0)
{
  load_png_(name_png, buffer, maxsize);
}


void Fl_PNG_Image::load_png_(const char *name_png, const unsigned char *buffer_png, int maxsize)
{
#if defined(HAVE_LIBPNG) && defined(HAVE_LIBZ)
  int i;		// Looping var
  int channels;		// Number of color channels
  png_structp pp;	// PNG read pointer
  png_infop info = 0;	// PNG info pointers
  png_bytep *rows;	// PNG row pointers
  fl_png_memory png_mem_data;
  int from_memory = (buffer_png != NULL); // true if reading image from memory

  // Note: The file pointer fp must not be an automatic (stack) variable
  // to avoid potential clobbering by setjmp/longjmp (gcc: [-Wclobbered]).
  static FILE *fp;	// intentionally initialized separately below
  fp = NULL;		// always initialize file pointer

  if (!from_memory) {
    if ((fp = fl_fopen(name_png, "rb")) == NULL) {
      ld(ERR_FILE_ACCESS);
      return;
    }
  }
  const char *display_name = (name_png ? name_png : "In-memory PNG data");

  // Setup the PNG data structures...
  pp = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (pp) info = png_create_info_struct(pp);
  if (!pp || !info) {
    if (pp) png_destroy_read_struct(&pp, NULL, NULL);
    if (!from_memory) fclose(fp);
    Fl::warning("Cannot allocate memory to read PNG file or data \"%s\".\n", display_name);
    w(0); h(0); d(0); ld(ERR_FORMAT);
    return;
  }

  if (setjmp(png_jmpbuf(pp))) {
    png_destroy_read_struct(&pp, &info, NULL);
    if (!from_memory) fclose(fp);
    Fl::warning("PNG file or data \"%s\" is too large or contains errors!\n", display_name);
    w(0); h(0); d(0); ld(ERR_FORMAT);
    return;
  }

  if (from_memory) {
    png_mem_data.current = buffer_png;
    png_mem_data.last = buffer_png + maxsize;
    png_mem_data.pp = pp;
    // Initialize the function pointer to the PNG read "engine"...
    png_set_read_fn (pp, (png_voidp) &png_mem_data, png_read_data_from_mem);
  } else {
    png_init_io(pp, fp); // Initialize the PNG file read "engine"...
  }

  // Get the image dimensions and convert to grayscale or RGB...
  png_read_info(pp, info);

  if (png_get_color_type(pp, info) == PNG_COLOR_TYPE_PALETTE)
    png_set_expand(pp);

  if (png_get_color_type(pp, info) & PNG_COLOR_MASK_COLOR)
    channels = 3;
  else
    channels = 1;

  int num_trans = 0;
  png_get_tRNS(pp, info, 0, &num_trans, 0);
  if ((png_get_color_type(pp, info) & PNG_COLOR_MASK_ALPHA) || (num_trans != 0))
    channels ++;

  w((int)(png_get_image_width(pp, info)));
  h((int)(png_get_image_height(pp, info)));
  d(channels);

  if (png_get_bit_depth(pp, info) < 8)
  {
    png_set_packing(pp);
    png_set_expand(pp);
  }
  else if (png_get_bit_depth(pp, info) == 16)
    png_set_strip_16(pp);

#  if defined(HAVE_PNG_GET_VALID) && defined(HAVE_PNG_SET_TRNS_TO_ALPHA)
  // Handle transparency...
  if (png_get_valid(pp, info, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(pp);
#  endif // HAVE_PNG_GET_VALID && HAVE_PNG_SET_TRNS_TO_ALPHA

  if (((size_t)w()) * h() * d() > max_size() ) longjmp(png_jmpbuf(pp), 1);
  array = new uchar[w() * h() * d()];
  alloc_array = 1;

  // Allocate pointers...
  rows = new png_bytep[h()];

  for (i = 0; i < h(); i ++)
    rows[i] = (png_bytep)(array + i * w() * d());

  // Read the image, handling interlacing as needed...
  for (i = png_set_interlace_handling(pp); i > 0; i --)
    png_read_rows(pp, rows, NULL, h());

  if (channels == 4) Fl::system_driver()->png_extra_rgba_processing((uchar*)array, w(), h());

  // Free memory and return...
  delete[] rows;

  png_read_end(pp, info);
  png_destroy_read_struct(&pp, &info, NULL);

  if (from_memory) {
    if (w() && h() && name_png) {
      Fl_Shared_Image *si = new Fl_Shared_Image(name_png, this);
      si->add();
    }
  } else {
    fclose(fp);
  }
#endif // HAVE_LIBPNG && HAVE_LIBZ
}

#if defined(HAVE_LIBPNG) && defined(HAVE_LIBZ)
/* structure to store PNG image bytes */
typedef struct {
  uchar *buffer;
  size_t size;
  size_t size_allocated;
} mem_encode;


static void my_png_write_data(png_structp png_ptr, png_bytep data, png_size_t length){
  mem_encode* p=(mem_encode*)png_get_io_ptr(png_ptr);
  size_t nsize = p->size + length;

  /* allocate or grow buffer */
  if(nsize > p->size_allocated){
      int alloc_size = nsize * 2;
      if(p->buffer)
        p->buffer = (uchar*)realloc(p->buffer, alloc_size);
      else
        p->buffer = (uchar*)malloc(alloc_size);

      if(!p->buffer) {
        png_error(png_ptr, "Write Error");
        return;
      }
      p->size_allocated = alloc_size;
  }

  /* copy new bytes to end of buffer */
  memcpy(p->buffer + p->size, data, length);
  p->size += length;
}

/* This is optional but included to show how png_set_write_fn() is called */
static void my_png_flush(png_structp png_ptr){}
#endif // HAVE_LIBPNG && HAVE_LIBZ

int Fl_PNG_Image::encode(Fl_Image *img, unsigned char **outbuffer, int &outlen){
#if defined(HAVE_LIBPNG) && defined(HAVE_LIBZ)
    int imgw = img->w(), imgh = img->h(), imgd = img->d();
    outlen = imgw * imgh * imgd;

    png_structp			pp;		// PNG data
    png_infop			info;		// PNG image info
    mem_encode state;
    /* initialise - put this before png_write_png() call */
    state.buffer = NULL;
    state.size = 0;
    state.size_allocated = 0;


    if( !((imgd == 1) || (imgd ==3) || (imgd ==4))) return -1;
    pp = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    if(!pp) return -1;

    info = png_create_info_struct(pp);
    if (!info){
        png_destroy_write_struct(&pp, 0);
        return -1;
    }

    if (setjmp(png_jmpbuf(pp))){
        png_destroy_write_struct(&pp, &info);
        return -1;
    }

    //png_init_io(pp, fp);
    /* if my_png_flush() is not needed, change the arg to NULL */
    png_set_write_fn(pp, &state, my_png_write_data, my_png_flush);


    png_set_compression_level(pp, Z_BEST_COMPRESSION);
    png_set_IHDR(pp, info, imgw, imgh, 8,
               imgd == 1 ? PNG_COLOR_TYPE_GRAY :
               imgd == 3 ? PNG_COLOR_TYPE_RGB : PNG_COLOR_TYPE_RGB_ALPHA,
           PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
           PNG_FILTER_TYPE_DEFAULT);
    png_set_sRGB(pp, info, PNG_sRGB_INTENT_PERCEPTUAL);
    //png_set_sRGB_gAMA_and_cHRM(pp, info, PNG_INFO_sRGB);

    png_write_info(pp, info);

    uchar *idata    = (uchar *)*img->data();
    int iwd = imgw * imgd;
    for (int y = 0; y < imgh; y++)
    {
        png_write_row(pp, (png_byte *)idata);
        idata += iwd;
    }

    png_write_end(pp, info);
    png_destroy_write_struct(&pp, 0);

    /* cleanup */
    //if(state.buffer) free(state.buffer);

    *outbuffer = state.buffer;
	outlen = state.size;

//	FILE* outfile = fopen("test.png", "wb");
//	if (outfile){
//	    fwrite(*outbuffer, outlen,1 ,outfile);
//	    fclose(outfile);
//	}
	return 0;
#else
    return -1;
#endif // HAVEHAVE_LIBPNG && HAVE_LIBZ
}

//
// End of "$Id: Fl_PNG_Image.cxx 12439 2017-09-10 11:38:23Z AlbrechtS $".
//
