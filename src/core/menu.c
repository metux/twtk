
#include <errno.h>

#include <twtk/menu.h>

/** FIXME **/
int twtk_menu_destroy(twtk_menu_t *menu)
{
    if (menu == NULL)
        return -EFAULT;

    return 0;
}
