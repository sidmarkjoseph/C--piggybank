/////////////////////////////////////////////////////////////////////////
// MsgClient.cpp - Demonstrates simple one-way HTTP messaging          //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2016       //
//  ver 1.2                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//Platform:        Macbook Pro                                        //
//  Application:   Prototype for CSE687 Pr4, Sp09                  //
//  Author:        Siddharth Mark Joseph , MS Computer Science             //
//                 Syracuse University 
/////////////////////////////////////////////////////////////////////

/*
 * This package implements a client that sends HTTP style messages and
 * files to a server that simply displays messages and stores files.
 *
 * It's purpose is to provide a very simple illustration of how to use
 * the Socket Package provided for Project #4.
 */
/*
* Required Files:
*   MsgClient.cpp, MsgServer.cpp
*   HttpMessage.h, HttpMessage.cpp
*   Cpp11-BlockingQueue.h
*   Sockets.h, Sockets.cpp
*   FileSystem.h, FileSystem.cpp
*   Logger.h, Logger.cpp
*   Utilities.h, Utilities.cpp
*/
/*
 * ToDo:
 * - pull the sending parts into a new Sender class
 * - You should create a Sender like this:
 *     Sender sndr(endPoint);  // sender's EndPoint
 *     sndr.PostMessage(msg);
 *   HttpMessage msg has the sending adddress, e.g., localhost:8080.
 */
#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../FileSystem/FileSystem.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include <string>
#include <iostream>
#include <thread>
#include <iostream>
#include <functional>
#include "../XmlDocument/XmlDocument.h"
#include "../XmlParser/XmlParser.h"
#include "../Utilities/Utilities.h"


using namespace XmlProcessing;
using sPtr = std::shared_ptr < AbstractXmlElement >;

using Show = StaticLogger<1>;
using namespace Utilities;
using Utils = StringHelper;

/////////////////////////////////////////////////////////////////////
// ClientCounter creates a sequential number for each client
//
class ClientCounter
{
public:
  ClientCounter() { ++clientCount; }
  size_t count() { return clientCount; }
private:
  static size_t clientCount;
};

size_t ClientCounter::clientCount = 0;

/////////////////////////////////////////////////////////////////////
// MsgClient class
// - was created as a class so more than one instance could be 
//   run on child thread
//
//create a Client Handler
class Sender
{
public:
  using EndPoint = std::string;
  void execute(std::string&);
  void extract();
  void sendDependency(std::vector<std::string>);
  void sendTheFile(std::string&);
  XmlDocument buildDocument(std::string&, std::string&);
  void messagecreate(std::string&);
  void messagextract(std::string&);
private:
	bool sendFile(const std::string&,const std::string& fqname, Socket& socket);
	HttpMessage makeMessage(size_t n, const std::string& msgBody, const EndPoint& ep);
	void sendMessage(HttpMessage& msg, Socket& socket);

 
};
class Reciever
{
public:
	Reciever(BlockingQueue<HttpMessage>& msgQ) : msgQ_(msgQ) {}
	void operator()(Socket socket);
	std::string package;
private:
	bool connectionClosed_;
	HttpMessage readMessage(Socket& socket);
	bool readFile(const std::string& filename, size_t fileSize, Socket& socket);
	BlockingQueue<HttpMessage>& msgQ_;
};
bool Reciever::readFile(const std::string& filename, size_t fileSize, Socket& socket)
{
	//std::cout << package << "hey" << "\n";
	std::string fqname = "../MsgClient/"+ package + "/" + filename + ".snt";
	FileSystem::File file(fqname);
	file.open(FileSystem::File::out, FileSystem::File::binary);
	if (!file.isGood())
	{
		/*
		* This error handling is incomplete.  The client will continue
		* to send bytes, but if the file can't be opened, then the server
		* doesn't gracefully collect and dump them as it should.  That's
		* an exercise left for students.
		*/
		Show::write("\n\n  can't open file " + fqname);
		return false;
	}

	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];

	size_t bytesToRead;
	while (true)
	{
		if (fileSize > BlockSize)
			bytesToRead = BlockSize;
		else
			bytesToRead = fileSize;

		socket.recv(bytesToRead, buffer);

		FileSystem::Block blk;
		for (size_t i = 0; i < bytesToRead; ++i)
			blk.push_back(buffer[i]);

		file.putBlock(blk);
		if (fileSize < BlockSize)
			break;
		fileSize -= BlockSize;
	}
	file.close();
	return true;
}
HttpMessage Reciever::readMessage(Socket& socket)
{
	connectionClosed_ = false;
	HttpMessage msg;

	// read message attributes

	while (true)
	{
		std::string attribString = socket.recvString('\n');
		if (attribString.size() > 1)
		{
			HttpMessage::Attribute attrib = HttpMessage::parseAttribute(attribString);
			msg.addAttribute(attrib);
		}
		else
		{
			break;
		}
	}
	// If client is done, connection breaks and recvString returns empty string

	if (msg.attributes().size() == 0)
	{
		connectionClosed_ = true;
		return msg;
	}
	// read body if POST - all messages in this demo are POSTs

	if (msg.attributes()[0].first == "POST")
	{
		// is this a file message?
		

		std::string filename = msg.findValue("file");
		if (filename != "")
		{
			size_t contentSize;
			std::string sizeString = msg.findValue("content-length");
			if (sizeString != "")
				contentSize = Converter<size_t>::toValue(sizeString);
			else
				return msg;

			//readFile(filename, contentSize, socket);
		}

		if (filename != "")
		{
			// construct message body

			msg.removeAttribute("content-length");
			std::string bodyString = "<file>" + filename + "</file>";
			std::string sizeString = Converter<size_t>::toString(bodyString.size());
			msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
			msg.addBody(bodyString);
		}
		else
		{
			// read message body
		
			size_t numBytes = 0;
			size_t pos = msg.findAttribute("content-length");
			if (pos < msg.attributes().size())
			{
				numBytes = Converter<size_t>::toValue(msg.attributes()[pos].second);
				Socket::byte* buffer = new Socket::byte[numBytes + 1];
				socket.recv(numBytes, buffer);
				buffer[numBytes] = '\0';
				std::string msgBody(buffer);
				msg.addBody(msgBody);
				delete[] buffer;
			}
		}
	}
	return msg;
}
void Reciever::operator()(Socket socket)
{
	/*
	* There is a potential race condition due to the use of connectionClosed_.
	* If two clients are sending files at the same time they may make changes
	* to this member in ways that are incompatible with one another.  This
	* race is relatively benign in that it simply causes the readMessage to
	* be called one extra time.
	*
	* The race is easy to fix by changing the socket listener to pass in a
	* copy of the clienthandler to the clienthandling thread it created.
	* I've briefly tested this and it seems to work.  However, I did not want
	* to change the socket classes this late in your project cycle so I didn't
	* attempt to fix this.
	*/
	while (true)
	{
		HttpMessage msg = readMessage(socket);
		if (connectionClosed_ || msg.bodyString() == "quit")
		{
			
		  //std::cout << "\n\n  clienthandler thread is terminating";
			break;
		}
		msgQ_.enQ(msg);
	}
	
}
//----< factory for creating messages >------------------------------
/*
 * This function only creates one type of message for this demo.
 * - To do that the first argument is 1, e.g., index for the type of message to create.
 * - The body may be an empty string.
 * - EndPoints are strings of the form ip:port, e.g., localhost:8081. This argument
 *   expects the receiver EndPoint for the toAddr attribute.
 */
void Sender::messagextract(std::string& package)
{
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
		{
			Show::write("\n client waiting to connect");
			::Sleep(100);
		}
		HttpMessage msg;
		std::string msgBody = "EXTRACT:"+package;
		msg = makeMessage(1, msgBody, "localhost:8000");
		sendMessage(msg, si);
		//msg = makeMessage(1, "quit", "toAddr:localhost:8080");
		//sendMessage(msg, si);
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}
void Sender::sendDependency(std::vector<std::string> dependent)
{
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
		{
			Show::write("\n client waiting to connect");
			::Sleep(100);
		}
		HttpMessage msg;
		
		std::string msgBody="DEPENDENT:";
		for (std::vector<std::string>::iterator it = dependent.begin(); it != dependent.end(); it++)
		{

			msgBody += *it + ":";
			
		}
		msg = makeMessage(1, msgBody, "localhost:8080");
		sendMessage(msg, si);
	
		
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}

}
HttpMessage Sender::makeMessage(size_t n, const std::string& body, const EndPoint& ep)
{
  HttpMessage msg;
  HttpMessage::Attribute attrib;
  EndPoint myEndPoint = "localhost:8081";  // ToDo: make this a member of the sender
                                           // given to its constructor.
  switch (n)
  {
  case 1:
    msg.clear();
    msg.addAttribute(HttpMessage::attribute("POST", "Message"));
    msg.addAttribute(HttpMessage::Attribute("mode", "oneway"));
    msg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ep));
    msg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + myEndPoint));

    msg.addBody(body);
    if (body.size() > 0)
    {
      attrib = HttpMessage::attribute("content-length", Converter<size_t>::toString(body.size()));
      msg.addAttribute(attrib);
    }
    break;
  default:
    msg.clear();
    msg.addAttribute(HttpMessage::attribute("Error", "unknown message type"));
  }
  return msg;
}
//----< send message using socket >----------------------------------

void Sender::sendMessage(HttpMessage& msg, Socket& socket)
{
  std::string msgString = msg.toString();
  socket.send(msgString.size(), (Socket::byte*)msgString.c_str());
}
//----< send file using socket >-------------------------------------
/*
 * - Sends a message to tell receiver a file is coming.
 * - Then sends a stream of bytes until the entire file
 *   has been sent.
 * - Sends in binary mode which works for either text or binary.
 */
bool Sender::sendFile(const std::string& package,const std::string& filename, Socket& socket)
{
  // assumes that socket is connected

  std::string fqname = "../MsgClient/"+ package + "/" +filename;
  FileSystem::FileInfo fi(fqname);
  size_t fileSize = fi.size();
  std::string sizeString = Converter<size_t>::toString(fileSize);
  FileSystem::File file(fqname);
  file.open(FileSystem::File::in, FileSystem::File::binary);
  if (!file.isGood())
    return false;
  
  HttpMessage msg = makeMessage(1, "", "localhost::8080");
  msg.addAttribute(HttpMessage::Attribute("file", filename));
  msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
  sendMessage(msg, socket);
  const size_t BlockSize = 2048;
  Socket::byte buffer[BlockSize];
  while (true)
  {
    FileSystem::Block blk = file.getBlock(BlockSize);
    if (blk.size() == 0)
      break;
    for (size_t i = 0; i < blk.size(); ++i)
      buffer[i] = blk[i];
    socket.send(blk.size(), buffer);
    if (!file.isGood())
      break;
  }
  file.close();
  return true;
}
//----< this defines the behavior of the client >--------------------
void Sender::sendTheFile(std::string& file)
{

	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
		{
			Show::write("\n client waiting to connect");
			::Sleep(100);
		}
	

	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}

}

void Sender::extract()
{
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
		{
			Show::write("\n client waiting to connect");
			::Sleep(100);
		}
		HttpMessage msg;
		std::string msgBody = "CHECKIN";
		msg = makeMessage(1, msgBody, "localhost:8000");
		sendMessage(msg, si);
		
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}
void Sender::messagecreate(std::string& package)
{
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8080))
		{
			Show::write("\n client waiting to connect");
			::Sleep(100);
		}

		// send a set of messages

		HttpMessage msg;
		msg = makeMessage(1,"TRANSFER:"+package, "localhost:8080");
		sendMessage(msg, si);
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}
void Sender::execute(std::string& package)
{
  // send NumMessages messages

  
  try
  {
    SocketSystem ss;
    SocketConnecter si;
    while (!si.connect("localhost", 8080))
    {
      Show::write("\n client waiting to connect");
      ::Sleep(100);
    }
    
    // send a set of messages
    HttpMessage msg;
    //  send all *.cpp files from TestFiles folder

    std::vector<std::string> files = FileSystem::Directory::getFiles("../MsgClient/"+package, "*.h");
	std::vector<std::string> filesone = FileSystem::Directory::getFiles("../MsgClient/"+package, "*.cpp");
    for (size_t i = 0; i < files.size(); ++i)
    {
		filesone.push_back(files[i]);
    }

	for (size_t i = 0; i < filesone.size(); ++i)
	{
		::Sleep(100);
		sendFile(package, filesone[i], si);
	
	}
    // shut down server's client handler

    msg = makeMessage(1, "quit", "toAddr:localhost:8080");
    sendMessage(msg, si);
    
  }
  catch (std::exception& exc)
  {
    Show::write("\n  Exeception caught: ");
    std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
    Show::write(exMsg);
  }
}
//----< entry point - runs two clients each on its own thread >------
XmlDocument Sender::buildDocument(std::string& file,std::string& parentfile)
{
	sPtr pRoot = makeTaggedElement("metadata");
	XmlDocument doc(XmlProcessing::makeDocElement(pRoot));

	sPtr child1 = makeTaggedElement("filename");
	child1->addChild(makeTextElement(parentfile));
	sPtr grandChild11 = makeTaggedElement("dependency");
	grandChild11->addChild(makeTextElement(file));
	child1->addChild(grandChild11);
	pRoot->addChild(child1);
	return doc;
}
int main(int argc, char *argv[])
{

	Sender c1;
	std::thread t1(
		[&]() { 
		BlockingQueue<HttpMessage> msgQ;
		
		try
		{
			SocketSystem ss;
			SocketListener sl(8081, Socket::IP6);
			Reciever cp(msgQ);
			sl.start(cp);	
			std::vector<std::string> dependent;
			std::string package = std::string(argv[1]);
			if (argc>2)
			{
				
				for (int i = 2; i < argc; i++)
					dependent.push_back(argv[i]);
				
			}
			c1.messagecreate(package);
			c1.execute(package);
			c1.sendDependency(dependent);

		}
		catch (std::exception& exc)
		{
			Show::write("\n  Exeception caught: ");
			std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
			Show::write(exMsg);
		}

	});
	
  t1.join();
 
  
 
}