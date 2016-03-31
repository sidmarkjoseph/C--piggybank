#ifndef ACTIONSANDRULES_H
#define ACTIONSANDRULES_H
/////////////////////////////////////////////////////////////////////
//  ActionsAndRules.h - declares new parsing rules and actions     //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2011                                 //
//  Platform:      MACBOOK PRO                                     //
//  Author:        Siddharth Joseph, MS Computer Science           //
//                 Syracuse University                             //
//                                                                 //
/////////////////////////////////////////////////////////////////////
/*
  Module Operations: 
  ==================
  This module defines several action classes.  Its classes provide 
  specialized services needed for specific applications.  The modules
  Parser, SemiExpression, and Tokenizer, are intended to be reusable
  without change.  This module provides a place to put extensions of
  these facilities and is not expected to be reusable. 

  Public Interface:
  =================
  Toker t(someFile);              // create tokenizer instance
  SemiExp se(&t);                 // create a SemiExp attached to tokenizer
  Parser parser(se);              // now we have a parser
  Rule1 r1;                       // create instance of a derived Rule class
  Action1 a1;                     // create a derived action
  r1.addAction(&a1);              // register action with the rule
  parser.addRule(&r1);            // register rule with parser
  while(se.getSemiExp())          // get semi-expression
    parser.parse();               //   and parse it

  Maintenance History:
  ====================
   ver 1.0 Release Date : 03/12/2016

*/
#include <queue>
#include <string>
#include <sstream>
#include "Parser.h"
#include "../SemiExp/itokcollection.h"
#include "../ScopeStack/ScopeStack.h"
#include "../Tokenizer/Tokenizer.h"
#include "../SemiExp/SemiExp.h"
#include "AbstractTree.h"
struct element
{
  std::string type;
  std::string name;
  size_t startlineCount=0;
  size_t endlineCount = 0;
  size_t complexity = 0;
  size_t lineCount = 0;
  std::string show()
  {
    std::ostringstream temp;
    temp << "(";
    temp << type;
    temp << ", ";
    temp << name;
    temp << ", ";
    temp << startlineCount;
	temp << ",";
	temp << endlineCount;
	temp << ",";
	temp << lineCount;
	temp << ",";
	temp << complexity;
    temp << ")"; 
    return temp.str();
  }
  std::vector<element*> _children;
};




class Repository  // Holds ScopeStack reference and and a pointer to the root element of the tree
{
	ScopeStack<element*> stack;
    Scanner::Toker* p_Toker;
public:
  Repository(Scanner::Toker* pToker)
  {
	
    p_Toker = pToker;
  }
  element *root;
  ScopeStack<element*>& scopeStack()
  {
    return stack;
  }
  Scanner::Toker* Toker()
  {
    return p_Toker;
  }
  size_t lineCount()
  {
    return (size_t)(p_Toker->currentLineCount());
  }
  int TreeWalk(element* pItem)//Finds Complexity
  {
	  int count = 0;	
	  auto iter = pItem->_children.begin();
	  while (iter != pItem->_children.end())
	  {
		  count++;
		  count+=TreeWalk(*iter);
		  ++iter;
	  }
	  return count;
  }
  void DisplayTree(element* pItem)//Display Tree
  {
	  static size_t indentLevel = 0;
	  if(pItem->type=="gsm" || pItem->type=="class")
	  {
		    
		  std::cout << "\n  " << std::string(2 * indentLevel, ' ') << pItem->name;
		  auto iter = pItem->_children.begin();
		  ++indentLevel;
		  while (iter != pItem->_children.end())
		  {
			  DisplayTree(*iter);
			  ++iter;
		  }
		  --indentLevel;
	  }
	  else
	  {
		
		  std::cout << "\n  " << std::string(2 * indentLevel, ' ') << pItem->show();
		  auto iter = pItem->_children.begin();
		  ++indentLevel;
		  while (iter != pItem->_children.end())
		  {

			  DisplayTree(*iter);
			  ++iter;
		  }
		  --indentLevel;
	  }
  }  
};
class ClassScope : public IRule// Rule for class, namespace and struct
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if (tc[tc.length() - 1] == "{")
		{
		   tc.trimFront();
		   if (tc[0] == "class" || tc[0] == "namespace" || tc[0]=="struct")
		   {
			   doActions(pTc);
			   return true;
		   }

		}
		return true;
	}
};
class PushClassScope : public IAction //Action corresponding to Class Rule
{
	Repository* p_Repos;

public:
	PushClassScope(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		size_t ind = pTc->find("\n");
		bool a = pTc->remove(ind);
		std::string name = (*pTc)[pTc->find("{") - 1];
		element* elem = new element;
		elem->type = "class";
		elem->name = name;
		elem->startlineCount = p_Repos->lineCount();
		if (p_Repos->scopeStack().size() == 0)
		{
			element* root = new element;
			root->name = "Global standard namespace";
			root->type = "gsm";
			root->startlineCount = p_Repos->lineCount();
			p_Repos->root = root;
			p_Repos->scopeStack().push(root);
		}
		p_Repos->scopeStack().top()->_children.push_back(elem);
		p_Repos->scopeStack().push(elem);

	}
};
class Control : public IRule // Rule for Control Statements
{
public:
	bool isSpecialKeyWord(const std::string& tok)
	{
		const static std::string keys[]
			= { "for", "while", "switch", "if", "catch" };
		for (int i = 0; i<5; ++i)
			if (tok == keys[i])
				return true;
		return false;
	}
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if (tc[tc.length() - 1] == "{")
		{
			
			size_t len = tc.find("(");
			if (len < tc.length() && isSpecialKeyWord(tc[len - 1]))
			{
				doActions(pTc);
				return true;
			}
		}
		return true;
	}
};
class PushControl : public IAction //Action corresponding to Control Rule
{
	Repository* p_Repos;
public:
	PushControl(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{

		std::string name = (*pTc)[pTc->find("(") - 1];
		element* elem = new element;
		elem->type = "control";
		elem->name = name;
		elem->startlineCount = p_Repos->lineCount();

		if (p_Repos->scopeStack().size() == 0)
		{
			element* root = new element;
			root->name = "Global standard namespace";
			root->type = "gsm";
			root->startlineCount = p_Repos->lineCount();
			p_Repos->root = root;
			p_Repos->scopeStack().push(root);
		}
		p_Repos->scopeStack().top()->_children.push_back(elem);
		p_Repos->scopeStack().push(elem);

	}

};
class OtherControl :public IRule // Rule for Control Statements like else and catch
{
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if (tc[tc.length() - 1] == "{")
		{
			size_t len = tc.length() - 1;
			if (tc[len - 2] == "else" || tc[len - 2] == "try")
			{
				doActions(pTc);
				return true;
			}
		}

		return true;
	}

};
class PushOtherControl :public IAction //Action corresponding to Other Control Rule
{
	Repository* p_Repos;

public:
	PushOtherControl(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{

		std::string name = (*pTc)[0];
		element* elem = new element;
		elem->type = "control";
		elem->name = name;
		elem->startlineCount = p_Repos->lineCount();

		if (p_Repos->scopeStack().size() == 0)
		{
			element* root = new element;
			root->name = "Global standard namespace";
			root->type = "gsm";
			root->startlineCount = p_Repos->lineCount();
			p_Repos->root = root;
			p_Repos->scopeStack().push(root);
		}
		p_Repos->scopeStack().top()->_children.push_back(elem);
		p_Repos->scopeStack().push(elem);
	}
};
class EndOfScope : public IRule//Rule to Detect End of Scope
{
public:
  bool doTest(ITokCollection*& pTc)
  {
    if(pTc->find("}") < pTc->length())
    {
      doActions(pTc);
      return true;
    }
    return true;
  }
};
class HandlePop : public IAction // Action corresponding to End of Scope Rule 
{
  Repository* p_Repos;
public:
  HandlePop(Repository* pRepos)
  {
    p_Repos = pRepos;
  }
  void doAction(ITokCollection*& pTc)
  {
	  if (p_Repos->scopeStack().size() == 0)
	  {
		  return;
	  }
	element* elem = p_Repos->scopeStack().pop();
	if (elem->type == "function" || elem->type == "control" || elem->type == "class")
	{
		int count = p_Repos->TreeWalk(elem);
		elem->complexity = count;
		elem->endlineCount = p_Repos->lineCount();
		elem->lineCount = elem->endlineCount - elem->startlineCount;
	}
	 if(elem->type == "function")
	{
		std::cout << "\n";
		std::cout << "NAME :" << elem->name << ",TYPE :" << elem->type << "," << "LINES:" << elem->lineCount << "," << "COMPLEXITY:" << elem->complexity << "\n";
		std::cout << "STATE OF AST" << "\n";
		p_Repos->DisplayTree(p_Repos->root);
		std::cout << "\n";	
     }
  }
};
class FunctionDefinition : public IRule // Rule to detect functions
{
public:
  bool isSpecialKeyWord(const std::string& tok)
  {
    const static std::string keys[]
      = { "for", "while", "switch", "if", "catch" };
    for(int i=0; i<5; ++i)
      if(tok == keys[i])
        return true;
    return false;
  }
  bool doTest(ITokCollection*& pTc)
  {
    ITokCollection& tc = *pTc;
    if(tc[tc.length()-1] == "{")
    {
      size_t len = tc.find("(");
      if(len < tc.length() && !isSpecialKeyWord(tc[len-1]))
      {
        doActions(pTc);
        return true;
      }
    }
    return true;
  }
};
class PushFunction : public IAction // Action corresponding to Rule to detect functions
{
  Repository* p_Repos;
public:
  PushFunction(Repository* pRepos)
  {
    p_Repos = pRepos;
  }
  void doAction(ITokCollection*& pTc)
  {
	  std::string name = (*pTc)[pTc->find("(") - 1];
	  element* elem = new element;
	  elem->type = "function";
	  elem->name = name;
	  //std::cout << elem.name;
	  elem->startlineCount = p_Repos->lineCount();
	  
	  if (p_Repos->scopeStack().size() == 0)
	  {
		  element* root = new element;
		  root->name = "Global standard namespace";
		  root->type = "gsm";
		  root->startlineCount = p_Repos->lineCount();
		  p_Repos->root = root;
		  p_Repos->scopeStack().push(root);
	  }
	  p_Repos->scopeStack().top()->_children.push_back(elem);
	  p_Repos->scopeStack().push(elem);
	}
	 
  
};
#endif
