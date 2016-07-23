/////////////////////////////////////////////////////////////////////
//  ConfigureParser.cpp - builds and configures parsers            //
//  ver 2.0                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2005                                 //
//  Platform:      MACBOOK PRO                                     //
//  Author:        Siddharth Joseph, MS Computer Science           //
//                 Syracuse University                             //
/////////////////////////////////////////////////////////////////////

#include <fstream>
#include "Parser.h"
#include "../SemiExp/SemiExp.h"
#include "../Tokenizer/Tokenizer.h"
#include "ActionsAndRules.h"
#include "ConfigureParser.h"


using namespace Scanner;

//----< destructor releases all parts >------------------------------

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
  if(pToker == 0)
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
	pEnumScope = new EnumScope;
	pPushEnumScope = new PushEnumScope(pRepo,result,temp);
	pEnumScope->addAction(pPushEnumScope);
	pParser->addRule(pEnumScope);
	pTypeDefScope = new TypeDefScope;
	pPushTypeDefScope = new PushTypeDefScope(pRepo, result, temp);
	pTypeDefScope->addAction(pPushTypeDefScope);
	pParser->addRule(pTypeDefScope);
	pClassScope = new ClassScope;
	pPushClassScope = new PushClassScope(pRepo,result,temp);
	pClassScope->addAction(pPushClassScope);
	pParser->addRule(pClassScope);
	pUsingScope = new UsingScope;
	pPushUsingScope = new PushUsingScope(pRepo,result,temp);
	pUsingScope->addAction(pPushUsingScope);
	pParser->addRule(pUsingScope);
    return pParser;
  }
  catch(std::exception& ex)
  {
    std::cout << "\n\n  " << ex.what() << "\n\n";
    return 0;
  }
}

#ifdef TEST_CONFIGUREPARSER

#include <queue>
#include <string>

int main(int argc, char* argv[])
{
  std::cout << "\n  Testing ConfigureParser module\n "
            << std::string(32,'=') << std::endl;

  // collecting tokens from files, named on the command line

  if(argc < 2)
  {
    std::cout 
      << "\n  please enter name of file to process on command line\n\n";
    return 1;
  }

  for(int i=1; i<argc; ++i)
  {
    std::cout << "\n  Processing file " << argv[i];
    std::cout << "\n  " << std::string(16 + strlen(argv[i]),'-');

    ConfigParseToConsole configure;
    Parser* pParser = configure.Build();
    try
    {
      if(pParser)
      {
        if(!configure.Attach(argv[i]))
        {
          std::cout << "\n  could not open file " << argv[i] << std::endl;
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
      std::cout << "\n\n";
    }
    catch(std::exception& ex)
    {
      std::cout << "\n\n    " << ex.what() << "\n\n";
    }
    std::cout << "\n\n";
  }
}
#endif
