#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#define PERMS 0644
struct my_msgbuf {
   long mtype;
   int intBuf;  // Integer to be recieved
};

int main(void) {
   struct my_msgbuf buf;
   int msqid;
   key_t key;
   system("touch msgq.txt");  // Creates file for msg queue to store info in
   int count = 0;

   if ((key = ftok("msgq.txt", 'B')) == -1) {
      perror("ftok");
      exit(1);
   }
   // Creates msg queue for sender to send to
   if ((msqid = msgget(key, PERMS | IPC_CREAT)) == -1) {
      perror("msgget");
      exit(1);
   }

   printf("message queue: ready to receive messages.\n");

   for (; count < 50;) {  // Loops until recieved 50 messages successfully
      // Waits for a message with size of a normal integer to arrive
      if (msgrcv(msqid, &buf, sizeof(buf.intBuf), 0, 0) == -1) {
         perror("msgrcv");
         exit(1);
      } else {
         count++;  // Increments count if message was recieved successfully
      }
      printf("recvd: %d\n", buf.intBuf);
   }
   if (msgctl(msqid, IPC_RMID, NULL) == -1) {
      perror("msgctl");
      exit(1);
   }
   printf("message queue: done receiving messages.\n");
   system("rm msgq.txt");  // Deletes file
   return 0;
}
