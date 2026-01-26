# Author: Shreejit Verma
# GitHub: https://github.com/shreejitverma
#
# Topic: Observer Pattern
# Description: A behavioral design pattern where an object (Subject) maintains a list of dependents (Observers)
#              and notifies them of any state changes. Common in Event Handling systems.

class Subject:
    def __init__(self):
        self._observers = []
    
    def attach(self, observer):
        if observer not in self._observers:
            self._observers.append(observer)
    
    def detach(self, observer):
        try:
            self._observers.remove(observer)
        except ValueError:
            pass
            
    def notify(self, message):
        for observer in self._observers:
            observer.update(message)

class Observer:
    def update(self, message):
        pass

# Concrete Subject
class NewsAgency(Subject):
    def add_news(self, news):
        print(f"News Agency: Breaking News -> {news}")
        self.notify(news)

# Concrete Observers
class MobileApp(Observer):
    def update(self, message):
        print(f"MobileApp Notification: New article available - {message}")

class EmailSubscriber(Observer):
    def update(self, message):
        print(f"Email Alert: You have a new message - {message}")

if __name__ == "__main__":
    agency = NewsAgency()
    
    app_user = MobileApp()
    email_user = EmailSubscriber()
    
    agency.attach(app_user)
    agency.attach(email_user)
    
    agency.add_news("Tech Stocks Rally!")
    
    print("\n--- Detaching Email User ---\n")
    agency.detach(email_user)
    
    agency.add_news("New AI Model Released")
