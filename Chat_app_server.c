#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <dirent.h>
#include "File_Operations.h"
#include "Chat_app.h"
#include <fcntl.h>


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
    //TODO #1 - Handle when incoming data is more than capacity in buffer

    printf("Client connected\n");
    int client_fd = *((int*)client_socket);

    //Server response messages
    char server_message[] = "Client message processed -> " ;

    char sign_up_success_reply[] = "1" ;
    char sign_up_fail_reply[] = "-1" ;

    char login_success_reply[] = "1";
    char login_fail_reply[] = "-1";

    char close_connection_message[] = "Closing server connection";

    //Receive client  selection
    recv(client_fd, &client_message, sizeof(client_message), 0); 

    //Sign up option
    if(client_message[0] == '1'){ 
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
            close(client_fd);
            pthread_exit(NULL);
            return NULL;
        }
    }

    //Login option
    else{  
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
        }
    }

    //Empty client message array to be reused later
    strcpy(client_message, empty_array);


    int value = recv(client_fd, &client_message, sizeof(client_message), 0);

    while(value > 0){
        puts("While start");
        // Empty packet is received from client, connection is closed

        //Close client connection based on close message from user
        if(strcmp(client_message, "cLoSe123") == 0){
            send(client_fd, &close_connection_message, strlen(close_connection_message), 0);
            puts("Closing connection");
            close(client_fd);
            pthread_exit(NULL);
        }

        //Serve client the select chat option after it is selected
        else if(strcmp(client_message, "selectChatMenu") == 0){
            puts("Serving select chat menu");
            printf("Client message - %s\n", client_message);
            serve_chat_menu(client_fd, client_username);
        }

        //Get user chat selection
        else if(strcmp(client_message, "retrieveChatMenu") == 0){
            puts("Serving retrieve chat menu");
            char requested_user_index[5];
            char requested_username[100];
            char requested_user_id[200];

            recv(client_fd, &requested_username, sizeof(requested_username), 0);
            
            printf("Client message - %s \nSubmitted user selection - %s --\n", client_message, requested_username);

            open_chat(client_fd, client_username, requested_username);
            // open_chat(client_fd, client_username, requested_username);

        }

        // Chat between two users is in progress 
        else if(strcmp(client_message, "chat_in_progress") == 0){
            puts("Chat in progress");
            char user_message[2000];
            int return_value;

            // Read the message sent by the user. 
            //Recv is blocing so it will wait until some data is read before it moves on 
            return_value = recv(client_fd, &user_message, sizeof(user_message), 0);

            //If client wants to end connection, this text sequence will cause 
            while(return_value > 0){
                
                //If client closes connection abruptly, this will close the connection
                if(return_value <= 0){
                    puts("Closing connection --");
                    break;
                }
                printf("Writing message to file %s ", user_message);

                // Write client message to chat file in case message received is valid and is not a close connection reques
                write_to_chat_file(chat_ID, user_message);

                //Attempt to send message to other user in conversation


                //Receive next message from the user
                return_value = recv(client_fd, &user_message, sizeof(user_message), 0); 

            }

            break;

        }
        

        
        
        value = recv(client_fd, &client_message, sizeof(client_message), 0);
        

    }
    puts("No data received. Closing connection");
    close(client_fd);
    pthread_exit(NULL);
   
}


// Function to write user details to file for authentication purposes
int store_user_details(char *username, char *password, char *IP){
        FILE *fileptr;
        char *username_header = "Username: ";
        char *password_header = "Password: ";
        char *IP_header = "IP Address: ";
        char *user_id_header = "User ID: ";
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
        char *user_id_array = (char *)malloc(strlen(username) + strlen(password) + strlen(user_id_header) + 3); //Autogenerated user ID is combination of first username and password

        strcpy(username_array, username_header);
        strcpy(password_array, password_header);
        strcpy(IP_array, IP_header);
        strcpy(user_id_array, user_id_header);

        //Concatenate format string and actual username, password and IP 
        strcat(username_array, username);
        strcat(password_array, password);
        strcat(IP_array, IP);

        strcat(user_id_array, username);
        strcat(user_id_array, password);

        
        fputs(username_array,fileptr);
        fputs(newline_sign, fileptr);
        puts("written uname");

        fputs(password_array, fileptr);
        fputs(newline_sign, fileptr);
        puts("written pass");

     

        fputs(user_id_array, fileptr);
        fputs(newline_sign, fileptr);
        fputs(newline_sign, fileptr);
        puts("generated and written user id");




        fclose(fileptr);
        return 1;
    }


/**
 * The function `find_difference_point` compares two strings character by character and identifies the
 * position where they differ, if any.
 */
void find_difference_point(char *str1, char *str2) {
    int i = 0;
    while (str1[i] && str2[i] && str1[i] == str2[i]) {
        i++;
    }
                    fflush(stdout);  // Manually flush the buffer

    if (str1[i] || str2[i]) {
        printf("Strings differ at position %d. String 1 - %c, String 2 - %c\n\n", i, str1[i], str2[i]);
                fflush(stdout);  // Manually flush the buffer

    } else {
        printf("Strings are identical\n");
                fflush(stdout);  // Manually flush the buffer

    }
}

int login_user(char *username, char *password, char *IP_address){
    char curr_detail_line[1000]; 
    char username_comp[1000];
    char password_comp[1000];

    //Open user_details file
    FILE *fileptr;
    fileptr = fopen("user_details.txt", "r");
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
        if(curr_detail_line[strlen(curr_detail_line) - 1] == '\n'){
            curr_detail_line[strlen(curr_detail_line) - 1]  = '\0';
            curr_detail_line[strlen(curr_detail_line) - 1]  = '\0';

            puts("Newline out");
            puts(curr_detail_line);
        }

        //Skip "Username: " formatting from file and compare actual username values
        strcpy(username_comp, curr_detail_line + username_offset);

       
        fflush(stdout);

        
        if(strcmp(username_comp, username) == 0){
            puts("Username match");
            fgets(curr_detail_line, 1000, fileptr);


            //Remove newline character that is read from the end of line in file.
            if(curr_detail_line[strlen(curr_detail_line) - 1] == '\n'){
            curr_detail_line[strlen(curr_detail_line) - 1]  = '\0';
            curr_detail_line[strlen(curr_detail_line) - 1]  = '\0';

            puts("Newline password out");
            puts(curr_detail_line);
        }
            
            //Skip "Password: " formatting from file and compare actual password values
            strcpy(password_comp, curr_detail_line + password_offset);
            puts("Comparing password");
            puts(password_comp);
            puts(password);
            if(strcmp(password_comp, password) == 0 ){
                puts("Password match");
                IP_line_position = ftell(fileptr);
                fgets(curr_detail_line, 1000, fileptr);
                //Remove newline character that is read from the end of line in file.
                curr_detail_line[strlen(curr_detail_line) - 1]  = '\0';
                //Skip "IP Address: " formatting from file and compare actual IP address values
                char *IP_comp = curr_detail_line + ip_offset;
            
                fclose(fileptr);
                return 1;
            }
            
    
        }
        username_comp[0] = '\0'; // Set username_comp to empty
        password_comp[0] = '\0'; // Set password_comp to empty
        // find_difference_point(username, username_comp);

/* The `memset` function is used to fill a block of memory with a particular value, in this case, 0. */
        memset(curr_detail_line, 0, sizeof(curr_detail_line));

    }
    
    fclose(fileptr);
    return -1;
}

// int login_user(char *username, char *password, char *IP_address){
//     char curr_detail_line[1000]; 

//     //Open user_details file
//     FILE *fileptr;
//     fileptr = fopen("user_details.txt", "r+");
//     puts("Submitted");
//     // printf("%ld", strlen(username));
//     puts(username);
//     puts(password);
//     puts(IP_address);

//     // Used to move pointer to the beginning of actual password, username and IP values for comparison since fgets() reads the whole line
//     // from the user details file
//     int username_offset = strlen("Username: ");
//     int password_offset = strlen("Password: ");
//     int ip_offset = strlen("IP Address: ");

//     //Store position of IP address for current user in details file
//     long int IP_line_position;

//     while(fgets(curr_detail_line, 1000, fileptr)){
//         //Remove newline character that is read from the end of line in file.
//         curr_detail_line[strlen(curr_detail_line) - 1]  = '\0';


//         //Skip "Username: " formatting from file and compare actual username values
//         char *username_comp = curr_detail_line + username_offset;

//         if(strcmp(username_comp, username) == 0){
//             puts("Username match");
//             fgets(curr_detail_line, 1000, fileptr);

//             //Remove newline character that is read from the end of line in file.
//             curr_detail_line[strlen(curr_detail_line) - 1]  = '\0';

//             //Skip "Password: " formatting from file and compare actual password values
//             char *password_comp = curr_detail_line + password_offset;

//             if(strcmp(password_comp, password) == 0 ){
//                 puts("Password match");

//                 IP_line_position = ftell(fileptr);

//                 fgets(curr_detail_line, 1000, fileptr);

//                 //Remove newline character that is read from the end of line in file.
//                 curr_detail_line[strlen(curr_detail_line) - 1]  = '\0';

//                 //Skip "IP Address: " formatting from file and compare actual IP address values
//                 char *IP_comp = curr_detail_line + ip_offset;


//                 if(strcmp(IP_comp, IP_address) == 0){
//                     puts("IP match");
//                 }
//                 else{
//                     puts("Editing IP");

//                     //Go back to line with IP address in user details file (fgets moves pointer to the next line after execution)
//                     //Edit it to current IP address
//                     if (fseek(fileptr, IP_line_position, SEEK_SET) == 0){
//                         puts("Seek success");
//                     }

//                      //Clear IP address line
//                     fputs("                         ", fileptr);

//                     //Write current IP address
//                     fseek(fileptr, IP_line_position, SEEK_SET);
//                     char *IP_header = "IP Address: ";
//                     char *IP_array = (char *)malloc(strlen(IP_address) + 2 + strlen(IP_header));
//                     char *newline_sign = "\n";

//                     strcpy(IP_array, IP_header);
//                     strcat(IP_array, IP_address);

//                     fputs(IP_array, fileptr);
//                     fputs(newline_sign, fileptr);

//                     puts("Edited IP");
                    

//                 }

//                 fclose(fileptr);
//                 return 1;
//             }

//         }
//     }
//     fclose(fileptr);
//     return -1;

// }



int serve_chat_menu(int client_fd, char *curr_username){
    get_user_list(user_list);
    char end_list_string[] = "/00/";
    char list_delimiter[] = "ld00"; //Sent in between each name to enable easy processing on client

    char empty_array[100];
    char requested_user_index[5];
    char requested_username[100];
    char requested_user_id[200];


    int i = 0;
    char hold_username[100];

    while(user_list[i] != NULL){
        printf("%s\n", user_list[i]);
        strcpy(hold_username, user_list[i]);
        send(client_fd, &hold_username, strlen(hold_username), 0);
        send(client_fd, &list_delimiter, strlen(list_delimiter), 0);
        strcpy(hold_username, empty_array);

        i++;
    

    }

    send(client_fd, &end_list_string, strlen(end_list_string), 0);
    


    return 1;



}

int open_chat(int client_fd, char *curr_username, char *requested_username){
    char list_delimiter[] = "ld00"; //Sent in between each chat sentence to enable easy processing on client
    char end_list_string[] = "/00/"; //Sent to show end of server response
    char hold_chat_line[1000]; //Chat line is read from array and sent from this variable to ensure predictable send behaviour (since char arrays deal with pointers)
    char empty_array[1000]; //Used to empty hold_chat_line array to reuse it

    get_chat_files("chats", chat_list);
    int value;
    value = is_chat_open(curr_username, requested_username, chat_list, chat_ID);

   

    //Find IP address of requested user and assign it to participant IP
    get_user_IP(requested_username);

    if(value == 1){
        puts("Retrieving chat");
        puts(chat_ID);

        retrieve_chat(chat_ID, chat_contents);
        int index = 0;
        puts("Printing chat contents");
            while (chat_contents[index] != NULL){
                puts(chat_contents[index]);
                strcpy(hold_chat_line, chat_contents[index]);
                send(client_fd, &hold_chat_line, strlen(hold_chat_line), 0);
                send(client_fd, &list_delimiter, strlen(list_delimiter), 0);
                index++;
                strcpy(hold_chat_line, empty_array);

            }
            send(client_fd, &end_list_string, strlen(end_list_string), 0);

        return 1;

    }

    else{
        puts("Conversation does not exist");
        puts("Attempting to create the conversation");
        create_new_chat_file(curr_username, requested_username);

        
        char test_string[] = "EMPTY CHATld00/00/"; 
        send(client_fd, &test_string, strlen(test_string), 0);
        // send(client_fd, &list_delimiter, strlen(list_delimiter), 0);
        // send(client_fd, &end_list_string, strlen(end_list_string), 0);

        //Will send client empty string but chat
        return 1;
    }
    return -1;
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
	server.sin_addr.s_addr = inet_addr("192.168.102.72");
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