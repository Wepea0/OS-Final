#ifndef CHAT_APP_H
#define CHAT_APP_H

//Add passed username, password and IP to user_details file - serve as sign up function
int store_user_details(char *username, char *password, char *IP_address);

//Log user in by comparing details with stored user details
//This function also updates user IP address to the most recently used one
int login_user(char *username, char *password, char *IP_address);

//Serves chat menu and allows user to select user to chat with
int serve_chat_menu(int client_fd, char *curr_username);

//Receive requested user from client and open (new/existing) chat
int open_chat(int client_fd, char *curr_username, char *requested_username);

int serve_login_signup_menu();



#endif