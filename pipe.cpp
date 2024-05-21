#include <stdio.h>
#include <string.h> // for strlen
#include <stdlib.h> // for exit
#include <unistd.h>  
#include <sys/wait.h>  



int main() {
  int pipes[2];
  int r;
  // Create our pipe
  r = pipe(pipes);
  if (r < 0) {
    fprintf(stderr, "pipe failed\n\n");
    // stderr is a FILE* variable for the standard error file (terminal)
    exit(2);
  }
  // use pipes[0] to read and pipes[1] to write

  // Create first child for ps -aux
  int pid = fork();
  if (pid < 0) {
    fprintf(stderr, "fork failed\n\n");
    exit(3);
  } else if (pid == 0) {
    // Child 1
    // We redirects stdout to write end of pipe and execute ps -aux
    dup2(pipes[1], STDOUT_FILENO);  
    // Close unused read end and close write end
    close(pipes[0]);
    close(pipes[1]);

    // Execute command
    execlp("ps", "ps", "-aux", NULL);
    exit(4);
  }

  // Create second child sort -r -n -k 5
  int pid2 = fork();
  if (pid2 < 0) {
    fprintf(stderr, "fork failed\n\n");
    exit(3);
  } else if (pid2 == 0) {
    // We redirect stdin from read end of pipe and execute sort
    dup2(pipes[0], STDIN_FILENO);  
    // Close unused write end and close read end
    close(pipes[1]);                 
    close(pipes[0]);                 

    // Execute command
    execlp("sort", "sort", "-r", "-n", "-k", "5", NULL);  
    exit(4);
  }

  // Close unused pipe ends
  close(pipes[0]);
  close(pipes[1]);

  // Wait for child processes
  waitpid(pid1, NULL, 0);
  waitpid(pid2, NULL, 0);

  return 0;
}
