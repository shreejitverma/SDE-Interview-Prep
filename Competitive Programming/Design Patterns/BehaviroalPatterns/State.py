'''
https://www.geeksforgeeks.org/state-method-python-design-patterns/
State method is Behavioral Design Pattern that allows an object to change its behavior when there occurs a change in its internal state. It helps in implementing the state as a derived class of the state pattern interface. If we have to change the behavior of an object based on its state, we can have a state variable in the Object and use if-else condition block to perform different actions based on the state. It may be termed as the object-oriented state machine. It implements the state transitions by invoking methods from the pattern’s superclass.
Problem without using State Method
The State method pattern represents the Finite State Machine.


Finite-state-machine-diagram
At any moment, there can be a finite no. of states that can be present in the program. Each and every state is unique in their kind of behavior and other things. Even the program can change itself from one state to another at any moment of time. A program can go from one state to another if and only if the required transition is available in rules. It will certainly become difficult when we add a large number of states. It will become difficult to handle the code as any small change in transition logic may lead to a change in state conditionals in each method. =
Solution using State Method
Let’s have a look at the solution to the above-described problem by considering the example of Radio. Here the radio has two states named as Am State and FM State. We can use the switch to toggle between these two states. The State method suggests that we should create a new class for all possible states of an object and extract all state-specific behaviors into these classes.
Instead of implementing all behaviors on its own, the original object called context, stores a reference to one of the state objects that represents its current state, and represents all the state-related work to that object.

Problem-without-State-Method
'''
"""State class: Base State class"""


class State:

    """Base state. This is to share functionality"""

    def scan(self):
        """Scan the dial to the next station"""
        self.pos += 1

        """check for the last station"""
        if self.pos == len(self.stations):
            self.pos = 0
        print("Visiting... Station is {} {}".format(
            self.stations[self.pos], self.name))


"""Separate Class for AM state of the radio"""


class AmState(State):

    """constructor for AM state class"""

    def __init__(self, radio):

        self.radio = radio
        self.stations = ["1250", "1380", "1510"]
        self.pos = 0
        self.name = "AM"

    """method for toggling the state"""

    def toggle_amfm(self):
        print("Switching to FM")
        self.radio.state = self.radio.fmstate


"""Separate class for FM state"""


class FmState(State):

    """Constriuctor for FM state"""

    def __init__(self, radio):
        self.radio = radio
        self.stations = ["81.3", "89.1", "103.9"]
        self.pos = 0
        self.name = "FM"

    """method for toggling the state"""

    def toggle_amfm(self):
        print("Switching to AM")
        self.radio.state = self.radio.amstate


"""Dedicated class Radio"""


class Radio:

    """A radio. It has a scan button, and an AM / FM toggle switch."""

    def __init__(self):
        """We have an AM state and an FM state"""
        self.fmstate = FmState(self)
        self.amstate = AmState(self)
        self.state = self.fmstate

    """method to toggle the switch"""

    def toggle_amfm(self):
        self.state.toggle_amfm()

    """method to scan """

    def scan(self):
        self.state.scan()


""" main method """
if __name__ == "__main__":

    """ create radio object"""
    radio = Radio()
    actions = [radio.scan] * 3 + [radio.toggle_amfm] + [radio.scan] * 3
    actions *= 2

    for action in actions:
        action()
'''
Output
Visiting... Station is 89.1 FM
Visiting... Station is 103.9 FM
Visiting... Station is 81.3 FM
Switching to AM
Visiting... Station is 1380 AM
Visiting... Station is 1510 AM
Visiting... Station is 1250 AM
Visiting... Station is 1380 AM
Visiting... Station is 1510 AM
Visiting... Station is 1250 AM
Switching to FM
Visiting... Station is 89.1 FM
Visiting... Station is 103.9 FM
Visiting... Station is 81.3 FM
UML Diagram
Following is the UML diagram for the state method

UML-diagram-state-method
Advantages
• Open/Closed Principle: We can easily introduce the new states without changing the content of existing states of client’s code.
• Single Responsibility Principle: It helps in organizing the code of particular states into the separate classes which helps in making the code feasible for the other depelopers too.
• Improves Cohesion: It also improves the Cohesion since state-specific behaviors are aggregated into the ConcreteState classes, which are placed in one location in the code.
Disadvantages
• Making System complex: If a system has only a few number of states, then its not a good choice to use the State Method as you will end up with adding unneccessary code.
• Changing states at run-time: State method is used when we need to change the state at run-time by inputting at different sub-classes, this will be considered as a disadvantage as well because because we have a clear separate State classes with some logic and from the other hand the number of classes grows up.
• Sub-Classes Dependencies: Here the each state derived class is coupled to its sibling, which directly or indirectly introduces the dependencies between sub-classes.

'''
