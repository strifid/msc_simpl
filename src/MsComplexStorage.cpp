/*
 * MsComplexStorage.cpp
 *
 *  Created on: Dec 17, 2014
 *      Author: trifid
 */

#include "MsComplexStorage.h"

void MsComplexStorage::printInfo(std::vector<FacePtr>& v) {

	std::cout << "m_complexesSet size " << m_complexesSet.size() << std::endl;
	std::cout << "m_complexByMax size " << m_complexByMax.size() << std::endl;
	std::cout << "m_complexBySaddle size " << m_complexBySaddle.size() << std::endl;

	std::set<AscArcPtr> aSet;
	std::set<DescArcPtr> dSet;
	SimplexStorage<FacePtr> max;
	for (MsComplexesSet::iterator it = m_complexesSet.begin(); it != m_complexesSet.end(); it++) {
		aSet.insert((*it)->m_aArcs[0]);
		aSet.insert((*it)->m_aArcs[1]);
		dSet.insert((*it)->m_dArcs[0]);
		dSet.insert((*it)->m_dArcs[1]);
		max.push((*it)->m_max);
	}

	/*
	 for (size_t i = 0; i < v.size(); ++i) {
	 if( !max.exist(v[i]))
	 std::cout << "can't find any complex for max " << *(v[i])<< std::endl;

	 }
	 */
	std::cout << "aSet " << aSet.size() << " dSet " << dSet.size() << std::endl;

}
