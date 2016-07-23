///////////////////////////////////////////////////////////////////////
// QueuedWorkItems.h - child thread processes enqueued work items    //
//                                                                   //
// Siddharh Joseph MS Computer Science Syracuse                      //
///////////////////////////////////////////////////////////////////////
/*
* A single child thread processes work items equeued by main thread
--------------------------------------------------------------------
Public Interface
setNumberOfThreads(int) - sets Number of Threads to operate on the Blocking queue
start() - creates threads and pushes the thread onto the thread array and associates each with the threadProc
startone() - start() for Tokenizer
doWork(WorkItem<Result>*) - pushes work item on the Blocking Queue
result() -  Obtains the result from the Blocking Result Queue
resultone() - result() for Tokenizer
setTypeTable(TypeTable *) - Passes TypeTable to Dependency Analyzier
*/

#include <thread>
#include <functional>
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include <exception>
#include "../Parser/Parser.h"
#include "../Parser/ActionsAndRules.h"
#include "../Parser/ActionsAndRules.h"
#define Util StringHelper
template<typename Result>
using WorkItem = std::function<Result()>;

///////////////////////////////////////////////////////////////////////
// class to process work items
// - each work item is processed sequentially on a single child thread

template<typename Result>
class ProcessWorkItem
{
public:
 void setNumberOfThreads(int);
  void start();
  void startone();
  void doWork(WorkItem<Result>* pWi);
  std::vector<std::string> resultone();
  TypeTable* result();
  void wait();
  void setTypeTable(TypeTable*);
  ~ProcessWorkItem();
  TypeTable* typetable;
private:
	int no_of_threads;
  std::vector<std::thread*> _pThread;
  BlockingQueue<WorkItem<Result>*> _workItemQueue;
  BlockingQueue<TypeTable*> _resultsQueue;
  BlockingQueue<std::vector<std::string>> _resultsOneQueue;
  
};
//----< wait for child thread to terminate >---------------------------

template<typename Result>
void ProcessWorkItem<Result>::setNumberOfThreads(int a)
{
	no_of_threads = a;
}
template<typename Result>
void ProcessWorkItem<Result>::setTypeTable(TypeTable* a)
{
	typetable = a;
}
template<typename Result>
void ProcessWorkItem<Result>::wait() //written
{
	for (auto _pTemp : _pThread)
	{
		try
		{
			_pTemp->join();
		}
		catch (std::exception &e)
		{
			std::cout << e.what() << std::endl;
		}
	}
	
}
//----< enqueue work item >--------------------------------------------

template<typename Result>
void ProcessWorkItem<Result>::doWork(WorkItem<Result> *pWi)
{
  _workItemQueue.enQ(pWi);
}
template<typename Result>
std::vector<std::string> ProcessWorkItem<Result>::resultone()
{
	return _resultsOneQueue.deQ();
}
//----< retrieve results with blocking call >--------------------------

template<typename Result>
TypeTable* ProcessWorkItem<Result>::result()
{
  return _resultsQueue.deQ();
}

//----< start child thread that dequeus work items >-------------------

template<typename Result>
void ProcessWorkItem<Result>::startone()
{
	for (int i = 0; i < no_of_threads; i++)
	{
		std::function<void()> threadProc =
			[&]() {
			while (true)
			{
				WorkItem<Result>* pWi = _workItemQueue.deQ();
				if (pWi == nullptr)
					return;
				Result result = (*pWi)();
				std::vector<std::string> vec;
				try
				{
					std::ifstream in(result);
					if (!in.good())
					{
						std::cout << "\n  can't open " << result << "\n\n";
						return;
					}
					Toker toker;
					toker.attach(&in);
					do
					{
						std::string tok = toker.getTok();
						if (tok == "\n" || tok == "[" || tok == "void" || tok == "=")
							continue;
						else
						{
							auto it = typetable->map.find(tok);
							
								if (it != typetable->map.end())
								{
								   if (it->second[1] == result)
									continue;
									else if (it->second[1].find('?') != std::string::npos)
									{
										int found = it->second[1].find('?');
										if (it->second[1].substr(0, found) == result)
											vec.push_back(it->second[1].substr(0, found) + std::string(" is dependent on") + it->second[1].substr(found + 1) + std::string(" because of symbol ") + tok + " of type " + it->second[0] + std::string("\n"));
										else if (it->second[1].substr(found + 1) == result)
											vec.push_back(it->second[1].substr(found + 1) + std::string(" is dependent on") + it->second[1].substr(0, found) + std::string(" because of symbol ") + tok + " of type " + it->second[0] + std::string("\n"));
									}
									else
										vec.push_back(result + std::string(" is dependent on ") + it->second[1] + std::string(" because of symbol ") + tok + " of type " + it->second[0] + std::string("\n"));
								}
						}
					} while (in.good());
				}
				catch (std::exception &e)
				{
					std::cout << e.what() << std::endl;
				}
				_resultsOneQueue.enQ(vec);
			}
		};
		std::thread* _pTemp = new std::thread(threadProc);
		_pThread.push_back(_pTemp);
	}
		
}
template<typename Result>
void ProcessWorkItem<Result>::start() 
{
	for (int i = 0; i < no_of_threads; i++)
	{
		std::function<void()> threadProc =
			[&]() {
			while (true)
			{
				WorkItem<Result>* pWi = _workItemQueue.deQ();
				if (pWi == nullptr)
				{
					return;
				}
				Result result = (*pWi)();
				TypeTable *temp = new TypeTable;
				ConfigParseToConsole configure;
				Parser* pParser = configure.Build(result, temp);
				try
				{
					if (pParser)
					{
						if (!configure.Attach(result))
							std::cout << "\n  could not open file " << std::endl;
					}
					else
						std::cout << "\n\n  Parser not built\n\n";
					while (pParser->next())
					{
						pParser->parse();
					}
				}
				catch (std::exception& ex)
				{
					std::cout << ex.what() << "\n";
				}
				_resultsQueue.enQ(temp);
			}
		};
		std::thread* _pTemp = new std::thread(threadProc);
		_pThread.push_back(_pTemp);
	}
}
template<typename Result>
ProcessWorkItem<Result>::~ProcessWorkItem()
{
	for (auto _pTemp : _pThread)
		delete _pTemp;
}


