#ifndef SEMIEXPRESSION_H
#define SEMIEXPRESSION_H
///////////////////////////////////////////////////////////////////////
// SemiExpression.h - collect tokens for analysis                    //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //
// Author:      Siddharth Joseph, MS Computer Science                //
//              Syracuse University                                  //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides a public SemiExp class that collects and makes
* available sequences of tokens.  SemiExp uses the services of a Toker
* class to acquire tokens.  Each call to SemiExp::get() returns a 
* sequence of tokens that ends in {, }, ;, and '\n' if the line begins
* with #.  There are three additiona termination conditions: a sequence
* of tokens that ends with : and the immediately preceding token is
* public, protected, or private.
*
* Each semiexpression returns just the right tokens to analyze one
* C++ grammatical construct, e.g., class definition, function definition,
* declaration, etc.
*
* Maintenance History:
  ver 1.0 Release Date 03/12/2016
*/
#include <vector>
#include <string>
#include "../Tokenizer/Tokenizer.h"
#include "../SemiExp/itokcollection.h"

namespace Scanner
{
  class SemiExp : public ITokCollection
  {
  public:
    SemiExp(Toker* pToker = nullptr);
    SemiExp(const SemiExp&) = delete;
    SemiExp& operator=(const SemiExp&) = delete;
    bool get(bool clear = true);
    std::string& operator[](size_t n);
    std::string operator[](size_t n) const;
    size_t length();
    size_t find(const std::string& tok);
    void trimFront();
    void toLower();
    bool remove(const std::string& tok);
    bool remove(size_t n);
    void push_back(const std::string& tok);
    void clear();
    bool isComment(const std::string& tok);
    std::string show(bool showNewLines = false);
    size_t currentLineCount();
  private:
    bool isTerminator(const std::string& tok);
    bool getHelper(bool clear = false);
    bool isSemiColonBetweenParens();
    bool hasFor = false;
    std::vector<std::string> _tokens;
    Toker* _pToker;
  };
}
#endif
