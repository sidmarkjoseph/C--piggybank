#ifndef UTILITIES_H
#define UTILITIES_H
///////////////////////////////////////////////////////////////////////
// Utilities.h - small, generally useful, helper classes             //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Most Projects, CSE687 - Object Oriented Design       //
// Author:      Siddharth Josep,MS Computer Science, Syracuse University//
//                                                                   //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides classes StringHelper and Converter and a global
* function putline().  This class will be extended continuously for 
* awhile to provide convenience functions for general C++ applications.
*
* Build Process:
* --------------
* Required Files: Utilities.h, Utilities.cpp
*
* Build Command: devenv Utilities.sln /rebuild debug
*

* ver 1.0 : April 7th 2016
* 
*
* Planned Additions and Changes:
* ------------------------------
* - none yet
*/
#include <string>
#include <vector>
#include <sstream>
#include <functional>
#include <conio.h>

namespace Utilities
{
  class StringHelper
  {
  public:
    static std::vector<std::string> split(const std::string& src);
    static void Title(const std::string& src, char underline = '=');
    static void title(const std::string& src);
  };

  void putline();
  void promptAndWait();

  template <typename T>
  class Converter
  {
  public:
    static std::string toString(const T& t);
    static T toValue(const std::string& src);
  };

  template <typename T>
  std::string Converter<T>::toString(const T& t)
  {
    std::ostringstream out;
    out << t;
    return out.str();
  }

  template<typename T>
  T Converter<T>::toValue(const std::string& src)
  {
    std::istringstream in(src);
    T t;
    in >> t;
    return t;
  }
}
#endif
