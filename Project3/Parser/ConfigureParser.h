#ifndef CONFIGUREPARSER_H
#define CONFIGUREPARSER_H
/////////////////////////////////////////////////////////////////////
//  ConfigureParser.h - builds and configures parsers              //
//  ver 2.0                                                        //
//                                                                 //
//  Lanaguage:     Visual C++ 2011                                 //
//  Platform:      MACBOOK PRO                                     //
//  Author:        Siddharth Joseph, MS Computer Science           //
//                 Syracuse University                             //
//                                                                 //
/////////////////////////////////////////////////////////////////////
/*
  Module Operations: 
  ==================
  This module builds and configures parsers.  It builds the parser
  parts and configures them with application specific rules and actions.

  Public Interface:
  =================
  ConfigParseToConsole conConfig;
  conConfig.Build();
  conConfig.Attach(someFileName);

  Maintenance History:
  ====================
  ver 1.0 Release Date: 03/12/2016
  ver 2.0 Release Date: April 7th 2016

*/

//
#include <fstream>
#include "Parser.h"
#include "../SemiExp/SemiExp.h"
#include "../Tokenizer/Tokenizer.h"
#include "ActionsAndRules.h"


// build parser that writes its output to console

class ConfigParseToConsole : IBuilder
{
public:
  ConfigParseToConsole() {};
  ~ConfigParseToConsole();
  bool Attach(const std::string& name, bool isFile=true);
  Parser* Build(std::string,TypeTable*);
private:
  // Builder must hold onto all the pieces

  std::ifstream* pIn;
  Scanner::Toker* pToker;
  Scanner::SemiExp* pSemi;
  Parser* pParser;
  Repository* pRepo;
  EndOfScope* pEndOfScope;
  HandlePop* pHandlePop;
  GlobalFunctionDefinition* pGlobalFunctionDefinition;
  PushGlobalFunction* pPushGlobalFunction;
  MemberFunctionDefinition* pMemberFunctionDefinition;
  PushMemberFunction* pPushMemberFunction;
  PushMemberFunctionDeclaration* pPushMemberFunctionDeclaration;
  MemberFunctionDeclaration *pMemberFunctionDeclaration;
  PushClassScope* pPushClassScope;
  ClassScope* pClassScope;
  PushStructScope *pPushStructScope;
  StructScope* pStructScope;
  PushEnumScope* pPushEnumScope;
  EnumScope* pEnumScope;
  TypeDefScope* pTypeDefScope;
  PushTypeDefScope* pPushTypeDefScope;
  UsingScope *pUsingScope;
  PushUsingScope *pPushUsingScope;
  ConfigParseToConsole(const ConfigParseToConsole&) = delete;
  ConfigParseToConsole& operator=(const ConfigParseToConsole&) = delete;
};


#endif
