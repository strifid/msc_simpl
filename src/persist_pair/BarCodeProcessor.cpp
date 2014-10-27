/*
 * BarCodeProcessor.cpp
 *
 *  Created on: Jul 21, 2014
 *      Author: trifid
 */

#include "BarCodeProcessor.h"

#include <stddef.h>
#include <algorithm>
#include <stdint.h>
#include <iostream>
#include <map>
#include <set>
#include <utility>
#include <vector>
#include <fstream>
#include "PPoint.h"

BarCodeProcessor::BarCodeProcessor() {
	// TODO Auto-generated constructor stub

}

BarCodeProcessor::~BarCodeProcessor() {
	// TODO Auto-generated destructor stub
}

void BarCodeProcessor::writeInFile(const std::string& path) {
	std::ofstream b0File;
	b0File.open((path + "_b0.txt").c_str());
	std::ofstream b1File;
	b1File.open((path + "_b1.txt").c_str());
	for (std::map<uint32_t, BarCode*>::iterator it = m_barCodes.begin(); it != m_barCodes.end(); it++) {
		if (it->second->m_stop) {
			if (it->second->m_dim == 0) {
				b0File << it->second->m_start << " " << it->second->m_stop << std::endl;
			} else {
				b1File << it->second->m_start << " " << it->second->m_stop << std::endl;
			}
		}
	}
	b0File.close();
	b1File.close();
}

void BarCodeProcessor::computeBarCodes(const std::string& path) {
	for (PointsMap::iterator it = m_points.begin(); it != m_points.end(); ++it) {
		m_sortedPoints.push_back(it->second);
	}
	std::sort(m_sortedPoints.begin(), m_sortedPoints.end(), PPointComparator());
	uint32_t lastValue = 0, step = 0;
	std::ofstream levelFile;
	levelFile.open((path + "_level.txt").c_str());
	levelFile << "level value\tb0\tb1"<< std::endl;

	bool levelChanged = false;
	for (size_t i = 0; i < m_sortedPoints.size(); ++i) {
		if (lastValue != m_sortedPoints[i]->value().getInt()) {
			lastValue = m_sortedPoints[i]->value().getInt();
			step++;
			levelChanged = true;
		}

		uint32_t point = m_sortedPoints[i]->m_id;
		filtered.insert(point);
		if (getPoint(point) && getPoint(point)->m_type != PPoint::POSITIVE) {
			barCode(point, step);
		} else {
			BarCodePtr barCode = new BarCode(getPoint(point), step);
			m_barCodes[point] = barCode;
		}

		if (levelChanged) {
			std::map<uint32_t, BarCode*>::iterator it = m_barCodes.begin();
			uint32_t levelB0 = 0, levelB1 = 0;
			while (it != m_barCodes.end()) {

				if (it->second->m_stop == 0) {
					if (it->second->m_dim == 0)
						levelB0++;
					else
						levelB1++;
				}

				it++;
			}

			levelFile << lastValue << " " << levelB0 << " " << levelB1 << std::endl;
			levelChanged = false;
		}

	}

	writeInFile(path);
}

void BarCodeProcessor::barCode(uint32_t point, uint32_t step) {

	PPointPtr curPp = getPoint(point);
	std::vector<uint32_t> neighbs = getNeighbs(point);

	if (curPp->m_type == PPoint::NEGATIVE) {
		std::vector<PPointPtr> pps = getPpoints(neighbs);
		for (size_t i = 0; i < pps.size(); i++) {
			if (m_barCodes.find(pps[i]->m_id) != m_barCodes.end())
				stopBarCode(pps[i]->m_id, step);
		}
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
			BarCodePtr barCode = new BarCode(getPoint(point), step);
			m_barCodes[point] = barCode;
			break;
		}

		prev = highest;
		if (m_cycles.find(highest) == m_cycles.end()) {
			m_cycles[highest] = curSet;
			m_cycles[point] = curSet;
			stopBarCode(highest, step);
			break;
		} else {
			addInCycle(curSet, m_cycles.find(highest)->second);
			if (curSet->empty()) {

				for (size_t i = 0; i < m_cycles.find(highest)->second->size(); i++) {
					PPointPtr pp = getPoint(m_cycles.find(highest)->second->at(i));
					if (pp->m_type == PPoint::POSITIVE && pp->m_dim == 0 && m_cycles.find(pp->m_id) == m_cycles.end()) {
						stopBarCode(pp->m_id, step);
					}

				}

				m_barCodes[point] = new BarCode(getPoint(point), step);

			}
		}
	}

}

std::ostream& operator <<(std::ostream& out, BarCode& bc) {
	out << "b" << bc.m_dim << " length: " << bc.m_stop - bc.m_start;
	return out;
}

void BarCodeProcessor::stopBarCode(uint32_t id, uint32_t step) {
	BarCodePtr bc = findBarCode(id);
	if (bc && bc->m_stop == 0)
		bc->m_stop = step;
}

std::vector<PPointPtr> BarCodeProcessor::getPpoints(std::vector<uint32_t>& ppIds) {
	std::vector<PPointPtr> pps;
	for (size_t i = 0; i < ppIds.size(); ++i) {
		pps.push_back(getPoint(ppIds[i]));
	}

	std::sort(pps.begin(), pps.end(), PPointComparator());
	return pps;
	/*
	 std::cout << "sorted ppoint: " << std::endl;
	 for (size_t i = 0; i < pps.size(); ++i) {
	 std::cout << "point val: " << pps[i]->value() << " ";
	 }
	 std::cout << std::endl;
	 */

}
