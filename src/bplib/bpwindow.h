#ifndef BPLIB_BPWINDOW_H
#define BPLIB_BPWINDOW_H

enum _BpWindowStatus
{
  /*
   * The user is interacting with this window
   */
  BP_WINDOW_STATUS_LOOPING,
  /*
   * The user finished interacting with this window
   */
  BP_WINDOW_STATUS_COMPLETE,
};

typedef enum _BpWindowStatus BpWindowStatus;

struct _BpWindow
{
  BpWindowStatus status;

  void *widget;
  //BpWidget *widget;

  void (*_custom_driver)(struct _BpWindow *window, int ch);
  WINDOW *_window;
};

typedef struct _BpWindow BpWindow;

typedef void (*BpInputDriver)(struct _BpWindow *window, int ch);

  extern void
bp_window_set_custom_driver(BpWindow *window, BpInputDriver driver);

  extern void
bp_window_loop(BpWindow *window);

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
