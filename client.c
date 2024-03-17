/*
	C ECHO client example using sockets
*/
#include <stdio.h>	//printf
#include <string.h>	//strlen
#include <sys/socket.h>	//socket
#include <arpa/inet.h>	//inet_addr
#include <unistd.h>

int main(int argc , char *argv[])
{
	int sock;
	struct sockaddr_in server;
	char client_message[1000] , server_reply[2000];
	// int numbers[10]; // Array to hold 100 elements
	char sum[100];

	
	//Create socket
	sock = socket(AF_INET, SOCK_STREAM , 0);
	if (sock == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");
	
	server.sin_addr.s_addr = inet_addr("172.16.0.86");
	server.sin_family = AF_INET;
	server.sin_port = htons( 8888 );

	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return 1;
	}
	
	puts("Connected\n");

	// Populate the array with values
	printf("An array of 9 elements will be sent to the server\n");
	// for (int i = 0; i < 100; i++){
	// 	numbers[i] = 8980;
	// }
	char message[1000] = "Attempting to send message from client"; 

	// Send the array to the server
	if (send(sock, message, sizeof(message), 0) < 0)
	{
		puts("Send failed");
		return 1;
	}

	// Receive the sum from the server
	if (recv(sock, &sum, sizeof(sum), 0) < 0)
	{
		puts("recv failed");
		return 1;
	}

	printf("Server sum: %s\n", sum);

	close(sock);
	return 0;
}

