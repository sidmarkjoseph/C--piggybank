///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//TypeAnaysis.h - Method fileReturn() - Returns all files of a specific pattern in a particular directory    //
//              - Forward Declartion to QueuedWorkItems that is reponsible for push Work Items into the      //
//                process queue, results into result Blocking Queue and also dequeing these work items and   //
//                results                                                                                    //
//ver 1.0                                                                                                    // 
//Author: Siddharth Joseph MS Computer Science    
/*Package Opearations
  Central Point of Application. Contains a forward declaration to the QueuedWorkItems to control the 
  Blocking Queue
*/
/*
Public Interface
filesReturn() - Returns files in a specfic directory that satisfy a particular pattern
processor - Instance of ProcessWorkItem<WorkResult> 
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "../FileMgr/FileMgr/FileMgr.h"
#include "../FileMgr/FileMgr/FileSystem.h"
#include "../FileMgr/FileMgr/IFileMgr.h"
//#include "../../../Desktop/Project3/QueuedWorkItems/QueuedWorkItems.h"
#include "../QueuedWorkItems/QueuedWorkItems.h"
using namespace FileManager;
using WorkResult = std::string;
using file = std::string;
class TypeAnalysis
{
public:
	std::vector<file>* filesReturn(std::string);
	ProcessWorkItem<WorkResult> processor;
private:
	IFileMgr* pFmgr;	
}
;
