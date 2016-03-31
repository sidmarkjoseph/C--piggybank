/////////////////////////////////////////////////////////////////////
//  Parser.cpp - Analyzes C++ language constructs                  //
//  ver 1.0                                                        //
//  Language:      Visual C++ 2011                                 //
//  Platform:      MACBOOK PRO                                     //
//  Author:        Siddharth Joseph, MS Computer Science           //
//                 Syracuse University                             //  
/////////////////////////////////////////////////////////////////////

#include <iostream>
#include <string>
#include "../Utilities/Utilities.h"
#include "../Tokenizer/Tokenizer.h"
#include "../SemiExp/SemiExp.h"
#include "Parser.h"
#include "ActionsAndRules.h"
#include "ConfigureParser.h"

using namespace Scanner;
using namespace Utilities;

//----< register parsing rule >--------------------------------

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
  if(!succeeded)
    return false;
  return true;
}

//----< parse the SemiExp by applying all rules to it >--------

bool Parser::parse()
{
  bool succeeded = false;
  for(size_t i=0; i<rules.size(); ++i)
  {
    if(rules[i]->doTest(pTokColl))
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

//----< test stub >--------------------------------------------

#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"

#ifdef TEST_PARSER

#include <queue>
#include <string>
#define Util StringHelper

int main(int argc, char* argv[])
{
  
  std::string path = "C:\\Users\\Siddharth Joseph\\Downloads\\Project2HelperCode\\Parser";
  std::vector<std::string> vector2 = FileSystem::Directory::getFiles(path, "*.cpp");
  for (std::vector<std::string>::iterator iter = vector2.begin(); iter != vector2.end();iter++)
  {
	std::string fileSpec = path + "\\" + *iter;
    std::string msg = "Processing file" + fileSpec;     
    Util::title(msg);
    putline();

    ConfigParseToConsole configure;
    Parser* pParser = configure.Build();
    try
    {
      if(pParser)
      {
        if(!configure.Attach(fileSpec))
        {
          std::cout << "\n  could not open file " <<  std::endl;
          continue;
        }
      }
      else
      {
        std::cout << "\n\n  Parser not built\n\n";
        return 1;
      }
      // now that parser is built, use it
      while(pParser->next())
        pParser->parse();
      std::cout << "\n";
    }
    catch(std::exception& ex)
    {
      std::cout << "\n\n    " << ex.what() << "\n\n";
    }
  }
  std::cout << "\n";
}

#endif
