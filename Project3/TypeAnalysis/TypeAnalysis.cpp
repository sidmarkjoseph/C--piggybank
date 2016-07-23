/////////////////////////////////////////////////////////////
// Author : Siddharth Joseph MS Computer Science           //
// Tests the functionality definied in TypeAnlysis.h       //
// ver 1.0                                                 //
/////////////////////////////////////////////////////////////
#include "TypeAnalysis.h"
#include <iostream>
#include <stdlib.h>
#include <cctype>
#include "../Parser/Parser.h"
#include "../SemiExp/SemiExp.h"
#include "../Tokenizer/Tokenizer.h"
#include "../Parser/ActionsAndRules.h"
#include "../Parser/ConfigureParser.h"
#include "../QueuedWorkItems/Utilities.h"
#include <cctype>
#include "../Parser/Parser.h"
#include "../SemiExp/SemiExp.h"
#include "../Tokenizer/Tokenizer.h"
#include "../Parser/ActionsAndRules.h"
#include "../Parser/ConfigureParser.h"
#include "../QueuedWorkItems/Utilities.h"
#include "windows.h" 


namespace Scanner
{
	///////////////////////////////////////////////////////////////////
	// Context is a shared data storage facility.
	/*
	* - with Context we don't need static members to share
	*   state pointes etc.  Context holds and shares all that.
	* - Context is an entirely private facility used by ConsumeState
	*   and its derived instances, so there is no need for encapsulation
	*   and we use a struct.  This won't be appropriate if we handed
	*   the context to clients of Toker.
	*/
	struct Context
	{
		Context();
		~Context();
		std::string token;
		std::istream* _pIn;
		std::vector<std::string> _oneCharTokens =
		{
			"\n", "<", ">", "{", "}", "[", "]", "(", ")", ":", "=", "+", "-", "*", "."
		};
		std::vector<std::string> _twoCharTokens =
		{
			"<<", ">>", "::", "++", "--", "==", "+=", "-=", "*=", "/="
		};
		int prevChar;
		int currChar;
		bool _doReturnComments;
		size_t _lineCount;
		ConsumeState* _pState;
		ConsumeState* _pEatCppComment;
		ConsumeState* _pEatCComment;
		ConsumeState* _pEatWhitespace;
		ConsumeState* _pEatPunctuator;
		ConsumeState* _pEatAlphanum;
		ConsumeState* _pEatSpecialCharacters;
		ConsumeState* _pEatDQString;
		ConsumeState* _pEatSQString;
	};


	class ConsumeState
	{
		friend class Toker;
	public:
		using Token = std::string;
		ConsumeState();
		ConsumeState(const ConsumeState&) = delete;
		ConsumeState& operator=(const ConsumeState&) = delete;
		virtual ~ConsumeState();
		void attach(std::istream* pIn) { _pContext->_pIn = pIn; }
		virtual void eatChars() = 0;
		void consumeChars() {
			_pContext->_pState->eatChars();
			_pContext->_pState = nextState();
		}
		bool canRead() { return _pContext->_pIn->good(); }
		std::string getTok() { return _pContext->token; }
		bool hasTok() { return _pContext->token.size() > 0; }
		ConsumeState* nextState();
		void returnComments(bool doReturnComments = false);
		size_t currentLineCount();
		void setSpecialTokens(const std::string& commaSeparatedString);
		void setContext(Context* pContext);
	protected:
		Context* _pContext;
		bool collectChar();
		bool isOneCharToken(Token tok);
		bool isTwoCharToken(Token tok);
		Token makeString(int ch);
	};
}
using namespace Scanner;
using namespace FileManager;
using namespace Utilities;
using file = std::string;
#define Util StringHelper
using Token = std::string;
using Token = std::string;
std::function<void(const std::string&)> Title =
[](auto src) { StringHelper::Title(src, '='); };

std::function<void(const std::string&)> title =
[](auto src) { StringHelper::Title(src, '-'); };

void StringHelper::title(const std::string& src)
{
	std::cout << "\n  " << src;
	std::cout << "\n " << std::string(src.size() + 2, '-');
}

void StringHelper::Title(const std::string& src, char underline)
{
	std::cout << "\n  " << src;
	std::cout << "\n " << std::string(src.size() + 2, underline);
}

std::vector<std::string> StringHelper::split(const std::string& src)
{
	std::vector<std::string> accum;
	std::string temp;
	size_t index = 0;
	do
	{
		while ((isspace(src[index]) || src[index] == ',') && src[index] != '\n')
		{
			++index;
			if (temp.size() > 0)
			{
				accum.push_back(temp);
				temp.clear();
			}
		}
		if (src[index] != '\0')
			temp += src[index];
	} while (index++ < src.size());
	if (temp.size() > 0)
		accum.push_back(temp);
	return accum;
}

void Utilities::putline()
{
	std::cout << "\n";
}

void Utilities::promptAndWait()
{
	std::cout << "\n  press any key to continue: ";
	_getch();
}
void ConsumeState::setContext(Context* pContext)
{
	_pContext = pContext;
}
//----< replace one and two char tokens >----------------------------

void ConsumeState::setSpecialTokens(const std::string& commaSeparatedTokens)
{
	std::vector<Token> result = Utilities::StringHelper::split(commaSeparatedTokens);
	_pContext->_oneCharTokens.clear();
	_pContext->_twoCharTokens.clear();
	for (std::string item : result)
	{
		if (item.size() == 1 || item == "\n")
			_pContext->_oneCharTokens.push_back(item);
		if (item.size() >= 2)
			_pContext->_twoCharTokens.push_back(item);
	}
}
//----< return number of newlines collected from stream >------------

size_t ConsumeState::currentLineCount() { return _pContext->_lineCount; }

//----< collect character from stream >------------------------------

bool ConsumeState::collectChar()
{
	if (_pContext->_pIn->good())
	{
		_pContext->prevChar = _pContext->currChar;
		_pContext->currChar = _pContext->_pIn->get();
		if (_pContext->currChar == '\n')
			++(_pContext->_lineCount);
		return true;
	}
	return false;
}

//----< logs to console if TEST_LOG is defined >---------------------

// forward declaration

//----< turn on\off returning comments as tokens >-------------------

void ConsumeState::returnComments(bool doReturnComments)
{
	_pContext->_doReturnComments = doReturnComments;
}
//----< is tok one of the special one character tokens? >------------

bool ConsumeState::isOneCharToken(Token tok)
{
	for (size_t i = 0; i < _pContext->_oneCharTokens.size(); ++i)
		if (_pContext->_oneCharTokens[i] == tok)
			return true;
	return false;
}
//----< is tok one of the special two character tokens? >------------

bool ConsumeState::isTwoCharToken(Token tok)
{
	for (size_t i = 0; i < _pContext->_twoCharTokens.size(); ++i)
		if (_pContext->_twoCharTokens[i] == tok)
			return true;
	return false;
}
//----< make a string with this one integer >------------------------

ConsumeState::Token ConsumeState::makeString(int ch)
{
	Token temp;
	return temp += ch;
}
//----< decide which state to use next >-----------------------------

ConsumeState* ConsumeState::nextState()
{
	if (!(_pContext->_pIn->good()))
	{
		return nullptr;
	}
	int chNext = _pContext->_pIn->peek();
	if (chNext == EOF)
	{
		_pContext->_pIn->clear();
		// if peek() reads end of file character, EOF, then eofbit is set and
		// _pIn->good() will return false.  clear() restores state to good
	}
	if (isOneCharToken(makeString(_pContext->currChar)))
	{
		return _pContext->_pEatSpecialCharacters;
	}
	if (std::isspace(_pContext->currChar) && _pContext->currChar != '\n')
	{

		return _pContext->_pEatWhitespace;
	}
	if (_pContext->currChar == '/' && chNext == '/')
	{

		return _pContext->_pEatCppComment;
	}
	if (_pContext->currChar == '/' && chNext == '*')
	{

		return _pContext->_pEatCComment;
	}
	if (std::isalnum(_pContext->currChar) || _pContext->currChar == '_')
	{

		return _pContext->_pEatAlphanum;
	}
	if (_pContext->currChar == '\"' && _pContext->prevChar != '\\')
	{
		return _pContext->_pEatDQString;
	}
	if (_pContext->currChar == '\'' && _pContext->prevChar != '\\')
	{
		return _pContext->_pEatSQString;
	}
	if (ispunct(_pContext->currChar))
	{

		return _pContext->_pEatPunctuator;
	}
	if (!_pContext->_pIn->good())
	{
		return _pContext->_pEatWhitespace;
	}
	std::string error = "invalid type, currChar = " + Utilities::Converter<char>::toString(_pContext->currChar);
	throw(std::logic_error(error.c_str()));
}
//----< class that consumes whitespace >-----------------------------

class EatWhitespace : public ConsumeState
{
public:
	EatWhitespace(Context* pContext)
	{
		_pContext = pContext;
	}
	virtual void eatChars()
	{
		_pContext->token.clear();
		do {
			if (!collectChar())
				return;
		} while (std::isspace(_pContext->currChar) && _pContext->currChar != '\n');
	}
};
//----< class that consumes C++ comment >----------------------------

class EatCppComment : public ConsumeState
{
public:
	EatCppComment(Context* pContext)
	{
		_pContext = pContext;
	}
	virtual void eatChars()
	{
		_pContext->token.clear();
		do {
			if (_pContext->_doReturnComments)
				_pContext->token += _pContext->currChar;
			if (!collectChar())
				return;
		} while (_pContext->currChar != '\n');
	}
};
//----< class that consumes C comment >------------------------------

class EatCComment : public ConsumeState
{
public:
	EatCComment(Context* pContext)
	{
		_pContext = pContext;
	}
	virtual void eatChars()
	{
		_pContext->token.clear();
		do {
			if (_pContext->_doReturnComments)
				_pContext->token += _pContext->currChar;
			if (!collectChar())
				return;
		} while (_pContext->currChar != '*' || _pContext->_pIn->peek() != '/');
		if (_pContext->_doReturnComments)
			_pContext->token += _pContext->currChar;
		if (!collectChar())      // get terminating '/'
			return;
		if (_pContext->_doReturnComments)
			_pContext->token += _pContext->currChar;
		collectChar();           // get first non-comment char
	}
};
//----< class that eats punctuators >--------------------------------

class EatPunctuator : public ConsumeState
{
public:
	EatPunctuator(Context* pContext)
	{
		_pContext = pContext;
	}
	virtual void eatChars()
	{
		_pContext->token.clear();
		do {
			if (_pContext->currChar == '\"' && _pContext->prevChar != '\\')    // start of double quoted string
			{
				return;
			}
			if (_pContext->currChar == '\'' && _pContext->prevChar != '\\')    // start of single quoted string
			{
				return;
			}
			Token temp = makeString(_pContext->currChar);
			if (isOneCharToken(temp))
			{
				return;
			}
			_pContext->token += _pContext->currChar;
			if (!_pContext->_pIn->good())  // end of stream
			{
				return;
			}
			if (!collectChar())
				return;
		} while (ispunct(_pContext->currChar));
	}
};
//----< class that eats alphanumeric characters >--------------------
/*
* treats underscore '_' as alphanumeric character
*/
class EatAlphanum : public ConsumeState
{
public:
	EatAlphanum(Context* pContext)
	{
		_pContext = pContext;
	}
	virtual void eatChars()
	{
		_pContext->token.clear();
		do {
			_pContext->token += _pContext->currChar;
			if (!collectChar())
				return;
		} while (isalnum(_pContext->currChar) || _pContext->currChar == '_');
	}
};
//----< class that eats one and two character tokens >---------------

class EatSpecialCharacters : public ConsumeState
{
public:
	EatSpecialCharacters(Context* pContext)
	{
		_pContext = pContext;
	}
	virtual void eatChars()
	{
		_pContext->token.clear();
		_pContext->token += _pContext->currChar;
		Token temp = makeString(_pContext->currChar) += _pContext->_pIn->peek();
		if (isTwoCharToken(temp))
		{
			collectChar();
			_pContext->token += _pContext->currChar;
		}
		collectChar();
	}
};
//----< class that eats double quoted string >-----------------------

bool isEscapted(std::string tok)
{
	size_t backSlashCount = 0;
	for (size_t i = tok.size() - 1; i >= 0; --i)
	{
		if (tok[i] == '\\')
			++backSlashCount;
		else break;
	}
	return (backSlashCount % 2 == 1);
}

class EatDQString : public ConsumeState
{
public:
	EatDQString(Context* pContext)
	{
		_pContext = pContext;
	}
	virtual void eatChars()
	{
		_pContext->token.clear();
		do
		{
			_pContext->token += _pContext->currChar;
			if (!collectChar())
				return;
		} while (_pContext->currChar != '\"' || isEscapted(_pContext->token));
		_pContext->token += _pContext->currChar;
		collectChar();
	}
};
//----< class that eats single quoted string >-----------------------

class EatSQString : public ConsumeState
{
public:
	EatSQString(Context* pContext)
	{
		_pContext = pContext;
	}
	virtual void eatChars()
	{
		_pContext->token.clear();
		do
		{
			_pContext->token += _pContext->currChar;
			if (!collectChar())
				return;
		} while (_pContext->currChar != '\'' || isEscapted(_pContext->token));
		_pContext->token += _pContext->currChar;
		collectChar();
	}
};
//----< construct shared data storage >------------------------------

Context::Context()
{
	_pEatAlphanum = new EatAlphanum(this);
	_pEatCComment = new EatCComment(this);
	_pEatCppComment = new EatCppComment(this);
	_pEatPunctuator = new EatPunctuator(this);
	_pEatWhitespace = new EatWhitespace(this);
	_pEatSpecialCharacters = new EatSpecialCharacters(this);
	_pEatDQString = new EatDQString(this);
	_pEatSQString = new EatSQString(this);
	_pState = _pEatWhitespace;
	_lineCount = 1;
	_doReturnComments = false;
}
//----< return shared resources >------------------------------------

Context::~Context()
{
	delete _pEatAlphanum;
	delete _pEatCComment;
	delete _pEatCppComment;
	delete _pEatPunctuator;
	delete _pEatWhitespace;
	delete _pEatSpecialCharacters;
	delete _pEatDQString;
	delete _pEatSQString;
}
//----< no longer has anything to do - will be removed >-------------

ConsumeState::ConsumeState() {}

//----< no longer has anything to do - will be removed >-------------

ConsumeState::~ConsumeState() {}

//----< construct toker starting in state EatWhitespace >------------

Toker::Toker()
{
	_pContext = new Context();
	pConsumer = _pContext->_pEatWhitespace;
	pConsumer->setContext(_pContext);
}

//----< destructor deletes the current state >-----------------------
/*
* deleting any state deletes them all since derived destructor
* implicitly invokes the base destructor
*/
Toker::~Toker()
{
	delete _pContext;
	_pContext = nullptr;
}

//----< attach tokenizer to stream >---------------------------------
/*
* stream can be either a std::fstream or std::stringstream
*/
bool Toker::attach(std::istream* pIn)
{
	if (pIn != nullptr && pIn->good())
	{
		pConsumer->attach(pIn);
		//_pContext->_pIn = pIn;
		return true;
	}
	return false;
}
//----< collect token generated by ConsumeState >--------------------

std::string Toker::getTok()
{
	while (true)
	{
		if (!pConsumer->canRead())
			return "";
		pConsumer->consumeChars();
		if (pConsumer->hasTok())
			break;
	}
	return pConsumer->getTok();
}
//----< has toker reached the end of its stream? >-------------------

bool Toker::canRead() { return pConsumer->canRead(); }

//----< start/stop returning comments as tokens >--------------------

void Toker::returnComments(bool doReturnComments)
{
	pConsumer->returnComments(doReturnComments);
}
//----< is this token a comment? >-----------------------------------

bool Toker::isComment(const std::string& tok)
{
	if (tok.find("//") < tok.size() || tok.find("/*") < tok.size())
		return true;
	return false;
}
//----< return number of newlines pulled from stream >---------------

size_t Toker::currentLineCount()
{
	return pConsumer->currentLineCount();
}
//----< set one and two char tokens >--------------------------------

void Toker::setSpecialTokens(const std::string& commaSeparatedTokens)
{
	pConsumer->setSpecialTokens(commaSeparatedTokens);
}
//----< initialize semiExpression with existing toker reference >----

SemiExp::SemiExp(Toker* pToker) : _pToker(pToker) {}

//----< returns position of tok in semiExpression >------------------

size_t SemiExp::find(const std::string& tok)
{
	for (size_t i = 0; i < length(); ++i)
		if (_tokens[i] == tok)
			return i;
	return length();
}
//----< push token onto back end of SemiExp >------------------------

void SemiExp::push_back(const std::string& tok)
{
	_tokens.push_back(tok);
}
//----< removes token passed as argument >---------------------------

bool SemiExp::remove(const std::string& tok)
{
	std::vector<Token>::iterator iter = _tokens.begin();
	while (iter != _tokens.end())
	{
		if (tok == *iter)
		{
			_tokens.erase(iter);
			return true;
		}
		++iter;
	}
	return false;
}
//----< removes token at nth position of semiExpression -------------

bool SemiExp::remove(size_t n)
{
	if (n < 0 || n >= length())
		return false;
	std::vector<Token>::iterator iter = _tokens.begin() + n;
	_tokens.erase(iter);
	return true;
}
//----< removes newlines from front of semiExpression >--------------

void SemiExp::trimFront()
{
	while (length() > 1)
	{
		if (_tokens[0] == "\n")
			remove(0);
		else
			break;
	}
}
//----< transform all tokens to lower case >-------------------------

void SemiExp::toLower()
{
	for (auto& token : _tokens)
	{
		for (auto& chr : token)
		{
			chr = tolower(chr);
		}
	}
}
//----< clear contents of SemiExp >----------------------------------

void SemiExp::clear()
{
	_tokens.clear();
}
//----< is this token a comment? >-----------------------------------

bool SemiExp::isComment(const std::string& tok)
{
	return _pToker->isComment(tok);
}
//----< return count of newlines retrieved by Toker >----------------

size_t SemiExp::currentLineCount()
{
	if (_pToker == nullptr)
		return 0;
	/*
	*  Tokenizer has collected first non-state char when exiting eatChars()
	*  so we need to subtract 1 from the Toker's line count.
	*/
	return _pToker->currentLineCount() - 1;
}
//----< helps folding for expressions >------------------------------

bool SemiExp::isSemiColonBetweenParens()
{
	size_t openParenPosition = find("(");
	size_t semiColonPosition = find(";");
	size_t closedParenPosition = find(")");

	if (openParenPosition < semiColonPosition && semiColonPosition < closedParenPosition)
		return true;
	return false;
}
//----< fills semiExpression collection from attached toker >--------

bool SemiExp::get(bool clear)
{
	bool ok = getHelper(clear);

	if (hasFor && isSemiColonBetweenParens())
	{
		getHelper(false);  // add test for loop termination
		getHelper(false);  // add counter increment
	}
	return ok;
}
//----< is token a SemiExpression terminator? >----------------------

bool SemiExp::isTerminator(const std::string& token)
{
	if (token == "{" || token == "}" || token == ";")
		return true;

	if (token == "\n")
	{
		trimFront();
		if (_tokens[0] == "#")
			return true;
	}

	if (length() < 2)
		return false;

	if (token == ":" && length() > 0 && _tokens[length() - 2] == "public")
		return true;

	if (token == ":" && length() > 0 && _tokens[length() - 2] == "protected")
		return true;

	if (token == ":" && length() > 0 && _tokens[length() - 2] == "private")
		return true;

	return false;
}
//----< does all the work of collecting tokens for collection >------

bool SemiExp::getHelper(bool clear)
{
	hasFor = false;
	if (_pToker == nullptr)
		throw(std::logic_error("no Toker reference"));
	if (clear)
		_tokens.clear();
	while (true)
	{
		std::string token = _pToker->getTok();
		if (token == "")
			break;
		_tokens.push_back(token);

		if (token == "for")
			hasFor = true;

		if (isTerminator(token))
			return true;
	}
	return false;
}
//----< read only indexing of SemiExpression >-----------------------

Token SemiExp::operator[](size_t n) const
{
	if (n < 0 || n >= _tokens.size())
		throw(std::invalid_argument("index out of range"));
	return _tokens[n];
}
//----< writeable indexing of SemiExpression >-----------------------

Token& SemiExp::operator[](size_t n)
{
	if (n < 0 || n >= _tokens.size())
		throw(std::invalid_argument("index out of range"));
	return _tokens[n];
}
//----< return number of tokens in semiExpression >------------------

size_t SemiExp::length()
{
	return _tokens.size();
}
//----< display collection tokens on console >-----------------------

std::string SemiExp::show(bool showNewLines)
{
	std::ostringstream out;
	out << "\n  ";
	for (auto token : _tokens)
		if (token != "\n" || showNewLines == true)
			out << token << " ";
	out << "\n";
	return out.str();
}

ConfigParseToConsole::~ConfigParseToConsole()
{
	// when Builder goes out of scope, everything must be deallocated
	delete pHandlePop;
	delete pEndOfScope;
	delete pPushMemberFunction;
	delete pMemberFunctionDefinition;
	delete pPushGlobalFunction;
	delete pGlobalFunctionDefinition;
	delete pRepo;
	delete pParser;
	delete pSemi;
	delete pToker;
	pIn->close();
	delete pIn;
}
//----< attach toker to a file stream or stringstream >------------

bool ConfigParseToConsole::Attach(const std::string& name, bool isFile)
{
	if (pToker == 0)
		return false;
	pIn = new std::ifstream(name);
	if (!pIn->good())
		return false;
	return pToker->attach(pIn);
}
//----< Here's where alll the parts get assembled >----------------

Parser* ConfigParseToConsole::Build(std::string result,TypeTable* temp)
//Parser* ConfigParseToConsole::Build()
{
	try
	{
		pToker = new Toker;
		pToker->returnComments(false);
		pSemi = new SemiExp(pToker);
		pParser = new Parser(pSemi);
		pRepo = new Repository(pToker);
		pEndOfScope = new EndOfScope();
		pHandlePop = new HandlePop(pRepo,result,temp);
		pEndOfScope->addAction(pHandlePop);
		pParser->addRule(pEndOfScope);
		pMemberFunctionDefinition = new MemberFunctionDefinition;
		pPushMemberFunction = new PushMemberFunction(pRepo);
		pMemberFunctionDefinition->addAction(pPushMemberFunction);
		pParser->addRule(pMemberFunctionDefinition);
		pGlobalFunctionDefinition = new GlobalFunctionDefinition;
		pPushGlobalFunction = new PushGlobalFunction(pRepo);
		pGlobalFunctionDefinition->addAction(pPushGlobalFunction);
		pParser->addRule(pGlobalFunctionDefinition);
		pMemberFunctionDeclaration = new MemberFunctionDeclaration;
		pPushMemberFunctionDeclaration = new PushMemberFunctionDeclaration(pRepo,result,temp);
		pMemberFunctionDeclaration->addAction(pPushMemberFunctionDeclaration);
		pParser->addRule(pMemberFunctionDeclaration);
		pStructScope = new StructScope;
		pPushStructScope = new PushStructScope(pRepo,result,temp);
		pStructScope->addAction(pPushStructScope);
		pParser->addRule(pStructScope);
		pClassScope = new ClassScope;
		pPushClassScope = new PushClassScope(pRepo,result,temp);
		pClassScope->addAction(pPushClassScope);
		pParser->addRule(pClassScope);
		pEnumScope = new EnumScope;
		pPushEnumScope = new PushEnumScope(pRepo, result, temp);
		pEnumScope->addAction(pPushEnumScope);
		pParser->addRule(pEnumScope);
		pTypeDefScope = new TypeDefScope;
		pPushTypeDefScope = new PushTypeDefScope(pRepo, result, temp);
		pTypeDefScope->addAction(pPushTypeDefScope);
		pParser->addRule(pTypeDefScope);
		pUsingScope = new UsingScope;
		pPushUsingScope = new PushUsingScope(pRepo, result, temp);
		pUsingScope->addAction(pPushUsingScope);
		pParser->addRule(pUsingScope);
		return pParser;
	}
	catch (std::exception& ex)
	{
		std::cout << "\n\n  " << ex.what() << "\n\n";
		return 0;
	}
}

void Parser::addRule(IRule* pRule)
{
	rules.push_back(pRule);
}
//----< get next ITokCollection >------------------------------

bool Parser::next()
{
	// default operation of doFold(pTokColl) is to simply return
	// - code analysis provides specific rules for handling
	//   for statements and operator expressions which are
	//   bound in ConfigureParser.cpp

	bool succeeded = pTokColl->get();
	if (!succeeded)
		return false;
	return true;
}

//----< parse the SemiExp by applying all rules to it >--------

bool Parser::parse()
{
	bool succeeded = false;
	for (size_t i = 0; i<rules.size(); ++i)
	{
		if (rules[i]->doTest(pTokColl))
			succeeded = true;
	}
	return succeeded;
}
//----< register action with a rule >--------------------------

void IRule::addAction(IAction *pAction)
{
	actions.push_back(pAction);
}
//----< invoke all actions associated with a rule >------------

void IRule::doActions(ITokCollection*& pTokColl)
{
	if (actions.size() > 0)
	{
		for (size_t i = 0; i < actions.size(); ++i)
		{
			actions[i]->doAction(pTokColl);
		}
	}
}


//----< used by Toker to provide Consumer with Context ptr >---------

struct FileHandler : IFileEventHandler
{
public:
	std::vector<file>* FileNames;
	void create()
	{
		FileNames = new std::vector<file>;
	}
	void attach(std::string path)
	{
		std::vector <std::string>::iterator it = FileNames->begin();
		while (it != FileNames->end())
		{
			*it = path + "\\" + *it;
			it++;
		}
	}
	void execute(const std::string& fileSpec)
	{
		FileNames->push_back(fileSpec);
	}

};

struct DirHandler : IDirEventHandler
{
	void execute(const std::string& dirSpec)
	{
		std::cout << dirSpec << "\n";
	}
};
std::vector<file>* TypeAnalysis::filesReturn(std::string param)
{
	std::string path = FileSystem::Path::getFullFileSpec(param);
	pFmgr = FileMgrFactory::create(path);
	FileHandler fh;
	DirHandler dh;
	fh.create();
	pFmgr->regForFiles(&fh);
	pFmgr->addPattern("*.h");
	pFmgr->addPattern("*.cpp");
	pFmgr->search();
	fh.attach(path);
	return fh.FileNames;
};

#ifdef TEST_TYPEANALYSIS   
int main()
{
	TypeAnalysis type,typeone;
	std::vector<file>* fileNames = type.filesReturn();
	//std::cout << (*fileNames)[2];
	type.processor.setNumberOfThreads(fileNames->size());
	type.processor.start();	
	for (int i = 0; i < fileNames->size(); i++)
	{
		/*if ((*fileNames)[i] == "TypeAnalysis.h" || (*fileNames)[i] == "TypeAnalysis.cpp")
		continue;*/
		WorkItem<WorkResult> wi1 = [=]() {
			//std::cout << "working on thread" << std::this_thread::get_id() << "\n";
	       		
			return (*fileNames)[i];
		};
		type.processor.doWork(&wi1);
		std::cout << "\n";
		
	}

	TypeTable* typetable = type.processor.result();
	for (int i = 1; i < fileNames->size(); i++)
	{
		TypeTable* temp = type.processor.result();
		typetable->concat(temp,(*fileNames)[i]);
	}
	
	for (int i = 0; i < fileNames->size(); i++)
	{
		type.processor.doWork(nullptr);
	}
	type.processor.wait();
	typeone.processor.setNumberOfThreads(fileNames->size());
	typeone.processor.setTypeTable(typetable);
	typeone.processor.startone();
	for (int i = 0; i < fileNames->size(); i++)
	{
		
		WorkItem<WorkResult> wi1 =[=]() {	
				return (*fileNames)[i];
		};
		typeone.processor.doWork(&wi1);
		std::cout << "\n";
	}
	std::vector<std::string> fin;
	for (int i = 0; i < fileNames->size(); i++)
	{
	 std::vector<std::string> a = typeone.processor.resultone();
	 std::vector<std::string>::iterator it = a.begin();
	 while (it != a.end())
	 {
		 fin.push_back(*it);
		 it++;
	 }
	}
	for (int i = 0; i < fileNames->size(); i++)
	{
		typeone.processor.doWork(nullptr);
	}
	std::vector<std::string>::iterator it = fin.begin();
	while (it != fin.end())
	{
		std::cout << *it << std::endl;
		it++;
	}
	typeone.processor.wait();
	std::cout << "Press any key to continue" << std::endl;
	std::getchar();
}
#endif