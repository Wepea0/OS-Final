#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include "File_Operations.h"

char *chat_list[100];
char *chat_file;
char *chat_contents[1000];
char chat_ID[1000];
char *user_list[100];



int get_user_list(char **user_list){
    char *username_prefix = "Username"; 
    char curr_detail_line[1000];

    char *prefix;
    char *suffix;


    //Open user_details file
    FILE *fileptr;
    fileptr = fopen("user_details.txt", "r+");

    int number_users = 0;
    while(fgets(curr_detail_line, 1000, fileptr)){

        //Remove newline character that is read from the end of line in file.
        // curr_detail_line[strlen(curr_detail_line) - 1]  = '\0';     
       
        //Split line in the user detail file to header (eg. Username, Password) = prefix and actual data = suffix
        prefix = strtok(curr_detail_line, ": ");
        suffix = strtok(NULL, ": ");

        
        //If this line contains a username (denoted by having a username prefix)
        if(prefix != NULL){
            if(strcmp(username_prefix, prefix) == 0){
                // puts("Username match");
                // printf("prefix - %s\n", prefix);
                // printf("suffix - %s\n", suffix);

                //Put username in list of usernames
                user_list[number_users] = (char *)malloc(strlen(suffix) + 1);
                strcpy(user_list[number_users], suffix);
                number_users++;

                }
        }
        
        
    }

    fclose(fileptr);
    return 1;

}


int get_chat_files(char * folder, char **chat_list) {
    DIR *dir;
    struct dirent *entry;

    // Open the directory
    dir = opendir(folder); // "." represents the current directory
    if (dir == NULL) {
        printf("Cannot open directory\n");
        return -1;
    }

    int number_of_chats = 0;
    // Read the directory entries
    while ((entry = readdir(dir)) != NULL) {
        if (  (strcmp(entry -> d_name, "..") != 0) && (strcmp(entry->d_name, "." ) != 0)){
            printf("%s\n", entry->d_name);
            chat_list[number_of_chats] = entry->d_name;
            number_of_chats++;

        }
        // puts("Cannot be printed");
        else{
            printf("Cannot be printed >> %s\n", entry->d_name);

        }

    }
    //Add delimeter value to list
    chat_list[number_of_chats] = "\0";

    // Close the directory
    closedir(dir);
    return 1;



}

int is_chat_open(char *client, char *requested_user, char **chat_list, char *chat_ID){
    char *chat_user_1;
    char *chat_user_2;

    int i = 0;
    while(strcmp(chat_list[i], "\0") != 0){
        strcpy(chat_ID, chat_list[i]);
        puts(chat_ID);

        // Split file name into user names
        chat_user_1 = strtok(chat_list[i], "_");
        chat_user_2 = strtok(NULL, "_");
        chat_user_2 = strtok(chat_user_2, ".");


        printf("User %s <---> User %s\n", chat_user_1, chat_user_2);

        if((strcmp(client, chat_user_1) == 0) || strcmp(client, chat_user_2) == 0){
            puts("Client found");
            if((strcmp(requested_user, chat_user_1) == 0) || strcmp(requested_user, chat_user_2) == 0){
                puts("Requested user found");
                puts(chat_ID);
                return 1;
            }
        }

        i++;
    }

    chat_ID = NULL;

    return -1;
}

int retrieve_chat(char *chat_filename, char **chat_contents){
    FILE *fileptr;
    char *chat_directory = "chats/";
    //2 accounts for string terminator character in each string since strlen doesnt't account for it
    char *chat_filepath = (char *)malloc(strlen(chat_directory) + 
                                        strlen(chat_filename) + 
                                        2);

    char chat_line[1000]; 
    char empty_array[1000];

    
    //Create string with complete filepath
    strcpy(chat_filepath, chat_directory);
    strcat(chat_filepath, chat_filename);
    // strcat(chat_filepath, chat_suffix);
    puts(chat_filepath);

    //Access file contents
    fileptr = fopen(chat_filepath, "r");
    if(fileptr == NULL){
        return -1;
    }
    
    //Empty chat contents array since it is global variable and will have been used before
    for (int i = 0; i < 1000; i++){
        free(chat_contents[i]);
    }


    int i = 0;
    while(fgets(chat_line, 1000, fileptr) != NULL){
        // puts(chat_line);
        chat_contents[i] = (char *)malloc(strlen(chat_line) + 1);
        strcpy(chat_contents[i], chat_line);
        // printf("%s", chat_contents[i]);

        i++;
        strcpy(chat_line, empty_array);
    }
    

    fclose(fileptr);

    return 1;
}


// int main(){
//     // char *chat_list[100];
//     // char *chat_file;

//     // get_chat_files("chats", chat_list);
//     // int i = 0;
//     // //Iterate until special delimeter character
//     // while(strcmp(chat_list[i], "\0") != 0){
//     //         printf("%d - %s\n", i, chat_list[i]); 
//     //         i++;         
//     //     }

//     // is_chat_open("Fastapi", "ThorganMichigan.txt", chat_list, chat_ID);

//     // if(chat_ID != NULL){
//     //     retrieve_chat(chat_ID, chat_contents);

//     // }
//     // else{
//     //     puts("Conversation does not exist");
//     // }

//     // int index = 0;
//     // puts("Printing chat contents II");
//     // while (chat_contents[index] != NULL){
//     //     puts(chat_contents[index]);
//     //     index++;
//     // }

//     get_user_list(user_list);
//     puts("Printing user list");
//     int i = 0;
//     while(user_list[i] != NULL){
//         printf("%s\n", user_list[i]);
//         i++;
//     }



//     return 1;
// }