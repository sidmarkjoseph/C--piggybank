//////////////////////////////////////////////////////////////////////////////////////
// Author : Siddharth Joseph MS Computer Science                                    //
// ver 1.0                                                                          //
// Purpose : Tests TypeTable Package                                                //
//////////////////////////////////////////////////////////////////////////////////////
#include "TypeTable.h"
#ifdef TESTTYPETABLE
int main()
{
	TypeTable test;
	std::string a = "Sid";
	std::vector<std::string> b = { "mark","joseph" };
	test.addRecord(a, b);
	test.show();
	std::getchar();

}
#endif
