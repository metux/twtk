#ifndef __CAIRO_JPEG_H_PUB
#define __CAIRO_JPEG_H_PUB

/**
 * Public interface for Cairo's public JPEG API.
 *
 * Only available when Cairo was compiled w/ JPEG support
 */

#include <cairo.h>

/**
 * Create a new image surface from JPEG image file.
 *
 * @param	fn		name of the jpeg file.
 *
 * @return	pointer to cairo_surface_t structure.
 *		may never be NULL, but the surface be in error state
 */
cairo_surface_t *cairo_image_surface_create_from_jpeg(const char *fn);

/**
 * Store image surface into JPEG file.
 *
 * @param	sfc	pointer to image surface structure
 *			currently only supports the following pixel formats:
 *			CAIRO_FORMAT_ARGB32 or CAIRO_FORMAT_RGB24
 * @param	fn	JPEG target filename
 * @param	quality	Compression quality, 0-100.
 *
 * @return	CAIRO_STATUS_SUCCESS (0)	if everything went fine
 *		CAIRO_STATUS_FILE_NOT_FOUND	if file could not be opened
 *						(errno is set accordingly)
 *		CAIRO_STATUS_INVALID_FORMAT	if sfc has an invalid format
 */
cairo_status_t cairo_image_surface_write_to_jpeg(cairo_surface_t *sfc,
						 const char *fn,
						 int quality);

#endif /* __CAIRO_JPEG_H_PUB */
