#include <signal.h>
#include <stdio.h>

void myhandler(int nsig) {
  if (nsig == SIGINT) { printf("ctrl+c"); }
  else if (nsig == SIGQUIT) { printf("ctrl+4"); }
}

int main(void) {
  //
  // Set the process response to the SIGINT signal
  //
  (void)signal(SIGINT, myhandler);
  (void)signal(SIGQUIT, myhandler);
  //
  // From this point, the process will print SIGINT message.
  //
  while(1);
  return 0;
}
