/*
 * TestPPairProcessor.cpp
 *
 *  Created on: Oct 13, 2013
 *      Author: trifid
 */

#include <mt/tester/CppUTest/TestHarness.h>
#include "../persist_pair/BarCodeProcessor.h"
#include "../Edge.h"

TEST_GROUP(BarCodeProcessor) {

	void initRelations(BarCodeProcessor &proc) {

		proc.m_relations.addPair(17, 11);
		proc.m_relations.addPair(17, 20);
		proc.m_relations.addPair(17, 6);

		proc.m_relations.addPair(14, 11);
		proc.m_relations.addPair(14, 20);
		proc.m_relations.addPair(14, 7);

		proc.m_relations.addPair(10, 6);
		proc.m_relations.addPair(10, 20);
		proc.m_relations.addPair(10, 2);

		proc.m_relations.addPair(8, 20);
		proc.m_relations.addPair(8, 7);
		proc.m_relations.addPair(8, 19);
		proc.m_relations.addPair(8, 2);

		proc.m_relations.addPair(12, 7);
		proc.m_relations.addPair(12, 1);
		proc.m_relations.addPair(12, 19);

		proc.m_relations.addPair(13, 2);
		proc.m_relations.addPair(13, 5);
		proc.m_relations.addPair(13, 19);

		proc.m_relations.addPair(18, 1);
		proc.m_relations.addPair(18, 21);
		proc.m_relations.addPair(18, 19);
		proc.m_relations.addPair(18, 5);

		proc.m_relations.addPair(15, 1);
		proc.m_relations.addPair(15, 21);
		proc.m_relations.addPair(15, 4);

		proc.m_relations.addPair(16, 5);
		proc.m_relations.addPair(16, 21);
		proc.m_relations.addPair(16, 3);

		proc.m_relations.addPair(9, 4);
		proc.m_relations.addPair(9, 21);
		proc.m_relations.addPair(9, 3);

		PPointPtr p = new PPoint(1, PPoint::POSITIVE, 0);
		proc.m_points[p->m_id] = p;
		p = new PPoint(2, PPoint::POSITIVE, 0);
		proc.m_points[p->m_id] = p;
		p = new PPoint(3, PPoint::POSITIVE, 0);
		proc.m_points[p->m_id] = p;
		p = new PPoint(4, PPoint::POSITIVE, 0);
		proc.m_points[p->m_id] = p;
		p = new PPoint(5, PPoint::POSITIVE, 0);
		proc.m_points[p->m_id] = p;
		p = new PPoint(6, PPoint::POSITIVE, 0);
		proc.m_points[p->m_id] = p;
		p = new PPoint(7, PPoint::POSITIVE, 0);
		proc.m_points[p->m_id] = p;
		p = new PPoint(8, PPoint::UNIVERSAL, 1);
		proc.m_points[p->m_id] = p;
		p = new PPoint(9, PPoint::UNIVERSAL, 1);
		proc.m_points[p->m_id] = p;
		p = new PPoint(10, PPoint::UNIVERSAL, 1);
		proc.m_points[p->m_id] = p;
		p = new PPoint(11, PPoint::POSITIVE, 0);
		proc.m_points[p->m_id] = p;
		p = new PPoint(12, PPoint::UNIVERSAL, 1);
		proc.m_points[p->m_id] = p;
		p = new PPoint(13, PPoint::UNIVERSAL, 1);
		proc.m_points[p->m_id] = p;
		p = new PPoint(14, PPoint::UNIVERSAL, 1);
		proc.m_points[p->m_id] = p;
		p = new PPoint(15, PPoint::UNIVERSAL, 1);
		proc.m_points[p->m_id] = p;
		p = new PPoint(16, PPoint::UNIVERSAL, 1);
		proc.m_points[p->m_id] = p;
		p = new PPoint(17, PPoint::UNIVERSAL, 1);
		proc.m_points[p->m_id] = p;
		p = new PPoint(18, PPoint::UNIVERSAL, 1);
		proc.m_points[p->m_id] = p;
		p = new PPoint(19, PPoint::NEGATIVE, 2);
		proc.m_points[p->m_id] = p;
		p = new PPoint(20, PPoint::NEGATIVE, 2);
		proc.m_points[p->m_id] = p;
		p = new PPoint(21, PPoint::NEGATIVE, 2);
		proc.m_points[p->m_id] = p;

	}


};

TEST(BarCodeProcessor, testBarCodeProc) {

	BarCodeProcessor proc;
	initRelations(proc);
	proc.computeBarCodes("test");


//	CHECK_EQUAL(5, proc.ppairs.size());

}

