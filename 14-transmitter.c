#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>

int receiver_pid;
int value_sent;
bool can_send = true;

void receiver_signal_handler(int nsig) {
	can_send = true;
}

void send_value() {
	int bits_count = sizeof(int) * 8;

	for (int i = 0; i < bits_count; ++i) {
		while (!can_send);
		if ((value_sent & (1 << i)) != 0) kill(receiver_pid, SIGUSR1);
		else kill(receiver_pid, SIGUSR2);
		can_send = false;
	}
	
	kill(receiver_pid, SIGCHLD);
}

int main() {
	signal(SIGUSR1, receiver_signal_handler);
	printf("PID of transmitter: %d\n", (int)getpid());
	
	// Читаем pid приемника.
	printf("Enter the PID for receiver:\n");
	if (scanf("%d", &receiver_pid) < 0) {
		printf("Reading pid error.\n");
		exit(-1);
	}
	
	// Ввод сообщения.
	printf("Enter a valid integer:\n");
	if (scanf("%d", &value_sent) < 0) {
		printf("Reading message (integer) error.\n");
		exit(-2);
	}
	
	send_value();

	printf("Succes.\n");

	return 0;
}