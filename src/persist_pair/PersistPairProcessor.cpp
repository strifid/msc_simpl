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
			m_relations.addPair(seddlePointId, maxPointId);
//			std::cout << "add ppoint " << seddlePointId << " " << maxPointId << std::endl;
		}
	}

	DescArcStorage::ArcsToSeddleMap &dMap = descArcs.arcsToSeddleMap();
	int n = 0;
	for (DescArcStorage::ArcsToSeddleMap::iterator it = dMap.begin(); it != dMap.end(); it++) {
		for (size_t i = 0; i < it->second.size(); ++i) {
			uint32_t minPointId = PPoint::createPPoint<VertexPtr>(it->second.at(i)->m_arcEnd, m_points, PPoint::POSITIVE, 0);
			uint32_t seddlePointId = PPoint::createPPoint<EdgePtr>(it->second.at(i)->m_arcBegin, m_points, PPoint::UNIVERSAL, 1);
			m_relations.addPair(seddlePointId, minPointId);

		}
		n++;
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

std::ostream & operator <<(std::ostream & out, const PPointPtr p) {

	if (p->m_dim == 0)
		out << "ppid " << p->m_id << " vtx " << *(static_cast<VertexPtr>(p->m_smplx));
	if (p->m_dim == 1)
		out << "ppid " << p->m_id << " " << *(static_cast<EdgePtr>(p->m_smplx));
	if (p->m_dim == 2)
		out << "ppid " << p->m_id << " " << *(static_cast<FacePtr>(p->m_smplx));

	return out;

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

/*

	for (PointsMap::iterator it = m_points.begin(); it != m_points.end(); it++) {
		bool inMap = false;
		for (size_t i = 0; i < ppairs.size(); i++) {
			if (it->first == ppairs[i].first || it->first == ppairs[i].second) {
				inMap = true;
				break;
			}
		}

		if (inMap == false) {
			PPointPtr p = getPoint(it->first);
			std::cout << "can't find pair for " << p << std::endl;
		}
	}
*/
}

struct PPairComparatorByPersistence {
public:
	bool operator()(const std::pair<PPointPtr, PPointPtr> &aPair, const std::pair<PPointPtr, PPointPtr> &bPair) {
		int32_t maxValA =
				aPair.first->value().getInt() >= aPair.second->value().getInt() ? aPair.first->value().getInt() : aPair.second->value().getInt();
		int32_t minValA =
				aPair.first->value().getInt() < aPair.second->value().getInt() ? aPair.first->value().getInt() : aPair.second->value().getInt();

		int32_t maxValB =
				bPair.first->value().getInt() >= bPair.second->value().getInt() ? bPair.first->value().getInt() : bPair.second->value().getInt();
		int32_t minValB =
				bPair.first->value().getInt() < bPair.second->value().getInt() ? bPair.first->value().getInt() : bPair.second->value().getInt();

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
	return m_ppairsPoint;
}

std::vector<std::pair<PPointPtr, PPointPtr> >& PersistPairProcessor::filter(uint32_t persistence) {

	createPpairVector();
	size_t i = 0;
	for (i = 0; i < m_ppairsPoint.size(); ++i) {
		int32_t maxValA =
				m_ppairsPoint[i].first->value().getInt() >= m_ppairsPoint[i].second->value().getInt() ?
						m_ppairsPoint[i].first->value().getInt() : m_ppairsPoint[i].second->value().getInt();
		int32_t minValA =
				m_ppairsPoint[i].first->value().getInt() < m_ppairsPoint[i].second->value().getInt() ?
						m_ppairsPoint[i].first->value().getInt() : m_ppairsPoint[i].second->value().getInt();

		if (maxValA - minValA > persistence)
			break;
	}

	if (i < m_ppairsPoint.size() - 1)
		m_ppairsPoint.erase(m_ppairsPoint.begin() + i, m_ppairsPoint.end());

	std::cout << "ppairs for remove:  " << m_ppairsPoint.size() << std::endl;

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
	bool out = false;
/*
	if (point == 1)
		out = true;
*/
	std::vector<uint32_t> neighbs = getNeighbs(point);

	if (out) {
		std::cout << "neigs ";
		for (size_t i = 0; i < neighbs.size(); i++) {
			std::cout << neighbs[i] << " ";
		}
		std::cout << std::endl;
	}
	std::vector<uint32_t> *curSet = new std::vector<uint32_t>();
	for (size_t i = 0; i < neighbs.size(); ++i) {
		PPointPtr b = getPoint(neighbs[i]);
		if (b->m_dim == curPp->m_dim - 1 && b->m_type != curPp->m_type)
			curSet->push_back(neighbs[i]);
	}

	if (out) {
		std::cout << "cur set ";
		for (int i = 0; i < curSet->size(); ++i) {
			std::cout << curSet->at(i) << " ";
		}
		std::cout << std::endl;
	}

	uint32_t prev = 0;
	while (!curSet->empty()) {

		uint32_t highest = getHighest(*curSet);
		if (out)
			std::cout << "highest " << highest << std::endl;

		if (prev == highest)
			break;

		prev = highest;
		if (m_cycles.find(highest) == m_cycles.end()) {
			m_cycles[highest] = curSet;
			ppairs.push_back(std::make_pair(highest, point));
			if (curSet->size() == 2 && curSet->at(0) == curSet->at(1))
				break;
			else
				m_cycles[point] = curSet;

			break;
		} else {
			if (out) {
				std::cout << "add to cur set ";

				for (int i = 0; i < m_cycles.find(highest)->second->size(); ++i) {
					std::cout << m_cycles.find(highest)->second->at(i) << " ";
				}
				std::cout << std::endl;
			}
			addInCycle(curSet, m_cycles.find(highest)->second);
			if (out) {
				std::cout << "new cur set ";
				for (int i = 0; i < curSet->size(); ++i) {
					std::cout << curSet->at(i) << " ";
				}
				std::cout << std::endl;
			}

		}
//		break;
	}

}

void f(std::vector<uint32_t>* a) {
	std::cout << "a: ";
	for (size_t i = 0; i < a->size(); i++) {
		std::cout << a->at(i) << " ";
	}
	std::cout << std::endl;

}
void PersistPairProcessor::addInCycle(std::vector<uint32_t>* a, std::vector<uint32_t>* b) {

	std::vector<uint32_t> res;
	a->insert(a->end(), b->begin(), b->end());
	for (size_t i = 0; i < a->size(); ++i) {
		if (std::count(a->begin(), a->end(), a->at(i)) == 1)
			res.push_back(a->at(i));
	}

	*a = res;
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

