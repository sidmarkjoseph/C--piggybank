#ifndef ACTIONSANDRULES_H
#define ACTIONSANDRULES_H
/////////////////////////////////////////////////////////////////////
//  ActionsAndRules.h - declares new parsing rules and actions     //
//  ver 2.0                                                        //
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
   ver 2.0 Release Date : April 7th 2016

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
  std::string classname;
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
};
class TypeDefScope : public IRule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if (tc[tc.length() - 1] == ";")
		{
			tc.trimFront();

			if (tc[0] == "typedef")
			{
				doActions(pTc);
				return true;
			}

		}
		return true;
	}
};
class UsingScope : public IRule //Alias Rule
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if (tc[tc.length() - 1] == ";")
		{
			tc.trimFront();

			if (tc[0] == "using")
			{
				doActions(pTc);
				return true;
			}

		}
		return true;
	}
};
class PushUsingScope : public IAction //Rule Associated with Alias Rule
{
	Repository* p_Repos;
	std::string res;
	TypeTable* t;
public:
	PushUsingScope(Repository* pRepos, std::string result, TypeTable* temp)
	{
		p_Repos = pRepos;
		res = result;
		t = temp;
	}
	void doAction(ITokCollection*& pTc)
	{
		std::string name = (*pTc)[pTc->find("using") + 1];
		element* elem = new element;
		elem->type = "ALIAS";
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
		std::vector<std::string> a = { elem->type, res };
		p_Repos->scopeStack().push(elem);
		t->addRecord(elem->name, a);
	}
};

class EnumScope : public IRule // Rule for ENUM 
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if (tc[tc.length() - 1] == "{")
		{
			tc.trimFront();

			if (tc[0] == "enum")
			{
				doActions(pTc);
				return true;
			}

		}
		return true;
	}

};
class PushEnumScope : public IAction //Action associated with ENUM Rule
{
	Repository* p_Repos;
	TypeTable* t;
	std::string res;

public:
	PushEnumScope(Repository* pRepos,std::string result, TypeTable* temp)
	{
		p_Repos = pRepos;
		res = result;
		t = temp;
	}
	void doAction(ITokCollection*& pTc)
	{
		std::string name = (*pTc)[pTc->find("enum") + 1];
		element* elem = new element;
		elem->type = "ENUM";
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
		std::vector<std::string> a = { elem->type,res };
		t->addRecord(elem->name, a);
	}
};
class StructScope : public IRule //Rule for detecting STRUCTS
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if (tc[tc.length() - 1] == "{")
		{
			tc.trimFront();
			if (tc[0] == "struct")
			{
				doActions(pTc);
				return true;
			}
		}
		return true;
	}

};
class ClassScope : public IRule// Rule for class
{
public:
	bool doTest(ITokCollection*& pTc)
	{
		ITokCollection& tc = *pTc;
		if (tc[tc.length() - 1] == "{")
		{
		   tc.trimFront();
		   if (tc[0] == "class")
		   {
			   doActions(pTc);
			   return true;
		   }
		}
		return true;
	}
};
class PushTypeDefScope : public IAction // Action associated with Typedef
{
	Repository* p_Repos;
	TypeTable *t;
	std::string res;
public:
	PushTypeDefScope(Repository* pRepos, std::string result, TypeTable* typetable)
	{
		p_Repos = pRepos;
		res = result;
		t = typetable;
	}
	void doAction(ITokCollection*& pTc)
	{
		std::string name = (*pTc)[pTc->find(";") - 1];
		element* elem = new element;
		elem->type = "TYPEDEF";
		elem->name = name;
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
		std::vector<std::string> a = { elem->type,res };
		t->addRecord(elem->name, a);
	}

};
class PushStructScope : public IAction //Action associated with SRTUCT RULE
{
	Repository* p_Repos;
	TypeTable *t;
	std::string res;
public:
	PushStructScope(Repository* pRepos, std::string result, TypeTable* typetable)
	{
		p_Repos = pRepos;
		res = result;
		t = typetable;
	}
	void doAction(ITokCollection*& pTc)
	{
	
		std::string name = (*pTc)[pTc->find("struct") + 1];
		element* elem = new element;
		elem->type = "STRUCT";
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
		std::vector<std::string> a = { elem->type,res };
		t->addRecord(elem->name, a);

	}
};
class PushClassScope : public IAction //Action corresponding to Class Rule
{
	Repository* p_Repos;
	std::string res;
	TypeTable* t;

public:
	PushClassScope(Repository* pRepos,std::string result,TypeTable* temp)
	{
		p_Repos = pRepos;
		res = result;
		t = temp;
	}
	void doAction(ITokCollection*& pTc)
	{
		
		std::string name = (*pTc)[pTc->find("class") + 1];
		element* elem = new element;
		elem->type = "CLASS";
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
		std::vector<std::string> a = { elem->type,res };
		t->addRecord(elem->name, a);
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
  std::string res;
  TypeTable* t;
public:
  HandlePop(Repository* pRepos,std::string result,TypeTable *temp)
  {
    p_Repos = pRepos;
	res = result;
	t = temp;
  }
  void doAction(ITokCollection*& pTc)
  {
	  if (p_Repos->scopeStack().size() == 0)
	  {
		  return;
	  }
	element* elem = p_Repos->scopeStack().pop();
	
	if (elem->type=="GLOBAL_FUNCTION" || elem->type=="ENUM" || elem->type=="MEMBER_FUNCTION")
	{
		std::vector<std::string> a = {elem->type,res};
		t->addRecord(elem->name, a);
	}
	
  }
};
class MemberFunctionDeclaration : public IRule //Rule to detect Member Function Declaration
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
		bool flag=false;
		bool flag1 = false;
		if (tc.find("("))
			flag = true;
		if (tc[tc.length() - 1] == ";")
		{
			tc.trimFront();
			size_t len = tc.find("(");
			try {
				if (tc[len - 2] == ">" || tc[len - 2] == "::" || tc[len - 2] == "." || tc[len - 2] == "new")
					flag1 = true;
			}
			catch (std::exception &e)
			{
				std::string a = e.what();
				flag1 = true;
			}
			if (len < tc.length() && flag && !isSpecialKeyWord(tc[len-1]) && !(flag1))
			{
				doActions(pTc);
				return true;
			}
		}
		return true;
	}
	
};
class PushMemberFunctionDeclaration : public IAction // Action Associated with Member Function Declaration
{
	Repository* p_Repos;
	std::string res;
	TypeTable* t;
public:
	PushMemberFunctionDeclaration(Repository* pRepos,std::string result, TypeTable* temp)
	{
		p_Repos = pRepos;
		res = result;
		t = temp;
	}
	void doAction(ITokCollection*& pTc)
	{
		    std::string name = (*pTc)[pTc->find("(") - 1];
			element* elem = new element;
			elem->type = "MEMBER_FUNCTION";
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
		     std::vector<std::string> a = { elem->type, res };
			 p_Repos->scopeStack().push(elem);
			t->addRecord(elem->name, a);
	} 
};

class MemberFunctionDefinition :public IRule // Rule for Member Function Definition
{
	Repository* p_Repos;
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
			tc.trimFront();
			size_t len = tc.find("(");
			try {

				if (len < tc.length() && !isSpecialKeyWord(tc[len - 1]) && tc[len - 2] == "::")
				{
					doActions(pTc);
					return true;
				}
			}
			catch (std::exception &e)
			{
				std::cout << e.what() << std::endl;
				return true;
			}
		}
		return true;
	}
};
class GlobalFunctionDefinition : public IRule // Rule to detect Global functions
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
		tc.trimFront();
      size_t len = tc.find("(");
	  try {

		  if (len < tc.length() && !isSpecialKeyWord(tc[len - 1]) && !(tc[len - 2] == "::"))
		  {
			  doActions(pTc);
			  return true;
		  }
	  }
	  catch (std::exception &e)
	  {
		  std::cout << e.what() << std::endl;
		  return true;
	  }
    }
    return true;
  }
};
class PushMemberFunction : public IAction // Action associated with Member Function Rule
{
	Repository* p_Repos;
public:
	PushMemberFunction(Repository* pRepos)
	{
		p_Repos = pRepos;
	}
	void doAction(ITokCollection*& pTc)
	{
		std::string name = (*pTc)[pTc->find("(") - 1];

		if ((*pTc)[pTc->find(name) - 1] == "::")
		{
			element* elem = new element;
			elem->type = "MEMBER_FUNCTION";
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
	}
};
class PushGlobalFunction : public IAction // Action corresponding to Rule to detect Global functions
{
  Repository* p_Repos;
public:
  PushGlobalFunction(Repository* pRepos)
  {
    p_Repos = pRepos;
  }
  void doAction(ITokCollection*& pTc)
  {
	  std::string name = (*pTc)[pTc->find("(") - 1];
	  element* elem = new element;
	  elem->type = "GLOBAL_FUNCTION";
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
#endif
