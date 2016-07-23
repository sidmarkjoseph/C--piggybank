#include "TypeTable.h"
int main()
{
	TypeTable test;
	std::string a = "Sid";
	std::vector<std::string> b = { "mark","joseph" };
	test.addRecord(a, b);
	test.show();
	std::getchar();

}
