
#define _DEBUG_NAME     "xcb-util"

#ifdef ENABLE_DEBUG_PLATFORM_XCB
#define ENABLE_DEBUG
#endif

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <twtk-private/debug.h>
#include <twtk-private/xcb-util.h>


int xcbutil_set_window_title(xcb_connection_t *conn, xcb_window_t window, const char *title)
{
    _DEBUG("setting window title to: \"%s\"", title);

    xcb_change_property (
        conn,
        XCB_PROP_MODE_REPLACE,
        window,
        XCB_ATOM_WM_NAME,
        XCB_ATOM_STRING,
        8,
        strlen (title),
        title
    );

    return 0;
}

xcb_visualtype_t *xcbutil_find_visual(xcb_connection_t *c, xcb_visualid_t visual)
{
    xcb_screen_iterator_t screen_iter = xcb_setup_roots_iterator(xcb_get_setup(c));

    for (; screen_iter.rem; xcb_screen_next(&screen_iter)) {
        xcb_depth_iterator_t depth_iter = xcb_screen_allowed_depths_iterator(screen_iter.data);
        for (; depth_iter.rem; xcb_depth_next(&depth_iter)) {
            xcb_visualtype_iterator_t visual_iter = xcb_depth_visuals_iterator(depth_iter.data);
            for (; visual_iter.rem; xcb_visualtype_next(&visual_iter))
                if (visual == visual_iter.data->visual_id)
                    return visual_iter.data;
        }
    }

    return NULL;
}

/*
 * Request with a reply, handling errors directly (default)
 *
 */
xcb_atom_t xcbutil_get_atom(xcb_connection_t *conn, const char *name) {
    xcb_intern_atom_cookie_t cookie;
    xcb_intern_atom_reply_t *reply;
    xcb_generic_error_t *error;

    _DEBUG("requesting atom: %s", name);
    cookie = xcb_intern_atom(conn, 0, strlen(name), name);

    if ((reply = xcb_intern_atom_reply(conn, cookie, &error))) {
        _DEBUG("atom \"%s\" has ID: %u", name, reply->atom);
        xcb_atom_t atom = reply->atom;
        free(reply);
        return atom;
    }

    _DEBUG("X11 Error %d", error->error_code);
    free(error);
    return XCB_ATOM_NONE;
}

int xcbutil_get_workspace_area(xcb_connection_t *conn, xcb_window_t root, uint32_t workspace, xcb_rectangle_t *r_rect)
{
    xcb_get_property_cookie_t cookie = xcb_get_property(
        conn,
        0,
        root,
        xcbutil_get_atom(conn, "_NET_WORKAREA"),
        XCB_ATOM_CARDINAL,
        0,
        4*32
    );

    xcb_get_property_reply_t *reply = xcb_get_property_reply(conn, cookie, NULL);
    if (reply == NULL)
    {
        _DEBUG("xcbutil_get_workspace_area: request failed");
        return -ENOENT;
    }

    uint32_t *data = (uint32_t*)xcb_get_property_value(reply);
    if (data == NULL)
    {
        _DEBUG("xcbuti_get_workspace_area: no data");
        free(reply);
        return -ENOENT;
    }

    _DEBUG("workspace: x=%d y=%d w=%d h=%d", data[0], data[1], data[2], data[3]);

    *r_rect = (xcb_rectangle_t){
        .x = data[0],
        .y = data[1],
        .width = data[2],
        .height = data[3]
    };

    free(reply);

    return 0;
}
