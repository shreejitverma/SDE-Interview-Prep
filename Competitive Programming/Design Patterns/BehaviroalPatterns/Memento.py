'''
https://www.geeksforgeeks.org/memento-method-python-design-patterns/
Memento Method is a Behavioral Design pattern which provides the ability to restore an object to its previous state. Without revealing the details of concrete implementations, it allows you to save and restore the previous version of the object. It tries not to disturb the encapsulation of the code and allows you to capture and externalize an object’s internal state.
 
Problem without using Memento Method
Imagine you are a student who wants to excel in the world of competitive programming but you are facing one problem i.e., finding a nice Code Editor for programming but none of the present code editors fulfills your needs so you trying to make one for yourself. One of the most important features of any Code Editor is UNDO and REDO which essentially you also need. As an inexperienced developer, you just used the direct approach of storing all the performed actions. Of course, this method will work but Inefficiently!
 

Memento-Problem-diagram
 

Solution by Memento Method
Let’s discuss the solution to the above-discussed problem. The whole problem can be easily resolved by not disturbing the encapsulation of the code. The problem arises when some objects try to perform extra tasks that are not assigned to them, and due to which they invade the private space of other objects. The Memento pattern represents creating the state snapshots to the actual owner of that state, the originator object. Hence, instead of other objects trying to copy the editor’s state from the “outside, ” the editor class itself can make the snapshot since it has full access to its own state.
According to the pattern, we should store the copy of the object’s state in a special object called Memento and the content of the memento objects are not accessible to any other object except the one that produced it.
 
• Python3
'''
"""Memento class for saving the data"""


class Memento:

    """Constructor function"""

    def __init__(self, file, content):
        """put all your file content here"""

        self.file = file
        self.content = content


"""It's a File Writing Utility"""


class FileWriterUtility:

    """Constructor Function"""

    def __init__(self, file):
        """store the input file data"""
        self.file = file
        self.content = ""

    """Write the data into the file"""

    def write(self, string):
        self.content += string

    """save the data into the Memento"""

    def save(self):
        return Memento(self.file, self.content)

    """UNDO feature provided"""

    def undo(self, memento):
        self.file = memento.file
        self.content = memento.content


"""CareTaker for FileWriter"""


class FileWriterCaretaker:

    """saves the data"""

    def save(self, writer):
        self.obj = writer.save()

    """undo the content"""

    def undo(self, writer):
        writer.undo(self.obj)


if __name__ == '__main__':

    """create the caretaker object"""
    caretaker = FileWriterCaretaker()

    """create the writer object"""
    writer = FileWriterUtility("GFG.txt")

    """write data into file using writer object"""
    writer.write("First vision of GeeksforGeeks\n")
    print(writer.content + "\n\n")

    """save the file"""
    caretaker.save(writer)

    """again write using the writer """
    writer.write("Second vision of GeeksforGeeks\n")

    print(writer.content + "\n\n")

    """undo the file"""
    caretaker.undo(writer)

    print(writer.content + "\n\n")
'''
UML Diagram
Following is the UML diagram for Memento’s Method


Memento-method-UML-Diagram
 
Advantages
 
• Encourages Encapsulation: Memento method can help in producing the state of the object without breaking the encapsulation of the client’s code.
• Simplifies Code: We can take the advantage of caretaker who can help us in simplifying the code by maintaining the history of the originator’s code.
• Generic Memento’s Implementation: It’s better to use Serialization to achieve memento pattern implementation that is more generic rather than Memento pattern where every object needs to have it’s own Memento class implementation.
 
Disadvanatges
 
• Huge Memory Consumption: If the Originator’s object is very huge then Memento object size will also be huge and use a lot of memory which is definitely not the efficient way to do the work.
• Problem with Dynamic Languages: Programming languages like Ruby, Python, and PHP are dynamically typed langauges, can’t give the guarantee that the memento object will not be touched.
• Difficult Deletion: It’s not easy to delete the memento object because the caretaker has to track the originator’s lifecycle inorder to get th result.
 
Applicability
 
• UNDO and REDO:Most of the software applications like Paint, Coding IDEs, text editor, and many others provide UNDO and REDO features for the ease of client.
• Providing Encapsulation: We can use the Memento’s method for avoiding the breakage of encapsulation in the client’s code which might be produced by direct access to the object’s internal implementation.

'''
