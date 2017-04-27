
#define _GNU_SOURCE
#define _POSIX_C_SOURCE 199309

#include <assert.h>
#include <signal.h>
#include <time.h>
#include <twtk/platform.h>
#include <twtk/color.h>

#define TIMER_SIG	SIGRTMIN
#define TIMER_INTERVAL	3

timer_t timerid;
int step = 0;

/** tick handler for switching the colors **/
static void tick_handler(union sigval sv)
{
    twtk_color_t c;
    switch (++step) {
        case 1:  c = TWTK_COLOR_RED;               break;
        case 2:  c = TWTK_COLOR_GREEN;             break;
        case 3:  c = TWTK_COLOR_BLUE;              break;
        default: c = TWTK_COLOR_BLACK;   step = 0; break;
    }

    twtk_widget_set_background_color(twtk_platform_get_root(), c);
    twtk_platform_redraw();
}

static void setup_timer()
{
    struct sigevent sev = {
        .sigev_notify = SIGEV_THREAD,
        .sigev_value.sival_ptr = &timerid,
        .sigev_notify_function = tick_handler
    };

    struct itimerspec its = {
        .it_value.tv_sec     = 0,
        .it_value.tv_nsec    = 1,
        .it_interval.tv_sec  = TIMER_INTERVAL,
        .it_interval.tv_nsec = 0
    };

    timer_create(CLOCK_REALTIME, &sev, &timerid);
    timer_settime(timerid, 0, &its, NULL);
}

int main(void)
{
    twtk_platform_setup();

    setup_timer();

    twtk_platform_redraw();
    twtk_platform_loop();
    twtk_platform_finish();

    return 0;
}
