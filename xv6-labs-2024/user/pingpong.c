#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
  int pipe_parent_to_child[2]; // Pipe for communication: Parent -> Child
  int pipe_child_to_parent[2]; // Pipe for communication: Child -> Parent
  char buf[1]; // Buffer to hold a single byte
  int pid;

  // Create the pipes
  pipe(pipe_parent_to_child);
  pipe(pipe_child_to_parent);

  // Fork the process
  pid = fork();

  if (pid == 0) { // Child process
    close(pipe_parent_to_child[1]); // Close unused write end of Parent -> Child pipe
    close(pipe_child_to_parent[0]); // Close unused read end of Child -> Parent pipe

    // Read a byte from the Parent -> Child pipe
    if (read(pipe_parent_to_child[0], buf, 1) == 1) {
      printf("%d: received ping\n", getpid()); 
      write(pipe_child_to_parent[1], buf, 1); // Write the byte to Child -> Parent pipe
    }

    // Close used pipe ends
    close(pipe_parent_to_child[0]);
    close(pipe_child_to_parent[1]);

    exit(0); 
  }
  else if (pid > 0) { // Parent process
    close(pipe_parent_to_child[0]); // Close unused read end of Parent -> Child pipe
    close(pipe_child_to_parent[1]); // Close unused write end of Child -> Parent pipe

    buf[0] = 'x'; 
    write(pipe_parent_to_child[1], buf, 1); // Write the byte to Parent -> Child pipe

    // Read the byte back from the Child -> Parent pipe
    if (read(pipe_child_to_parent[0], buf, 1) == 1) {
      printf("%d: received pong\n", getpid()); // Print "received pong" with parent's PID
    }

    // Close used pipe ends
    close(pipe_parent_to_child[1]);
    close(pipe_child_to_parent[0]);

    exit(0); 
  }
  else { 
    fprintf(2, "fork failed\n");
    exit(1);
  }
}
