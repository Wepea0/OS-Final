#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <dirent.h>
#include "Test_File_Operations.h"
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
    char client_username[1000];
    char client_password[1000];
    char client_IP[1000];
    char *usernames_list[1000];
    //TODO #1 - Handle when incoming data is more than capacity in buffer

    printf("Client connected\n");
    int client_fd = *((int*)client_socket);
    // char server_reply[] = "Connecton actve!" ;
    char server_message[] = "Client message processed -> " ;
    // char sign_up_success_reply[] = "Congratulations Sign up complete " ;
    // char sign_up_fail_reply[] = "Error!!! Sign up incomplete" ;

    char sign_up_success_reply[] = "1" ;
    char sign_up_fail_reply[] = "-1" ;

    //Login messages
    // char login_success_reply[] = "Login successful";
    // char login_fail_reply[] = "Login fail";
    char login_success_reply[] = "1";
    char login_fail_reply[] = "-1";

    char close_connection_message[] = "Closing server connection";



    recv(client_fd, &client_message, sizeof(client_message), 0); 
    if(client_message[0] == '1'){ //Sign up option
        puts("Server signup");
        
        recv(client_fd, &client_username, sizeof(client_username), 0); //Get username
        puts(client_username);
        recv(client_fd, &client_password, sizeof(client_password), 0); //Get user password
        puts(client_password);

        recv(client_fd, &client_IP, sizeof(client_IP), 0); //Get client IP address

        puts(client_IP);

        if(store_user_details(client_username, client_password, client_IP) == 1){
            send(client_fd, &sign_up_success_reply, strlen(sign_up_success_reply), 0);
            puts("Sign up complete");

        }
        else{
            send(client_fd, &sign_up_fail_reply, strlen(sign_up_fail_reply), 0);
            puts("Sign up failed");
            //Test code
            close(client_fd);
            pthread_exit(NULL);
            return NULL;
            //ISSUE #9 - Write function for login/signup


        }


    }
    else{ //Login option 
        puts("Server login");
        recv(client_fd, &client_username, sizeof(client_username), 0); //Get username
        recv(client_fd, &client_password, sizeof(client_password), 0); //Get user password
        recv(client_fd, &client_IP, sizeof(client_IP), 0); 

        if(login_user(client_username, client_password, client_IP) == 1){
            send(client_fd, &login_success_reply, strlen(login_success_reply), 0);
            puts("Login successful");



        }
        else{
            send(client_fd, &login_fail_reply, strlen(login_fail_reply), 0);
            puts("Login failed");
            //Test code
            close(client_fd);
            pthread_exit(NULL);
            return NULL;

            //ISSUE #9 - Write function for login/signup


        }
    }

    strcpy(client_message, empty_array);



    while(1){
        // Empty packet is received from client, connection is closed
        if (recv(client_fd, &client_message, sizeof(client_message), 0) <= 0) {
            puts("No data received. Closing connection");
            close(client_fd);
            pthread_exit(NULL);

        }

        //Close client connection based on close message from user
        if(strcmp(client_message, "cLoSe123") == 0){
            send(client_fd, &close_connection_message, strlen(close_connection_message), 0);
            puts("Closing connection");
            close(client_fd);
            pthread_exit(NULL);

        }

        //Serve client the select chat option after it is selected
        if(strcmp(client_message, "selectChatMenu") == 0){
            puts("Serving select chat menu");
            serve_chat_menu("user_details.txt", "chats");
            //Print all usernames in file
            //Either creates a new file or responds with the contents of the file with chat history of client
            // and selected user  
        }


        //Receive login or signup option 
        //Route to appropriate function based on response 
        //In signup function 
            //Essential
                //Write passed user name, password and find IP address to add to user_details 
            //Phase 2
                //Access user details file
                //Function to read all names in the file (to check whether name already exists)**
                //Function to write, name, username and IP (need to get IP) to file
            //Separate function to determine whether username has already been used. If yes, return message with error and close connection (prelim), re-serve initial options(final)
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
            
        else{
            char *final_server_reply = (char *)malloc(sizeof(server_message) + sizeof(client_message) + 1);
            strcpy(final_server_reply, server_message);
            strcat(final_server_reply, client_message);

            send(client_fd, final_server_reply, strlen(final_server_reply), 0);
            puts(final_server_reply);
            
            strcpy(client_message, empty_array);
        }
        

    }
    close(client_fd);
    pthread_exit(NULL);
}


// Function to write user details to file for authentication purposes
int store_user_details(char *username, char *password, char *IP){
        FILE *fileptr;
        char *username_header = "Username: ";
        char *password_header = "Password: ";
        char *IP_header = "IP Address: ";
        char *newline_sign = "\n";
        fileptr = fopen("user_details.txt", "a");
        if(fileptr == NULL){
            return 0;
        }
        puts("Opened file");

        // +2 accounts for null terminator on both strings (null pointer added automatically by C at string creation)
        //strlen() does not count null terminator
        char *username_array = (char *)malloc(strlen(username) + 2 + strlen(username_header));
        char *password_array = (char *)malloc(strlen(password) + 2 + strlen(password_header));
        char *IP_array = (char *)malloc(strlen(IP) + 2 + strlen(IP_header));

        strcpy(username_array, username_header);
        strcpy(password_array, password_header);
        strcpy(IP_array, IP_header);

        //Concatenate format string and actual username, password and IP 
        strcat(username_array, username);
        strcat(password_array, password);
        strcat(IP_array, IP);



        
        fputs(username_array,fileptr);
        fputs(newline_sign, fileptr);
        puts("written uname");

        fputs(password_array, fileptr);
        fputs(newline_sign, fileptr);
        puts("written pass");

        fputs(IP_array, fileptr);
        fputs(newline_sign, fileptr);
        fputs(newline_sign, fileptr);

        puts("written IP");



        fclose(fileptr);
        return 1;
    }

int login_user(char *username, char *password, char *IP_address){
    char curr_detail_line[1000]; 

    //Open user_details file
    FILE *fileptr;
    fileptr = fopen("user_details.txt", "r+");
    puts("Submitted");
    // printf("%ld", strlen(username));
    puts(username);
    puts(password);
    puts(IP_address);

    // Used to move pointer to the beginning of actual password, username and IP values for comparison since fgets() reads the whole line
    // from the user details file
    int username_offset = strlen("Username: ");
    int password_offset = strlen("Password: ");
    int ip_offset = strlen("IP Address: ");

    //Store position of IP address for current user in details file
    long int IP_line_position;

    while(fgets(curr_detail_line, 1000, fileptr)){
        //Remove newline character that is read from the end of line in file.
        curr_detail_line[strlen(curr_detail_line) - 1]  = '\0';

        //Skip "Username: " formatting from file and compare actual username values
        char *username_comp = curr_detail_line + username_offset;

        if(strcmp(username_comp, username) == 0){
            puts("Username match");
            fgets(curr_detail_line, 1000, fileptr);

            //Remove newline character that is read from the end of line in file.
            curr_detail_line[strlen(curr_detail_line) - 1]  = '\0';

            //Skip "Password: " formatting from file and compare actual password values
            char *password_comp = curr_detail_line + password_offset;

            if(strcmp(password_comp, password) == 0 ){
                puts("Password match");

                IP_line_position = ftell(fileptr);

                fgets(curr_detail_line, 1000, fileptr);

                //Remove newline character that is read from the end of line in file.
                curr_detail_line[strlen(curr_detail_line) - 1]  = '\0';

                //Skip "IP Address: " formatting from file and compare actual IP address values
                char *IP_comp = curr_detail_line + ip_offset;


                if(strcmp(IP_comp, IP_address) == 0){
                    puts("IP match");
                }
                else{
                    puts("Editing IP");

                    //Go back to line with IP address in user details file (fgets moves pointer to the next line after execution)
                    //Edit it to current IP address
                    if (fseek(fileptr, IP_line_position, SEEK_SET) == 0){
                        puts("Seek success");
                    }

                     //Clear IP address line
                    fputs("                         ", fileptr);

                    //Write current IP address
                    fseek(fileptr, IP_line_position, SEEK_SET);
                    char *IP_header = "IP Address: ";
                    char *IP_array = (char *)malloc(strlen(IP_address) + 2 + strlen(IP_header));
                    char *newline_sign = "\n";

                    strcpy(IP_array, IP_header);
                    strcat(IP_array, IP_address);

                    fputs(IP_array, fileptr);
                    fputs(newline_sign, fileptr);

                    puts("Edited IP");
                    

                }

                fclose(fileptr);
                return 1;
            }

        }
    }
    fclose(fileptr);
    return -1;

}


int serve_chat_menu(char *user_details_file, char *chat_folder){
    puts("Inside chat service function");
    char *username_header = "Username: ";
    //Split each item in the user detail file by :
    //Check if first split is "Username". If yes, access second split and store 
    //Otherwise move to next entry.

    //Will need a list as well, otherwise will not be able to enforce user selected actually exists.


    return 1;
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
	server.sin_addr.s_addr = inet_addr("172.16.9.34");
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