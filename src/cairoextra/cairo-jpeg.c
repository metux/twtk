/*! This file contains two functions for reading and writing JPEG files from
 * and to Cairo image surfaces. It uses the functions from the libjpeg.
 * Most of the code is directly derived from the online example at
 * http://libjpeg-turbo.virtualgl.org/Documentation/Documentation
 *
 * cairo_status_t cairo_image_surface_write_to_jpeg(cairo_surface_t *sfc, const char *filename, int quality)
 * cairo_surface_t *cairo_image_surface_create_from_jpeg(const char *filename);
 *
 * The functions parameters and return values are described below directly at the functions.
 *
 * To compile the code you need to have installed the packages libcairo2-dev
 * and libjpeg-dev.
 * Compile with
 * gcc -Wall -g `pkg-config cairo libjpeg --cflags --libs` cairo_jpg.c
 *
 * @author Bernhard R. Fischer, 2048R/5C5FFD47 bf@abenteuerland.at
 * @version 2015/12/29
 * @license This code is free software. Do whatever you like to do with it.
 */

#include <stdio.h>
#include <stdlib.h>
#include <cairo.h>
#include <jpeglib.h>


/*! This function creates a JPEG file from a Cairo image surface.
 * @param sfc Pointer to a Cairo *image* surface. Its format must either be
 * CAIRO_FORMAT_ARGB32 or CAIRO_FORMAT_RGB24. Other formats are not supported
 * by this function, yet.
 * @param filename Pointer to filename of the destination file.
 * @param quality Compression quality, 0-100.
 * @return On success, the function returns 0. If the file could not be opened,
 * CAIRO_STATUS_FILE_NOT_FOUND is returned and errno is set according to
 * fopen(3). If sfc has an invalid format CAIRO_STATUS_INVALID_FORMAT is
 * returned.
 */
cairo_status_t cairo_image_surface_write_to_jpeg(cairo_surface_t *sfc, const char *filename, int quality)
{
   struct jpeg_compress_struct cinfo;
   struct jpeg_error_mgr jerr;
   FILE *outfile;
   JSAMPROW row_pointer[1];

   // check valid input format
   if (cairo_surface_get_type(sfc) != CAIRO_SURFACE_TYPE_IMAGE &&
         cairo_image_surface_get_format(sfc) != CAIRO_FORMAT_ARGB32 &&
         cairo_image_surface_get_format(sfc) != CAIRO_FORMAT_RGB24)
      return CAIRO_STATUS_INVALID_FORMAT;

   // finish queued drawing operations
   cairo_surface_flush(sfc);

   // init jpeg compression structures
   cinfo.err = jpeg_std_error(&jerr);
   jpeg_create_compress(&cinfo);

   // open file
   if ((outfile = fopen(filename, "wb")) == NULL)
      return CAIRO_STATUS_FILE_NOT_FOUND;

   // set compression parameters
   jpeg_stdio_dest(&cinfo, outfile);
   cinfo.image_width = cairo_image_surface_get_width(sfc);
   cinfo.image_height = cairo_image_surface_get_height(sfc);
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
   cinfo.in_color_space = JCS_EXT_BGRX;
#else
   cinfo.in_color_space = JCS_EXT_XRGB;
#endif
   cinfo.input_components = 4;
   jpeg_set_defaults(&cinfo);
   jpeg_set_quality(&cinfo, quality, TRUE);

   // start compressor
   jpeg_start_compress(&cinfo, TRUE);

   // loop over all lines and compress
   while (cinfo.next_scanline < cinfo.image_height)
   {
      row_pointer[0] = cairo_image_surface_get_data(sfc) + (cinfo.next_scanline
            * cairo_image_surface_get_stride(sfc));
      (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);
   }

   // finalize and close everything
   jpeg_finish_compress(&cinfo);
   fclose(outfile);
   jpeg_destroy_compress(&cinfo);

   return CAIRO_STATUS_SUCCESS;
}


/*! This function reads an JPEG image from a file an creates a Cairo image
 * surface.
 * @param filename Pointer to filename of JPEG file.
 * @return Returns a pointer to a cairo_surface_t structure. It should be
 * checked with cairo_surface_status() for errors.
 * @note If the returned surface is invalid you can use errno to determine
 * further reasons. Errno is set according to fopen(3) and malloc(3). If you
 * intend to check errno you shall set it to 0 before calling this function
 * because it does not modify errno itself.
 */
cairo_surface_t *cairo_image_surface_create_from_jpeg(const char *filename)
{
   struct jpeg_decompress_struct cinfo;
   struct jpeg_error_mgr jerr;
   FILE * infile;
   JSAMPROW row_pointer[1];
   cairo_surface_t *sfc;
   void *data;
   size_t len;

   // open input file
   if ((infile = fopen(filename, "rb")) == NULL)
      //return NULL;
      return cairo_image_surface_create(CAIRO_FORMAT_INVALID, 0, 0);

   // read data to memory
   fseek(infile, 0, SEEK_END);
   len = ftell(infile);
   fseek(infile, 0, SEEK_SET);
   if ((data = malloc(len)) == NULL)
      return cairo_image_surface_create(CAIRO_FORMAT_INVALID, 0, 0);

   fread(data, len, 1, infile);
   fclose(infile);

   // initialize jpeg decompression structures
   cinfo.err = jpeg_std_error(&jerr);
   jpeg_create_decompress(&cinfo);
   jpeg_mem_src(&cinfo, data, len);
   (void) jpeg_read_header(&cinfo, TRUE);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
   cinfo.out_color_space = JCS_EXT_BGRA;
#else
   cinfo.out_color_space = JCS_EXT_ARGB;
#endif

   // start decompressor
   (void) jpeg_start_decompress(&cinfo);

   // create Cairo image surface
   sfc = cairo_image_surface_create(CAIRO_FORMAT_RGB24, cinfo.output_width, cinfo.output_height);
   if (cairo_surface_status(sfc) != CAIRO_STATUS_SUCCESS)
   {
      jpeg_destroy_decompress(&cinfo);
      return sfc;
   }

   // loop over all scanlines and fill Cairo image surface
   while (cinfo.output_scanline < cinfo.output_height)
   {
      row_pointer[0] = cairo_image_surface_get_data(sfc) +
         (cinfo.output_scanline * cairo_image_surface_get_stride(sfc));
      (void) jpeg_read_scanlines(&cinfo, row_pointer, 1);
   }

   // finish and close everything
   cairo_surface_mark_dirty(sfc);
   (void) jpeg_finish_decompress(&cinfo);
   jpeg_destroy_decompress(&cinfo);

   // set jpeg mime data
   cairo_surface_set_mime_data(sfc, CAIRO_MIME_TYPE_JPEG, data, len, free, data);

   return sfc;
}
