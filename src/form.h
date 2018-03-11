#ifndef SRC_FORM_H
#define SRC_FORM_H

enum FormFieldType
{
  FORM_FIELD_TYPE_INPUT,
  FORM_FIELD_TYPE_PASSWORD,
  FORM_FIELD_TYPE_LABEL
};

struct _FormField
{
  enum FormFieldType type;
  char *value;
};

typedef struct _FormField FormField;

  extern int
show_form(char *title, FormField **fields, int field_count,
          int sizex, int sizey, int x, int y);

  extern FormField *
form_field_create(enum FormFieldType type, char *value);

  extern void
form_field_destroy(FormField *field);

#endif
