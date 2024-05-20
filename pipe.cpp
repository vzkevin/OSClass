#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
  int pipes[2];
  int r;

  // Create the pipe
  r = pipe(pipes);
  if (r < 0) {
    fprintf(stderr, "pipe failed\n\n");
    exit(2);
  }

  // Create first child (ps -aux)
  int pid1 = fork();
  if (pid1 < 0) {
    fprintf(stderr, "fork failed\n\n");
    exit(3);
  } else if (pid1 == 0) {
    // Child 1: Redirect stdout to write end of pipe and execute "ps -aux"
    dup2(pipes[1], STDOUT_FILENO);  // Redirect stdout to pipe
    close(pipes[0]);                 // Close unused read end
    close(pipes[1]);                 // Close write end after dup2

    execlp("ps", "ps", "-aux", NULL);  // Execute "ps -aux"
    fprintf(stderr, "execlp failed\n\n");  // Error handling if execlp fails
    exit(4);
  }

  // Create second child (sort -r -n -k 5)
  int pid2 = fork();
  if (pid2 < 0) {
    fprintf(stderr, "fork failed\n\n");
    exit(3);
  } else if (pid2 == 0) {
    // Child 2: Redirect stdin from read end of pipe and execute "sort"
    dup2(pipes[0], STDIN_FILENO);  // Redirect stdin from pipe
    close(pipes[1]);                 // Close unused write end
    close(pipes[0]);                 // Close read end after dup2

    execlp("sort", "sort", "-r", "-n", "-k", "5", NULL);  // Sort by memory usage (-k 5)
    fprintf(stderr, "execlp failed\n\n");                // Error handling
    exit(4);
  }

  // Close unused pipe ends in parent
  close(pipes[0]);
  close(pipes[1]);

  // Wait for child processes to finish
  waitpid(pid1, NULL, 0);
  waitpid(pid2, NULL, 0);

  return 0;
}
