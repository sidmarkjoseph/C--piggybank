///////////////////////////////////////////////////////////////////////
// TestExecutive.cpp - Tests the Requirements Specification          //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //                                                                    
// Application: Test Component, CSE687 - Object Oriented Design      //
// Author:      Siddharth Mark Joseph, Syracuse University           //
// SUID:         384284085                                           //
// Email:        smjoseph@syr.edu                                    //
///////////////////////////////////////////////////////////////////////
#include "../SemiExp/SemiExp.h"
#include "../Tokenizer/Tokenizer.h"
#include "../SemiExp/iotkcollection.h"
#include <fstream>
#include <iostream>
using namespace Scanner;
int dealSemiExpressions()
{
	Toker tok;
	std::string fileSpec1 = "../Tokenizer/test_tokenizer.txt";
	std::fstream inn(fileSpec1);
	if (!inn.good())
	{
		std::cout << "\n  can't open file " << fileSpec1 << "\n\n";
		return 1;
	}
	tok.attach(&inn);
	tok.setComments(false);
	tok.set("<,>,[,],(,),{,},:,=,+,-,*,\n", "<,<,>,>,:,:,+,+,-,-,=,=,+,=,-,=,*,=,/,=,-,>");
	SemiExp semi(&tok);
	semi.get();

	while (semi.get())
	{
		std::cout << "\n  -- semiExpression --";
		std::string str = semi.show();
		std::cout << str;
	}

	if (semi.length() > 0)
	{
		std::cout << "\n  -- semiExpression --";
		semi.show();
		std::cout << "\n\n";
	}
	return 0;
}
int dealTokenizer(Toker& toker)
{
	std::cout << "Input for Tokenizer" << std::endl;
	std::string fileSpec = "../Tokenizer/test_tokenizer.txt";
	std::ifstream file("../Tokenizer/test_tokenizer.txt");

	std::string str;
	while (std::getline(file, str))
	{
		std::cout << str << std::endl;
	}

	std::cout << "-----------------------------------------------------------------------------------------\n";

	std::ifstream in(fileSpec);
	if (!in.good())
	{
		std::cout << "\n  can't open " << fileSpec << "\n\n";
		return 1;
	}
	
	toker.attach(&in);
	toker.setComments(true);
	toker.set("<,>,[,],(,),{,},:,=,+,-,*,\n", "<,<,>,>,:,:,+,+,-,-,=,=,+,=,-,=,*,=,/,=,-,>");
	std::cout << "OUPUT \n";
	do
	{
		std::string tok = toker.getTok();
		if (tok == "\n")
		{
			tok = "newline";
		
		}
		std::cout << "\n -- " << tok;
	 
	} while (in.good());
	std::cout << "\n";
	
	return 0;
}
int main()
{
	std::cout << "-----------------------------------------------------------------------------------------------------\n";
	std::cout << "Requirement 1 -" << " " << " I have used Microsoft Visual Studio C++ Console Projects" << std::endl;
	std::cout << "-------------------------------------------------------------------------------------------------------\n";
	std::cout << "Requirement 2 -" << " " << " I have used <iostream> for all I/O and new and delete for heap based management " << std::endl;
	std::cout << "----------------------------------------------------------------------------------------------------------\n";
	std::cout << "Requirement 3 -" << " " << " I have provided Tokenzier Package, SemiExp package whose .ccp implements an interface named iTokCollection.h" << std::endl;
	std::cout << "------------------------------------------------------------------------------------------------------------\n";
	std::cout << "Requirement 4 -" << " " << "Tokenizing Test File into Alphanumeric, Punctuator, Special One Character, Special Two Character, C Style Comments, C++ Comments and Quoted Strings" << std::endl;
	std::cout << "-------------------------------------------------------------------------------------------------------------\n";
	Toker toker;
	int f = dealTokenizer(toker);
	toker.des();
	std::cout << "------------------------------------------------------------------------------------------\n";
	std::cout << "Requirement 5 - " << " " << "The getTok() method prodduces one token for each call to it" << std::endl;
	std::cout << "-------------------------------------------------------------------------------------------\n";
	std::cout << "Requirement 6 -" << " " << "SemiExp package produces set of tokens by repeatdely calling getTok() until we run into a termination condition " << std::endl;
	std::cout << "--------------------------------------------------------------------------------------------\n";
	std::cout << "Requirement 7 - " << " " << "SemiExpressions are terminated on },{,; and : public, :private and :protected " << std::endl;
	std::cout << "---------------------------------------------------------------------------------------------\n";
	std::cout << "Requirement 8 - " << " " << "Folding for" << std::endl;
	std::cout << "----------------------------------------------------------------------------------------------\n";
	std::cout << "Output for 5,6,7,8 with same input file passed to Tokennizer with the Comment toggle turned off" << std::endl;
	int g = dealSemiExpressions();
	std::cout << "\n------------------------------------------------------------------------------------------------\n";
	std::cout << "Requirement 9 -" << " " << " All the virtual methods in the iTokCollection interface have been implemented by the SemiExp Class " << std::endl;
	std::cout << "------------------------------------------------------------------------------------------------\n";
	std::cout << "Requirement 10 -" << " " << " TestExecutive which illustrates all possible test cases" << std::endl;
	std::cout << "---------------------------------------------------------------------------------------------------\n";
	std::cout << "Please enter any character to continue " << std::endl;

	std::getchar();
	
}
