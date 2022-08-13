'''
https://www.geeksforgeeks.org/chain-of-responsibility-python-design-patterns/
Chain of Responsibility method is Behavioral design pattern and it is the object-oriented version of if … elif … elif … else and make us capable to rearrange the condition-action blocks dynamically at the run-time. It allows us to pass the requests along the chain of handlers. The processing is simple, whenever any handler received the request it has two choices either to process it or pass it to the next handler in the chain. 
This pattern aims to decouple the senders of a request from its receivers by allowing the request to move through chained receivers until it is handled. 
 

chain-of-responsibility-method
 
Problem without using Chain of Responsibility Method
Imagine you are building a simple website that takes input strings and tells about the various properties of the strings such as is the string palindrome? is string upperCase? is string lowerCase? and many other properties too. After the complete planning, you decide that these checks for the input string should be performed sequentially. So, here the problem arises for the developer that he/she has to implement such an application that can decide on run-time which action should be performed next.
 

Solution using Chain of Responsibility Method
Chain of Responsibility Method provides the solution for the above-described problem. It creates a separate Abstract handler which is used to handle the sequential operations which should be performed dynamically. For eg., we create four handlers named as FirstConcreteHandler, SecondConcreteHandler, ThirdConcreteHandler, and Defaulthandler and calls them sequentially from the user class.
 
Python3
'''


class AbstractHandler(object):

    """Parent class of all concrete handlers"""

    def __init__(self, nxt):
        """change or increase the local variable using nxt"""

        self._nxt = nxt

    def handle(self, request):
        """It calls the processRequest through given request"""

        handled = self.processRequest(request)

        """case when it is not handled"""

        if not handled:
            self._nxt.handle(request)

    def processRequest(self, request):
        """throws a NotImplementedError"""

        raise NotImplementedError('First implement it !')


class FirstConcreteHandler(AbstractHandler):

    """Concrete Handler # 1: Child class of AbstractHandler"""

    def processRequest(self, request):
        '''return True if request is handled '''

        if 'a' < request <= 'e':
            print("This is {} handling request '{}'".format(
                self.__class__.__name__, request))
            return True


class SecondConcreteHandler(AbstractHandler):

    """Concrete Handler # 2: Child class of AbstractHandler"""

    def processRequest(self, request):
        '''return True if the request is handled'''

        if 'e' < request <= 'l':
            print("This is {} handling request '{}'".format(
                self.__class__.__name__, request))
            return True


class ThirdConcreteHandler(AbstractHandler):

    """Concrete Handler # 3: Child class of AbstractHandler"""

    def processRequest(self, request):
        '''return True if the request is handled'''

        if 'l' < request <= 'z':
            print("This is {} handling request '{}'".format(
                self.__class__.__name__, request))
            return True


class DefaultHandler(AbstractHandler):

    """Default Handler: child class from AbstractHandler"""

    def processRequest(self, request):
        """Gives the message that th request is not handled and returns true"""

        print("This is {} telling you that request '{}' has no handler right now.".format(self.__class__.__name__,
                                                                                          request))
        return True


class User:

    """User Class"""

    def __init__(self):
        """Provides the sequence of handles for the users"""

        initial = None

        self.handler = FirstConcreteHandler(SecondConcreteHandler(
            ThirdConcreteHandler(DefaultHandler(initial))))

    def agent(self, user_request):
        """Iterates over each request and sends them to specific handles"""

        for request in user_request:
            self.handler.handle(request)


"""main method"""

if __name__ == "__main__":

    """Create a client object"""
    user = User()

    """Create requests to be processed"""

    string = "GeeksforGeeks"
    requests = list(string)

    """Send the requests one by one, to handlers as per the sequence of handlers defined in the Client class"""
    user.agent(requests)
'''
Output
This is DefaultHandler telling you that request 'G' has no handler right now.
This is FirstConcreteHandler handling request 'e'
This is FirstConcreteHandler handling request 'e'
This is SecondConcreteHandler handling request 'k'
This is ThirdConcreteHandler handling request 's'
This is SecondConcreteHandler handling request 'f'
This is ThirdConcreteHandler handling request 'o'
This is ThirdConcreteHandler handling request 'r'
This is DefaultHandler telling you that request 'G' has no handler right now.
This is FirstConcreteHandler handling request 'e'
This is FirstConcreteHandler handling request 'e'
This is SecondConcreteHandler handling request 'k'
This is ThirdConcreteHandler handling request 's'
Class Diagram
Class Diagram for Chain of Responibility Method
 

Chain-Of-Responsibility_class_diagram
 
Advantages
 
• Single Responsibility Principle: It’s easy to decouple the classes here that invoke operations from classes that perform operations.
• Open/Closed principle: We can introduce the new code classes without breaking th existing client code.
• Increases Flexibility: While giving the responsibilities to the objects, it increases the flexibility of the code.
 
Disadvantages
 
• Unassured about request: This method doesn’t provide any assurance whether the object will be received or not.
• Spotting characteristics: Due to debugging, it becomes difficult task to observe the characteristics of operations.
• Depriciated System Performance: It might affect the system’s performance due to continuous cycle calls
 
Applicability
 
• Processing several handlers in order: Chain of responsibility method helps very much when it is required to process several handlers in a particular order because the linking is possible in any order
• Decoupling requests: This method is generally used when you want to decouple the request’s sender and receiver.
Unspecified handlers: When you don’t want to specify handlers in the code, it is always preferred to use the Chain of Responsibility.
'''
