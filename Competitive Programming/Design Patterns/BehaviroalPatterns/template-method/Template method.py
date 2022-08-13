'''
https://www.geeksforgeeks.org/template-method-python-design-patterns/
The Template method is a Behavioral Design Pattern that defines the skeleton of the operation and leaves the details to be implemented by the child class. Its subclasses can override the method implementations as per need but the invocation is to be in the same way as defined by an abstract class. It is one of the easiest among the Behavioral design pattern to understand and implements. Such methods are highly used in framework development as they allow us to reuse the single piece of code at different places by making certain changes. This leads to avoiding code duplication also.
 
Problem without using Template Method
Imagine you are working on a Chatbot application as a software developer which uses data mining techniques to analyze the data of the corporate documents. Initially, your applications were fine with the pdf version of the data only but later your applications also require to collect and convert data from other formats also such as XML, CSV, and others. After implementing the whole scenario for the other formats also, you noticed that all the classes have lots of similar code. Part of the code like analyzing and processing was identical in almost all classes whereas they differ in dealing with the data.
 

Problem-Template-Method
 

Solution using Template Method
Let’s discuss the solution to the above-described problem using the template method. It suggests to break down the code into a series of steps and convert these steps into methods and put series call inside the template_function. Hence we created the template_function separately and create methods such as get_xml, get_pdf and get_csvfor dealing with the code separately.
 
• Python3
'''
""" method to get the text of file"""


def get_text():

    return "plain_text"


""" method to get the xml version of file"""


def get_xml():

    return "xml"


""" method to get the pdf version of file"""


def get_pdf():

    return "pdf"


"""method to get the csv version of file"""


def get_csv():

    return "csv"


"""method used to convert the data into text format"""


def convert_to_text(data):

    print("[CONVERT]")
    return "{} as text".format(data)


"""method used to save the data"""


def saver():

    print("[SAVE]")


"""helper function named as template_function"""


def template_function(getter, converter=False, to_save=False):
    """input data from getter"""
    data = getter()
    print("Got `{}`".format(data))

    if len(data) <= 3 and converter:
        data = converter(data)
    else:
        print("Skip conversion")

    """saves the data only if user want to save it"""
    if to_save:
        saver()

    print("`{}` was processed".format(data))


"""main method"""
if __name__ == "__main__":

    template_function(get_text, to_save=True)

    template_function(get_pdf, converter=convert_to_text)

    template_function(get_csv, to_save=True)

    template_function(get_xml, to_save=True)
'''
Output
Got `plain_text`
Skip conversion
[SAVE]
`plain_text` was processed
Got `pdf`
[CONVERT]
`pdf as text` was processed
Got `csv`
Skip conversion
[SAVE]
`csv` was processed
Class Diagram
Following is the class diagram for the Template Method
 

Class-diagram-template-method
 
Advantages
 
• Equivalent Content: It’s easy to consider the duplicate code in the superclass by pulling it there where you want to use it.
• Flexibility: It provides vast flexibility such that subclasses are able to decide how to implement the steps of the algorithms.
• Possibility of Inheritance: We can reuse our code as the Template Method uses inheritance which provides the ability of code reusability.
 
Disadvantages
 
• Complex Code: The code may become enough complex sometimes while using the template method such that it becomes too much hard to understand the code even by the developers who are writing it.
• Limitness: Clients may ask for the extended version because sometimes they feel lack of algorithms in the provided skeleton.
• Violation: It might be possible that by using Template method, you may end up with violating the Liskov Substitution Principle which is definitely not the good thing to follow.
 
Applicability
 
• Extension by Clients: This method is always preferred to use when you want to let clients extend the algorithm using particular steps but with not the whole structure of the algorithm.
• Similar Algorithms: When you have a lot of similar algorithms with minor changes, its always better to use the template design pattern because if some changes occur in the algorithm, then you don’t have to make changes in each algorithm.
• Development of Frameworks: It is highly recommended to use the template design pattern while developing a framework because it will help us to avoid the duplicate code as well as reusing the piece of code again and again by making certain changes.

'''
