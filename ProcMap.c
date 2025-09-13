#include "ProcMap.h"
#include <linux/string.h>
#include <linux/slab.h>

ProcMap new_pm() {
  ProcMap pm = {0};
  return pm;
}

ProcMapEntry new_pme() {
  ProcMapEntry pme = {0};
  return pme;
}

ErrorCode pm_add(ProcMap *pm, char *msg, int msglen, pid_t receiver, pid_t sender) {
  ProcMapEntry *new_pme = (ProcMapEntry *) kcalloc(1, sizeof (ProcMapEntry), GFP_KERNEL);

  int len = (msglen <= MAX_MSG_LEN) ? msglen : MAX_MSG_LEN;
  
  new_pme->receiver = receiver;
  new_pme->sender = sender;
  new_pme->msglen = len;
  memcpy(new_pme->msg, (void *) msg, len);

  ProcMapEntry *cur_pme = pm->entries[receiver % TABLE_SIZE];
  if (cur_pme == NULL) {
    pm->entries[receiver % TABLE_SIZE] = new_pme;
  } else {
    while (cur_pme->next != NULL) {
      cur_pme = cur_pme->next;
    }
    cur_pme->next = new_pme;
  }
  
  return Ok;
}

ErrorCode pm_consume(ProcMap *pm, char *msg, int max_msglen, pid_t receiver) {
  ProcMapEntry *cur_pme = pm->entries[receiver % TABLE_SIZE];
  ProcMapEntry *pre_pme = NULL;
  
  if (cur_pme == NULL) return NoMessageAvailable;

  while ((cur_pme != NULL) && (cur_pme->receiver != receiver)) {
    pre_pme = cur_pme;
    cur_pme = cur_pme->next;
  }

  if (cur_pme->receiver == receiver) {
    max_msglen = (max_msglen > cur_pme->msglen) ? (cur_pme->msglen) : max_msglen;
    memcpy(msg, (void *) cur_pme->msg, max_msglen);

    if (pre_pme == NULL) {
      ProcMapEntry *nex_pme = cur_pme->next;
      kfree(cur_pme);
      pm->entries[receiver % TABLE_SIZE] = nex_pme;
    } else {
      pre_pme->next = cur_pme->next;
      kfree(cur_pme);
    }
  } else {
    return NoMessageAvailable;
  }

  return Ok;
}
