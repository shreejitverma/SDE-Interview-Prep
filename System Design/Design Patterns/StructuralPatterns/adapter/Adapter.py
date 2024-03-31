'''
https://www.geeksforgeeks.org/adapter-method-python-design-patterns/
Adapter method is a Structural Design Pattern which helps us in making the incompatible objects adaptable to each other. The Adapter method is one of the easiest methods to understand because we have a lot of real-life examples that show the analogy with it. The main purpose of this method is to create a bridge between two incompatible interfaces. This method provides a different interface for a class. We can more easily understand the concept by thinking about the Cable Adapter that allows us to charge a phone somewhere that has outlets in different shapes.
Using this idea, we can integrate the classes that couldn’t be integrated due to interface incompatibility.
Problem without using the Adapter Method
Imagine you are creating an application that shows the data about all different types of vehicles present. It takes the data from APIs of different vehicle organizations in XML format and then displays the information.
But suppose at some time you want to upgrade your application with a Machine Learning algorithms that work beautifully on the data and fetch the important data only. But there is a problem, it takes data in JSON format only.
It will be a really poor approach to make changes in Machine Learning Algorithm so that it will take data in XML format.


Problem-Adapter-Method
Solution using Adapter Method
For solving the problem we defined above, We can use Adapter Method that helps by creating an Adapter object.
To use an adapter in our code:
1. Client should make a request to the adapter by calling a method on it using the target interface.
2. Using the Adaptee interface, the Adapter should translate that request on the adaptee.
3. Result of the call is received the client and he/she is unaware of the presence of the Adapter’s presence.
'''
# Dog - Cycle
# human - Truck
# car - Car


class MotorCycle:

    """Class for MotorCycle"""

    def __init__(self):
        self.name = "MotorCycle"

    def TwoWheeler(self):
        return "TwoWheeler"


class Truck:

    """Class for Truck"""

    def __init__(self):
        self.name = "Truck"

    def EightWheeler(self):
        return "EightWheeler"


class Car:

    """Class for Car"""

    def __init__(self):
        self.name = "Car"

    def FourWheeler(self):
        return "FourWheeler"


class Adapter:
    """
    Adapts an object by replacing methods.
    Usage:
    motorCycle = MotorCycle()
    motorCycle = Adapter(motorCycle, wheels = motorCycle.TwoWheeler)
    """

    def __init__(self, obj, **adapted_methods):
        """We set the adapted methods in the object's dict"""
        self.obj = obj
        self.__dict__.update(adapted_methods)

    def __getattr__(self, attr):
        """All non-adapted calls are passed to the object"""
        return getattr(self.obj, attr)

    def original_dict(self):
        """Print original object dict"""
        return self.obj.__dict__


""" main method """
if __name__ == "__main__":

    """list to store objects"""
    objects = []

    motorCycle = MotorCycle()
    objects.append(Adapter(motorCycle, wheels=motorCycle.TwoWheeler))

    truck = Truck()
    objects.append(Adapter(truck, wheels=truck.EightWheeler))

    car = Car()
    objects.append(Adapter(car, wheels=car.FourWheeler))

    for obj in objects:
        print("A {0} is a {1} vehicle".format(obj.name, obj.wheels()))
'''
Class Diagram
Class diagram for the Adapter method which is a type of Structural Design pattern:

Adapter-class-diagram
Advantages
• Principle of Single Responsibility: We can achieve the principle of Single responsibility with Adapter Method because here we can separate the concrete code from the primary logic of the client.
• Flexibility: Adapter Method helps in achieving the flexibility and reusability of the code.
• Less complicated class: Our client class is not complicated by having to use a different interface and can use polymorphism to swap between different implementations of adapters.
• Open/Closed principle: We can introduce the ne wadapter classes into the code without violating the Open/Closed principle.
Disadvantages
• Complexity of the Code: As we have introduced the set of new classes, objects and interfaces, the complexity of or code definitely rises.
• Adaptability: Most of the times, we require many adaptations with the adaptee chain to reach the compatibility what we want.
Applicability
• To make classes and interfaces compatible : Adapter method is always used when we are in need to make certain classes compatible to communicate.
• Relatable to Inheritance: When we want to reuse some piece of code i.e., classes and interfaces that lack some functionalities, it can be done using the Adapter Method.
'''
