#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
using Key = std::string;
using Values = std::vector<std::string>;
class TypeTable
{
	
public:
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


		}
	}

private:
	std::unordered_map<Key, Values> map;
};
