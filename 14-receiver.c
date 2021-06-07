#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>

int transmitter_pid;
int current_bit_index = 0;
int result_number = 0;
bool received_value = false;

void sigusr1_handler_bit(int nsig) {
	result_number |= 1 << current_bit_index++;
	kill(transmitter_pid, SIGUSR1);
}

void sigusr2_handler_nobit(int nsig) {
	current_bit_index++;
	kill(transmitter_pid, SIGUSR1);
}

void sigchld_handler(int nsig) {
	received_value = true;
}

int main() {
	printf("PID of receiver: %d\n", (int)getpid());
	printf("Enter the PID for transmitter:");
	
    // Читаем pid передатчика.
	scanf("%d", &transmitter_pid);
	
	printf("Waiting...\n");

	signal(SIGUSR1, sigusr1_handler_bit);
	signal(SIGUSR2, sigusr2_handler_nobit);
	signal(SIGCHLD, sigchld_handler);
	
	// Ожидаем получения сообщения.
	while (!received_value);
	
	printf("Message: %d\n", result_number);
    
	return 0;
}