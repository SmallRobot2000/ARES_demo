

#ifndef ARES_KEYBOARD_H
#define ARES_KEYBOARD_H

#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <linux/input.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        uint16_t key;

        /*
         * Linux EV_KEY values:
         *
         * 0 = released
         * 1 = pressed
         * 2 = autorepeat
         */
        int value;

    } keyboard_event_t;

    /*
     * Initialize keyboard interface.
     *
     * Returns:
     *   0  success
     *  -1  error
     */
    int keyboard_init(void);

    /*
     * Non-blocking keyboard event read.
     *
     * Returns:
     *   1  event returned
     *   0  no event available
     *  -1  error
     */
    int keyboard_get_event(keyboard_event_t *event);

#ifdef __cplusplus
}
#endif

#endif