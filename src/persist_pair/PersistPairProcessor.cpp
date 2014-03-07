/*
 * PersistPairProcessor.cpp
 *
 *  Created on: Sep 28, 2013
 *      Author: trifid
 */

#include "PersistPairProcessor.h"
#include "../MsComplex.h"

struct PPointComparator {
	bool operator()(const PPointPtr a, const PPointPtr b) const {
		return a->value() < b->value();
	}
} pPointComparator;

PersistPairProcessor::PersistPairProcessor() {
	// TODO Auto-generated constructor stub

}

PersistPairProcessor::~PersistPairProcessor() {
	// TODO Auto-generated destructor stub
}

bool PersistPairProcessor::init(MsComplexesSet& msCmplxSet) {

	for (MsComplexesSet::iterator it = msCmplxSet.begin(); it != msCmplxSet.end(); it++) {
		MsComplex *cmplx = *it;
		PPoint *ppointMax = new PPoint(cmplx->m_max->maxVertex());
		ppointMax->m_type = PPoint::NEGATIVE;
		ppointMax->m_dim = 2;
		m_points[ppointMax->m_id] = ppointMax;

		PPointPtr ppointMin = new PPoint(cmplx->m_min);
		ppointMin->m_type = PPoint::POSITIVE;
		ppointMin->m_dim = 0;

		m_points[ppointMin->m_id] = ppointMin;

		for (size_t i = 0; i < cmplx->m_seddles.size(); ++i) {
			PPointPtr ppointSeddle = new PPoint(cmplx->m_seddles[i]->maxVertex());
			ppointSeddle->m_type = PPoint::UNIVERSAL;
			ppointSeddle->m_dim = 1;

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

	while (!curSet->empty()) {
		uint32_t highest = getHighest(*curSet);
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

/*

 void PersistPairProcessor::findPairs() {

 std::set<uint32_t> filtered;
 std::set<uint32_t> paired;
 std::set<std::pair<uint32_t, uint32_t> > paires;

 for (size_t i = 0; i < m_points.size(); ++i) {
 PPointPtr ppoint = m_points[i];
 if (ppoint->m_type == PPoint::POSITIVE) {
 filtered.insert(ppoint->m_id);
 } else if (ppoint->m_type == PPoint::UNIVERSAL) {
 std::vector<uint32_t> *neighbs = m_relations.getNeighbs(ppoint->m_id);
 MT_EXCEPTION_ASSERT(neighbs!=NULL, "getNeighbs return NULL for id " << ppoint->m_id);
 uint32_t nearest = getNearest(neighbs);
 if(isInComponent(nearest)){
 PComponentPtr component = getComponent(nearest);
 }else{
 PComponentPtr component = new PComponent();
 component->addPoint((*neighbs)[i]);
 }


 } else {

 }

 }

 }
 */

/*
 bool PersistPairProcessor::isInComponent(uint32_t id) {
 return m_components.find(id) != m_components.end();
 }

 void PersistPairProcessor::addComponent(uint32_t id, PComponentPtr cmp) {
 m_components[id] = cmp;
 }

 PComponentPtr PersistPairProcessor::getComponent(uint32_t id) {
 std::map<uint32_t, PComponentPtr>::iterator it = m_components.find(id);
 if (it == m_components.end())
 return NULL;
 return it->second;

 }

 */
