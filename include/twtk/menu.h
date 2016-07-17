#ifndef __TWTK_MENU_H_
#define __TWTK_MENU_H_

#include <twtk/types.h>
#include <twtk/util.h>

struct __twtk_menu_entry
{
    const char *text;
    const char *id;
    twtk_menu_t *sub;
};

struct __twtk_menu
{
    const char *id;
    twtk_menu_entry_t *entries;
    int refcnt;
    int count;
};

#define TWTK_DECLARE_MENU_STATIC(_symbol, _id, _entries) \
    twtk_menu_t _symbol = { \
        .id = _id, \
        .entries = _entries, \
        .count = TWTK_ARRAY_SIZE(_entries) \
    };

#define TWTK_DECLARE_MENU_STATIC_ENTRY(_id, _text) \
    { \
        .id = _id, \
        .text = _text \
    }

#define TWTK_DECLARE_MENU_STATIC_SUBMENU(_id, _text, _menu) \
    { \
        .id = _id, \
        .text = _text, \
        .sub = _menu \
    }

int twtk_menu_destroy(twtk_menu_t *menu);

static inline void twtk_menu_unref(twtk_menu_t *menu)
{
    if (menu == NULL)
        return;

    /** refcnt < 0 means static / never free it **/
    if (menu->refcnt < 0)
        return;

    menu->refcnt--;

    if (menu->refcnt == 1)
        twtk_menu_destroy(menu);
}

static inline twtk_menu_t *twtk_menu_ref(twtk_menu_t *menu)
{
    if (menu == NULL)
        return NULL;

    if (menu->refcnt < 0)
        return menu;

    menu->refcnt++;

    return menu;
}

#endif /* __TWTK_MENU_H_ */
