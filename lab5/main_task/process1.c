#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>



#define size 1024u



int main(){

const char *path="/home/mac/workspace/Eng.Fady/git_workspace/Embedded-Linux/lab5";

 int key = ftok(path, 'A'); 
 
 
 int shmid = shmget(key, size, 0644 | IPC_CREAT);

  void *shared_mem_ptr = shmat(shmid, NULL, 0);

printf("Shared memory attached at: %p\n", shared_mem_ptr);
    
    char *data = (char *)shared_mem_ptr;
    char input[256];
    
    printf("=== Shared Memory Writer ===\n");
    printf("Type messages to write to shared memory.\n");
    printf("Type 'exit' to quit.\n\n");
    
    // Initialize shared memory
    strcpy(data, "Shared memory ready!");

      while (1) {
        printf("Enter message: ");
        
        // Read input from user
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;  // EOF or error
        }
        
        // Remove newline character
        input[strcspn(input, "\n")] = 0;
        
        // Check for exit condition
        if (strcmp(input, "exit") == 0) {
            strcpy(data, "Writer process exited");
            printf("Exiting...\n");
            break;
        }
        
        // Write to shared memory
        strncpy(data, input, size - 1);
        data[size - 1] = '\0';  // Ensure null termination
        
        printf("Written to shared memory: '%s'\n", data);
        printf("--- Waiting for Process 2 to read ---\n\n");
    }
    
    // Cleanup
    printf("Detaching shared memory...\n");
    if (shmdt(shared_mem_ptr) == -1) {
        perror("shmdt failed");
    } else {
        printf("Shared memory detached.\n");
    }

    // Optional: Remove shared memory when done
    printf("Removing shared memory segment...\n");
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl IPC_RMID failed");
    } else {
        printf("Shared memory segment removed.\n");
    }
   
     return 0;

}