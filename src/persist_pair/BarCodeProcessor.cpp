/*
 * BarCodeProcessor.cpp
 *
 *  Created on: Jul 21, 2014
 *      Author: trifid
 */

#include "BarCodeProcessor.h"

BarCodeProcessor::BarCodeProcessor() {
	// TODO Auto-generated constructor stub

}

BarCodeProcessor::~BarCodeProcessor() {
	// TODO Auto-generated destructor stub
}

void BarCodeProcessor::computeBarCodes() {
	for (PointsMap::iterator it = m_points.begin(); it != m_points.end(); ++it) {
		m_sortedPoints.push_back(it->second);
	}
	std::sort(m_sortedPoints.begin(), m_sortedPoints.end(), PPointComparator());

	for (size_t i = 0; i < m_sortedPoints.size(); ++i) {
		uint32_t point = m_sortedPoints[i]->m_id;
		filtered.insert(point);
		if (getPoint(point) && getPoint(point)->m_type != PPoint::POSITIVE) {
			barCode(point, i);
		} else {
			BarCodePtr barCode = new BarCode(getPoint(point), i);
			m_barCodes[point] = barCode;
		}
	}

	for (std::map<uint32_t, BarCode*>::iterator it = m_barCodes.begin(); it != m_barCodes.end(); it++) {
		std::cout << *(it->second) << std::endl;
	}
}

void BarCodeProcessor::barCode(uint32_t point, uint32_t step) {

	PPointPtr curPp = getPoint(point);
	std::vector<uint32_t> neighbs = getNeighbs(point);

	if (curPp->m_type == PPoint::NEGATIVE) {

		std::vector<PPointPtr> pps = getPpoints(neighbs);
		return;
	}
	std::vector<uint32_t> *curSet = new std::vector<uint32_t>();
	for (size_t i = 0; i < neighbs.size(); ++i) {
		PPointPtr b = getPoint(neighbs[i]);
		if (b->m_dim == curPp->m_dim - 1 && b->m_type != curPp->m_type)
			curSet->push_back(neighbs[i]);
	}

	uint32_t prev = 0;
	while (!curSet->empty()) {

		uint32_t highest = getHighest(*curSet);

		if (prev == highest) {
			std::cout << "1. create cycle by point " << point << std::endl;
			BarCodePtr barCode = new BarCode(getPoint(point), step);
			m_barCodes[point] = barCode;
			break;
		}

		prev = highest;
		if (m_cycles.find(highest) == m_cycles.end()) {
			m_cycles[highest] = curSet;
			m_cycles[point] = curSet;
			BarCodePtr bc = findBarCode(highest);
			if (bc)
				bc->m_stop = step;

			break;
		} else {
			addInCycle(curSet, m_cycles.find(highest)->second);
			if (curSet->empty()) {
				std::cout << "2. create cycle by point " << point << std::endl;
				BarCodePtr barCode = new BarCode(getPoint(point), step);
				m_barCodes[point] = barCode;

			}
		}
	}

}

std::ostream& operator <<(std::ostream& out, BarCode& bc) {
	out << "bar code for point: " << bc.m_id << " length: " << bc.m_stop - bc.m_start << " (start: " << bc.m_start << ", stop: " << bc.m_stop << ")";

}

std::vector<PPointPtr> BarCodeProcessor::getPpoints(std::vector<uint32_t>& ppIds) {
	std::vector<PPointPtr> pps;
	for (size_t i = 0; i < ppIds.size(); ++i) {
		PPointPtr p = getPoint(ppIds[i]);
		if (p == NULL) {
			std::cout << "impossible error " << std::endl;
			return pps;
		}
		std::cout << "ppp " << std::endl;

		pps.push_back(p);
	}

	std::sort(pps.begin(), pps.end(), PPointComparator());
	std::cout << "sorted ppoint: " << std::endl;
	for (size_t i = 0; i < pps.size(); ++i) {
		std::cout << "point val: " << pps[i]->value() << " ";
	}
	std::cout << std::endl;
}
