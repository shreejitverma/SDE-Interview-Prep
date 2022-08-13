'''
https://www.geeksforgeeks.org/builder-method-python-design-patterns/
Builder Method is a Creation Design Pattern which aims to “Separate the construction of a complex object from its representation so that the same construction process can create different representations.” It allows you to construct complex objects step by step. Here using the same construction code, we can produce different types and representations of the object easily.
 It is basically designed to provide flexibility to the solutions to various object creation problems in object-oriented programming.

UML Diagram of Builder Design Pattern

Builder-method
Problem without using the Builder Method:
Imagine you want to join one of the elite batches of GeeksforGeeks. So, you will go there and ask about the Fee structure, timings available, and batches about the course you want to join. After looking at the system, they will tell you about the courses, their Fee structures, timings available and batches. That’s it! (No! we are not done yet because we are good developers).
Our main purpose is to design the system flexible, reliable, organized and lubricative. what Unexperienced developers will do is that they will create a separate and unique class for each and every course provided by GeeksforGeeks. Then they will create separate object instantiation for each and every class although which is not required every time. The main problem will arise when GeeksforGeeks will start new courses and developers have to add new classes as well because their code is not much flexible.

problem-builder-Method
Note: Following code is written without using the Builder Method.'''

# concrete course


class DSA():

    """Class for Data Structures and Algorithms"""

    def Fee(self):
        self.fee = 8000

    def available_batches(self):
        self.batches = 5

    def __str__(self):
        return "DSA"

# concrete course


class SDE():

    """Class for Software development Engineer"""

    def Fee(self):
        self.fee = 10000

    def available_batches(self):
        self.batches = 4

    def __str__(self):
        return "SDE"

# concrete course


class STL():

    """class for Standard Template Library of C++"""

    def Fee(self):
        self.fee = 5000

    def available_batches(self):
        self.batches = 7

    def __str__(self):
        return "STL"


# main method
if __name__ == "__main__":

    sde = SDE()   # object for SDE
    dsa = DSA()   # object for DSA
    stl = STL()   # object for STL

    print(f'Name of Course: {sde} and its Fee: {sde.fee}')
    print(f'Name of Course: {stl} and its Fee: {stl.fee}')
    print(f'Name of Course: {dsa} and its Fee: {dsa.fee}')
'''Solution by Builder Method:
Our final end product should be any course from GeeksforGeeks. It might be either SDE, STL or DSA. We have to go through many steps before choosing a particular course such as finding details about the courses, syllabus, fee structure, timings, and batches. Here using the same process we can select different courses available at GeeksforGeeks. That’s the benefit of using the builder Pattern.

solution-Builder-Method'''

# Abstract course


class Course:

    def __init__(self):
        self.Fee()
        self.available_batches()

    def Fee(self):
        raise NotImplementedError

    def available_batches(self):
        raise NotImplementedError

    def __repr__(self):
        return 'Fee : {0.fee} | Batches Available : {0.batches}'.format(self)

# concrete course


class DSA(Course):

    """Class for Data Structures and Algorithms"""

    def Fee(self):
        self.fee = 8000

    def available_batches(self):
        self.batches = 5

    def __str__(self):
        return "DSA"

# concrete course


class SDE(Course):

    """Class for Software Development Engineer"""

    def Fee(self):
        self.fee = 10000

    def available_batches(self):
        self.batches = 4

    def __str__(self):
        return "SDE"

# concrete course


class STL(Course):

    """Class for Standard Template Library"""

    def Fee(self):
        self.fee = 5000

    def available_batches(self):
        self.batches = 7

    def __str__(self):
        return "STL"

# Complex Course


class ComplexCourse:

    def __repr__(self):
        return 'Fee : {0.fee} | available_batches: {0.batches}'.format(self)

# Complex course


class Complexcourse(ComplexCourse):

    def Fee(self):
        self.fee = 7000

    def available_batches(self):
        self.batches = 6

# construct course


def construct_course(cls):

    course = cls()
    course.Fee()
    course.available_batches()

    return course    # return the course object


# main method
if __name__ == "__main__":

    dsa = DSA()  # object for DSA course
    sde = SDE()  # object for SDE course
    stl = STL()  # object for STL course

    complex_course = construct_course(Complexcourse)
    print(complex_course)

'''
General Class Diagram for Builder Pattern Method:

General class diagram for builder method
Advantages of using Builder Method:
1. Reusability: While making the various representations of the products, we can use the same construction code for other representations as well.
2. Single Responsibility Principle: We can separate out both the business logic as well as the complex construction code from each other.
3. Construction of the object: Here we construct our object step by step, defer construction steps or run steps recursively.
Disadvantages of using Builder method:
1. Code complexity increases: The complexity of our code increases, because the builder pattern requires creating multiple new classes.
2. Mutability: It requires the builder class to be mutable
3. Initialization: Data members of the class are not guaranteed to be initialized.
Applicability:
1. Constructing Complex objects : The Builder Method allows you to construct the products step-by-step. Even, we can defer the execution of some steps without breaking th efinal product. To create an object tree, it is handy to call the steps recursively.It prevents the client code from fetching the incomplete data because it doesn’t allow the exposing of an unfinished object.
2. Differ by Representations: The Builder pattern is applicable when construction of various representations of the product involves similar steps that differ only in the details. The base builder intterface is used to define all the construction steps while these steps are implemented by concrete builders.

'''
