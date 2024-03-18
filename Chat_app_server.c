#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "Chat_app.h"

#define PORT 8888

int global_variable = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// void* handle_client(void* client_socket) {
//     int client_fd = *((int*)client_socket);

//     // Read size and data in a single buffer
//     int value_to_increment;
    // read(client_fd, &value_to_increment, sizeof(value_to_increment));

//     pthread_mutex_lock(&lock);
//     global_variable += value_to_increment;
//     pthread_mutex_unlock(&lock);

//     // Send the updated value back to the client
//     write(client_fd, &global_variable, sizeof(global_variable));

//     close(client_fd);
//     pthread_exit(NULL);
// }

void * handle_client(void* client_socket){
    char client_message[1000];
    char empty_array[1000];

    printf("Client connected\n");
    int client_fd = *((int*)client_socket);
    // char server_reply[] = "Connecton actve!" ;
    char server_message[] = "Client message processed -> " ;

    while(1){
        read(client_fd, &client_message, sizeof(client_message)); //Read dummy message (check if it is dummy and don't send mirror reply)
        //Send login or signup option 
        //Route to appropriate function based on response 
        //In signup function 
            //Access user details file
            //Determine whether username has already been used. If yes, return message with error and close connection (prelim), re-serve initial options(final)
            //If not, add username, IP address and password to list. (Decide on format, should be optimized for later seraches (username used already & find details for login))
            //Return success message, send to select chat screen
        //In login function
            //Access user details file
            //Determine whether username, password combo exists in file
            //If it does, return success message and send to select chat screen
            //If it does not, return error messsage and return message with error and close connection (prelim), re-serve initial options(final)
        //Select chat screen
            //Serve list of all users. If user selects someone with whom prior chat exists, print prior messages else create new file and print file content (empty)
            //List of chats should be stored somewhere. 
            

        char *final_server_reply = (char *)malloc(sizeof(server_message) + sizeof(client_message) + 1);
        strcpy(final_server_reply, server_message);
        strcat(final_server_reply, client_message);
        write(client_fd, final_server_reply, strlen(final_server_reply));
        puts(final_server_reply);
        
        strcpy(client_message, empty_array);

    }
    




    close(client_fd);


    pthread_exit(NULL);
    

}


int main() {
    int server_fd, client_fd;
   	struct sockaddr_in server , client;
    socklen_t addr_len = sizeof(client);
    pthread_t thread_id;

    // Creating a  socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    //Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("172.16.9.173");
	server.sin_port = htons( 8888 );
    
    // Binding the socket
    if (bind(server_fd, (struct sockaddr*)&server, sizeof(server)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, 1) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on %d\n", PORT);

    while (1) {
        // Accept a connection
        client_fd = accept(server_fd, (struct sockaddr*)&client, &addr_len);
        if (client_fd == -1) {
            perror("Accept failed");
            continue;
        }

        // Create a separate thread for each client
        puts("Client accepted");
        if (pthread_create(&thread_id, NULL, handle_client, (void*)&client_fd) < 0) {
            perror("Thread creation failed");
            exit(EXIT_FAILURE);
        }
    }

    close(server_fd);
    return 0;
}
