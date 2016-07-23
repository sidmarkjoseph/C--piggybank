#include "TypeTable.h"
int main()
{
	TypeTable a;
	std::vector<std::string> str = { "sid", "mark" };
	std::string b = "SJBHS";
	a.addRecord(b, str);
	a.show();
	std::getchar();
	
}