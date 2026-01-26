# Author: Shreejit Verma
 # GitHub: https://github.com/shreejitverma

from django.apps import AppConfig


class UsersConfig(AppConfig):
    name = 'users'

    def ready(self):
        import users.signals
