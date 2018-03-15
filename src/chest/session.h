#ifndef SESSION_H
#define SESSION_H

struct _Session
{
  char *token;
};

typedef struct _Session Session;

  extern Session *
session_allocate(void);

  extern void
session_destroy(Session *session);

  extern int
session_extract_header(Session *session, char **headers);

  extern char *
session_create_header(Session *session);

  extern void
session_load(Session *session, const char *filename);

  extern void
session_save(Session *session, const char *filename);

  extern void
session_drop(Session *session);

#endif
