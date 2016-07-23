#ifndef ITOKCOLLECTION_H
#define ITOKCOLLECTION_H
/////////////////////////////////////////////////////////////////////
//  ITokCollection.h - package for the ITokCollection interface    //
//  Lanaguage:     Visual C++ 2011                                 //
//  Platform:      MACBOOK PRO                                     //
//  Author:        Siddharth Joseph, MS Computer Science           //
//                 Syracuse University                             //
//   Pupose: Interface for SemiExp                                 //
/////////////////////////////////////////////////////////////////////


 /* Maintenance History:
  ====================
  ver 1.0 : 03/12/2016
  - first release
  ver 2.0 : April 7th 2016
*/
#include <string> 

struct ITokCollection
{
  virtual bool get(bool clear=true)=0;
  virtual size_t length()=0;
  virtual std::string& operator[](size_t n) = 0;
  virtual std::string operator[](size_t n) const = 0;
  virtual size_t find(const std::string& tok)=0;
  virtual void push_back(const std::string& tok)=0;
  virtual bool remove(const std::string& tok)=0;
  virtual bool remove(size_t i)=0;
  virtual void toLower()=0;
  virtual void trimFront()=0;
  virtual void clear()=0;
  virtual std::string show(bool showNewLines=false)=0;
  virtual bool isComment(const std::string& tok) = 0;
  virtual ~ITokCollection() {};
};

#endif
