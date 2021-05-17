#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int msqid;
    char pathname[] = "11-3server.c";

    key_t key;
    int max, len;

    // Server msg structure.
    struct sserverbuf {
        long mtype;
        struct {
            int iinfo;
        } info;
    } serverbuf;

    // Client msg structure.
    struct sclientbuf {
        long mtype;
        struct {
            int iinfo;
            pid_t pid;
        } info;
    } clientbuf;

    if ((key = ftok(pathname, 0)) < 0) {
        printf("Can\'t generate key.\n");
        exit(-1);
    }

    if ((msqid = msgget(key, 0666 | IPC_CREAT)) < 0) {
        printf("Can\'t get msqid.\n");
        exit(-1);
        
    }

    while (1) {
        max = sizeof(clientbuf.info);

        if ((len = msgrcv(msqid, (struct sclientbuf *) &clientbuf, max, 1, 0)) < 0) {
            printf("Can\'t get message.\n");
            exit(-1);
        }

        printf("Got message from #%d: '%d'\n", clientbuf.info.pid, clientbuf.info.iinfo);

        serverbuf.mtype = clientbuf.info.pid;
        serverbuf.info.iinfo = clientbuf.info.iinfo - 1;
        len = sizeof(serverbuf.info.iinfo);

        if (msgsnd(msqid, (struct sserverbuf *) &serverbuf, len, 0) < 0) {
            printf("Can\'t send message.\n");
            msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
            exit(-1);
        }
        
        printf("Send response.\n");
    }

    return 0;
}