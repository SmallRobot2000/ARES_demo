#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <kbd.h>
#include <vdp.h>
// Bok
#include <app.h>
#include <vdp_api.h>

int main(int argc, char **argv)
{
    if (vdp_init() != 0)
    {
        fprintf(stderr,
                "ERROR: UIO device 'vdp' not found\n");
        return 1;
    }

    if (keyboard_init() != 0)
    {
        fprintf(stderr,
                "ERROR: EVENT keyboard not found\n");
        vdp_close();
        return 1;
    }

    vdp_b0_disable_linux_mode();
    start(argc, argv);
    while (1)
    {
        loop();
    }

    return 0;
}