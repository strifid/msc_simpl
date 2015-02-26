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

template<typename SimplesAPtr, typename SimplesBPtr>
class Arc {

public:
	bool m_isAsc;
	Arc() :
			m_isAsc(false), m_arcBegin(NULL), m_arcEnd(NULL) {
	}
	Arc(EdgePtr saddle, std::vector<SimplesAPtr> &arcs, SimplesBPtr end) {
		m_arcBegin = saddle;
		m_arc = arcs;
		m_arcEnd = end;
		m_isAsc = false;
	}

	EdgePtr m_arcBegin;
	std::vector<SimplesAPtr> m_arc;
	SimplesBPtr m_arcEnd;

	bool operator==(Arc<SimplesAPtr, SimplesBPtr> arc) {
		if (*m_arcBegin == *(arc.m_arcBegin) && *m_arcEnd == *(arc.m_arcEnd)) {
			if (m_arc.size() == arc.m_arc.size()) {
				for (int i = 0; i < m_arc.size(); ++i) {
					if (*(m_arc[i]) != *(arc.m_arc[i]))
						return false;
				}
				return true;
			}
		}
		return false;
	}

	void addFirstLeg(Arc<SimplesAPtr, SimplesBPtr>* arc) {
		if (m_isAsc)
			m_arc.insert(m_arc.end(), arc->m_arc.rbegin() + 1, arc->m_arc.rend());
		else
			m_arc.insert(m_arc.end(), arc->m_arc.rbegin(), arc->m_arc.rend());

	}

	void addSecondLeg(Arc<SimplesAPtr, SimplesBPtr>* arc) {
		if (m_isAsc)
			m_arc.insert(m_arc.end(), arc->m_arc.begin(), arc->m_arc.end());
		else
			m_arc.insert(m_arc.end(), arc->m_arc.begin() + 1, arc->m_arc.end());
		m_arcEnd = arc->m_arcEnd;
	}

protected:

};

typedef Arc<EdgePtr, VertexPtr> DescArc;
typedef DescArc* DescArcPtr;
typedef std::vector<DescArcPtr> DescArcs;

typedef Arc<FacePtr, FacePtr> AscArc;
typedef AscArc* AscArcPtr;
typedef std::vector<AscArcPtr> AscArcs;

#endif /* ARC_H_ */
