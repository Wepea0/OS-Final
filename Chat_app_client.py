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
STATE_USER_CHAT = 6

# Initialize the client state
client_state = STATE_SEND_LOGIN_DETAILS

server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
IP_address = "172.16.1.110"
port = 8888
server.connect((IP_address, port))

selector_object = selectors.DefaultSelector()
# server.setblocking(False) #ISSUE #8 - Does blocking need to be True or False 

events = selectors.EVENT_READ | selectors.EVENT_WRITE
selector_object.register(server, events)

# Save the current client username for the session
current_username = ""

''' Helper functions '''
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
        global current_username 
        current_username = username

        user_password = input("Enter your preferred password >> ")
        server.send(user_password.encode())

        user_ip = server.getsockname()[0]
        server.send(user_ip.encode())

    else:
        print("\nEntering login menu...")
        username = input("Enter your username >> ")
        server.send(username.encode())
        current_username = username

        user_password = input("Enter your password >> ")
        server.send(user_password.encode())

        user_ip = server.getsockname()[0]
        server.send(user_ip.encode())

    server_reply = server.recv(2048).decode()
    


    # Scaffolding
    # print("Server response = ", server_response)

    if(server_reply == "1"):
        return 1
    else:
        return -1

def display_chat_selection_menu():
    print("Selecting chat")

    #Prompt server to serve user list for chat selection
    select_chat_prompt = "selectChatMenu"
    server.send(select_chat_prompt.encode())

    num_users = 0
    username_list = [] #List for maintaining usernames sent from server, to send the actual requested username back to the server
    username = server.recv(2048).decode()
    concat_string = ""

    while(username != "/00/"):
        concat_string += username
        concat_string.strip("\n")
        if(concat_string[-4:-1] == "/00"):
            break
        username = server.recv(2048).decode()
        
    # Process incoming list of users 
    username_list = concat_string.split("ld00") 
    username_list = username_list[:-1] 
    current_name_position = 0       
    for s in username_list:
        if(s.strip() == current_username):
            current_name_position = num_users
            continue
        print(num_users, ". " , s)
        num_users+=1
    
    # Remove current client username from list
    username_list.pop(current_name_position)

    # Prompt server to serve retrieve chat function - take user selection and retrieve chat
    retrieve_chat_prompt = "retrieveChatMenu"
    server.send(retrieve_chat_prompt.encode()) #Prompt server to take requested user selection and retrieve chat

    # Take client selection of user to chat with
    try:
        user_selection = int(input("Enter the number of the user you would like to chat with >> "))
    
    except ValueError:
        server.send("".encode())
        print("Error! Closing connection")
        sys.exit(0)

    # Send requested username to server
    selected_username = username_list[user_selection].strip()
    print("Selected user - ", selected_username)
    value= server.send(selected_username.encode())
    print(value)


# Communicate with server
while True:
    print("Here")
    events = selector_object.select()
    for key, mask in events:
        # if mask & selectors.EVENT_WRITE:
        if client_state == STATE_SEND_LOGIN_DETAILS:
            user_option = serve_authentication_options()
            server_response = send_login_signup_details(user_option)

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
            client_state = STATE_WAIT_FOR_RESPONSE

        elif client_state == STATE_SELECT_CHAT: 
            display_chat_selection_menu()
            num_chats = 0
            chat_list = [] #List for maintaining usernames sent from server, to send the actual requested username back to the server
            chat = server.recv(2048).decode()
            concat_string = ""

            while(chat != "/00/"):
                concat_string += chat
                concat_string.strip("\n")
                if(concat_string[-4:-1] == "/00"):
                    break
                chat = server.recv(2048).decode()

            # Split received chat content by delimiter
            chat_content_list = []
            chat_content_list = concat_string.split("ld00")
            chat_content_list.remove("/00/")
            for s in chat_content_list:
                print(">>", s)

            #Pass control over to chat functionality
            client_state = STATE_USER_CHAT

        elif client_state == STATE_USER_CHAT:
            print("Under construction by ssblank")
            sys.exit(0)
        #chat_in_progress
        




            

            
        #Scaffolding
        print("Current state >> ", client_state)
    
        # sys.exit(0)


