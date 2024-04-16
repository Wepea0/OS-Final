import socket
import selectors
import sys
import json
import threading


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
IP_address = "192.168.137.1"
port = 8888
server.connect((IP_address, port))

selector_object = selectors.DefaultSelector()
# server.setblocking(False) #ISSUE #8 - Does blocking need to be True or False 

events = selectors.EVENT_READ | selectors.EVENT_WRITE
selector_object.register(server, events)

# Save the current client username for the session
current_username = ""

#Username of the user currently being chatted with
selected_username = ""

# Variable to pass end program signal from second thread to main thread
end_program = False

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
        server.sendall(user_password.encode()  + b'\0')  # Add a null byte as a delimiter

     

    else:
        print("\nEntering login menu...")
        username = input("Enter your username >> ")
        # server.send(username.encode())
        current_username = username

        user_password = input("Enter your password >> ")
        user_details = username + "\\0" + user_password + "\0"
        print("attempting to send password")
        server.sendall(user_details.encode())

    server_reply = server.recv(2048).decode()

    print("server reply - ", server_reply)

    return server_reply

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
    username_list = username_list[:-1] \

    # New list without username of the current logged in client
    new_username_list = []
    current_name_position = 0       
    for s in username_list:
        if(s.strip() == current_username):
            current_name_position = num_users
            continue
        print(num_users, ". " , s)
        new_username_list.append(s)
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
    global selected_username
    selected_username = new_username_list[user_selection].strip()
    print("Selected user - ", selected_username)
    value= server.send(selected_username.encode())
    print(value)

def send_message(server):
    original_user_message = input("Enter your message >> ")
    if(original_user_message.strip() == "cLoSe123"):
        print("Connection closing")
        
        # ISSUE - At this point, redirect to login/signup menu instead of exit
        global end_program
        end_program = True
        message_sent_event.set()
        return -1
        # sys.exit(0)

    else:
        user_message = "\n" + current_username + ": " + original_user_message
        server.sendall(user_message.encode() + b'\0')
    return 1

def receive_replies(thread_event, message_sent_event, server):
    '''Function for setting client to receive mode until user attempts to send message'''
    start_chat_signal = "chat_in_progress"
    value= server.send(start_chat_signal.encode())
    # print(value)
    print("Type `send` to send a message")


    while True:
        if not thread_event.is_set():
            server.setblocking(False)

            try:
                server_reply = server.recv(2048).decode()
                if(server_reply):
                    print(server_reply)
                else:
                    print("Server error")
                    message_sent_event.set()
                    sys.exit(0)


                    
            except BlockingIOError:
                pass
                # print("No data available")

        else:
            # print("Switching to user send mode")
            if(send_message(server) == -1):
                break
            else:
                thread_event.clear()
                message_sent_event.set()




# Communicate with server
while True:
    print("Here")
    events = selector_object.select()
    for key, mask in events:
        # if mask & selectors.EVENT_WRITE:
        if client_state == STATE_SEND_LOGIN_DETAILS:
            user_option = serve_authentication_options()
            server_response = send_login_signup_details(user_option)

            if(server_response.strip() == "1"):
                print("Success")

                client_state = STATE_SELECT_CHAT
            else:
                print("Error! Wrong details, closing connection")
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

            chat = server.recv(2048).decode()
            hold_string = ""
            while(chat != "/00/"):
                hold_string += chat
                hold_string.strip("\n")
                if(hold_string[-4:-1] == "/00"):
                    break
                chat = server.recv(2048).decode()

            # Split received chat content by delimiter
            chat_content_list = []
            chat_content_list = hold_string.split("ld00")
            print(chat_content_list)
            try:
                chat_content_list.remove("/00/")
            except:
                pass
            print("Showing chat history with", selected_username)
            if chat_content_list == [""]:
                print("This user has not chatted with you yet")
                
            else:
                for s in chat_content_list :
                    if s.strip() == "EMPTY CHAT":
                        print("This user has not chatted with you yet")
                        break
                    if s.strip() != "":
                        print(">>", s)

            #Pass control over to chat functionality
            client_state = STATE_USER_CHAT

        elif client_state == STATE_USER_CHAT:
            #Client should be attempting to read from server until user wants to send a message
            #Before client sends a message to the server, should send primer message (chat_in_progress)
            #Server will stay in loop until user wants to exit the chat 
            #   Will allow user to logout or select new chat

            #To achieve this, a multithreaded approach will be used
            #First step: Create function to run in thread (done above receive_replies())
            #Second step: Create thread event to monitor for user entry eevnt 
            thread_event = threading.Event()

            # Message sent event to return the client to a state of listening for server replies
            message_sent_event = threading.Event()

            #Third step: Create thread to run receive replies function
            send_message_thread = threading.Thread(name = "send_message_thread",
                                                   target = receive_replies, 
                                                   args=(thread_event, message_sent_event, server, ))
            
            #Fourth step: Start thread
            send_message_thread.start()

            #Fifth step: Run loop to monitor for user input
            while True:
                if end_program:
                    break
                while input().lower() != "send":
                    print("Still receiving")
                    thread_event.clear()

                # print("User break detected")
                thread_event.set()

                message_sent_event.wait()
                message_sent_event.clear()
                    
                
                    
            
            #Sixth step: Join thread to main execution thread to finish running program    
            send_message_thread.join()


        
            
            print("Ending program, closing connection")
            sys.exit(0)
        #chat_in_progress
        




            

            
        #Scaffolding
        print("Current state >> ", client_state)
    
        # sys.exit(0)