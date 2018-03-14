#ifndef BPLIB_BPFORM_H
#define BPLIB_BPFORM_H

#include "bpwindow.h"
#include "form.h"

/*
 * The user sent the form and the operation was successful.
 */
#define BP_FORM_OK 1
/*
 * The user cancelled the form.
 */
#define BP_FORM_CANCEL 2
/*
 * The user sent the form but the operation failed.
 */
#define BP_FORM_ERR 3

enum BpFormFieldType
{
  BP_FORM_FIELD_TYPE_INPUT,
  BP_FORM_FIELD_TYPE_PASSWORD,
  BP_FORM_FIELD_TYPE_LABEL
};

struct _BpFormField
{
  enum BpFormFieldType type;
  char *value;
};

typedef struct _BpFormField BpFormField;

struct _BpForm
{
  int sizex, sizey, x, y;

  /*
   * Internal: Holds the form fields.
   */
  BpFormField **_form_fields;
  /*
   * Internal: Holds the created form.
   */
  FORM *_form;
  /*
   * Internal: Holds the created fields.
   */
  FIELD **_fields;
  /*
   * Internal: Holds the created window for the form body.
   */
  WINDOW *_body_win;
  /*
   * Internal: Holds the created main window.
   */
  BpWindow *_main_win;
};

typedef struct _BpForm BpForm;

/*
 * Creates a form and renders it.
 */
  extern BpForm *
bp_form_create(char *title, BpFormField **form_fields, int field_count,
              int sizex, int sizey, int x, int y);

/*
 * Handles the user input on a form.
 *
 * After a form is created, the user input must be handled.
 *
 * This function waits for user input and handles it using
 * a common form driver.
 *
 */
  extern int
bp_form_loop(BpForm *form);

/*
 * Store the form values back into the form_fields.
 *
 * Use this to obtain user input.
 *
 */
  extern void
bp_form_sync_input(BpForm *form);

  extern void
bp_form_update_fields(BpForm *form, BpFormField **fields, int field_count);

/*
 * Frees the memory allocated for form structure.
 *
 *   form = bp_form_create(...);
 *   // ...
 *   bp_form_destroy(form);
 *
 */
  extern void
bp_form_destroy(BpForm *form);

/*
 * Handles the whole form lifecyle, from creation, looping,
 * storing using input back to the given fields and then
 * freeing the allocated memory.
 *
 * Returns the form result which indicates the user intention.
 */
  extern int
bp_show_form(char *title, BpFormField **fields, int field_count,
             int sizex, int sizey, int x, int y);

/*
 * Sets the initial value for a form field.
 */
  extern void
bp_form_field_set_value(BpFormField *field, char *value);

  extern BpFormField *
bp_form_field_create(enum BpFormFieldType type, char *value);

  extern void
bp_form_field_destroy(BpFormField *field);

#endif
