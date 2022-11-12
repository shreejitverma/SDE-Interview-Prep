#include <iostream>
using namespace std;

class IButton{
public:
    virtual void press() = 0;
};

class MacButton : public IButton{
public:
    void press(){
        cout << "Mac Button pressed" << endl;
    }
};

class WindowsButton : public IButton{
public:
    void press(){
        cout << "Windows Button pressed" << endl;
    }
};

class ITextBox
{
public:
    virtual void showText() = 0;
};

class MacTextBox : public ITextBox
{
public:
    void showText(){
        cout << "Mac TextBox" << endl;
    }
};

class WindowsTextBox : public ITextBox
{
public:
    void showText(){
        cout << "Windows TextBox" << endl;
    }
};

class IFactory{
public:
    virtual IButton* CreateButton() = 0;
    virtual ITextBox *CreateTextBox() = 0;
};

class MacFactory : public IFactory{
public:
    IButton* CreateButton(){
        return new MacButton();
    }
    ITextBox *CreateTextBox()
    {
        return new MacTextBox();
    }
};

class WindowsFactory : public IFactory{
public:
    IButton* CreateButton(){
        return new WindowsButton();
    }
    ITextBox *CreateTextBox()
    {
        return new WindowsTextBox();
    }
};

class GUIAbstractFactory{
public:
    static IFactory* CreateFactory(string os){
        if(os == "mac"){
            return new MacFactory();
        }
        else if(os == "windows"){
            return new WindowsFactory();
        }
        else{
            return NULL;
        }
    }
};

int main(){
    cout<<"Enter your machine OS version: "<< endl;
    string osType;
    cin>>osType;

    IFactory* factory = GUIAbstractFactory::CreateFactory(osType);

    IButton* button = factory->CreateButton();
    button->press();

    ITextBox* textBox = factory->CreateTextBox();
    textBox->showText();

    return 0;
}