tco: Chat_app_server.c File_Operations.c
	gcc -o tco Chat_app_server.c File_Operations.c -I. 

run: tco
	./tco
