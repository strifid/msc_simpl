/*
 * PPointsRelations.h
 *
 *  Created on: Sep 28, 2013
 *      Author: trifid
 */

#ifndef PPOINTSRELATIONS_H_
#define PPOINTSRELATIONS_H_
#include <stdint.h>
#include <vector>
#include <map>

class PPointsRelations {
public:
	PPointsRelations();
	virtual ~PPointsRelations();

	void addPair(uint32_t a, uint32_t b);
	std::vector<uint32_t>* getNeighbs(uint32_t id);
protected:

	typedef std::map<uint32_t, std::vector<uint32_t>  > RelationsMap;
	RelationsMap  m_relations;

};

#endif /* PPOINTSRELATIONS_H_ */
