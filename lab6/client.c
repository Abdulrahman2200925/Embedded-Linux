#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
 #include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 5000
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // 1. Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 2. Define server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("192.168.1.51"); // same machine

    // 3. Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server.\n");

    // 4. Loop for user input
    while (1) {
        float a, b;
        char op;

        printf("\nEnter operation (a + b), or Ctrl+C to exit: ");
        if (scanf("%f %c %f", &a, &op, &b) != 3)
            break;

        snprintf(buffer, sizeof(buffer), "%.2f %c %.2f", a, op, b);
        send(sockfd, buffer, strlen(buffer), 0);

        memset(buffer, 0, sizeof(buffer));
        int bytes = recv(sockfd, buffer, sizeof(buffer), 0);
        if (bytes <= 0)
            break;

        printf("%s\n", buffer);
    }

    // 5. Close
    close(sockfd);
    printf("Connection closed.\n");

    return 0;
}
