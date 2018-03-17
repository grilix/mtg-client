#ifndef BPLIB_BPWINDOW_H
#define BPLIB_BPWINDOW_H

/*
 * Widgets are also user interactions by themselves,
 * so this represents the status of a widget, which
 * can then transfered to the window.
 *
 * A looping window would have a looping widget, when
 * the widget is done looping, the application will
 * then process a result which could result in the
 * widget to go back to loop, or the window to
 * become complete.
 */
enum _BpWindowStatus
{
  /*
   * The user is interacting with this window
   */
  BP_WINDOW_STATUS_LOOPING,
  /*
   * The user input is waiting to be processed.
   */
  BP_WINDOW_STATUS_PROCESSING,
  /*
   * The user finished interacting with this window
   */
  BP_WINDOW_STATUS_COMPLETE,
};

typedef enum _BpWindowStatus BpWindowStatus;

struct _BpWindow
{
  BpWindowStatus status;

  WINDOW *_window;
};

typedef struct _BpWindow BpWindow;

  extern int
bp_window_getch(BpWindow *window);

  extern bool
bp_window_driver(BpWindow *window, int ch);

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
