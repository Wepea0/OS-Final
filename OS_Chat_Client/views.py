from django.shortcuts import render
from django.http import HttpResponse
from . import helpers
from django.views.decorators.csrf import csrf_exempt
import json

# Create your views here.

def home_screen(request):

    return render(request,'home_screen.html')

def login_page(request):
    user_ip = helpers.get_ip_address()
    return_data = {'user_ip':user_ip}
    return render(request, 'login_screen.html',return_data)

@csrf_exempt
def signup_page(request):
    """ Returns the sign up page with the users ip"""
    user_ip = helpers.get_ip_address()
    return_data = {'user_ip': user_ip}
    return render(request, 'signup_screen.html',return_data)

@csrf_exempt
def home_screen_choice(request):
    """Redirects the user to the login or signup screen"""

    # Decode json and get choice
    rawJson = request.body.decode('utf-8')
    data = json.loads(rawJson)
    choice = data.get("choice")
    print("choice was->", choice)
    if choice == 1:
        print("user requested to signup")

        # TODO reply with 1 to server
        return signup_page(request)
    elif choice == 2:
        print("user requested to login")

        # TODO reply with 1 to server
        return login_page(request)



def chat_screen(request):
    return render(request, 'chat_screen.html')