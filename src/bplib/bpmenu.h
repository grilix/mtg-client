#ifndef BPLIB_BPMENU_H
#define BPLIB_BPMENU_H

struct _BpMenuItem
{
  char *title;
  int value;
};

typedef struct _BpMenuItem BpMenuItem;

  extern BpMenuItem *
bp_show_menu(char *title, BpMenuItem **options, int options_count,
             int sizex, int sizey, int x, int y);

#endif
