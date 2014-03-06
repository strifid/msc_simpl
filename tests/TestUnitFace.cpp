#include <mt/tester/CppUTest/TestHarness.h>
#include "../Vertex.h"
#include "../Edge.h"

TEST_GROUP(TestUnitFace) {
};

/*
TEST(TestUnitFace, testAsKeyMap) {
	Vertex a(0, 0);
	Vertex b(0, 1);
	Vertex c(1, 0);
//	Vertex d(1, 1);

	Edge ab(&a, &b);
	Edge bc(&b, &c);
	Edge ca(&c, &a);
	Edge ac(&a, &c);

	std::map<EdgePtr, int, EdgesComparator> myMap;
	myMap[&ab] = 0;
	myMap[&bc] = 10;
	myMap[&ca] = 20;
	myMap[&ac] = 30;

	CHECK_EQUAL(3, myMap.size());
	CHECK_EQUAL(30, myMap[&ca]);

}

*/
