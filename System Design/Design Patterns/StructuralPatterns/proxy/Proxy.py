'''
https://www.geeksforgeeks.org/proxy-method-python-design-patterns/
The Proxy method is Structural design pattern that allows you to provide the replacement for an another object. Here, we use different classes to represent the functionalities of another class. The most important part is that here we create an object having original object functionality to provide to the outer world.
The meaning of word Proxy is “in place of” or “on behalf of” that directly explains the Proxy Method.
Proxies are also called surrogates, handles, and wrappers. They are closely related in structure, but not purpose, to Adapters and Decorators.


proxy-design-method
A real-world example can be a cheque or credit card is a proxy for what is in our bank account. It can be used in place of cash and provides a means of accessing that cash when required. And that’s exactly what the Proxy pattern does – “Controls and manage access to the object they are protecting“.
Problem Without Using Proxy method
Let’s understand the problem by considering the example of the College’s Database which takes care of all the student’s records. For e.g., we need to find the name of those students from Database whose balance fee if greater than 500. So, if we traverse the whole list of students and for each student object if we make a separate connection to the database then it will be proved as an expensive task.

Proxy-method-Problem
Solution using Proxy Method
Here comes the Proxy Method to solve the above-discussed problem. We will create a proxy server or maybe a proxy connection to the database and after that, we don’t have to create separate connections to the database for each student object.
We will simply get our needed data using the proxy without wasting a huge amount of memory for the creation of the object.

Proxy-method-Solution
'''


class College:
    '''Resource-intensive object'''

    def studyingInCollege(self):
        print("Studying In College....")


class CollegeProxy:
    '''Relatively less resource-intensive proxy acting as middleman.
     Instantiates a College object only if there is no fee due.'''

    def __init__(self):

        self.feeBalance = 1000
        self.college = None

    def studyingInCollege(self):

        print(
            "Proxy in action. Checking to see if the balance of student is clear or not...")
        if self.feeBalance <= 500:
            # If the balance is less than 500, let him study.
            self.college = College()
            self.college.studyingInCollege()
        else:

            # Otherwise, don't instantiate the college object.
            print("Your fee balance is greater than 500, first pay the fee")


"""main method"""

if __name__ == "__main__":

    # Instantiate the Proxy
    collegeProxy = CollegeProxy()

    # Client attempting to study in the college at the default balance of 1000.
    # Logically, since he / she cannot study with such balance,
    # there is no need to make the college object.
    collegeProxy.studyingInCollege()

    # Altering the balance of the student
    collegeProxy.feeBalance = 100

    # Client attempting to study in college at the balance of 100. Should succeed.
    collegeProxy.studyingInCollege()
'''
Output:
Proxy in action. Checking to see if the balance of student is clear or not...
Your fee balance is greater than 500, first pay the fee
Proxy in action. Checking to see if the balance of student is clear or not...
Studying In College....
Class Diagram
Following is the class diagram for the Proxy Method:

Proxy-method-Class-Diagram
Advantages
• Open/Closed Principle: Without changing the client code, we can easily introduce the new proxies in our application.
• Smooth Service: The proxy that we creates works even when the service object is not ready or is not available at the current scenario.
• Security: Proxy method also provides the security to the system.
• Performance: It increases the performance of the application by avoiding the duplication of the objects which might be huge size and memory intensive.
Disadvantages
• Slow response: It is possible that the service might get slow or delayed.
• Layer of Abstraction: This pattern introduces another layer of abstraction which sometimes may be an issue if the RealSubject code is accessed by some of the clients directly and some of them might access the Proxy classes
• Complexity Increases: Our Code may become highly complicated due to the introduction of lot of new classes.
Applicability
• Virtual Proxy: Most importantly used in Databases for example there exist certain heavy resource consuming data in the database and we need it frequently. so. here we can use the proxy pattern which would create multiple proxies and point to the object.
• Protective Proxy: It creates a protective layer over the application and can be used in the scenarios of Schools or Colleges where only a few no. of websites are allowed to open with there WiFi.
• Remote Proxy: It is particularly used when the service object is located on a remote server. In such cases, the proxy passes the client request over the network handling all the details.
• Smart proxy: It is used to provide the additional security to the application by intervene specific actions whenever the object would be accessed.
'''
