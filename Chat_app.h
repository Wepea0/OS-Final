#ifndef CHAT_APP_H
#define CHAT_APP_H

//Add passed username, password and IP to user_details file - serve as sign up function
int store_user_details(char *username, char *password, char *IP_address);

//Log user in by comparing details with stored user details
int login_user(char *username, char *password, char *IP_address);

#endif