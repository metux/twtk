#ifndef __TWTK_PRIVATE_XCB_UTIL_H
#define __TWTK_PRIVATE_XCB_UTIL_H

#include <xcb/xcb.h>


/** XCB: set window title **/
int xcbutil_set_window_title(xcb_connection_t *conn, xcb_window_t window, const char *title)
    __attribute__((nonnull(1,3)));

/** XCB: find visual structure by ID **/
xcb_visualtype_t *xcbutil_find_visual(xcb_connection_t *c, xcb_visualid_t visual)
    __attribute__((nonnull(1)));

/** XCB: get atom id by name **/
xcb_atom_t xcbutil_get_atom(xcb_connection_t *conn, const char *name)
    __attribute__((nonnull(1,2)));

/** XCB: get workspace area **/
int xcbutil_get_workspace_area(xcb_connection_t *conn, xcb_window_t root, uint32_t workspace, xcb_rectangle_t *r_rect)
    __attribute__((nonnull(1,4)));

#endif /* __TWTK_PRIVATE_XCB_UTIL_H */
