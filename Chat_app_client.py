# ISSUE - Add error handling for failed server connection

import socket 
import select 
import sys 
import selectors

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 
# if len(sys.argv) != 3: 
#     print ("Correct usage: script, IP address, port number")
#     exit() 172.16.3.40
IP_address = str("172.16.9.173") 
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



send_message = True #Flag to control flow of execution between reading and writing. Without it, switch between reading from server and allowing writing to server is unpredictable
first_message = True #Used to send dummmy message that prompts server to provide sign up and login options
dummy_message = "dUmmY mEsSaGe"

#Following code allows writing and reading to and from the server in alternating order under a close command is entered to close server
while True:
    events = selector_object.select()
    for key, mask in events:
        if mask & selectors.EVENT_READ:
            print("Receiving data")
            data = key.fileobj.recv(2048)
            if not data:                               # Socket is closed
                print("Socket is closed")
                selector_object.unregister(key.fileobj)
                key.fileobj.close()
                sys.exit(0)
            print("Server data -> ", data)
            send_message = True

        if mask & selectors.EVENT_WRITE and send_message:
            if(first_message):
                key.fileobj.send(dummy_message.encode())
                first_message = False
                continue

            print("Enter your message>>", end=" ", flush=True)
            message = sys.stdin.readline()

            if(message.strip() == "cLoSe123"):
                print("Closing connection and exiting")
                key.fileobj.close()
                sys.exit(0)

            elif(message.strip() == ""):
                print("Error!! Empty message content")
                continue;
            
            key.fileobj.send(message.encode())
            send_message = False
            



