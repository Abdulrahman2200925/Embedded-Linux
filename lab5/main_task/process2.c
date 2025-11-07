#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>



#define size 1024u
volatile unsigned int keep_running = 1;



void handle_signal(int sig) {
    keep_running = 0;
}




int main(){

const char *path="/home/mac/workspace/Eng.Fady/git_workspace/Embedded-Linux/lab5";

 int key = ftok(path, 'A'); 

 
 int shmid = shmget(key, 0, 0644);

  void *shared_mem_ptr = shmat(shmid, NULL, SHM_RDONLY);

     printf("Shared memory attached at: %p\n", shared_mem_ptr);
    printf("=== Shared Memory Reader ===\n");
    printf("Monitoring shared memory for changes...\n");
    printf("Press Ctrl+C to stop.\n\n");

      char *data = (char *)shared_mem_ptr;
    char last_message[size] = "";

        while (keep_running) {
        // Check if message has changed
        if (strcmp(data, last_message) != 0) {
            printf("[Reader] New message: '%s'\n", data);
            strncpy(last_message, data, size - 1);
            last_message[size - 1] = '\0';
            
            // Check if writer exited
            if (strstr(data, "exit") != NULL) {
                printf("[Reader] Writer process has exited. Stopping...\n");
                break;
            }
        }
        
        sleep(1);  // Check every second
    }
    
    printf("Detaching shared memory...\n");
    if (shmdt(shared_mem_ptr) == -1) {
        perror("shmdt failed");
    } else {
        printf("Shared memory detached.\n");
    }
   
    return 0;

}