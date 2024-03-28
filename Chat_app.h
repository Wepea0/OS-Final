#ifndef CHAT_APP_H
#define CHAT_APP_H

//Add passed username, password and IP to user_details file - serve as sign up function
int store_user_details(char *username, char *password, char *IP_address);

//Log user in by comparing details with stored user details
int login_user(char *username, char *password, char *IP_address);

//Serves chat menu and allows user to select user to chat with
int serve_chat_menu(char *user_details_file, char *chat_folder);

#endif