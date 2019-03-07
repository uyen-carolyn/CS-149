#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h> // from notes
#include <fcntl.h> 
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
// program that runs a command with multiple arguments and putting the results of each 
// argument into its own .out file (i.e: 1.out, 2.out, ... n.out)
// results include whether the program executed, its output, and its exit code
// will also respond to ^C and ^\ using sigaction()

pid_t cPID; // help from Jefferson Ly

//sigaction code from lecture video 3-4-19
static void handler(int sig, siginfo_t *si, void *ignore) {
    if (sig == SIGINT) {
        printf("Signalling %d\n", cPID); 
    }
    else if(sig == SIGQUIT) {
        printf("Signalling %d\n", pPID); 
        printf("Exiting due to quit signal\n"); 
        exit(0); 
    }
//    kill(wpid,SIGINT);
}

int main(int argc, char **argv) {
    int status;
    int fd; 
    
    // sigaction code from lecture video 3-4-19
    struct sigaction sa; 
    sa.sa_flags = SA_SIGINFO; 
    sigemptyset(&sa.sa_mask); 
    sa.sa_sigaction = handler;
    
    if(sigaction(SIGINT, &sa, NULL)) {
        perror("sigaction SIGINT"); 
        exit(errno); 
    }
   else if(sigaction(SIGQUIT, &sa, NULL)) {
        perror("sigaction SIGQUIT"); 
        exit(errno);
    } 
// props to dudeman for helping me set this up.  
    else {
        for(int i = 2; i < argc; i++) {
               // from discussion on snprintf
            int TEMP_BUFFER_SIZE = 100; 
            char fileName[TEMP_BUFFER_SIZE]; 
            snprintf(fileName,TEMP_BUFFER_SIZE, "%d.out", i-1);
        
            pid_t wpid; 
            fd = open(fileName, O_CREAT|O_TRUNC|O_RDWR, S_IWUSR|S_IRUSR);
        
            dprintf(fd, "Executing %s %s\n", argv[1], argv[i]); // dprintf from dudeman

            pid_t forkThis = fork(); 
            
            cPID = forkThis; 
            
            if (forkThis == 0) {    // child process
                dup2(fd, 1); 
                dup2(fd, 2); 
                execlp(argv[1], argv[1], argv[i], NULL); 
                exit(0);
            }
        
            else {
            // from discussion about do while bug
                do {
                    wpid = wait(&status);
                } while(wpid == -1 && errno == EINTR); 

            // from "quest for understanding the fundamentals of 
            // linux operating ststems and embedded systems" about wait()
                if(WIFEXITED(status)) {
                    dprintf(fd,"Finished executing %s %s at exit = %d\n", argv[1], argv[i], WEXITSTATUS(status));
                }
                else {
                    dprintf(fd, "Stopped executing %s %s exit code = %d\n",argv[1], argv[i], WTERMSIG(status)); // worked with Jefferson Ly
                }
            }
        }
    }
}
