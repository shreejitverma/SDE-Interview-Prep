#include <iostream>
#include <list>
using namespace std;

class ISubscriber {
    public:
        virtual void notify(string msg) = 0;
};

class User : public ISubscriber{
    private:
        int userId;
    public:
        User(int userId) {
            this->userId = userId;
        }
        void notify(string msg) {
            cout << "User " << userId << " received message: " << msg << endl;
        }
};

class Group{
    private:
        list<ISubscriber *> subscribers;

    public:
        void subscribe(ISubscriber* subscriber) {
            subscribers.push_back(subscriber);
        }
        void unsubscribe(ISubscriber* subscriber) {
            subscribers.remove(subscriber);
        }
        void notify(string msg) {
            for(auto subscriber : subscribers) {
                subscriber->notify(msg);
            }
        }
};
int main()
{
    Group* group = new Group();

    User* user1 = new User(1);
    User* user2 = new User(2);
    User* user3 = new User(3);

    group->subscribe(user1);
    group->subscribe(user2);
    group->subscribe(user3);

    group->notify("new msg");

    group->unsubscribe(user1);
    group->notify("new new msg");

    return 0;
}