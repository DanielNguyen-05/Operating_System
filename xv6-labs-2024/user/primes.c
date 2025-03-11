#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void primes(int num_pipe) __attribute__((noreturn));

void primes(int num_pipe){
    int prime;

    if(read(num_pipe, &prime, sizeof(prime)) == 0){
        close(num_pipe);
        exit(0);
    }

    printf("prime %d \n", prime);

    int next_pipe[2];
    pipe(next_pipe);

    if(fork() == 0){
        close(next_pipe[1]);
        close(num_pipe);
        primes(next_pipe[0]);
    }else{
        close(next_pipe[0]);

        int n;
        while(read(num_pipe, &n, sizeof(n)) > 0){
            if(n % prime != 0) write(next_pipe[1], &n, sizeof(n));
        }

        close(num_pipe);
        close(next_pipe[1]);
        wait(0);
    }
    exit(0);
}


int main (){
    int pipe_root[2];
    pipe(pipe_root);

    if(fork() == 0){
        close(pipe_root[1]);
        primes(pipe_root[0]);
    }else{
        close(pipe_root[0]);
        for(int i = 2; i <= 280; i ++){
            write(pipe_root[1], &i, sizeof(i));
        }
        close(pipe_root[1]);
        wait(0);
    }
    exit(0);
}