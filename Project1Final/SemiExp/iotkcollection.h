#ifndef ITOKCOLLECTION_H
#define ITOKCOLLECTION_H
/////////////////////////////////////////////////////////////////////
//  ITokCollection.h - package for the ITokCollection interface    //
//  ver 1.0                                                        //
//  Language:      C++, Visual Studio 2015                         //
//  Platform:      MacBook Pro, Microsoft Windows 10               //
//  Author:        Siddharth Mark Joseph, Syracuse University      //
//  Email:         smjoseph@syr.edu                                //
//  SUID:          384284085                                       //
/////////////////////////////////////////////////////////////////////
/*
Module Purpose:
===============
ITokCollection is an interface that declares pure virtual functions which are
meant to be implemented by the SemiExp Class

Maintenance History:
====================
ver 1.0 : 9th Febuary 2016
- first release
*/
#include<string>

struct ITokCollection
{
	virtual bool get(bool clear = true) = 0;
	virtual size_t length() = 0;
	virtual std::string operator[](size_t n) = 0;
	virtual int find(const std::string& tok) = 0;
	virtual void push_back(const std::string& tok) = 0;
	virtual bool remove(const std::string& tok) = 0;
	virtual bool remove(size_t i) = 0;
	virtual void toLower() = 0;
	virtual void trimFront() = 0;
	virtual void clear() = 0;
	virtual std::string show(bool showNewLines = false) = 0;
	virtual ~ITokCollection() {};
};

#endif


