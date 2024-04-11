#ifndef FILE_OPERATIONS_H
#define FILE_OPERATIONS_H

//Contain list of all existing users
extern char *user_list[100];

//Contain list of all chat files (denoting active conversations)
extern char *chat_list[100];

//Contain name of a requested user chat file if it exists
extern char *chat_file;

//Contain the contents of a user chat
extern char *chat_contents[1000];

//Contain filename of selected user conversation
extern char chat_ID[1000];

//Return array with names of all users
int get_user_list(char **user_list);

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


/**
 * Writes a new message between clients to their chat file.
 *
 * @param chat_filename  a pointer to the chat file for 2 clients.
 * @param new_line dereferenced pointer to the actual content that is to be written to the file
 * @return returns 1 if the file write was successful, -1 if the write failed.
 */
int write_to_chat_file(char *chat_filename, char* new_line);

/** Creates a chat file for 2 clients engaging for the first time
 *
 * @return n returns 1 if file is sucesfully create, -1 if file creation fails
 */
int create_new_chat_file(char *client1_name, char *client2_name);






#endif