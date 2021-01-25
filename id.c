#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main() {
	printf("User ID: %d\n", getuid());
	printf("Group ID: %d\n", getgid());

	return 0;
}
