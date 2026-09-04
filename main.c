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

#define SCREEN_WIDTH   640
#define SCREEN_HEIGHT  480


volatile uint32_t *r0; //registers
volatile uint16_t *b0; //bitmap
 
volatile uint16_t *t0_map; //Tile Map 0
volatile uint8_t *t0_data; //Tile Data 0

volatile uint16_t *t1_map; //Tile Map 1
volatile uint8_t *t1_data; //TIle Date 1

volatile uint64_t *s0_att; //Sprite attributes
volatile uint8_t *s0_data; //Sprite data

size_t b0_size;
size_t r0_size;
size_t t0_map_size;
size_t t0_data_size;
size_t t1_map_size;
size_t t1_data_size;
size_t s0_att_size;
size_t s0_data_size;


typedef struct {
    uint16_t key;
    int value;
} keyboard_event_t;

static int find_uio(const char *wanted_name)
{
    DIR *dir;
    struct dirent *ent;

    dir = opendir("/sys/class/uio");
    if (!dir) {
        perror("/sys/class/uio");
        return -1;
    }

    while ((ent = readdir(dir)) != NULL) {

        int number;
        char path[128];
        char name[128];

        /*
         * Only accept names like:
         *
         * uio0
         * uio1
         * uio2
         * ...
         */
        if (sscanf(ent->d_name, "uio%d", &number) != 1)
            continue;

        /*
         * Build path from integer instead of ent->d_name.
         */
        snprintf(path, sizeof(path),
                 "/sys/class/uio/uio%d/name",
                 number);

        FILE *f = fopen(path, "r");

        if (!f)
            continue;

        if (fgets(name, sizeof(name), f)) {

            /*
             * Remove newline.
             */
            name[strcspn(name, "\r\n")] = '\0';

            if (strcmp(name, wanted_name) == 0) {
                fclose(f);
                closedir(dir);

                return number;
            }
        }

        fclose(f);
    }

    closedir(dir);

    return -1;
}


static int find_uio_map(int uio,
                        const char *wanted_name)
{
    char path[256];
    char name[128];

    for (int map = 0; map < 32; map++) {

        snprintf(path, sizeof(path),
                 "/sys/class/uio/uio%d/maps/map%d/name",
                 uio, map);

        FILE *f = fopen(path, "r");

        if (!f)
            continue;

        if (fgets(name, sizeof(name), f)) {

            name[strcspn(name, "\r\n")] = 0;

            if (strcmp(name, wanted_name) == 0) {
                fclose(f);
                return map;
            }
        }

        fclose(f);
    }

    return -1;
}


static void *map_uio(int fd,
                     int uio,
                     const char *name,
                     size_t *mapped_size)
{
    char path[256];
    unsigned long size;
    int map;
    long page_size;
    void *ptr;

    map = find_uio_map(uio, name);

    if (map < 0) {
        fprintf(stderr,
                "ERROR: UIO map '%s' not found\n",
                name);
        return MAP_FAILED;
    }

    snprintf(path, sizeof(path),
             "/sys/class/uio/uio%d/maps/map%d/size",
             uio, map);

    FILE *f = fopen(path, "r");

    if (!f) {
        perror(path);
        return MAP_FAILED;
    }

    if (fscanf(f, "%lx", &size) != 1) {
        fclose(f);
        fprintf(stderr,
                "ERROR: cannot read size of map '%s'\n",
                name);
        return MAP_FAILED;
    }

    fclose(f);

    page_size = sysconf(_SC_PAGESIZE);

    ptr = mmap(NULL,
               size,
               PROT_READ | PROT_WRITE,
               MAP_SHARED,
               fd,
               map * page_size);

    if (ptr == MAP_FAILED) {
        perror("mmap");
        return MAP_FAILED;
    }

    if (mapped_size)
        *mapped_size = size;

    printf("%s: map%d size=0x%lx\n",
           name, map, size);

    return ptr;
}


int vdp_init(void)
{
    char dev0[64];
    char dev1[64];

    int uio0;
    int uio1;

    int fd0 = -1;
    int fd1 = -1;


    /*
     * Start with all mappings invalid.
     */
    r0      = MAP_FAILED;
    b0      = MAP_FAILED;

    t0_map  = MAP_FAILED;
    t0_data = MAP_FAILED;

    t1_map  = MAP_FAILED;
    t1_data = MAP_FAILED;

    s0_att  = MAP_FAILED;
    s0_data = MAP_FAILED;


    /*
     * ------------------------------------------------------------
     * Find both VDP UIO devices
     * ------------------------------------------------------------
     */

    uio0 = find_uio("vdp0");

    if (uio0 < 0) {
        fprintf(stderr,
                "ERROR: UIO device 'vdp0' not found\n");
        goto error;
    }


    uio1 = find_uio("vdp1");

    if (uio1 < 0) {
        fprintf(stderr,
                "ERROR: UIO device 'vdp1' not found\n");
        goto error;
    }


    snprintf(dev0, sizeof(dev0),
             "/dev/uio%d", uio0);

    snprintf(dev1, sizeof(dev1),
             "/dev/uio%d", uio1);


    printf("VDP0 found: %s\n", dev0);
    printf("VDP1 found: %s\n", dev1);


    /*
     * ------------------------------------------------------------
     * Open both UIO devices
     * ------------------------------------------------------------
     */

    fd0 = open(dev0, O_RDWR);

    if (fd0 < 0) {
        perror(dev0);
        goto error;
    }


    fd1 = open(dev1, O_RDWR);

    if (fd1 < 0) {
        perror(dev1);
        goto error;
    }


    /*
     * ============================================================
     * VDP0
     * ============================================================
     *
     * map0 = r0
     * map1 = b0
     * map2 = t0_data
     * map3 = t0_map
     * map4 = s0_data
     */


    /*
     * R0
     */
    r0 = map_uio(fd0, uio0,
                 "r0",
                 &r0_size);

    if (r0 == MAP_FAILED)
        goto error;


    /*
     * B0
     */
    b0 = map_uio(fd0, uio0,
                 "b0",
                 &b0_size);

    if (b0 == MAP_FAILED)
        goto error;


    /*
     * T0 data
     */
    t0_data = map_uio(fd0, uio0,
                      "t0_data",
                      &t0_data_size);

    if (t0_data == MAP_FAILED)
        goto error;


    /*
     * T0 map
     */
    t0_map = map_uio(fd0, uio0,
                     "t0_map",
                     &t0_map_size);

    if (t0_map == MAP_FAILED)
        goto error;


    /*
     * S0 data
     */
    s0_data = map_uio(fd0, uio0,
                      "s0_data",
                      &s0_data_size);

    if (s0_data == MAP_FAILED)
        goto error;



    /*
     * ============================================================
     * VDP1
     * ============================================================
     *
     * map0 = s0_att
     * map1 = t1_data
     * map2 = t1_map
     */


    /*
     * S0 attributes
     */
    s0_att = map_uio(fd1, uio1,
                     "s0_att",
                     &s0_att_size);

    if (s0_att == MAP_FAILED)
        goto error;


    /*
     * T1 data
     */
    t1_data = map_uio(fd1, uio1,
                      "t1_data",
                      &t1_data_size);

    if (t1_data == MAP_FAILED)
        goto error;


    /*
     * T1 map
     */
    t1_map = map_uio(fd1, uio1,
                     "t1_map",
                     &t1_map_size);

    if (t1_map == MAP_FAILED)
        goto error;


    /*
     * mmap mappings remain valid after close().
     */
    close(fd0);
    close(fd1);

    fd0 = -1;
    fd1 = -1;


    printf("VDP mapped successfully\n");

    printf("  r0      : %p size 0x%zx\n",
           (void *)r0, r0_size);

    printf("  b0      : %p size 0x%zx\n",
           (void *)b0, b0_size);

    printf("  t0_map  : %p size 0x%zx\n",
           (void *)t0_map, t0_map_size);

    printf("  t0_data : %p size 0x%zx\n",
           (void *)t0_data, t0_data_size);

    printf("  t1_map  : %p size 0x%zx\n",
           (void *)t1_map, t1_map_size);

    printf("  t1_data : %p size 0x%zx\n",
           (void *)t1_data, t1_data_size);

    printf("  s0_att  : %p size 0x%zx\n",
           (void *)s0_att, s0_att_size);

    printf("  s0_data : %p size 0x%zx\n",
           (void *)s0_data, s0_data_size);

    return 0;


error:

    fprintf(stderr,
            "ERROR: failed to initialize VDP\n");


    /*
     * Unmap anything that succeeded.
     */

    if (s0_att != MAP_FAILED) {
        munmap((void *)s0_att, s0_att_size);
        s0_att = MAP_FAILED;
    }

    if (t1_data != MAP_FAILED) {
        munmap((void *)t1_data, t1_data_size);
        t1_data = MAP_FAILED;
    }

    if (t1_map != MAP_FAILED) {
        munmap((void *)t1_map, t1_map_size);
        t1_map = MAP_FAILED;
    }

    if (s0_data != MAP_FAILED) {
        munmap((void *)s0_data, s0_data_size);
        s0_data = MAP_FAILED;
    }

    if (t0_data != MAP_FAILED) {
        munmap((void *)t0_data, t0_data_size);
        t0_data = MAP_FAILED;
    }

    if (t0_map != MAP_FAILED) {
        munmap((void *)t0_map, t0_map_size);
        t0_map = MAP_FAILED;
    }

    if (b0 != MAP_FAILED) {
        munmap((void *)b0, b0_size);
        b0 = MAP_FAILED;
    }

    if (r0 != MAP_FAILED) {
        munmap((void *)r0, r0_size);
        r0 = MAP_FAILED;
    }


    /*
     * Close only descriptors that were actually opened.
     */
    if (fd1 >= 0)
        close(fd1);

    if (fd0 >= 0)
        close(fd0);

    return -1;
}

void vdp_close(void)
{
    if (s0_data != MAP_FAILED) {
        munmap((void *)s0_data, s0_data_size);
        s0_data = MAP_FAILED;
    }

    if (s0_att != MAP_FAILED) {
        munmap((void *)s0_att, s0_att_size);
        s0_att = MAP_FAILED;
    }

    if (t1_data != MAP_FAILED) {
        munmap((void *)t1_data, t1_data_size);
        t1_data = MAP_FAILED;
    }

    if (t1_map != MAP_FAILED) {
        munmap((void *)t1_map, t1_map_size);
        t1_map = MAP_FAILED;
    }

    if (t0_data != MAP_FAILED) {
        munmap((void *)t0_data, t0_data_size);
        t0_data = MAP_FAILED;
    }

    if (t0_map != MAP_FAILED) {
        munmap((void *)t0_map, t0_map_size);
        t0_map = MAP_FAILED;
    }

    if (b0 != MAP_FAILED) {
        munmap((void *)b0, b0_size);
        b0 = MAP_FAILED;
    }

    if (r0 != MAP_FAILED) {
        munmap((void *)r0, r0_size);
        r0 = MAP_FAILED;
    }
}

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

    for (int i = 0; i < 32; i++) {

        snprintf(dev, sizeof(dev),
                 "/dev/input/event%d", i);

        int fd = open(dev, O_RDONLY | O_NONBLOCK);

        if (fd < 0)
            continue;

        memset(name, 0, sizeof(name));

        if (ioctl(fd,
                  EVIOCGNAME(sizeof(name)),
                  name) >= 0) {

            if (strcmp(name, wanted_name) == 0) {

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

    if (keyboard_fd < 0) {
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

    while (1) {

        ssize_t n =
            read(keyboard_fd, &ev, sizeof(ev));

        if (n < 0) {
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

        event->key   = ev.code;
        event->value = ev.value;

        return 1;
    }
}

int main(void)
{   
    keyboard_event_t ev;


    if(vdp_init() != 0)
    {
        fprintf(stderr,
                "ERROR: UIO device 'vdp' not found\n");
        return 1;
    }
    if (keyboard_init() != 0) {
        fprintf(stderr,
                "ERROR: EVENT keyboard not found\n");
        vdp_close();
        return 1;
    }

    while (1) {

        if (keyboard_get_event(&ev) == 1) {

            printf("key=%u  ", ev.key);


            if (ev.value == 1)
                printf("DOWN\n");

            else if (ev.value == 0)
                printf("UP\n");

            else if (ev.value == 2)
                printf("REPEAT\n");


            if(ev.key == KEY_W)
            {
                for(int i = 0; i < SCREEN_WIDTH*SCREEN_HEIGHT; i++)
                {
                    b0[i] = 0x0000;
                }
            }

            if(ev.key == KEY_S)
            {
                for(int i = 0; i < SCREEN_WIDTH*SCREEN_HEIGHT; i++)
                {
                    b0[i] = 0xFFFF;
                }
            }

            uint64_t tmp;

/*
    wire [9:0]  x_pos    = spr_att_vdp_dout[9:0];   //10 bit
    wire [9:0]  y_pos    = spr_att_vdp_dout[25:16]; //10 bit
    wire [10:0] offset   = spr_att_vdp_dout[46:36]; //11 bit
    wire [1:0]  scale    = spr_att_vdp_dout[54:53]; //2 bit  ne postoji
    wire        size     = spr_att_vdp_dout[52];    //1 bit
    wire        v_flip   = spr_att_vdp_dout[51];    //1 bit
    wire        h_flip   = spr_att_vdp_dout[50];    //1 bit
    wire [1:0]  pall_num = spr_att_vdp_dout[49:48]; //2 bit
    wire        active   = spr_att_vdp_dout[63];    //1 bit

    s0_att

    s0_data

    uint64_t;




*/
            if(ev.key == KEY_D)
            {
                
                s0_att[0] += 1;


            }

            if(ev.key == KEY_A)
            {
                
                s0_att[0] -= 1;


            }
        }

        /*
         * Do other VDP/game work here.
         */

        
    }

    return 0;
}