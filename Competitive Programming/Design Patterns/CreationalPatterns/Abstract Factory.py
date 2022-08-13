'''https://www.geeksforgeeks.org/abstract-factory-method-python-design-patterns/
Abstract Factory Method – Python Design Patterns
• Difficulty Level : Medium
• Last Updated : 30 Sep, 2021
Abstract Factory Method is a Creational Design pattern that allows you to produce the families of related objects without specifying their concrete classes. Using the abstract factory method, we have the easiest ways to produce a similar type of many objects. 
It provides a way to encapsulate a group of individual factories. Basically, here we try to abstract the creation of the objects depending on the logic, business, platform choice, etc.
Problem we face without Abstract Factory Method:
Imagine you want to join one of the elite batches of GeeksforGeeks. So, you will go there and ask about the Courses available, their Fee structure, their timings, and other important things. They will simply look at there system and will give you all the information you required. Looks simple? Think about the developers how they make the system so organized and how their website is so lubricative.
 Developers will make unique classes for each course which will contain its properties like Fee structure, timings, and other things. But how they will call them and how will they instantiate their objects?

Here arises the problem, suppose initially there are only 3-4 courses available at GeeksforGeeks, but later they added 5 new courses. 
So, we have to manually instantiate their objects which is not a good thing according to the developer’s side. 

Abstract Factory
Diagrammatic representation of Problems without using Abstract Factory Method
Note: Following code is written without using the abstract factory method 
• Python3
'''
# Python Code for object
# oriented concepts without
# using the Abstract factory
# method in class

import random


class DSA:

    """ Class for Data Structure and Algorithms """

    def price(self):
        return 11000

    def __str__(self):
        return "DSA"


class STL:

    """Class for Standard Template Library"""

    def price(self):
        return 8000

    def __str__(self):
        return "STL"


class SDE:

    """Class for Software Development Engineer"""

    def price(self):
        return 15000

    def __str__(self):
        return 'SDE'


# main method
if __name__ == "__main__":

    sde = SDE()    # object for SDE class
    dsa = DSA()    # object for DSA class
    stl = STL()    # object for STL class

    print(f'Name of the course is {sde} and its price is {sde.price()}')
    print(f'Name of the course is {dsa} and its price is {dsa.price()}')
    print(f'Name of the course is {stl} and its price is {stl.price()}')
'''
Solution by using Abstract Factory Method:
Its solution is to replace the straightforward object construction calls with calls to the special abstract factory method. Actually, there will be no difference in the object creation but they are being called within the factory method. 
Now we will create a unique class whose name is Course_At_GFG which will handle all the object instantiation automatically. Now, we don’t have to worry about how many courses we are adding after some time.

solution using abstract factory pattern
• Python3'''

# Python Code for object
# oriented concepts using
# the abstract factory
# design pattern


class Course_At_GFG:

    """ GeeksforGeeks portal for courses """

    def __init__(self, courses_factory=None):
        """course factory is out abstract factory"""

        self.course_factory = courses_factory

    def show_course(self):
        """creates and shows courses using the abstract factory"""

        course = self.course_factory()

        print(f'We have a course named {course}')
        print(f'its price is {course.Fee()}')


class DSA:

    """Class for Data Structure and Algorithms"""

    def Fee(self):
        return 11000

    def __str__(self):
        return "DSA"


class STL:

    """Class for Standard Template Library"""

    def Fee(self):
        return 8000

    def __str__(self):
        return "STL"


class SDE:

    """Class for Software Development Engineer"""

    def Fee(self):
        return 15000

    def __str__(self):
        return 'SDE'


def random_course():
    """A random class for choosing the course"""

    return random.choice([SDE, STL, DSA])()


if __name__ == "__main__":

    course = Course_At_GFG(random_course)

    for i in range(5):
        course.show_course()

'''
Class Diagram for Abstract Factory Method:
Let’s look at the class diagram considering the example of Courses at GeeksforGeeks. 
Fee structure of all the available courses at GeeksforGeeks 
 

class diagram for the abstract factory pattern
Timings of all the available courses at GeeksforGeeks 
 

class diagram 2 abstract method pattern
Advantages of using Abstract Factory method:
This pattern is particularly useful when the client doesn’t know exactly what type to create. 
1. It is easy to introduce new variants of the products without breaking the existing client code.
2. Products which we are getting from the factory are surely compatible with each other.
Disadvantages of using Abstract Factory method:
1. Our simple code may become complicated due to the existence of a lot of classes.
2. We end up with a huge number of small files i.e, cluttering of files.
Applicability:
1. Most commonly, abstract factory method pattern is found in the sheet metal stamping equipment used in the manufacture of automobiles.
2. It can be used in a system that has to process reports of different categories such as reports related to input, output, and intermediate transactions.
'''
