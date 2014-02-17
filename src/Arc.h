/*
 * Arc.h
 *
 *  Created on: Jan 1, 2013
 *      Author: trifid
 */

#ifndef ARC_H_
#define ARC_H_

#include "Triangle.h"
#include "Vertex.h"
#include "Edge.h"
#include "Quad.h"

template<typename SimplesAPtr, typename SimplesBPtr>
class Arc {

public:

	Arc() {
	}
	Arc(EdgePtr saddle, std::vector<SimplesAPtr> &arcs, SimplesBPtr end) {

		std::cout << "create arc " << *end << " begin " << *saddle << std::endl;

		m_arcBegin = saddle;
		m_arc = arcs;
		m_arcEnd = end;
	}

	EdgePtr m_arcBegin;
	std::vector<SimplesAPtr> m_arc;
	SimplesBPtr m_arcEnd;

	bool operator==(Arc<SimplesAPtr, SimplesBPtr> arc) {
		if (*m_arcBegin == *(arc.m_arcBegin))
			return *m_arcEnd == *(arc.m_arcEnd);
		return false;
	}

	void addFirstLeg(Arc<SimplesAPtr, SimplesBPtr>* arc) {
		m_arc.insert(m_arc.end(), arc->m_arc.rbegin(), arc->m_arc.rend());
	}

	void addSecondLeg(Arc<SimplesAPtr, SimplesBPtr>* arc) {

		m_arc.insert(m_arc.end(), arc->m_arc.begin(), arc->m_arc.end());
		m_arcEnd = arc->m_arcEnd;
	}

	bool hasCommon(std::vector<SimplesAPtr> arc) {
		uint32_t common = 0;
		for (size_t i = 0; i < arc.size(); ++i) {
			for (size_t z = 0; z < m_arc.size(); ++z) {
				if (*(arc[i]) == *(m_arc[z]))
					common++;
			}
		}

		if(common > 3)
			return true;
		return false;
	}

protected:

};

typedef Arc<EdgePtr, VertexPtr> DescArc;
typedef DescArc* DescArcPtr;
typedef std::vector<DescArcPtr> DescArcs;

#ifdef QUAD
typedef Arc<FacePtr, FacePtr> AscArc;
typedef AscArc* AscArcPtr;
typedef std::vector<AscArcPtr> AscArcs;


#endif

#endif /* ARC_H_ */
