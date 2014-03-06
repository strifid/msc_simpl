/*
 * PersistPairProcessor.h
 *
 *  Created on: Sep 28, 2013
 *      Author: trifid
 */

#ifndef PERSISTPAIRPROCESSOR_H_
#define PERSISTPAIRPROCESSOR_H_
#include "MsComplex.h"
#include "PPoint.h"
#include "PPointsRelations.h"
#include "PComponent.h"


class PersistPairProcessor {
public:
	PersistPairProcessor();
	virtual ~PersistPairProcessor();
	bool init(MsComplexesSet& msCmplxSet);
	void findPairs();

	typedef std::map<uint32_t, PPointPtr> PointsMap;

	PointsMap m_points;
	std::vector<PPointPtr> m_sortedPoints;
	PPointsRelations m_relations;

	std::set<uint32_t> filtered;
	std::vector<std::pair<uint32_t, uint32_t> > ppairs;
	std::map<uint32_t, std::vector<uint32_t>* > m_cycles;

protected:

	PPointPtr getPoint(uint32_t id);

	void addInCycle(std::vector<uint32_t>* a, std::vector<uint32_t>* b);
	void cycleSearch(uint32_t point);

	uint32_t getHighest(std::vector<uint32_t>& v);


};


#endif /* PERSISTPAIRPROCESSOR_H_ */
