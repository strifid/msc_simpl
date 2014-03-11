/*
 * PersistPairProcessor.cpp
 *
 *  Created on: Sep 28, 2013
 *      Author: trifid
 */

#include "PersistPairProcessor.h"

#include <mt/common/Exception.h>
#include <stddef.h>
#include <stdint.h>
#include <algorithm>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <fstream>

#include "../Edge.h"
#include "../Triangle.h"
#include "../Vertex.h"

struct PPointComparator {
	bool operator()(const PPointPtr a, const PPointPtr b) const {
		return a->value() < b->value();
	}
} pPointComparator;

PersistPairProcessor::PersistPairProcessor() {

}

PersistPairProcessor::~PersistPairProcessor() {
}

bool PersistPairProcessor::init(MsComplexesSet& msCmplxSet) {

	for (MsComplexesSet::iterator it = msCmplxSet.begin(); it != msCmplxSet.end(); it++) {
		MsComplex *cmplx = *it;
		PPoint *ppointMax = new PPoint(cmplx->m_max->maxVertex()->value(), PPoint::NEGATIVE, (uint32_t) 2);
		ppointMax->x = cmplx->m_max->maxVertex()->x;
		ppointMax->y = cmplx->m_max->maxVertex()->y;

		m_points[ppointMax->m_id] = ppointMax;

		PPointPtr ppointMin = new PPoint(cmplx->m_min->value(), PPoint::POSITIVE, 0);
		ppointMin->x = cmplx->m_min->x;
		ppointMin->y = cmplx->m_min->y;

		m_points[ppointMin->m_id] = ppointMin;

		for (size_t i = 0; i < cmplx->m_seddles.size(); ++i) {
			PPointPtr ppointSeddle = new PPoint(cmplx->m_seddles[i]->maxVertex()->value(), PPoint::UNIVERSAL, 1);
			ppointSeddle->x = cmplx->m_seddles[i]->maxVertex()->x;
			ppointSeddle->y = cmplx->m_seddles[i]->maxVertex()->y;

			m_points[ppointSeddle->m_id] = ppointSeddle;
			m_relations.addPair(ppointMax->m_id, ppointSeddle->m_id);
			m_relations.addPair(ppointMin->m_id, ppointSeddle->m_id);
		}

	}
	return true;
}

void PersistPairProcessor::findPairs() {
	for (PointsMap::iterator it = m_points.begin(); it != m_points.end(); ++it) {
		m_sortedPoints.push_back(it->second);
	}
	std::sort(m_sortedPoints.begin(), m_sortedPoints.end(), pPointComparator);

	for (size_t i = 0; i < m_sortedPoints.size(); ++i) {
		uint32_t point = m_sortedPoints[i]->m_id;
		filtered.insert(point);
		if (getPoint(point) && getPoint(point)->m_type != PPoint::POSITIVE)
			cycleSearch(point);
	}
}

struct PPairComparatorByPersistence {
public:
	bool operator()(const std::pair<PPointPtr, PPointPtr> &aPair, const std::pair<PPointPtr, PPointPtr> &bPair) {
		int32_t maxValA = aPair.first->value() >= aPair.second->value() ? aPair.first->value() : aPair.second->value();
		int32_t minValA = aPair.first->value() < aPair.second->value() ? aPair.first->value() : aPair.second->value();

//		std::cout << "maxValA " << maxValA << " minValA " << minValA << std::endl;

		int32_t maxValB = bPair.first->value() >= bPair.second->value() ? bPair.first->value() : bPair.second->value();
		int32_t minValB = bPair.first->value() < bPair.second->value() ? bPair.first->value() : bPair.second->value();

		//	std::cout << "maxValB " << maxValB << " minValB " << minValB << std::endl;

		return (maxValA - minValA) > (maxValB - minValB);
	}

};

void PersistPairProcessor::filter(uint32_t persistence) {

	for (size_t i = 0; i < ppairs.size(); ++i) {
		PPointPtr pA = getPoint(ppairs[i].first);
		PPointPtr pB = getPoint(ppairs[i].second);
		if (pA != NULL && pB != NULL) {
			ppairsPoint.push_back(std::make_pair(pA, pB));

		}
	}

	std::sort(ppairsPoint.begin(), ppairsPoint.end(), PPairComparatorByPersistence());

	/*
	 for (size_t i = 0; i < ppairsPoint.size(); ++i) {
	 std::cout << "a: " << ppairsPoint[i].first->m_id << " b: " << ppairsPoint[i].second->m_id << std::endl;
	 }
	 */

	size_t i = 0;
	for (i = 0; i < ppairsPoint.size(); ++i) {
		int32_t maxValA =
				ppairsPoint[i].first->value() >= ppairsPoint[i].second->value() ? ppairsPoint[i].first->value() : ppairsPoint[i].second->value();
		int32_t minValA =
				ppairsPoint[i].first->value() < ppairsPoint[i].second->value() ? ppairsPoint[i].first->value() : ppairsPoint[i].second->value();
//		std::cout << "max :  " << maxValA << " min " << minValA << " pers " << persistence << std::endl;

		if (maxValA - minValA < persistence)
			break;
	}

	if (i < ppairsPoint.size() - 1)
		ppairsPoint.erase(ppairsPoint.begin() + i, ppairsPoint.end());

	/*
	 std::cout << "i:  " << i << std::endl;

	 for (i = 0; i < ppairsPoint.size(); ++i) {
	 std::cout << "a: " << ppairsPoint[i].first->m_id << " b: " << ppairsPoint[i].second->m_id << std::endl;
	 }
	 */

}

std::vector<uint32_t> PersistPairProcessor::getNeighbs(uint32_t point) {

	std::vector<uint32_t> ret;
	std::vector<uint32_t> *neighbs = m_relations.getNeighbs(point);
	if (neighbs == NULL)
		return ret;

	for (size_t i = 0; i < neighbs->size(); ++i) {
		if (filtered.find(neighbs->at(i)) != filtered.end()) {
			ret.push_back(neighbs->at(i));
		}
	}

	return ret;

}

void PersistPairProcessor::cycleSearch(uint32_t point) {

	PPointPtr curPp = getPoint(point);
	std::vector<uint32_t> neighbs = getNeighbs(point);

	std::vector<uint32_t> *curSet = new std::vector<uint32_t>();
	for (size_t i = 0; i < neighbs.size(); ++i) {
		PPointPtr b = getPoint(neighbs[i]);
		if (b->m_dim == curPp->m_dim - 1 && b->m_type != curPp->m_type)
			curSet->push_back(neighbs[i]);
	}

	uint32_t prev = 0;
	while (!curSet->empty()) {
		uint32_t highest = getHighest(*curSet);
		if (prev == highest)
			break;
		prev = highest;
		if (m_cycles.find(highest) == m_cycles.end()) {
			m_cycles[highest] = curSet;
			m_cycles[point] = curSet;
			ppairs.push_back(std::make_pair(highest, point));
			break;
		} else {
			addInCycle(curSet, m_cycles.find(highest)->second);
		}
	}

}

void PersistPairProcessor::addInCycle(std::vector<uint32_t>* a, std::vector<uint32_t>* b) {

	std::vector<uint32_t> res(*a);
	res.insert(res.end(), b->begin(), b->end());

	std::vector<uint32_t> common;
	for (size_t i = 0; i < a->size(); ++i) {
		for (size_t j = 0; j < b->size(); ++j) {
			if ((*a)[i] == (*b)[j])
				common.push_back((*a)[i]);
		}
	}
	std::vector<uint32_t> mv;

	for (size_t i = 0; i < res.size(); ++i) {
		bool hasCommon = false;
		for (size_t j = 0; j < common.size(); ++j) {
			if (res[i] == common[j]) {
				hasCommon = true;
				break;
			}
		}
		if (!hasCommon)
			mv.push_back(res[i]);
	}

	a->clear();
	*a = mv;
}

PPointPtr PersistPairProcessor::getPoint(uint32_t id) {
	PointsMap::iterator it = m_points.find(id);
	if (it == m_points.end())
		return NULL;
	return it->second;
}

void PersistPairProcessor::printToFile(const std::string& path) {
	std::ofstream coordFile;
	coordFile.open(path.c_str());
	coordFile << "x\ty\tz" << std::endl;

	if (!ppairsPoint.size())
		filter(0);

	for (size_t i = 0; i < ppairsPoint.size(); ++i) {
		coordFile << ppairsPoint[i].first->x << "\t" << ppairsPoint[i].first->y << "\t" << ppairsPoint[i].first->value() << std::endl;
		coordFile << ppairsPoint[i].second->x << "\t" << ppairsPoint[i].second->y << "\t" << ppairsPoint[i].second->value() << std::endl;

	}
	coordFile.close();

}

uint32_t PersistPairProcessor::getHighest(std::vector<uint32_t>& v) {
	MT_EXCEPTION_ASSERT(v.size(), "can't get hiegst becouse v.size = 0")

	uint32_t val = m_points[v[0]]->value(), index = v[0];

	for (size_t i = 1; i < v.size(); ++i) {
		if (m_points[v[i]]->value() > val) {
			val = m_points[v[i]]->value();
			index = v[i];
		}
	}

	return index;
}

class PPairComparator {
public:
	bool operator()(const std::pair<uint32_t, uint32_t> &a, const std::pair<uint32_t, uint32_t> &b) {
		uint32_t maxA = a.first > a.second ? a.first : a.second;
		uint32_t minA = a.first > a.second ? a.second : a.first;

		uint32_t maxB = b.first > b.second ? b.first : b.second;
		uint32_t minB = b.first > b.second ? b.second : b.first;

		return (maxA - minA) > (maxB - minB);
	}

};

void PersistPairProcessor::sort() {

	std::sort(ppairs.begin(), ppairs.end(), PPairComparator());
}

