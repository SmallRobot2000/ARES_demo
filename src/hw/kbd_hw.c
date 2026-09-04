#include <kbd.h>

static int keyboard_fd = -1;

/*
 * Find Linux input event device by its registered name.
 *
 * Looks for:
 *     "ARES MSX Keyboard"
 *
 * Returns open fd, or -1.
 */
static int find_keyboard(const char *wanted_name)
{
    char dev[64];
    char name[256];

    for (int i = 0; i < 32; i++)
    {

        snprintf(dev, sizeof(dev),
                 "/dev/input/event%d", i);

        int fd = open(dev, O_RDONLY | O_NONBLOCK);

        if (fd < 0)
            continue;

        memset(name, 0, sizeof(name));

        if (ioctl(fd,
                  EVIOCGNAME(sizeof(name)),
                  name) >= 0)
        {

            if (strcmp(name, wanted_name) == 0)
            {

                printf("Keyboard found: %s (%s)\n",
                       dev, name);

                return fd;
            }
        }

        close(fd);
    }

    return -1;
}

int keyboard_init(void)
{
    keyboard_fd =
        find_keyboard("ARES MSX Keyboard");

    if (keyboard_fd < 0)
    {
        fprintf(stderr,
                "ERROR: ARES MSX Keyboard not found\n");
        return -1;
    }

    return 0;
}

/*
 * Returns:
 *
 *  1 = keyboard event received
 *  0 = no event waiting
 * -1 = error
 */
int keyboard_get_event(keyboard_event_t *event)
{
    struct input_event ev;

    if (keyboard_fd < 0)
        return -1;

    while (1)
    {

        ssize_t n =
            read(keyboard_fd, &ev, sizeof(ev));

        if (n < 0)
        {
            /*
             * O_NONBLOCK:
             * no event currently available.
             */
            return 0;
        }

        if (n != sizeof(ev))
            return -1;

        /*
         * Ignore EV_SYN, EV_LED, etc.
         */
        if (ev.type != EV_KEY)
            continue;

        event->key = ev.code;
        event->value = ev.value;

        return 1;
    }
}