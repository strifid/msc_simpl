/*
 * TestPPairProcessor.cpp
 *
 *  Created on: Oct 13, 2013
 *      Author: trifid
 */

#include <mt/tester/CppUTest/TestHarness.h>
#include "../persist_pair/PersistPairProcessor.h"
#include "../Edge.h"

TEST_GROUP(PPairProcessor) {

	void initRelations(PersistPairProcessor &proc) {

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

		PPointPtr p = new PPoint(0,0,1, PPoint::POSITIVE);
		proc.m_points[p->m_id] = p;
		p = new PPoint(0,0,2, PPoint::POSITIVE);
		proc.m_points[p->m_id] = p;
		p = new PPoint(0,0,3, PPoint::POSITIVE);
		proc.m_points[p->m_id] = p;
		p = new PPoint(0,0,4, PPoint::POSITIVE);
		proc.m_points[p->m_id] = p;
		p = new PPoint(0,0,5, PPoint::POSITIVE);
		proc.m_points[p->m_id] = p;
		p = new PPoint(0,0,6, PPoint::POSITIVE);
		proc.m_points[p->m_id] = p;
		p = new PPoint(0,0,7, PPoint::POSITIVE);
		proc.m_points[p->m_id] = p;
		p = new PPoint(0,0,8, PPoint::UNIVERSAL);
		proc.m_points[p->m_id] = p;
		p = new PPoint(0,0,9, PPoint::UNIVERSAL);
		proc.m_points[p->m_id] = p;
		p = new PPoint(0,0,10, PPoint::UNIVERSAL);
		proc.m_points[p->m_id] = p;
		p = new PPoint(0,0,11, PPoint::POSITIVE);
		proc.m_points[p->m_id] = p;
		p = new PPoint(0,0,12, PPoint::POSITIVE);
		proc.m_points[p->m_id] = p;
		p = new PPoint(0,0,13, PPoint::POSITIVE);
		proc.m_points[p->m_id] = p;
		p = new PPoint(0,0,14, PPoint::POSITIVE);
		proc.m_points[p->m_id] = p;
		p = new PPoint(0,0,15, PPoint::POSITIVE);
		proc.m_points[p->m_id] = p;
		p = new PPoint(0,0,16, PPoint::POSITIVE);
		proc.m_points[p->m_id] = p;
		p = new PPoint(0,0,17, PPoint::POSITIVE);
		proc.m_points[p->m_id] = p;
		p = new PPoint(0,0,18, PPoint::POSITIVE);
		proc.m_points[p->m_id] = p;
		p = new PPoint(0,0,19, PPoint::NEGATIVE);
		proc.m_points[p->m_id] = p;
		p = new PPoint(0,0,20, PPoint::NEGATIVE);
		proc.m_points[p->m_id] = p;
		p = new PPoint(0,0,21, PPoint::NEGATIVE);
		proc.m_points[p->m_id] = p;

	}
};


TEST(PPairProcessor, testPPairProccc) {
	CHECK(false);
}

TEST(PPairProcessor, testPPairProc) {

	PersistPairProcessor proc;
	initRelations(proc);
	proc.findPairs();

	CHECK(false);
	CHECK_EQUAL(3, proc.ppairs.size());

}
