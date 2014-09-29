/*
 * TestBigDecimal.cpp
 *
 *  Created on: Sep 29, 2014
 *      Author: trifid
 */


#include <mt/tester/CppUTest/TestHarness.h>
#include "BigDecimal.h"


TEST_GROUP(TestUnitBigDecimal) {


};

TEST(TestUnitBigDecimal, testCompare) {

	BigDecimal a(1, 20);
	BigDecimal b(1, 25);

	CHECK(a < b);
	CHECK(!(a > b));
	CHECK(!(a == b));
	CHECK(a != b);

}
