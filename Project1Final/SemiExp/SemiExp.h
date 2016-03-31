#ifndef SEMIEXPRESSION_H
#define SEMIEXPRESSION_H
///////////////////////////////////////////////////////////////////////
// SemiExpression.h - collect tokens for analysis                    //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Parser component, CSE687 - Object Oriented Design    //
// Author:      Siddharth Joseph, Syracuse University                //
// Email        smjoseph@syr.edu                                     //
// SUID         384284085                                            //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides a public SemiExp class that collects and makes
* available sequences of tokens.  SemiExp uses the services of a Toker
* class to acquire tokens.  Each call to SemiExp::get() returns a
* sequence of tokens that ends in {,}, ;, '\n' if the line begins with #,
*  : and the previos token is public, private or protected, and also for loop condition
* Build Process:
* --------------
* Required Files: 
*   SemiExpression.h, SemiExpression.cpp, Tokenizer.h, Tokenizer.cpp
* 
* Build Command: devenv Project1.sln /rebuild debug
*
* Maintenance History:
* --------------------
* ver 1.0 Release Date 9th Feb 2016
*/

#include <vector>
#include "../Tokenizer/Tokenizer.h"
#include "iotkcollection.h"
namespace Scanner
{
  using Token = std::string;

  class SemiExp : public ITokCollection
  {
  public:
    SemiExp(Toker* pToker = nullptr);
    SemiExp(const SemiExp&) = delete;
    SemiExp& operator=(const SemiExp&) = delete;
    bool get(bool clear=true);
    Token operator[](size_t n);
    size_t length();
	int getLineCount();
    std::string show(bool shownewlines=false);
	int find(const std::string& tok);
	void push_back(const std::string& tok);
	bool remove(const std::string& tok);
	bool remove(size_t i);
	void toLower();
	void SemiExp::clear();
	bool tokencollectionhasfor();
	bool tokencollectionhassemi();
	void trimFront();
	

	
  private:
    std::vector<Token> _tokens;
    Toker* _pToker;
  };
}
#endif
