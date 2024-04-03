co: Chat_app_server.c File_Operations.c
	gcc -o co Chat_app_server.c File_Operations.c -I. 

run: co
	./co
