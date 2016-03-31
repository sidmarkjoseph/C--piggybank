/////////////////////////////////////////////////////////////////////
// Tokenizer.cpp - read words from a std::stream                   //
// ver 1.0                                                         //
// Language:    C++, Visual Studio 2015                            //
// Application: Parser component, CSE687 - Object Oriented Design  //
// Author:      Siddharth Mark Joseph, Syracuse University         //  
// Email        smjoseph@syr.edu                                   //
// SUID         384284085                                          //
/////////////////////////////////////////////////////////////////////

#include "Tokenizer.h"
#include <iostream>
#include <cctype>
#include <string>
#include<vector>
#include<algorithm>
namespace Scanner
{
  class ConsumeState
  {
  public:
    ConsumeState(); 
    ConsumeState(const ConsumeState&) = delete;
    ConsumeState& operator=(const ConsumeState&) = delete;
    virtual ~ConsumeState();
    void attach(std::istream* pIn) { _pIn = pIn; }/* Intializes stream*/
    virtual void eatChars() = 0;
	/*Eats Characters till I hit the nextState()*/
    void consumeChars() {
      _pState->eatChars();
      _pState = nextState();
    }
	void destroy()
	{
		token = "";
		_pIn = nullptr;
		
		currChar=NULL;
		specialone = "";
		specialtwo="";
		singlechar = {};
		doublechar = {};
		_pState = _pEatWhitespace;
			
		
	}
	int getLineNumber()
	{
		return line_count;
	}
    bool canRead() { return _pIn->good(); }
    std::string getTok() { return token; }
    bool hasTok() { return token.size() > 0; }
	  /* Sets the static Single Special Character Vector*/
	void setSpecialSingleChars(std::string ssc) {
		specialone = ssc;
		for (size_t i = 0; i < specialone.length(); i++)
		{
			if (specialone[i] == ',')
				continue;
			singlechar.push_back(specialone[i]);
		}

	}
	/* Sets the static Two Special Character Vector*/
	void setSpecialCharPairs(std::string scp) {
		specialtwo = scp;
		for (size_t i = 0; i < specialtwo.length(); i++)
		{
			if (specialtwo[i] == ',')
				continue;
			doublechar.push_back(specialtwo[i]);
		}


	}
    ConsumeState* nextState();
	/*Sets the Comment toggle*/
	void setComments(bool c) {
		commenttoggle = c;
	}
	
  protected:
	  static int line_count;
    static std::string token;
    static std::istream* _pIn;
    static int prevChar;
    static int currChar;
	static bool commenttoggle;
	static  std::string specialone;
	static std::string specialtwo;
	static std::vector<char> singlechar;
	static std::vector<char> doublechar;
    static ConsumeState* _pState; //Holds Current State informations
	/*States implemented as sub classes*/
    static ConsumeState* _pEatCppComment;										 								
    static ConsumeState* _pEatCComment;
    static ConsumeState* _pEatWhitespace;
    static ConsumeState* _pEatPunctuator;
    static ConsumeState* _pEatAlphanum;
    
	static ConsumeState* _pEatNewlineAndOneChar;
	static ConsumeState* _pEatQuotedString;
	static ConsumeState* _pEatSingleQuotedString;
	static ConsumeState* _pEatTwoChars;
  };
}

using namespace Scanner;

std::string ConsumeState::token;
std::istream* ConsumeState::_pIn = nullptr;
int ConsumeState::line_count = 0;
int ConsumeState::prevChar;
int ConsumeState::currChar;
bool ConsumeState::commenttoggle;
std::string ConsumeState::specialone;
std::string ConsumeState::specialtwo;
std::vector<char> ConsumeState::singlechar;
std::vector<char> ConsumeState::doublechar;
ConsumeState* ConsumeState::_pState = nullptr;
ConsumeState* ConsumeState::_pEatCppComment = nullptr;
ConsumeState* ConsumeState::_pEatCComment = nullptr;
ConsumeState* ConsumeState::_pEatWhitespace = nullptr;
ConsumeState* ConsumeState::_pEatPunctuator = nullptr;
ConsumeState* ConsumeState::_pEatAlphanum = nullptr;

ConsumeState* ConsumeState::_pEatNewlineAndOneChar = nullptr;
ConsumeState* ConsumeState::_pEatQuotedString = nullptr;
ConsumeState* ConsumeState::_pEatSingleQuotedString = nullptr;
ConsumeState* ConsumeState::_pEatTwoChars = nullptr;
void testLog(const std::string& msg);
/*Determines nextState()*/
ConsumeState* ConsumeState::nextState()
{
  if (!(_pIn->good()))
  {
    return nullptr;
  }
  int chNext = _pIn->peek();
  if (chNext == EOF)
  {
    _pIn->clear();
    // if peek() reads end of file character, EOF, then eofbit is set and
    // _pIn->good() will return false.  clear() restores state to good
  }
  if (std::isspace(currChar) && currChar != '\n')
  {
    testLog("state: eatWhitespace");
    return _pEatWhitespace;
  }
  if (currChar == '/' && chNext == '/')
  {
	  testLog("state: eatWhitespace");
	  
    return _pEatCppComment;
  }
  if (currChar == '/' && chNext == '*')
  {
	 
    testLog("state: eatCComment");
    return _pEatCComment;
  }
  for (std::vector<char>::iterator it = doublechar.begin(); it != doublechar.end(); it += 2)
  {
	  char currentchar = *it;
	  char charnext = *(it + 1);
	  if (currentchar == currChar && chNext == charnext)
	  {
		 
		  return _pEatTwoChars;
	  }


  }
  for (std::vector<char>::iterator it = singlechar.begin(); it != singlechar.end(); it++)
  {
	  if (currChar == *it)
	  {
		  
		  return _pEatNewlineAndOneChar;
	  }
  }

  if (currChar == '\'')
  {
	  
	  return _pEatSingleQuotedString;
  }

  if (currChar == '"')
  {
	  
	  return _pEatQuotedString;

  }
  if (std::isalnum(currChar) || currChar=='_')
  {
	  
    testLog("state: eatAlphanum");
    return _pEatAlphanum;
  }
  if (ispunct(currChar))
  {
	 
    testLog("state: eatPunctuator");
    return _pEatPunctuator;
  }
  if (!_pIn->good())
  {
    //std::cout << "\n  end of stream with currChar = " << currChar << "\n\n";
    return _pEatWhitespace;
  }
  throw(std::logic_error("invalid type"));
}

class EatWhitespace : public ConsumeState
{
public:
  virtual void eatChars()
  {
    token.clear();
    //std::cout << "\n  eating whitespace";
    do {
		
      if (!_pIn->good())  // end of stream
        return;
	  
      currChar = _pIn->get();
    } while (std::isspace(currChar) && currChar != '\n');
  }
};

class EatCppComment : public ConsumeState
{
public:
  virtual void eatChars()
  {
    token.clear();
	//std::cout << "\n CPP Style Comment";
	if (commenttoggle == true)
	{
		do {
			token += currChar;
			if (!_pIn->good())  // end of stream
				return;
			currChar = _pIn->get();
		} while (currChar != '\n');
	}
	else
	{
	
		do {
			
			if (!_pIn->good())  // end of stream
				return;
			currChar = _pIn->get();
		} while (currChar != '\n');
	}
	//token += currChar;
	//token += _pIn->peek();
  }
};

class EatCComment : public ConsumeState
{
public:
  virtual void eatChars()
  {
    token.clear();
    //std::cout << "\n  C Style comment";
	if (commenttoggle == true)
	{
		do {
			token += currChar;
			if (!_pIn->good())  // end of stream
				return;
			currChar = _pIn->get();
		} while (currChar != '*' || _pIn->peek() != '/');
		token += currChar;
		currChar = _pIn->get();
		token += currChar;
		currChar = _pIn->get();
	}
	else
	{
		do {

			if (!_pIn->good())  // end of stream
				return;
			currChar = _pIn->get();
		} while (currChar != '*' || _pIn->peek() != '/');

		currChar = _pIn->get();

		currChar = _pIn->get();
	}
    
  }
};

class EatPunctuator : public ConsumeState
{
public:
  virtual void eatChars()
  {
    token.clear();
	//std::cout << "\n Punctuator Token";
	std::vector<char> temp = {'<','<','>','>',':',':','+','+','-','-','=','=','+','=','-','=','*','=','/','=','-','>'};
	std::vector<char> first = { '<','>','[',']','(',')','{' ,'}',':','=','+','-','*', };

	int flag = 0;
    //std::cout << "\n  eating punctuator";
    do {
	
      token += currChar;
      if (!_pIn->good())  // end of stream
        return;
      currChar = _pIn->get();

	  for (std::vector<char>::iterator it = temp.begin(); it != temp.end(); it += 2)
	  {
		  char currentchar = *it;
		  char charnext = *(it + 1);
		  if (currentchar == currChar && charnext==_pIn->peek())
		  {
			  flag = 1;
			  break;
			 
		  }


	  }
	  for (std::vector<char>::iterator it = first.begin(); it != first.end(); it++)
	  {
		  if (currChar == *it)
		  {
			  flag = 2;
			  break;
		  }

	  }
	  if (flag == 1 || flag==2)
		  break;
	  
	  
    } while (ispunct(currChar) && currChar!='\'' && currChar !='"' && currChar != '_' );
  }
};

class EatAlphanum : public ConsumeState
{
public:
  virtual void eatChars()
  {
    token.clear();
    //std::cout << "\n Alphanumeric Token";
    do {
      token += currChar;
      if (!_pIn->good())  // end of stream
        return;
      currChar = _pIn->get();
    } while (isalnum(currChar) || currChar=='_');
  }
};
class EatTwoChars : public ConsumeState
{
public:
	virtual void eatChars()
	{
		token.clear();
		
		token += currChar;
		currChar = _pIn->get();
		token += currChar;
		currChar = _pIn->get();
		

	}
};
class EatSingleQuotedString : public ConsumeState
{
public:
	virtual void eatChars()
	{
		//std::cout << "\n Special Single Quoted String";
		token.clear();
		do
		{
			if (currChar == '\\' && _pIn->peek() == '\'')
			{
				currChar = _pIn->get();
				token += currChar;
				currChar = _pIn->get();
				break;


			}

			if (!_pIn->good())  // end of stream
				return;
			token += currChar;



			currChar = _pIn->get();
		} while (currChar != '\'');

		token += currChar;
		currChar = _pIn->get();
		
	}
};
class EatQuotedString : public ConsumeState
{
public:
	virtual void eatChars()
	{
		//std::cout << "\n Special Quoted String";
		token.clear();

		do
		{
			if (currChar == '\\' && _pIn->peek() == '"')
			{
				currChar = _pIn->get();
				token += currChar;
				currChar = _pIn->get();
				if (currChar == '"')
					break;
				if (currChar == '\\')
					continue;

			}
			if (!_pIn->good())  // end of stream
				return;
			token += currChar;



			currChar = _pIn->get();
		} while (currChar != '"');
		token += currChar;
		currChar = _pIn->get();
		
	}
};
class EatNewlineAndOneChar : public ConsumeState
{
public:
  virtual void eatChars()
  {
    token.clear();
	//std::cout << "\n Special One Character Token";
    //std::cout << "\n  eating alphanum";
    token += currChar;
	if (currChar == '\n')
		line_count++;
    if (!_pIn->good())  // end of stream
      return;
    currChar = _pIn->get();
  }
};
/*  Constructor for ConsumeState */
ConsumeState::ConsumeState()
{
  static bool first = true;
  if (first)
  {
    first = false;
    _pEatAlphanum = new EatAlphanum();
    _pEatCComment = new EatCComment();
    _pEatCppComment = new EatCppComment();
    _pEatPunctuator = new EatPunctuator();
    _pEatWhitespace = new EatWhitespace();
    _pEatNewlineAndOneChar = new EatNewlineAndOneChar();
	_pEatSingleQuotedString = new EatSingleQuotedString();
	_pEatQuotedString = new EatQuotedString();
	_pEatTwoChars = new EatTwoChars();
    _pState = _pEatWhitespace;
  }
}
/* Destructor for ConsumeState*/
ConsumeState::~ConsumeState()
{
  static bool first = true;
  if (first)
  {
    first = false;
    delete _pEatAlphanum;
    delete _pEatCComment;
    delete _pEatCppComment;
    delete _pEatPunctuator;
    delete _pEatWhitespace;
    delete _pEatNewlineAndOneChar;
  }
}

Toker::Toker() : pConsumer(new EatWhitespace()) {}/*Constructor for Toker that intializes state to EatWhiteSpace*/

Toker::~Toker() { delete pConsumer; }/* Destructor for Toker*/

bool Toker::attach(std::istream* pIn)/*Calls attach of ConsumeState to intialize _pIn*/
{
  if (pIn != nullptr && pIn->good())
  {
    pConsumer->attach(pIn);
    return true;
  }
  return false;
}
void Toker::set(std::string s1, std::string s2)/* Sets the Vector for Single Special Characters and Double Special Characters*/
{

	ptemp->setSpecialSingleChars(s1);
	ptemp->setSpecialCharPairs(s2);

}
std::string Toker::getTok()/* Returns one token at a time*/
{

  while(true) 
  {
    if (!pConsumer->canRead())
      return "";
    pConsumer->consumeChars();
    if (pConsumer->hasTok())
      break;
  }
  return pConsumer->getTok();
}

bool Toker::canRead() { return pConsumer->canRead(); }
void Toker::setComments(bool c)/*Set Comments on and off depending on the value of c*/
{
	pone->setComments(c);
}
int Toker::getLineCount()
{
	return pointatline->getLineNumber();
}
void Toker::des()/* Re-intializes the static variables*/
{
	point->destroy();
}
void testLog(const std::string& msg)
{
#ifdef TEST_LOG
  std::cout << "\n  " << msg;
#endif
}

//----< test stub >--------------------------------------------------

#ifdef TEST_TOKENIZER

#include <fstream>

int main()
{
  //std::string fileSpec = "../Tokenizer/Tokenizer.cpp";
  //std::string fileSpec = "../Tokenizer/Tokenizer.h";
  std::string fileSpec = "../Tokenizer/Test.txt";

  std::ifstream in(fileSpec);
  if (!in.good())
  {
    std::cout << "\n  can't open " << fileSpec << "\n\n";
    return 1;
  }
  Toker toker;
  toker.attach(&in);
  toker.setComments(false);
  toker.set("<,>,[,],(,),{,},:,=,+,-,*,\n", "<,<,>,>,:,:,+,+,-,-,=,=,+,=,-,=,*,=,/,=,-,>");
  do
  {
    std::string tok = toker.getTok();
    if (tok == "\n")
      tok = "newline";
    std::cout << "\n -- " << tok;
  } while (in.good());

  std::cout << "\n\n";
  return 0;
}
#endif
