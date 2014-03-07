/*
 * PPointsRelations.cpp
 *
 *  Created on: Sep 28, 2013
 *      Author: trifid
 */

#include "PPointsRelations.h"
#include <stddef.h>

PPointsRelations::PPointsRelations() {
	// TODO Auto-generated constructor stub

}

PPointsRelations::~PPointsRelations() {
	// TODO Auto-generated destructor stub
}

void PPointsRelations::addPair(uint32_t seddle, uint32_t vertex) {

	RelationsMap::iterator itForA = m_relations.find(seddle);
	if (itForA == m_relations.end()) {
		m_relations[seddle] = std::vector<uint32_t>();
		m_relations[seddle].push_back(vertex);
	} else
		itForA->second.push_back(vertex);

	RelationsMap::iterator itForB = m_relations.find(vertex);
	if (itForB == m_relations.end()) {
		m_relations[vertex] = std::vector<uint32_t>();
		m_relations[vertex].push_back(seddle);
	} else
		itForB->second.push_back(seddle);

}

std::vector<uint32_t>* PPointsRelations::getNeighbs(uint32_t id) {

	RelationsMap::iterator it = m_relations.find(id);
	if (it == m_relations.end())
		return NULL;


	return &(it->second);

}

