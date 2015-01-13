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
#include "../Utils.h"

PersistPairProcessor::PersistPairProcessor() {

}

PersistPairProcessor::~PersistPairProcessor() {
}

bool PersistPairProcessor::init(AscArcStorage& ascArcs, DescArcStorage& descArcs) {

	PPoint::pointId = 1;
	PPoint::m_vrtx2Id.clear();

	AscArcStorage::ArcsToSeddleMap &aMap = ascArcs.arcsToSeddleMap();

	for (AscArcStorage::ArcsToSeddleMap::iterator it = aMap.begin(); it != aMap.end(); it++) {
		for (size_t i = 0; i < it->second.size(); ++i) {
			uint32_t maxPointId = PPoint::createPPoint<FacePtr>(it->second.at(i)->m_arcEnd, m_points, PPoint::NEGATIVE, 2);
			uint32_t seddlePointId = PPoint::createPPoint<EdgePtr>(it->second.at(i)->m_arcBegin, m_points, PPoint::UNIVERSAL, 1);
			m_relations.addPair(maxPointId, seddlePointId);
		}
	}

	DescArcStorage::ArcsToSeddleMap &dMap = descArcs.arcsToSeddleMap();

	for (DescArcStorage::ArcsToSeddleMap::iterator it = dMap.begin(); it != dMap.end(); it++) {
		for (size_t i = 0; i < it->second.size(); ++i) {
			uint32_t minPointId = PPoint::createPPoint<VertexPtr>(it->second.at(i)->m_arcEnd, m_points, PPoint::POSITIVE, 0);
			uint32_t seddlePointId = PPoint::createPPoint<EdgePtr>(it->second.at(i)->m_arcBegin, m_points, PPoint::UNIVERSAL, 1);
			m_relations.addPair(minPointId, seddlePointId);
		}
	}

	std::cout << "point add " << PPoint::pointId << std::endl;

	return true;
}

bool PersistPairProcessor::init(MsComplexesSet& msCmplxSet) {

	PPoint::pointId = 1;
	PPoint::m_vrtx2Id.clear();

	for (MsComplexesSet::iterator it = msCmplxSet.begin(); it != msCmplxSet.end(); it++) {
		MsComplex *cmplx = *it;
		uint32_t maxPointId = PPoint::createPPoint<FacePtr>(cmplx->m_max, m_points, PPoint::NEGATIVE, 2);
		uint32_t minPointId = PPoint::createPPoint<VertexPtr>(cmplx->m_min, m_points, PPoint::POSITIVE, 0);

		for (size_t i = 0; i < cmplx->m_seddles.size(); ++i) {
			uint32_t seddlePointId = PPoint::createPPoint<EdgePtr>(cmplx->m_seddles[i], m_points, PPoint::UNIVERSAL, 1);
			m_relations.addPair(maxPointId, seddlePointId);
			m_relations.addPair(minPointId, seddlePointId);
		}
	}

	std::cout << " point add " << PPoint::pointId << std::endl;

	return true;
}

void PersistPairProcessor::findPairs() {
	for (PointsMap::iterator it = m_points.begin(); it != m_points.end(); ++it) {
		m_sortedPoints.push_back(it->second);
	}
	std::sort(m_sortedPoints.begin(), m_sortedPoints.end(), PPointComparator());

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
		int32_t maxValA =
				aPair.first->value().getInt() >= aPair.second->value().getInt() ? aPair.first->value().getInt() : aPair.second->value().getInt();
		int32_t minValA =
				aPair.first->value().getInt() < aPair.second->value().getInt() ? aPair.first->value().getInt() : aPair.second->value().getInt();

//		std::cout << "maxValA " << maxValA << " minValA " << minValA << std::endl;

		int32_t maxValB =
				bPair.first->value().getInt() >= bPair.second->value().getInt() ? bPair.first->value().getInt() : bPair.second->value().getInt();
		int32_t minValB =
				bPair.first->value().getInt() < bPair.second->value().getInt() ? bPair.first->value().getInt() : bPair.second->value().getInt();

		//	std::cout << "maxValB " << maxValB << " minValB " << minValB << std::endl;

		return (maxValA - minValA) < (maxValB - minValB);
	}

};

std::vector<std::pair<PPointPtr, PPointPtr> >& PersistPairProcessor::createPpairVector() {
	m_ppairsPoint.clear();
	for (size_t i = 0; i < ppairs.size(); ++i) {
		PPointPtr pA = getPoint(ppairs[i].first);
		PPointPtr pB = getPoint(ppairs[i].second);
		if (pA != NULL && pB != NULL) {
			m_ppairsPoint.push_back(std::make_pair(pA, pB));
		}
	}
	std::sort(m_ppairsPoint.begin(), m_ppairsPoint.end(), PPairComparatorByPersistence());

/*
	for (size_t i = 0; i < m_ppairsPoint.size(); ++i) {
		VertexPtr vtx;
		EdgePtr edge;
		FacePtr face;
		if (Utils::ppairToSimplex(m_ppairsPoint[i], &vtx, &edge, &face))
			std::cout << "pair: " << *face << " seddle " << *edge << " distance: "
					<< face->maxVertex()->value().getInt() - edge->maxVertex()->value().getInt() << std::endl;
		else
			std::cout << "pair: " << *vtx << " seddle " << *edge << " distance: " << edge->maxVertex()->value().getInt() - vtx->value().getInt()
					<< std::endl;

	}
*/

	return m_ppairsPoint;
}

std::vector<std::pair<PPointPtr, PPointPtr> >& PersistPairProcessor::filter(uint32_t persistence) {

	createPpairVector();
	/*
	 for (size_t i = 0; i < ppairsPoint.size(); ++i) {
	 std::cout << "a: " << ppairsPoint[i].first->m_id << " b: " << ppairsPoint[i].second->m_id << std::endl;
	 }
	 */
	size_t i = 0;
	for (i = 0; i < m_ppairsPoint.size(); ++i) {
		int32_t maxValA =
				m_ppairsPoint[i].first->value().getInt() >= m_ppairsPoint[i].second->value().getInt() ?
						m_ppairsPoint[i].first->value().getInt() : m_ppairsPoint[i].second->value().getInt();
		int32_t minValA =
				m_ppairsPoint[i].first->value().getInt() < m_ppairsPoint[i].second->value().getInt() ?
						m_ppairsPoint[i].first->value().getInt() : m_ppairsPoint[i].second->value().getInt();
//		std::cout << "max :  " << maxValA << " min " << minValA << " pers " << persistence << std::endl;

		if (maxValA - minValA > persistence)
			break;
	}

	if (i < m_ppairsPoint.size() - 1)
		m_ppairsPoint.erase(m_ppairsPoint.begin() + i, m_ppairsPoint.end());

	std::cout << "ppairs for remove:  " << m_ppairsPoint.size() << std::endl;
/*
	for (size_t i = 0; i < m_ppairsPoint.size(); ++i) {
		VertexPtr vtx;
		EdgePtr edge;
		FacePtr face;

		if (Utils::ppairToSimplex(m_ppairsPoint[i], &vtx, &edge, &face))
			std::cout << "pair: " << *face << " seddle " << *edge << " distance "
					<< face->maxVertex()->value().getInt() - edge->maxVertex()->value().getInt() << std::endl;
		else
			std::cout << "pair: " << *vtx << " seddle " << *edge << " distance " << edge->maxVertex()->value().getInt() - vtx->value().getInt()
					<< std::endl;

	}
*/

	/*
	 std::cout << "i:  " << i << std::endl;

	 for (i = 0; i < ppairsPoint.size(); ++i) {
	 std::cout << "a: " << ppairsPoint[i].first->m_id << " b: " << ppairsPoint[i].second->m_id << std::endl;
	 }
	 */
	return m_ppairsPoint;

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

	if (b->size() != 2)
		return;
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

	if (!m_ppairsPoint.size())
		filter(0);

	std::set<std::pair<uint32_t, uint32_t> > processed;

	for (size_t i = 0; i < m_ppairsPoint.size(); ++i) {
		if (processed.find(std::make_pair(m_ppairsPoint[i].first->x, m_ppairsPoint[i].first->y)) == processed.end()) {
			coordFile << m_ppairsPoint[i].first->x << "\t" << m_ppairsPoint[i].first->y << "\t" << m_ppairsPoint[i].first->value().getInt()
					<< std::endl;
			processed.insert(std::make_pair(m_ppairsPoint[i].first->x, m_ppairsPoint[i].first->y));
		}

		if (processed.find(std::make_pair(m_ppairsPoint[i].second->x, m_ppairsPoint[i].second->y)) == processed.end()) {
			coordFile << m_ppairsPoint[i].second->x << "\t" << m_ppairsPoint[i].second->y << "\t" << m_ppairsPoint[i].second->value().getInt()
					<< std::endl;
			processed.insert(std::make_pair(m_ppairsPoint[i].second->x, m_ppairsPoint[i].second->y));
		}

	}
	coordFile.close();

}

uint32_t PersistPairProcessor::getHighest(std::vector<uint32_t>& v) {

	BigDecimal *val = &(m_points[v[0]]->value());
	uint32_t index = v[0];

	for (size_t i = 1; i < v.size(); ++i) {
		if (m_points[v[i]]->value() > *val) {
			val = &(m_points[v[i]]->value());
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

