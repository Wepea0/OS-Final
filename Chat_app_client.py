#ISSUE - Add error handling for failed server connection

import socket 
import select 
import sys 
import selectors

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 
# if len(sys.argv) != 3: 
#     print ("Correct usage: script, IP address, port number")
#     exit() 
IP_address = str("172.16.2.147") 
port = int(8888) 
server.connect((IP_address, port))

#Create selector object to monitor multiple sockets simultaneously for reading or writing 
selector_object = selectors.DefaultSelector()

#Sets server to return error/special value if an operation cannot be completed immediately instead of waiting 
server.setblocking(False)

#Specify the types of events that will be monitored
events = selectors.EVENT_READ | selectors.EVENT_WRITE

#Register the socket to the selector which will allow the events being monitored to be monitored on the specific socket.
selector_object.register(server, events)

while True:
    server.send("Initial message")
    events = selector_object.select()
    for key, mask in events:
        if mask & selectors.EVENT_READ: #The socket is ready to have data read from it
            print("Read event occurring")
            data = key.fileobj.recv(2048) #Get data from the socket(the server)
            
            if not data: #Data received from server is empty, check if the socket to the client is closed
                print("Socket is closed")
                key.fileobj.close()
                selector_object.unregister(key.fileobj)
                key.fileobj.close()
                sys.exit(0)
            else:
                print("Data received successfully. It is \n<Server> : ", data)
            
        elif mask and selectors.EVENT_WRITE:
            #Socket is ready to be written to 
            message = sys.stdin.readline()
            if(message.strip() == "close"):
                print("<Client> Closing connection")
                key.fileobj.close()
            elif(message.strip() == ""):
                print("Error no content")

            else:
                key.fileobj.send(message.encode('utf-8'))
                
            
            



