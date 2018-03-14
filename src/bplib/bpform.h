#ifndef BPLIB_BPFORM_H
#define BPLIB_BPFORM_H

#define BP_FORM_OK 1
#define BP_FORM_CANCEL 2
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

  extern int
bp_show_form(char *title, BpFormField **fields, int field_count,
             int sizex, int sizey, int x, int y);

  extern BpFormField *
bp_form_field_create(enum BpFormFieldType type, char *value);

  extern void
bp_form_field_destroy(BpFormField *field);

#endif
