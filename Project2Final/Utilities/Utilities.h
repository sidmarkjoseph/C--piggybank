#ifndef UTILITIES_H
#define UTILITIES_H
///////////////////////////////////////////////////////////////////////
// Utilities.h - small, generally usefule, helper classes            //
// ver 1.1                                                           //
// Language:    C++  2011                                            //
// Author:      Siddharth Joseph, MS Computer Science                //
//              Syracuse University                                  //
//                                                                   //
///////////////////////////////////////////////////////////////////////

#include <string>
#include <vector>
#include <sstream>
#include <functional>

namespace Utilities
{
  class StringHelper
  {
  public:
    static std::vector<std::string> split(const std::string& src);
    static void Title(const std::string& src, char underline = '-');
    static void title(const std::string& src);
  };

  void putline();

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
