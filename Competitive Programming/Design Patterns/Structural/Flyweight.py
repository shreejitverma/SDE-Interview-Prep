'''
https://www.geeksforgeeks.org/flyweight-method-python-design-patterns/
Flyweight method is a Structural Design Pattern that focus on minimizing the number of objects that are required by the program at the run-time. Basically, it creates a Flyweight object which is shared by multiple contexts. It is created in such a fashion that you can not distinguish between an object and a Flyweight Object. One important feature of flyweight objects is that they are immutable. This means that they cannot be modified once they have been constructed.
To implement the Flyweight method in Python, we use Dictionary that stores reference to the object which have already been created, every object is associated with a key.
 
Why we do care for the number of objects in our program ?
Less number of objects reduces the memory usage, and it manages to keep us away from errors related to memory
• Although creating an object in Python is really fast, we can still reduce the execution time of our program by sharing objects.
 

Problem without using Flyweight Method
Imagine you are a game developer who likes Racing Games much and also wants to develop a racing game for you and your friend. As you are a flawless Game developer, you created one and start enjoying the game. Then you sent the game to your friend also but he did not enjoy the game too much because the game kept crashing after every few minutes. 
But Why? ( Guess the reason if you think you are a Pro Game Developer).After debugging for several hours, you found that the issue is lack of RAM on your friend’s system. Your system is much powerful as compared to your friend’s system that’s why the game was running smoothly on your system but not on your friend’s system.
 

Flyweight-problem-Diagram
 
Solution using Flyweight Method
so, what will you do as a developer to improve the performance? (of course! not going to upgrade the RAM).The actual problem is related to car objects because each car is represented by separate objects containing plenty of data related to its color, size, seats, maximum speed, etc. Whenever your RAM got filled and unable to add more new objects which are required currently, your game gets crashed. For avoiding such situations in applications, it is the prior duty of the developer to use Flyweight Methodwhich allows you to fit more objects into the available amount of RAM by sharing common parts of the objects.
 

Following Code is written using the Flyweight method
 
Python3
'''


class ComplexCars(object):

    """Separate class for Complex Cars"""

    def __init__(self):

        pass

    def cars(self, car_name):

        return "ComplexPattern[% s]" % (car_name)


class CarFamilies(object):

    """dictionary to store ids of the car"""

    car_family = {}

    def __new__(cls, name, car_family_id):
        try:
            id = cls.car_family[car_family_id]
        except KeyError:
            id = object.__new__(cls)
            cls.car_family[car_family_id] = id
        return id

    def set_car_info(self, car_info):
        """set the car information"""

        cg = ComplexCars()
        self.car_info = cg.cars(car_info)

    def get_car_info(self):
        """return the car information"""

        return (self.car_info)


if __name__ == '__main__':
    car_data = (('a', 1, 'Audi'), ('a', 2, 'Ferrari'), ('b', 1, 'Audi'))
    car_family_objects = []
    for i in car_data:
        obj = CarFamilies(i[0], i[1])
        obj.set_car_info(i[2])
        car_family_objects.append(obj)

    """similar id's says that they are same objects """

    for i in car_family_objects:
        print("id = " + str(id(i)))
        print(i.get_car_info())
'''
Output
id = 58598800
ComplexPattern[Audi]
id = 58598832
ComplexPattern[Ferrari]
id = 58598800
ComplexPattern[Audi]
Class diagram
Following is the class diagram for the Flyweight method
 

Flyweight-class-diagram
 
Advantages
 
• Reduced use of RAM: when we have a lot of similar objects present in our application, its always better to use Flyweight method inorder to save a lot of space in RAM
• Improved Data Caching: When the need of client or user is High response time, it is always preferred to use Flyweight method because it helps in improving the Data Caching.
• Improved performance: It ultimately leads to improve in performance because we are using less number of heavy objects.
 
Disadvantages
 
• Breaking Encapsulation: Whenever we try to move the state outside the object, we do breaking of encapsulation and may become less efficient then keeping the state inside the object.
• Hard to handle: Usage of Flyweight method depends upon the language we use, easy to use in language like Python, Java where all object variables are references but typical to use in language like C, C++ where objects can be allocated as local variables on the stack and destroyed as a result of programmer action.
• Complicated Code: Using Flyweight method always increases the complexity of the code to understand for the new developers.
 
Applicability
 
• To Reduce the number of Objects: Generally, Flyweight method is used when our application has a lot of heavy weight objects, to solve this problem we use Flyweight method to get rid of unnecessary memory consumption.
• Object independent Applications: When our application if independent of the object created, then we can make use of this method inorder to save lot of machine space.
• Project Cost Reduction: When it is required to reduce the cost of project in terms of space and time complexity, it is always preferred to use the Flyweight method.
'''
