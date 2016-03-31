#ifndef TOKENIZER_H
#define TOKENIZER_H
///////////////////////////////////////////////////////////////////////
// Tokenizer.h - read words from a std::stream                       //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //                                                                    
// Application: Parser component, CSE687 - Object Oriented Design    //
// Author:      Siddharth Mark Joseph, Syracuse University           //
// SUID:         384284085                                           //
// Email:        smjoseph@syr.edu                                    //
///////////////////////////////////////////////////////////////////////
/*
 * Package Operations:
 * -------------------
 * This package provides a public Toker class and private ConsumeState class.
 * Toker reads words from a std::stream and throws away whitespace Tokens. Comment tokens can be returned based on a toggle
 * Tokenizer returns Special One Character Tokens, Special Two Character Tokens, Alpha-Numeric tokens, Punctuator tokens,
   Quoted String tokens that hadle \" and Single-Quoted String Tokens
 * This is a new version, based on the State Design Pattern.
 * Build Process:
 * --------------
 * Required Files: Tokenizer.h, Tokenizer.cpp
 * Build Command: devenv Tokenizer.sln /rebuild debug
 * ver 1.0 : 9th Febuary 2016
 * - first release of new design
 */
#include <iosfwd>
#include <string>

namespace Scanner
{
  class ConsumeState;

  class Toker
  {
  public:
    Toker(); 
    Toker(const Toker&) = delete;
    ~Toker();
    Toker& operator=(const Toker&) = delete;
    bool attach(std::istream* pIn); 
    std::string getTok();
    bool canRead();
	void set(std::string s1, std::string s2);
	void setComments(bool c);
	void des();
	int getLineCount();
  private:
    ConsumeState* pConsumer;
	ConsumeState* ptemp;
	ConsumeState* pone;
	ConsumeState* point;
	ConsumeState* pointatline;
  };
}
#endif
