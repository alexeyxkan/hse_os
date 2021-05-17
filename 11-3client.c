#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
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

    clientbuf.mtype = 1;
    clientbuf.info.pid = getpid();
    
    printf("Enter the number: ");
    int temp;
    scanf("%d", &temp);
    len = sizeof(clientbuf.info);
    clientbuf.info.iinfo = temp;

    if (msgsnd(msqid, (struct sclientbuf *)&clientbuf, len, 0) < 0) {
        printf("Can\'t send message.\n");
        msgctl(msqid, IPC_RMID, (struct msqid_ds *) NULL);
        exit(-1);
    }

    printf("Client #%d send message to server: %d\n", clientbuf.info.pid, clientbuf.info.iinfo);

    max = sizeof(serverbuf.info);

    // Receive message from server.
    if (len = msgrcv(msqid, &serverbuf, max, getpid(), 0) < 0) {
        printf("Can\'t get message.\n");
        exit(-1);
    }

    printf("Server message: '%d'\n", serverbuf.info.iinfo);

    return 0;
}