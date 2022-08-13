/*
 * C++ Design Patterns:
 * Author: Shreejit Verma [github.com/shreejitverma/]
 * 2022
 *
 */

#include <iostream>

class Subject {
public:
  virtual ~Subject() {}

  virtual void request() = 0;
};

class RealSubject : public Subject {
public:
  void request() {
    std::cout << "RealSubject Request" << std::endl;
  }
};

class Proxy : public Subject
{
public:
  Proxy()
  {
    subject = new RealSubject();
  }

  ~Proxy()
  {
    delete subject;
  }

  void request()
  {
    subject->request();
  }

private:
  RealSubject *subject;
};


int main()
{
  Proxy *proxy = new Proxy();
  proxy->request();

  delete proxy;
  return 0;
}