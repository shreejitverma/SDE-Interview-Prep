'''https://www.geeksforgeeks.org/decorator-method-python-design-patterns/
Decorator Method is a Structural Design Pattern which allows you to dynamically attach new behaviors to objects without changing their implementation by placing these objects inside the wrapper objects that contains the behaviors.
It is much easier to implement Decorator Method in Python because of its built-in feature. It is not equivalent to the Inheritance because the new feature is added only to that particular object, not to the entire subclass.
Problem Without Using Decorator Method
Imagine We are working with a formatting tool that provides features likes Bold the text and Underlines the text. But after some time, our formatting tools got famous among the targetted audience and on taking the feedback we got that our audience wants more features in the application such as making the text Italic and many other features.
Looks Simple? It’s not the easy task to implement this or to extend our classes to add more functionalities without disturbing the existing client code because we have to maintain the Single Responsibility Principle.

Solution Using Decorator Method
Now let’s look at the solution that we have to avoid such conditions. Initially, we have only WrittenText but we have to apply filters like BOLD, ITALIC, UNDERLINE. So, we will create separate wrapper classes for each function like BoldWrapperClass, ItalicWrapperClass, and UnderlineWrapperclass.

Decorator-Written-Text
First, we will call BoldWrapperclass over the Written text which ultimately converts the text into BOLD letters

Decorator-Wrapper
Then we will apply the ItalicWrapperClass and UnderlineWrapperClass over the Bold text which will give us the result what we want.

Decorator-Underline
Following Code is written using Decorator Method:
'''


class WrittenText:

    """Represents a Written text """

    def __init__(self, text):
        self._text = text

    def render(self):
        return self._text


class UnderlineWrapper(WrittenText):

    """Wraps a tag in <u>"""

    def __init__(self, wrapped):
        self._wrapped = wrapped

    def render(self):
        return "<u>{}</u>".format(self._wrapped.render())


class ItalicWrapper(WrittenText):

    """Wraps a tag in <i>"""

    def __init__(self, wrapped):
        self._wrapped = wrapped

    def render(self):
        return "<i>{}</i>".format(self._wrapped.render())


class BoldWrapper(WrittenText):

    """Wraps a tag in <b>"""

    def __init__(self, wrapped):
        self._wrapped = wrapped

    def render(self):
        return "<b>{}</b>".format(self._wrapped.render())


""" main method """

if __name__ == '__main__':

    before_gfg = WrittenText("GeeksforGeeks")
    after_gfg = ItalicWrapper(UnderlineWrapper(BoldWrapper(before_gfg)))

    print("before :", before_gfg.render())
    print("after :", after_gfg.render())
'''
Class-Diagram for Decorator Method
Following is the class diagram for Decorator Method:

Decorator-Class-Diagram
Advantages
• Single Responsibility Principle: It is easy to divide a monolithic class which implements many possible variants of behavior into several classes using the Decorator method.
• Runtime Responsibilities: We can easily add or remove the responsbilites from an object at runtime.
• SubClassing: The decorator pattern is an alternative to subclassing. Subclassing adds behavior at compile time, and the change affects all instances of the original class; decorating can provide new behavior at runtime for individual objects.
Disadvantages
1. removing Wrapper: It is very hard to remove a particular wrapper from the wrappers stack.
2. Complicated Decorators: It can be complicated to have decorators keep track of other decorators, because to look back into multiple layers of the decorator chain starts to push the decorator pattern beyond its true intent.
3. Ugly Configuration: Large number of code of layers might make the configurations ugly.
Applicability
1. Incapable Inheritance: Generally, Decorator method is used when it is not possible to extend the behavior of an object using the Inheritance.
2. Runtime Assignment: One of the most important feature of Decorator method is to assign different and unique behaviors to the object at the Runtime.
'''
