#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define PERMS 0644
struct my_msgbuf {
   long mtype;
   int intBuf;
};

int main(void) {
   struct my_msgbuf buf;
   int msqid;
   int toend;
   key_t key;
   int len;
   system("touch msgq.txt");
   int count = -1;

   if ((key = ftok("msgq.txt", 'B')) == -1) {
      perror("ftok");
      exit(1);
   }

   if ((msqid = msgget(key, PERMS | IPC_CREAT)) == -1) { /* connect to the queue */
      perror("msgget");
      exit(1);
   }

   printf("message queue: ready to receive messages.\n");

   for(;;) { /* normally receiving never ends but just to make conclusion */
             /* this program ends with string of end */
      if (count == 49)
         break;
      if (msgrcv(msqid, &buf, sizeof(buf.intBuf), 0, 0) == -1) {
         perror("msgrcv");
         exit(1);
      } else
      {
         count++;
      }
      printf("recvd: \"%d\"\n", buf.intBuf);
   }
   if (msgctl(msqid, IPC_RMID, NULL) == -1) {
      perror("msgctl");
      exit(1);
   }
   printf("message queue: done receiving messages.\n");
   system("rm msgq.txt");
   return 0;
}
