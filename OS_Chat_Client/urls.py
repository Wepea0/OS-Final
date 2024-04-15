from django.urls import path
from . import views


urlpatterns = [
    path('home/',views.home_screen),
    path('login/',views.login_page),
    path('signup/',views.signup_page),
    path('home/choice',views.home_screen_choice),
    path('chatscreen/',views.chat_screen)
]