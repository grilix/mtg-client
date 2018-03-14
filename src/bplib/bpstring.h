#ifndef BPLIB_BPSTRING_H
#define BPLIB_BPSTRING_H

  extern int
bp_count_str(const char *str, const char *sub, int max);

  extern char **
bp_split_str(char *str, char *delim, int max);

/*
 * Joins an array of strings using a +glue+.
 *
 * Returned pointer must be freed.
 *
 * Without end glue:
 *
 * bpjoin_str({{"line 1"}, {"line 2"}}, "\n", 0)
 *   -> "line1\nline 2"
 *
 * With end glue:
 * bpjoin_str({{"line 1"}, {"line 2"}}, "\n", 1)
 *   -> "line1\nline 2\n"
 *
 */
  extern char *
bp_join_str(char **strings, char *glue, int glue_end);

#endif
