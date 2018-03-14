#ifndef BPLIB_BPWINDOW_H
#define BPLIB_BPWINDOW_H

/*
 * Just wrap ncurses' WINDOW for now.
 */
typedef WINDOW BpWindow;

/*
 * Creates a window with a title.
 */
  extern BpWindow *
bp_window_create_frame(char *title, int sizex, int sizey, int x, int y);

/*
 * Creates a simple bordered box window.
 */
  extern BpWindow *
bp_window_create_box(int sizex, int sizey, int x, int y);

/*
 * Draws a frame for a window with a title.
 */
  extern void
bp_window_draw_frame(char *title, BpWindow *window, int sizex);

/*
 * Destroys a window but doesn't clear its contents.
 *
 */
  extern void
bp_window_destroy(BpWindow *window);

/*
 * Destroys a window and clears its contents.
 */
  extern void
bp_window_destroy_clear(BpWindow *window);

#endif
