                                                          README

1. Design Decisions
   a. Firstly I started off building a package called TypeAnalysis which I decided to be the central point of the application.
   b. In the TestExecutive I created a TypeAnalysis object whose member method filesReturn uses FileHandlers and FileMgr to return a vector of .h and .cpp files
   c. I also created a Forward Declartion in the TypeAnalysis.h file to a QueuedWorkItems instance. And I control the ThreadPool using this instance.(Queuing and 
      Dequeing the Blocking Queue with WorkItems that return fileNames)
   d. Using the FileNames from the ThreadProc I load the Parser and Tokenzier respectively for threads associated with that FileName.
   e. I passed the FileName arguments and TypeTable arguments to the Parser through the Build function of ConfigParser.
   f. The Partial TypeTables are merged and then put into a Result Queue which I merge into one big typetable using Concat function of TypeTable.h.
   g. If a particular symbol on Tokenizing File A is present in the TypeTable and is declared in File B I then say File A is dependent on File B
  
2. Test Explaination
    a. I have created a Test.h and Test.cpp and TestOther.h in a Test Folder. Mutual dependcies are shown between Test.h and Test.cpp becasue of Class, Enum,
    Typedef, Global Functions and Alias's. Another file called TestAnother.h contains a forward declaration of class Test and this dependency is also show. 
      
3. Pass "..\\TestFolder" as argument to bat file to run the tests.


