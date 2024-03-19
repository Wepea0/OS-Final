# ISSUE - Add error handling for failed server connection

import socket 
import sys 
import selectors

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 
# if len(sys.argv) != 3: 
#     print ("Correct usage: script, IP address, port number")
#     exit() 172.16.3.40
IP_address = str("172.16.0.99") 
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


#Helper functions
def serve_authentication_options(server_key):
    """Serve user option to login or sign up to chat server """

    print("Welcome to SWE chat.\nEnter 1 to signup or 2 to login >> ", end="", flush=True)
    user_input = input()
    if(user_input == "1"):
        print("\nEntering sign up menu...")
        server_key.fileobj.send(user_input.encode())
    elif(user_input == "2"):
        print("\nEntering login menu...")
        server_key.fileobj.send(user_input.encode())
    else:
        serve_authentication_options(server_key)
    return user_input


def serve_signup(server_key):
    """Function to take user info for sign up"""
    user_ip = get_ip()

    print("\nWelcome to Sign Up.\nEnter your preferred username >> ", end="", flush=True)
    username = input()
    print("\nEnter your preferred password >> ", end="", flush=True)
    user_password = input()
    server_key.fileobj.send(username.encode())
    server_key.fileobj.send(user_password.encode())
    server_key.fileobj.send(user_ip.encode())


def serve_login(server_key):
    """Function to take user info for login"""
    pass

def get_ip():
    """Access client ip """
    return server.getsockname()[0]



    

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
                first_message = False
                user_input = serve_authentication_options(key)
                if(user_input == "1"):
                    serve_signup(key)
                else:
                    serve_login(key)
                # key.fileobj.send(dummy_message.encode())
                # first_message = False
                # continue

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
            



