#include <stdint.h>
#include <string.h>
#include <pthread.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>

#include <linux/input-event-codes.h>

#include <kbd.h>
#include <kbd_sim_internal.h>

#define KBD_FIFO_SIZE 256

static keyboard_event_t fifo[KBD_FIFO_SIZE];

static unsigned int fifo_read;
static unsigned int fifo_write;
static unsigned int fifo_count;

/*
 * Used to generate value=2 when we receive another KeyPress
 * for a key which is already down.
 */
static uint8_t key_down[KEY_MAX + 1];

static pthread_mutex_t fifo_mutex =
    PTHREAD_MUTEX_INITIALIZER;

/*
 * ============================================================
 * X11 KeySym -> Linux input KEY_* code
 * ============================================================
 */

static uint16_t keysym_to_linux(unsigned long sym)
{
    switch (sym)
    {

    /*
     * Letters
     */
    case XK_a:
    case XK_A:
        return KEY_A;

    case XK_b:
    case XK_B:
        return KEY_B;

    case XK_c:
    case XK_C:
        return KEY_C;

    case XK_d:
    case XK_D:
        return KEY_D;

    case XK_e:
    case XK_E:
        return KEY_E;

    case XK_f:
    case XK_F:
        return KEY_F;

    case XK_g:
    case XK_G:
        return KEY_G;

    case XK_h:
    case XK_H:
        return KEY_H;

    case XK_i:
    case XK_I:
        return KEY_I;

    case XK_j:
    case XK_J:
        return KEY_J;

    case XK_k:
    case XK_K:
        return KEY_K;

    case XK_l:
    case XK_L:
        return KEY_L;

    case XK_m:
    case XK_M:
        return KEY_M;

    case XK_n:
    case XK_N:
        return KEY_N;

    case XK_o:
    case XK_O:
        return KEY_O;

    case XK_p:
    case XK_P:
        return KEY_P;

    case XK_q:
    case XK_Q:
        return KEY_Q;

    case XK_r:
    case XK_R:
        return KEY_R;

    case XK_s:
    case XK_S:
        return KEY_S;

    case XK_t:
    case XK_T:
        return KEY_T;

    case XK_u:
    case XK_U:
        return KEY_U;

    case XK_v:
    case XK_V:
        return KEY_V;

    case XK_w:
    case XK_W:
        return KEY_W;

    case XK_x:
    case XK_X:
        return KEY_X;

    case XK_y:
    case XK_Y:
        return KEY_Y;

    case XK_z:
    case XK_Z:
        return KEY_Z;

    /*
     * Number row
     */
    case XK_0:
        return KEY_0;

    case XK_1:
        return KEY_1;

    case XK_2:
        return KEY_2;

    case XK_3:
        return KEY_3;

    case XK_4:
        return KEY_4;

    case XK_5:
        return KEY_5;

    case XK_6:
        return KEY_6;

    case XK_7:
        return KEY_7;

    case XK_8:
        return KEY_8;

    case XK_9:
        return KEY_9;

    /*
     * Main control keys
     */
    case XK_Escape:
        return KEY_ESC;

    case XK_Return:
        return KEY_ENTER;

    case XK_space:
        return KEY_SPACE;

    case XK_Tab:
        return KEY_TAB;

    case XK_BackSpace:
        return KEY_BACKSPACE;

    /*
     * Modifiers
     */
    case XK_Shift_L:
        return KEY_LEFTSHIFT;

    case XK_Shift_R:
        return KEY_RIGHTSHIFT;

    case XK_Control_L:
        return KEY_LEFTCTRL;

    case XK_Control_R:
        return KEY_RIGHTCTRL;

    case XK_Alt_L:
        return KEY_LEFTALT;

    case XK_Alt_R:
        return KEY_RIGHTALT;

    case XK_Super_L:
    case XK_Meta_L:
        return KEY_LEFTMETA;

    case XK_Super_R:
    case XK_Meta_R:
        return KEY_RIGHTMETA;

    case XK_Caps_Lock:
        return KEY_CAPSLOCK;

    /*
     * Cursor/navigation
     */
    case XK_Left:
        return KEY_LEFT;

    case XK_Right:
        return KEY_RIGHT;

    case XK_Up:
        return KEY_UP;

    case XK_Down:
        return KEY_DOWN;

    case XK_Home:
        return KEY_HOME;

    case XK_End:
        return KEY_END;

    case XK_Page_Up:
        return KEY_PAGEUP;

    case XK_Page_Down:
        return KEY_PAGEDOWN;

    case XK_Insert:
        return KEY_INSERT;

    case XK_Delete:
        return KEY_DELETE;

    /*
     * Function keys
     */
    case XK_F1:
        return KEY_F1;

    case XK_F2:
        return KEY_F2;

    case XK_F3:
        return KEY_F3;

    case XK_F4:
        return KEY_F4;

    case XK_F5:
        return KEY_F5;

    case XK_F6:
        return KEY_F6;

    case XK_F7:
        return KEY_F7;

    case XK_F8:
        return KEY_F8;

    case XK_F9:
        return KEY_F9;

    case XK_F10:
        return KEY_F10;

    case XK_F11:
        return KEY_F11;

    case XK_F12:
        return KEY_F12;

    /*
     * Punctuation
     */
    case XK_grave:
    case XK_asciitilde:
        return KEY_GRAVE;

    case XK_minus:
    case XK_underscore:
        return KEY_MINUS;

    case XK_equal:
    case XK_plus:
        return KEY_EQUAL;

    case XK_bracketleft:
    case XK_braceleft:
        return KEY_LEFTBRACE;

    case XK_bracketright:
    case XK_braceright:
        return KEY_RIGHTBRACE;

    case XK_backslash:
    case XK_bar:
        return KEY_BACKSLASH;

    case XK_semicolon:
    case XK_colon:
        return KEY_SEMICOLON;

    case XK_apostrophe:
    case XK_quotedbl:
        return KEY_APOSTROPHE;

    case XK_comma:
    case XK_less:
        return KEY_COMMA;

    case XK_period:
    case XK_greater:
        return KEY_DOT;

    case XK_slash:
    case XK_question:
        return KEY_SLASH;

    /*
     * Keypad
     */
    case XK_KP_0:
        return KEY_KP0;

    case XK_KP_1:
        return KEY_KP1;

    case XK_KP_2:
        return KEY_KP2;

    case XK_KP_3:
        return KEY_KP3;

    case XK_KP_4:
        return KEY_KP4;

    case XK_KP_5:
        return KEY_KP5;

    case XK_KP_6:
        return KEY_KP6;

    case XK_KP_7:
        return KEY_KP7;

    case XK_KP_8:
        return KEY_KP8;

    case XK_KP_9:
        return KEY_KP9;

    case XK_KP_Add:
        return KEY_KPPLUS;

    case XK_KP_Subtract:
        return KEY_KPMINUS;

    case XK_KP_Multiply:
        return KEY_KPASTERISK;

    case XK_KP_Divide:
        return KEY_KPSLASH;

    case XK_KP_Decimal:
        return KEY_KPDOT;

    case XK_KP_Enter:
        return KEY_KPENTER;

    default:
        return KEY_RESERVED;
    }
}

/*
 * ============================================================
 * FIFO
 * ============================================================
 */

static void push_event(uint16_t key, int value)
{
    pthread_mutex_lock(&fifo_mutex);

    /*
     * If FIFO is full, drop newest event.
     */
    if (fifo_count < KBD_FIFO_SIZE)
    {

        fifo[fifo_write].key = key;
        fifo[fifo_write].value = value;

        fifo_write =
            (fifo_write + 1) % KBD_FIFO_SIZE;

        fifo_count++;
    }

    pthread_mutex_unlock(&fifo_mutex);
}

/*
 * ============================================================
 * Called by vdp_sim.c when an X11 keyboard event occurs.
 * ============================================================
 */

void keyboard_sim_x11_event(unsigned long keysym,
                            int pressed)
{
    uint16_t key;

    key = keysym_to_linux(keysym);

    if (key == KEY_RESERVED)
        return;

    if (key > KEY_MAX)
        return;

    if (pressed)
    {

        /*
         * Match Linux EV_KEY semantics:
         *
         * 1 = initial press
         * 2 = autorepeat
         */
        if (key_down[key])
            push_event(key, 2);
        else
        {
            key_down[key] = 1;
            push_event(key, 1);
        }
    }
    else
    {

        if (key_down[key])
        {
            key_down[key] = 0;
            push_event(key, 0);
        }
    }
}

/*
 * ============================================================
 * Public API
 * ============================================================
 */

int keyboard_init(void)
{
    pthread_mutex_lock(&fifo_mutex);

    fifo_read = 0;
    fifo_write = 0;
    fifo_count = 0;

    memset(key_down, 0, sizeof(key_down));

    pthread_mutex_unlock(&fifo_mutex);

    return 0;
}

int keyboard_get_event(keyboard_event_t *event)
{
    if (!event)
        return -1;

    pthread_mutex_lock(&fifo_mutex);

    if (fifo_count == 0)
    {
        pthread_mutex_unlock(&fifo_mutex);
        return 0;
    }

    *event = fifo[fifo_read];

    fifo_read =
        (fifo_read + 1) % KBD_FIFO_SIZE;

    fifo_count--;

    pthread_mutex_unlock(&fifo_mutex);

    return 1;
}