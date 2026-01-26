# Author: Shreejit Verma
 # GitHub: https://github.com/shreejitverma

from django.urls import path
from . import views

urlpatterns = [
    path('', views.home, name='blog-home'),
    path('about/', views.about, name='blog-about'),
]
