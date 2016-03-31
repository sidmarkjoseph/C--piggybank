#ifndef SCOPESTACK_H
#define SCOPESTACK_H
/////////////////////////////////////////////////////////////////////////////
// ScopeStack.h - implements template stack holding specified element type //
// ver 2.0                                                                 //
// Language:      Visual C++ 2011                                          //
// Platform:      MACBOOK PRO                                              //                                                        //
// Author:        Siddharth Joseph, MS Computer Science                    //
//                Syracuse University                                      //
/////////////////////////////////////////////////////////////////////////////
/*
  Package Operations:
  ===================
  This package contains one class: ScopeStack<element>.  The element type
  is an application specific type designed to hold whatever information you
  need to stack.

  Maintenance History:
  ====================
  ver 1.0 : 03/12/2016
  - first release
*/
#include <list>

// element is a application specific type.  It must provide a function:
// std::string show() that returns a string display of its parts. see
// test stub for an example.

template<typename element>
class ScopeStack {
public:
  typename typedef std::list<element>::iterator iterator;
  void push(const element& item);
  element pop();
  element top();
  size_t size();
  iterator begin();
  iterator end();
private:
  std::list<element> stack;
};

template<typename element>
typename inline size_t ScopeStack<element>::size() { return stack.size(); }

template<typename element>
typename inline ScopeStack<element>::iterator ScopeStack<element>::begin() { return stack.begin(); }

template<typename element>
typename inline ScopeStack<element>::iterator ScopeStack<element>::end() { return stack.end(); }



template<typename element>
void ScopeStack<element>::push(const element& item)
{
  stack.push_back(item);
}

template<typename element>
element ScopeStack<element>::pop() 
{ 
  element item = stack.back(); 
  stack.pop_back(); 
  return item; 
}

template<typename element>
element ScopeStack<element>::top()
{
  return stack.back();
}

template<typename element>
void showStack(ScopeStack<element>& stack, bool indent = true)
{
  if (stack.size() == 0)
  {
    std::cout << "\n  ScopeStack is empty";
    return;
  }
  ScopeStack<element>::iterator iter = stack.begin();
  while (iter != stack.end())
  {
    std::string strIndent = std::string(2 * stack.size(), ' ');
    if (!indent)
      strIndent = "";
    element temp = *iter;
    std::cout << "\n  " << strIndent << temp->show();
    ++iter;
  }
}

template<typename element>
void showStack(ScopeStack<element*>& stack, bool indent = true)
{
  if (stack.size() == 0)
  {
    std::cout << "\n  ScopeStack is empty";
    return;
  }
  ScopeStack<element*>::iterator iter = stack.begin();
  while (iter != stack.end())
  {
    std::string strIndent = std::string(2 * stack.size(), ' ');
    if (!indent)
      strIndent = "";
    std::cout << "\n  " << strIndent << ((*iter)->show());
    ++iter;
  }
}
#endif
