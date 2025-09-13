#ifndef PROCMAP_H
#define PROCMAP_H

#include <linux/types.h>
#include <linux/stddef.h>

#define TABLE_SIZE 1024
#define MAX_MSG_LEN 1024

typedef enum error_code {
  Ok = 0,
  NoMessageAvailable = -1,
} ErrorCode;

typedef struct process_map_entry {
  pid_t receiver;
  pid_t sender;
  int msglen;
  char msg[MAX_MSG_LEN];
  struct process_map_entry *next;
} ProcMapEntry;

typedef struct process_map {
  ProcMapEntry *entries[TABLE_SIZE];
} ProcMap;

ProcMap new_pm(void);
ProcMapEntry new_pme(void);
ErrorCode pm_add(ProcMap *pm, char* msg, int msglen, pid_t receiver, pid_t sender);
ErrorCode pm_consume(ProcMap *pm, char* msg, int msglen, pid_t receiver);

#endif
