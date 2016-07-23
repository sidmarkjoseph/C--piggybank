/////////////////////////////////////////////////////////////////////////
// MsgServer.cpp - Demonstrates simple one-way HTTP style messaging    //
//                 and file transfer                                   //
// Jim Fawcett, CSE687 - Object Oriented Design, Spring 2016       //
//  ver 1.2                                                        //
//  Language:      Visual C++ 2008, SP1                            //
//Platform:        Macbook Pro                                         //
//  Application:   Prototype for CSE687 Pr4, Sp09                  //
//  Author:        Siddharth Mark Joseph, MS Computer Science            //
//                 Syracuse University 
/////////////////////////////////////////////////////////////////////

/*
* This package implements a server that receives HTTP style messages and
* files from multiple concurrent clients and simply displays the messages
* and stores files.
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
 * - pull the receiving code into a Receiver class
 * - Receiver should own a BlockingQueue, exposed through a
 *   public method:
 *     HttpMessage msg = Receiver.GetMessage()
 * - You will start the Receiver instance like this:
 *     Receiver rcvr("localhost:8080");
 *     Reciever ch;
 *     rcvr.start(ch);
 */
#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../FileSystem/FileSystem.h"
#include "../Logger/Cpp11-BlockingQueue.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include <string>
#include <iostream>
#include <functional>
using Show = StaticLogger<1>;
using namespace Utilities;
using Utils = StringHelper;
#include "../XmlDocument/XmlDocument.h"
#include "../XmlParser/XmlParser.h"
#include <fstream>


using Show = StaticLogger<1>;
using namespace Utilities;
using namespace XmlProcessing;
using sPtr = std::shared_ptr <AbstractXmlElement>;
using Utils = StringHelper;
/////////////////////////////////////////////////////////////////////
// Reciever class
/////////////////////////////////////////////////////////////////////
// - instances of this class are passed by reference to a SocketListener
// - when the listener returns from Accept with a socket it creates an
//   instance of this class to manage communication with the client.
// - You need to be careful using data members of this class
//   because each client handler thread gets a reference to this 
//   instance so you may get unwanted sharing.
// - I may change the SocketListener semantics (this summer) to pass
//   instances of this class by value.
// - that would mean that all Recievers would need either copy or
//   move semantics.
//
//create a Messgae Client here
class Reciever
{
public:
  Reciever(BlockingQueue<HttpMessage>& msgQ) : msgQ_(msgQ) {}
  void operator()(Socket socket);
  void createDirectory();
  std::string package;
  std::string version;
  XmlDocument buildDocument(std::string&);
  XmlDocument build(std::string&, std::vector<std::string>&,std::vector<std::string>&);
  void Descendents(XmlDocument&, std::string&);
  void testDFS(XmlDocument&);
  void Element(XmlDocument&, std::string&);
  std::vector<std::string> dep;
  
private:
  bool connectionClosed_;
  HttpMessage readMessage(Socket& socket);
  bool readFile(const std::string& filename, size_t fileSize, Socket& socket);
  BlockingQueue<HttpMessage>& msgQ_;
  
};
class Sender
{
public:
	using EndPoint = std::string;
	void execute();
	void publicforsendFile();
	std::vector<std::string> list;
private:
	HttpMessage makeMessage(size_t n, const std::string& msgBody, const EndPoint& ep);
	void sendMessage(HttpMessage& msg, Socket& socket);
	bool sendFile(const std::string&, const std::string&, Socket&);
	
};
void Sender::publicforsendFile()
{
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8081))
		{
			Show::write("\n client waiting to connect");
			::Sleep(100);
		}

		// send a set of messages

		HttpMessage msg;
		//msg = makeMessage(1, package, "localhost:8080");
		//sendMessage(msg, si);
		/*for (size_t i = 0; i < NumMessages; ++i)
		{
		std::string msgBody =
		"<msg>Message #" + Converter<size_t>::toString(i + 1) +
		" from client #" + myCountString + "</msg>";
		msg = makeMessage(1, msgBody, "localhost:8080");*/
		/*
		* Sender class will need to accept messages from an input queue
		* and examine the toAddr attribute to see if a new connection
		* is needed.  If so, it would either close the existing connection
		* or save it in a map[url] = socket, then open a new connection.
		*/
		/*sendMessage(msg, si);
		Show::write("\n\n  client" + myCountString + " sent\n" + msg.toIndentedString());
		::Sleep(TimeBetweenMessages);
		}*/
		//  send all *.cpp files from TestFiles folder
		//HttpMessage msg;
		for (std::vector<std::string>::iterator it = list.begin(); it != list.end(); it++)
		{
			msg = makeMessage(1, *it, "localhost:8081");
			sendMessage(msg, si);
			std::vector<std::string> files = FileSystem::Directory::getFiles("../MsgServer/" + *it ,"*.h.snt");
			std::vector<std::string> filesone = FileSystem::Directory::getFiles("../MsgServer/" + *it , "*.cpp.snt");
			for (size_t i = 0; i < files.size(); ++i)
			{
			  filesone.push_back(files[i]);
			}
			for (size_t i = 0; i < filesone.size(); ++i)
			{
				sendFile(*it, filesone[i], si);
			}
		}
		
		// shut down server's client handler

		msg = makeMessage(1, "quit", "toAddr:localhost:8080");
		sendMessage(msg, si);
		//Show::write("\n\n  client" + myCountString + " sent\n" + msg.toIndentedString());

		/*Show::write("\n");
		Show::write("\n  All done folks");*/
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}
bool Sender::sendFile(const std::string& package, const std::string& filename, Socket& socket)
{
	// assumes that socket is connected

	std::string fqname = "../MsgServer/" + package + "/" + filename;
	std::cout << fqname;
	FileSystem::FileInfo fi(fqname);
	size_t fileSize = fi.size();
	std::string sizeString = Converter<size_t>::toString(fileSize);
	FileSystem::File file(fqname);
	file.open(FileSystem::File::in, FileSystem::File::binary);
	if (!file.isGood())
		return false;

	HttpMessage msg = makeMessage(1, "", "localhost::8081");
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
void Reciever::Descendents(XmlDocument& doc, std::string& search)
{
	//std::string testTag = "dependency";
	std::vector<sPtr> found = doc.descendents(search).select(); 
	//std::vector<auto> finalone;
	
	if (found.size() > 0)
	{
		for (auto pElem : found)
		{
			if (pElem->tag() != "")
			{
				Descendents(doc, pElem->tag());
				std::cout << "\n  found: " << pElem->tag();
			}
			else
				std::cout << "\n  found: " << pElem->value();
		}
	}
	else
	{
		std::cout << "\n  couldn't find descendent elements";
		return;
	}
	std::cout << "\n";
}
void Reciever::testDFS(XmlDocument& doc)
{


	std::function<void(AbstractXmlElement&)> f;
	f = [&](AbstractXmlElement& Elem) {
		if (Elem.tag().size() > 0)
			if (Elem.tag() == "dependencies")
				Element(doc,Elem.tag());
	};

	DFS(doc, f);
	std::cout << "\n";
    
}
void Reciever::Element(XmlDocument& doc, std::string& search)
{
	
	//title("testing doc.element(" + testTag + ").descendents()");
	std::vector<sPtr> found = doc.element(search).descendents().select();
	if (found.size() > 0)
	{
		for (auto pElem : found)
		{

			if (pElem->tag() != "")
			{
				if (pElem->tag() == std::string("dependency"))
					continue;
				//dep.push_back(pElem->tag());
			}
			else
			   dep.push_back(pElem->value());
		}
	}
	else
		std::cout << "\n  couldn't find descendent elements";
	std::cout << "\n";
}
HttpMessage Sender::makeMessage(size_t n, const std::string& body, const EndPoint& ep)
{
	HttpMessage msg;
	HttpMessage::Attribute attrib;
	EndPoint myEndPoint = "localhost:8080";  // ToDo: make this a member of the sender
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

void Sender::sendMessage(HttpMessage& msg, Socket& socket)
{
	std::string msgString = msg.toString();
	socket.send(msgString.size(), (Socket::byte*)msgString.c_str());
}
//----< this defines processing to frame messages >------------------
void Sender :: execute()
{
	try
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8081))
		{
			Show::write("\n client waiting to connect");
			::Sleep(100);
		}
		//std::cout << "Connection made";
		// send a set of messages
		
		HttpMessage msg;
		std::vector<std::string> packages = FileSystem::Directory::getDirectories("../MsgServer");
		std::string msgBody = "\nLIST OF PACKAGES ON THE SERVER\n";
		size_t i = 0;
		std::vector<std::string>::iterator bt;
		for (std::vector<std::string>::iterator it = packages.begin(); it != packages.end(); ++it)
		{
			if (*it == "Debug")
				continue;
			if (i == 0)
			{
				size_t ind = it->find_last_of("ver");
				*it = it->substr(0, ind + 1);
				msgBody += *it + "\n";
			}
			else
			{
				bt = it-1;
				size_t ind = it->find_last_of("ver");
				//*it = it->substr(0, ind + 1);
				if (bt->substr(0, ind + 1) == it->substr(0, ind + 1))
					continue;
				else
					msgBody += it->substr(0,ind-2) + "\n";
			}
			i++;
		}
		
		msg = makeMessage(1, msgBody, "localhost:8081");
		sendMessage(msg, si);
		//msg = makeMessage(1, "quit", "localhost:8081");
		//sendMessage(msg, si);
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}

}
/*XmlDocument Reciever::buildDocument()
{
	return NULL;
}*/
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
      readFile(filename, contentSize, socket);
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
//----< read a binary file from socket and save >--------------------
/*
 * This function expects the sender to have already send a file message, 
 * and when this function is running, continuosly send bytes until
 * fileSize bytes have been sent.
 */
XmlDocument Reciever::buildDocument(std::string& package)
{
	sPtr pRoot = makeTaggedElement("metadata");
	XmlDocument doc(XmlProcessing::makeDocElement(pRoot));
	sPtr child1 = makeTaggedElement("package");
	child1->addChild(makeTextElement(package));
	sPtr grandChild11 = makeTaggedElement("version");
	grandChild11->addChild(makeTextElement(version));
	child1->addChild(grandChild11);
	pRoot->addChild(child1);
	return doc;
}
XmlDocument Reciever::build(std::string& package, std::vector<std::string>& dependent,std::vector<std::string>& second)
{
	sPtr pRoot = makeTaggedElement("metadata");
	XmlDocument doc(XmlProcessing::makeDocElement(pRoot));
	sPtr child1 = makeTaggedElement("package");
	child1->addChild(makeTextElement(package));
	sPtr grandChild11 = makeTaggedElement("version");
	grandChild11->addChild(makeTextElement(version));
	sPtr grandChild21 = makeTaggedElement("dependencies");
	for (std::vector<std::string>::iterator it = dependent.begin(); it != dependent.end(); it++)
	{
		sPtr temp = makeTaggedElement("dependency");
		std::vector<std::string> listone = FileSystem::Directory::getDirectories("../MsgServer/", *it + "*");
		temp->addChild(makeTextElement(listone.back()));
		grandChild21->addChild(temp);

	}
	for (std::vector<std::string>::iterator it = second.begin(); it != second.end(); it++)
	{
		sPtr temp = makeTaggedElement("dependency");
	
		temp->addChild(makeTextElement(*it));
		grandChild21->addChild(temp);
	}
	child1->addChild(grandChild11);
	child1->addChild(grandChild21);
	pRoot->addChild(child1);
	return doc;


}
void Reciever::createDirectory()
{
	FileSystem::Directory::create("..MsgServer/" + package);
}
bool Reciever::readFile(const std::string& filename, size_t fileSize, Socket& socket)
{

  std::string fqname = "../MsgServer/"+ package + version + "/" + filename + ".snt";
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
//----< receiver functionality is defined by this function >---------

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
   * copy of the Reciever to the clienthandling thread it created.  
   * I've briefly tested this and it seems to work.  However, I did not want
   * to change the socket classes this late in your project cycle so I didn't
   * attempt to fix this.
   */
  while (true)
  {
    HttpMessage msg = readMessage(socket);
    if (connectionClosed_ || msg.bodyString() == "quit")
    {
     std::cout   << "\n\n  Reciever thread is terminating";
      break;
    }
    msgQ_.enQ(msg);
  }
}

//----< test stub >--------------------------------------------------

int main()
{
	std::cout << "----------------------Server------------------------------\n";
	BlockingQueue<HttpMessage> msgQ;
  try
  {
    SocketSystem ss;
    SocketListener sl(8080, Socket::IP6);
    Reciever cp(msgQ);
    sl.start(cp);
    /*
     * Since this is a server the loop below never terminates.
     * We could easily change that by sending a distinguished 
     * message for shutdown.
     */
	
    while (true)
    {
	  ::Sleep(100);
      HttpMessage msg = msgQ.deQ();
	  size_t ind = msg.bodyString().find(":");
	  std::string sub = msg.bodyString().substr(0, ind);
	  if (msg.bodyString() == "CHECKIN")
	  {
		  Sender c1;
		  c1.execute();
		  break;
	  }
	  
	  
	  if(sub == "TRANSFER")
	  {

		  sub = msg.bodyString().substr(ind+1);
		  std::vector<std::string> list = FileSystem::Directory::getDirectories("../MsgServer/",sub+"*");
		  if(list.empty())
		  {
			  cp.version = "ver1";
			  FileSystem::Directory::create("../MsgServer/" + sub + cp.version);
		  }
		  else
		  {

			  std::string str= list.back();
			  size_t a = str.find("ver");
			  a = a + 3;
			  size_t i_auto = std::stoi(str.substr(a), nullptr, 0);
			  i_auto++;
			  std::stringstream ss;
			  ss << i_auto;
			  cp.version = "ver" + ss.str();
			  FileSystem::Directory::create("../MsgServer/" + sub + cp.version);

		  }
		  cp.package = msg.bodyString().substr(ind+1); 
		  
		  
	  }
	  if (sub == "EXTRACT")
	  {
		  Sender c1;
		  sub = msg.bodyString().substr(ind + 1);
		  cp.package = msg.bodyString().substr(ind + 1);
		  c1.list = FileSystem::Directory::getDirectories("../MsgServer/", cp.package + "*");
		  for (std::vector<std::string>::iterator it = c1.list.begin(); it != c1.list.end();it++)
			  FileSystem::Directory::create("../MsgClient/" + *it);
		  c1.publicforsendFile();

	  }
	  if (sub == "DEPENDENT")
	  {
		  
		  sub = msg.bodyString().substr(ind+1);
		  if (sub == "")
		  {
			 XmlDocument doc = cp.buildDocument(cp.package);
			 std::ofstream myfile;
			 myfile.open("../MsgServer/"+cp.package+cp.version+"/"+cp.package+".xml");
			 myfile << doc.toString();
			 myfile.close();
			 break;
		  }
		  else
		  {
			  std::vector<std::string> dependent;
			  size_t i = 0;
			  size_t j = 0;
			  for (i = 0; i < sub.length(); i++)
			  {
				  if (sub.at(i) == ':')
				  {
					  dependent.push_back(sub.substr(j, i - j));
					  j = i + 1;
				  }
			  }
			  for (std::vector<std::string>::iterator it = dependent.begin(); it != dependent.end(); it++)
			  {
				  std::string temp = FileSystem::Directory::getDirectories("../MsgServer/", *it + "*").back();
				  std::vector<std::string> vec = FileSystem::Directory::getFiles("../MsgServer/" + temp, "*.xml");
				  std::string path = "../MsgServer/" + temp + "/" +vec[0];
				  try
				  {
					  XmlDocument doc2(path, XmlDocument::file);
					  cp.testDFS(doc2);
				  }
				  catch (std::exception& ex)
				  {
					  std::cout << "\n\n  " << ex.what();
				  }
			  }
			 
			  XmlDocument doc = cp.build(cp.package, dependent,cp.dep);
			  std::ofstream myfile;
			  myfile.open("../MsgServer/" + cp.package + cp.version + "/" + cp.package + ".xml");
			  myfile << doc.toString();
			  myfile.close();
			  break;
		  }
	  }
	  
	 
    }
  }
  catch (std::exception& exc)
  {
    Show::write("\n  Exeception caught: ");
    std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
    Show::write(exMsg);
  }
}