#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"


#define MAXLINE 100

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(2, "Usage: xargs command [args...]\n");
        exit(1);
    }

    char buf[MAXLINE];
    int i = 0;

    while (read(0, &buf[i], 1) == 1) {
        if (buf[i] == '\n') { // End of a line
            buf[i] = 0; // Null-terminate
            
            char *cmd_args[MAXARG];
            for (int j = 0; j < argc - 1; j++) {
                cmd_args[j] = argv[j + 1];
            }
            cmd_args[argc - 1] = buf; // Append input line as an argument
            cmd_args[argc] = 0;
            
            // Fork and execute command
            if (fork() == 0) {
                exec(cmd_args[0], cmd_args);
                fprintf(2, "exec failed\n");
                exit(1);
            }
            wait(0); // Wait for child process
            i = 0; // Reset buffer index
        } else {
            i++;
        }
    }
    exit(0);
}
