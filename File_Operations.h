#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

//Contain list of all chat files (denoting active conversations)
extern char *chat_list[100];

//Contain name of a requested user chat file if it exists
extern char *chat_file;

//Contain the contents of a user chat
extern char *chat_contents[1000];

//Contain filename of selected file
extern char chat_ID[1000];

//Return array with names of all active chats
int get_chat_files(char * folder, char **chat_list);

//Given two usernames, identify whether a chat between those users exists.
//chat_ID = filenaame if operation was successful 
//chat_ID = NULL if operation was unsuccessful
int is_chat_open(char *client, char *requested_user, char **chat_list, char *chat_ID);

//Return list with contents of requested conversation
//Return 1 if operation was successful 
//Return 0 if operation was unsuccessful
int retrieve_chat(char *chat_filename, char **chat_contents);







#endif