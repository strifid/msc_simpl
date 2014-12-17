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
#include "ArcStorage.h"

class PersistPairProcessor {
public:
	PersistPairProcessor();
	virtual ~PersistPairProcessor();
	bool init(MsComplexesSet& msCmplxSet);
	bool init(AscArcStorage& ascArcs, DescArcStorage& descArcs);

	void findPairs();

	typedef std::map<uint32_t, PPointPtr> PointsMap;

	PointsMap m_points;
	std::vector<PPointPtr> m_sortedPoints;
	PPointsRelations m_relations;

	std::set<uint32_t> filtered;
	std::vector<std::pair<uint32_t, uint32_t> > ppairs;
	std::map<uint32_t, std::vector<uint32_t>*> m_cycles;
	std::vector<std::pair<PPointPtr, PPointPtr> > m_ppairsPoint;

	void sort();
	void printToFile(const std::string& file);
	std::vector<std::pair<PPointPtr, PPointPtr> >& filter(uint32_t persistence);

	std::vector<std::pair<PPointPtr, PPointPtr> >& createPpairVector();

protected:


	std::vector<uint32_t> getNeighbs(uint32_t point);
	PPointPtr getPoint(uint32_t id);

	void addInCycle(std::vector<uint32_t>* a, std::vector<uint32_t>* b);
	void cycleSearch(uint32_t point);

	uint32_t getHighest(std::vector<uint32_t>& v);

};

struct PPointComparator {
	bool operator()(const PPointPtr a, const PPointPtr b) const {
		return a->value() < b->value();
	}
};

#endif /* PERSISTPAIRPROCESSOR_H_ */
