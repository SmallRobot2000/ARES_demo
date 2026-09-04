#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <kbd.h>
#include <vdp.h>
// Bok
#include <app.h>

int main(void)
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

    start();
    while (1)
    {
        loop();
    }

    return 0;
}