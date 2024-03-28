# # ISSUE - Add error handling for failed server connection

# import socket 
# import sys 
# import selectors

# server = socket.socket(socket.AF_INET, socket.SOCK_STREAM) 
# # if len(sys.argv) != 3: 
# #     print ("Correct usage: script, IP address, port number")
# #     exit() 172.16.3.40
# IP_address = str("172.16.8.95") 
# port = int(8888) 
# server.connect((IP_address, port))

# #Create selector object to monitor multiple sockets simultaneously for reading or writing 
# selector_object = selectors.DefaultSelector()

# #Sets server to return error/special value if an operation cannot be completed immediately instead of waiting 
# server.setblocking(False)

# #Specify the types of events that will be monitored
# events = selectors.EVENT_READ | selectors.EVENT_WRITE

# #Register the socket to the selector which will allow the events being monitored to be monitored on the specific socket.
# selector_object.register(server, events)


# #Helper functions
# def serve_authentication_options(server_key):
#     """Serve user option to login or sign up to chat server """

#     print("Welcome to SWE chat.\nEnter 1 to signup or 2 to login >> ", end="", flush=True)
#     user_input = input()
#     if(user_input == "1"):
#         print("\nEntering sign up menu...")
#         server_key.fileobj.send(user_input.encode())
#     elif(user_input == "2"):
#         print("\nEntering login menu...")
#         server_key.fileobj.send(user_input.encode())
#     else:
#         serve_authentication_options(server_key)
#     return user_input


# def serve_signup(server_key):
#     """Function to take user info for sign up"""
#     user_ip = get_ip()

#     print("\nWelcome to Sign Up.\nEnter your preferred username >> ", end="", flush=True)
#     username = input()
#     print("\nEnter your preferred password >> ", end="", flush=True)
#     user_password = input()
#     server_key.fileobj.send(username.encode())
#     server_key.fileobj.send(user_password.encode())
#     server_key.fileobj.send(user_ip.encode())


# def serve_login(server_key):
#     """Function to take user info for login"""
    
#     user_ip = get_ip()

#     print("\nWelcome to Log In.\nEnter your username >> ", end="", flush=True)
#     username = input()
#     print("\nEnter your password >> ", end="", flush=True)
#     user_password = input()
#     server_key.fileobj.send(username.encode())
#     server_key.fileobj.send(user_password.encode())
#     server_key.fileobj.send(user_ip.encode())


# def get_ip():
#     """Access client ip """
#     return server.getsockname()[0]



    

# send_message = True #Flag to control flow of execution between reading and writing. Without it, switch between reading from server and allowing writing to server is unpredictable
# first_message = True #Used to send dummmy message that prompts server to provide sign up and login options
# dummy_message = "dUmmY mEsSaGe"

# #Following code allows writing and reading to and from the server in alternating order under a close command is entered to close server
# while True:
#     events = selector_object.select()
#     for key, mask in events:
#         if mask & selectors.EVENT_READ:
#             print("Receiving data")
#             data = key.fileobj.recv(2048)
#             if not data:                               # Socket is closed
#                 print("Socket is closed")
#                 selector_object.unregister(key.fileobj)
#                 key.fileobj.close()
#                 sys.exit(0)
#             print("Server data -> ", data)
#             send_message = True

#         if mask & selectors.EVENT_WRITE and send_message:
#             if(first_message):
#                 first_message = False
#                 user_input = serve_authentication_options(key)
#                 if(user_input == "1"):
#                     serve_signup(key)
#                 else:
#                     serve_login(key)
#                 # key.fileobj.send(dummy_message.encode())
#                 # first_message = False
#                 # continue

#             print("Enter your message>>", end=" ", flush=True)
#             message = sys.stdin.readline()

#             if(message.strip() == "cLoSe123"):
#                 print("Closing connection and exiting")
#                 key.fileobj.close()
#                 sys.exit(0)

#             elif(message.strip() == ""):
#                 print("Error!! Empty message content")
#                 continue;
            
#             key.fileobj.send(message.encode())
#             send_message = False
        
#         else:
#             print("No events")
#             break;
            


import socket
import selectors
import sys
import json

# Define the states
STATE_SEND_LOGIN_DETAILS = 1
STATE_WAIT_FOR_RESPONSE = 2
STATE_SEND_MESSAGE = 3
STATE_READ_MESSAGE = 4
STATE_SELECT_CHAT = 5

# Initialize the client state
client_state = STATE_SEND_LOGIN_DETAILS

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
IP_address = "172.16.0.70"
port = 8888
server.connect((IP_address, port))

selector_object = selectors.DefaultSelector()
# server.setblocking(False) #ISSUE #8 - Does blocking need to be True or False 

events = selectors.EVENT_READ | selectors.EVENT_WRITE
selector_object.register(server, events)

def serve_authentication_options():
    user_option = input("Welcome to SWE chat.\nEnter 1 to signup or 2 to login >> ")
    if user_option not in ["1", "2"]:
        return serve_authentication_options()
    server.send(user_option.encode())
    return user_option

def send_login_signup_details(user_option):
    """Serve login and sign up options to the user"""

    if user_option == "1":
        print("\nEntering sign up menu...")
        username = input("Enter your preferred username >> ")
        server.send(username.encode())

        user_password = input("Enter your preferred password >> ")
        server.send(user_password.encode())

        user_ip = server.getsockname()[0]
        server.send(user_ip.encode())

    else:
        print("\nEntering login menu...")
        username = input("Enter your username >> ")
        server.send(username.encode())

        user_password = input("Enter your password >> ")
        server.send(user_password.encode())

        user_ip = server.getsockname()[0]
        server.send(user_ip.encode())

    server_response = server.recv(2048).decode()
    


    # Scaffolding
    # print("Server response = ", server_response)

    if(server_response == "1"):
        return 1;
    else:
        return -1;


   

while True:
    print("Here")
    events = selector_object.select()
    for key, mask in events:
        # if mask & selectors.EVENT_WRITE:
        if client_state == STATE_SEND_LOGIN_DETAILS:
            user_option = serve_authentication_options()
            server_response = send_login_signup_details(user_option)

            #Scaffolding
            # print("Second server response = ", server_response)

            if(server_response == 1):
                print("Success")

                client_state = STATE_SELECT_CHAT
            else:
                print("Error! Closing connection")
                # TODO #10 Test code - Ideally should return to login/sign up menu. Need support from server-side to implement
                server.close()
                sys.exit(-1)

        # if mask & selectors.EVENT_READ:
        elif client_state == STATE_WAIT_FOR_RESPONSE:
            response = server.recv(2048).decode()
            print(f"Server response: {response}")
            client_state = STATE_SEND_MESSAGE  

        elif client_state == STATE_SEND_MESSAGE:
            user_message = input("Enter your message >> ")
            if(user_message.strip() == ""):
                print("Error! Empty message content")
                break
            
            
            server.send(user_message.encode())
            if(user_message.strip() == "cLoSe123"):
                print("Connection closing")
                server.close()
                sys.exit(0)
            # print(f"Server message: {message}")
            client_state = STATE_WAIT_FOR_RESPONSE

        elif client_state == STATE_SELECT_CHAT: 
            
            pass

        



        #Scaffolding
        print("Current state >> ", client_state)
    
        # sys.exit(0)


