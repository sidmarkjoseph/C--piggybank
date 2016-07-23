////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//TypeTable.h - Definies the functionaties associated with a type table which is defined as an unordered map  // 
//Author : Siddharth Joseph, MS Computer Science                                                              // 
//ver 1.0                                                                                                     //
// Package Operations - Has methods for controlling TypeTable which is implemented using an unordered map     //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*Public Interface                                                                                            //
addRecord(std::string, std::vector<std::string>) - Adds a record to the TypeTable                             //
show() - shows the Type Table                                                                                 //
concat(TypeTable*) - Concats 2 TypeTables*/                                                                   //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
class TypeTable
{
	using Key = std::string;
	using Values = std::vector<std::string>;
public:

	std::unordered_map<Key, Values> map;
	void addRecord(std::string temp, std::vector<std::string> vec)
	{

		map[temp] = vec;
	}
	void show()
	{
		std::unordered_map<Key, Values>::iterator it;
		
		for (it = map.begin(); it != map.end(); it++)
		{
			std::cout << it->first << " ";  // string (key)
			for (auto temp : it->second)
			{
				std::cout << temp << " ";
			}
			std::cout << std::endl;


		}
	}
	void concat(TypeTable* b,std::string add)
	{
		std::unordered_map<Key, Values>::iterator it;
		std::unordered_map<Key, Values>::iterator it1;
		for (it = b->map.begin(); it != b->map.end(); it++)
		{
			it1 = map.find(it->first);
			if (it1 != map.end())
			{
				it1->second[1] = it1->second[1] + "?" + it->second[1];
				
				
			}
			else
			{
				map[it->first] = it->second;
			}

		}
	}

	
};
