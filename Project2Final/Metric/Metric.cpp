#include <iostream>
#include <string>
#include "../Utilities/Utilities.h"
#include "../Tokenizer/Tokenizer.h"
#include "../SemiExp/SemiExp.h"
#include "../Parser/Parser.h"
#include "../Parser/ActionsAndRules.h"
#include "../Parser/ConfigureParser.h"
#include "../FileSystem-Windows/FileSystemDemo/FileSystem.h"
#include <queue>
#include <string>
#define Util StringHelper
using namespace Utilities;
int main(int argc, char* argv[])/*Entry Point for Application*/
{
	
	//std::string path = argv[1];
	//std::string pattern = argv[2];
	/*std::cout << "Requirement 1 : Used Visual Studio 2015 and its C++ Windows Console Projects \n";
	std::cout << "-----------------------------------------------------------------------------\n";
	std::cout << "Requirement 2 : Used the C++ standard library's streams for all I/O and new and delete for all heap-based memory management \n";
	std::cout << "-----------------------------------------------------------------------------\n";
	std::cout << "Requirement 3 : Packages for analyzing function size and complexity metrics for a set of specified packages. These packages will use the Tokenizer and SemiExpression packages you developed2 in Project #1. \n";
	std::cout << "-----------------------------------------------------------------------------\n";
	std::cout << "Requirement 4 : Parser package with a Parser class that is a container for Rules and that provides the interfaces IRule and IAction for rules contained in the Parser and actions contained in each rule \n";
	std::cout << "-----------------------------------------------------------------------------\n";
	std::cout << "Requirement 5 : RulesAndActions to detect all functions and begining and end of all C++ Scopes and and actions to build Abstract Syntax Tree \n";
	std::cout << "-----------------------------------------------------------------------------\n";
	std::cout << "Requirement 6 : Facility for building an abstract syntax tree that provides an interface for adding scope nodes to the tree and an methods to analyze the contents of the tree. \n";
	std::cout << "-----------------------------------------------------------------------------\n";
	std::cout << "Requirement 7 : FileMgr package that supports finding files and/or directories in a directory tree rooted at a specified path \n";
	std::cout << "-----------------------------------------------------------------------------\n";
	std::cout << "Requirement 8 : MetricsAnalysis package for evaluating and displaying the size and complexity of all global functions, static member functions, and non-static member functions in each of a set of specified packages \n";
	std::cout << "-----------------------------------------------------------------------------\n";
	std::cout << "Requirement 9 : MetricsExecutive package that enables collecting metrics for all the packages with names that match a specified pattern in a directory tree rooted at a specified path. Please define the path and file patterns on the MetricsExecutive command line \n";
	std::cout << "-----------------------------------------------------------------------------\n";
	std::cout << "Requiremnet 10 : Test Executive: \n";
	std::cout << " I have created a folder named Test where there are two cpp Files namely Parser.cpp and Source.cpp and a text file namely sid.txt. I ran my Metrics Analyzer on \".cpp\" files with the specified path and pattern \".cpp\". Everytime I pop a function I display the metrics of the function together with the state of the AST for that file\n";*/
	std::vector<std::string> vec = FileSystem::Directory::getDirectories("C:\\Users\\Siddharth Joseph\\Desktop\\Project2Final\\");
	//std::cout << vec[0];
	/*std::vector<std::string> vector2 = FileSystem::Directory::getFiles(path,pattern);

	
	for (std::vector<std::string>::iterator iter = vector2.begin(); iter != vector2.end(); iter++)
	{
		std::string fileSpec = path + "\\" + *iter;
		std::string msg = "Processing file" + fileSpec;
		Util::title(msg);
		putline();

		ConfigParseToConsole configure;
		Parser* pParser = configure.Build();
		try
		{
			if (pParser)
			{
				if (!configure.Attach(fileSpec))
				{
					std::cout << "\n  could not open file " << std::endl;
					continue;
				}
			}
			else
			{
				std::cout << "\n\n  Parser not built\n\n";
				return 1;
			}
			// now that parser is built, use it
			while (pParser->next())
				pParser->parse();
			std::cout << "\n";
		}
		catch (std::exception& ex)
		{
			std::cout << "\n\n    " << ex.what() << "\n\n";
		}
	}
	std::cout << "\n";*/
	std::cout << "Press any key to continue" << std::endl;
	getchar();
}