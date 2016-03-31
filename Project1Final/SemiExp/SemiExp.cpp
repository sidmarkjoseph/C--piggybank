///////////////////////////////////////////////////////////////////////
// SemiExpression.cpp - collect tokens for analysis                  //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Parser component, CSE687 - Object Oriented Design    //
// Author:      Siddharth Mark Joseph                                //
// SUID:        384284085                                            //
// Email:       smjoseph@syr.edu                                     //
///////////////////////////////////////////////////////////////////////

#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <exception>
#include "SemiExp.h"
#include "../Tokenizer/Tokenizer.h"
#include<algorithm>
using namespace Scanner;
using Token = std::string;

SemiExp::SemiExp(Toker* pToker) : _pToker(pToker) {}
int SemiExp::getLineCount()
{
	return _pToker->getLineCount();
}
bool SemiExp::get(bool get)/* Obtains a sequence of Tokens by repeadtedly calling getTok() till we reach termination condition*/
{

	if (_pToker == nullptr)
		throw(std::logic_error("no Toker reference"));
	_tokens.clear();
	
	while (true)
	{
		
		std::string token = _pToker->getTok();
	  
		if (token == "")
			break;
		push_back(token);
		
		if (token == ":")
		{
			size_t n = find(token);
			if (operator[](--n) == "public" || operator[](--n) == "private" || operator[](--n) == "protected")
			{
				std::cout << getLineCount();
				return true;
			}
		}

		if (token == "\n" && length() > 1)
		{
			int flag = 0;
			
			if (operator[](0) == "#" || (operator[](0) == "\n" && operator[](1) == "#"))
			{
				std::cout << getLineCount();
				return true;
			}
			for (std::vector<Token>::iterator it = _tokens.begin(); it != _tokens.end(); ++it)
			{
				if (*it == "#")
				{

					for (std::vector<Token>::iterator it2 = it; it2 != _tokens.begin(); it2++)
					{
						if (*it2 != "\n")
						{
							flag = 1;
							break;
						}
					}
					if (flag == 1)
					{
						std::cout << getLineCount();
				
						return true;
					}
					
				}
			}
			

		}
		if (token == ";" && tokencollectionhasfor() && tokencollectionhassemi())
		{
			std::cout << getLineCount();
			return true;
		}
		if (token == ";" && tokencollectionhasfor())
			continue;
		

		
		
		
		if (token == "{" || token == "}" || token == ";")
		{
			std::cout << getLineCount();
			return true;
		}
	}
	return false;


}

std::string SemiExp::operator[](size_t n)/* Returns token in SemiExpression*/
{
  if (n < 0 || n >= _tokens.size())
    throw(std::invalid_argument("index out of range"));
  return _tokens[n];
}
bool SemiExp::tokencollectionhasfor() /* Method which determines if there is a for in the SemiCollection*/
{
	int flag = 0;
	for (std::vector<Token>::iterator it = _tokens.begin(); it != _tokens.end(); ++it)
	{
		if (*it == "for")
		{
			
			return true;
		}
		
	}
	
		return false;
}   
size_t SemiExp::length()/*Length of Semi-Expression*/
{
  return _tokens.size();
}

void SemiExp::trimFront()/*Trims newline Token from the begining of Semi-Expression*/
{
	for (std::vector<Token>::iterator it = _tokens.begin(); it != _tokens.end(); ++it)
	{
		if (*it != "\n")
		{
			
			break;
		}
		for (std::vector<Token>::iterator it2 = _tokens.begin(); it2 != it; ++it)
		{
			_tokens.erase(it2 + 0);
		}

	}

}
int SemiExp::find(const std::string& tok)/*Finds index of token in Semi-Expression*/
{
	int i = 0,flag=0;
	for (std::vector<Token>::iterator it = _tokens.begin(); it != _tokens.end(); ++it)
	{
		if (*it == tok)
		{
			
				return i;
		}
		i++;
	}
	
		return -1;
}

void SemiExp::push_back(const std::string& tok)/*Pushes token back into Semi-Expression vector*/
{
	_tokens.push_back(tok);
}
bool SemiExp::remove(const std::string& tok)/*Removes specified token from Semi-Expression*/
{
	int flag = 0,i=0;
	for (std::vector<Token>::iterator it = _tokens.begin(); it != _tokens.end(); ++it)
	{
		if (*it == tok)
		{
			flag = 1;
			break;
		}
	}
	if (flag == 1)
	{
		_tokens.erase(_tokens.begin() + i);
		return true;
	}
	else
		return false;

   
}
bool SemiExp::remove(size_t i)/*Removes token at index i*/
{
	if (i < 0 || i >= _tokens.size())
		return false;
	_tokens.erase(_tokens.begin() + i);
		return true;
		 
}
std::string SemiExp::show(bool shownewlines)/*Displays Semi-Expression*/
{
  std::cout << "\n  ";
  std::string str = "";
  if (shownewlines == false)
  {
	  
	  for (auto token : _tokens)
		  if (token != "\n")
			  str += (token + " ");
	  std::cout << "\n";
  }
  if (shownewlines == true)
  {
	  for (auto token : _tokens)
		  str += (token + " ");
  }
  return str;
}
bool SemiExp::tokencollectionhassemi()/*Method that used to find for loop*/
{
	int count=0;
	for (std::vector<Token>::iterator it = _tokens.begin(); it != _tokens.end()-1; ++it)
	{
		if (*it == ";")
		{
			count++;
			
		}
		
	
	}
	
	
	if (count == 2)
		return true;
	return false;

}
void SemiExp::toLower()/*Coverts all the tokens in _tokens to Lower Case*/
{
	for (std::vector<Token>::iterator it = _tokens.begin(); it != _tokens.end(); ++it)
	{
		std::string temp = *it;
		std::transform( temp.begin(), temp.end(), temp.begin(), ::tolower);
		*it = temp;
	}
	
}
void SemiExp::clear()/*Clears Semi-Expression Vector*/
{
	_tokens.clear();

}
//----< test stub >--------------------------------------------------

#ifdef TEST_SEMIEXP

int main()
{
  Toker toker;
  std::string fileSpec = "../Tokenizer/Tokenizer.cpp";
  //std::string fileSpec = "../Tokenizer/Test.txt";
  std::fstream in(fileSpec);
  if (!in.good())
  {
    std::cout << "\n  can't open file " << fileSpec << "\n\n";
    return 1;
  }
  toker.attach(&in);
  toker.setComments(false);
  toker.set("<,>,[,],(,),{,},:,=,+,-,*,\n", "<,<,>,>,:,:,+,+,-,-,=,=,+,=,-,=,*,=,/,=,-,>");
  SemiExp semi(&toker);
  while(semi.get())
  {
    std::cout << "\n  -- semiExpression --";
    std::string str = semi.show();
	std::cout << str;
  }
  /*
     May have collected tokens, but reached end of stream
     before finding SemiExp terminator.
   */
  if (semi.length() > 0)
  {
    std::cout << "\n  -- semiExpression --";
    semi.show();
    std::cout << "\n\n";
  }
  
  return 0;
}
#endif
