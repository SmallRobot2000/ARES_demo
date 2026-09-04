#ifndef ARES_KEYBOARD_SIM_INTERNAL_H
#define ARES_KEYBOARD_SIM_INTERNAL_H

/*
 * Internal interface between the X11 VDP/window simulator
 * and keyboard_sim.c.
 *
 * main.c should NOT use this function.
 */
void keyboard_sim_x11_event(unsigned long keysym,
                            int pressed);

#endif