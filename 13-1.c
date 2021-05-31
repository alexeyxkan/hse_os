#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

int create(char* name){
    FILE* fd = fopen(name, "w");
    if (fd == NULL) {
        printf("Can\'t create file\n");
        return 0;
    }

    fclose(fd);
    return 1;
}


int main(int argc, char* argv[]){
    char* directory = "dir";

    struct stat st = { 0 };

    if (stat(directory, &st) == -1) {
        printf("Directory doesn\'t exists. Try to create new\n");
        if (mkdir(directory, 0700)) {
            printf("Can\'t create dir\n");
            exit(-1);
        }
    }

    DIR *dir = opendir(directory);

    if(dir == NULL){
        printf("Can\'t open directory\n");
        exit(1);
    }

    char* filename = malloc(sizeof("999"));
    sprintf(filename, "dir/%03d", 0);

    if (!create(filename)){
        printf("Can\'t open file\n");
        exit(1);
    }

    int count = 1;

    while(1) {
        char* filename1 = malloc(sizeof("999"));
        sprintf(filename1, "%03d", count - 1);
        char* prev = filename1;

        char* filename2 = malloc(sizeof("999"));
        sprintf(filename2, "dir/%03d", count);
        char* current = filename2;

        int link = symlink(prev, current);

        if(link) exit(-1);

        if (fopen(current, "r") == NULL) {
            printf("Recursion: %d\n", count);
            if (closedir(dir) != 0) {
                printf("Error closing dir!\n");
                return(-1);
            }

            exit(1);
        }

        free(prev);
        free(current);

        count++;
    }

    return 0;
}