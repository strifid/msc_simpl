/*
 * PPoint.h
 *
 *  Created on: Sep 28, 2013
 *      Author: trifid
 */

#ifndef PPOINT_H_
#define PPOINT_H_
#include "Vertex.h"
#include "Triangle.h"
#include "Edge.h"

struct SimplexPtrComparator {
	bool operator()(Simplex* a, Simplex* b) const {
		if (a->m_dim == b->m_dim){
			if(a->m_dim == 0)
				return *(static_cast<const VertexPtr>(a)) < *(static_cast<const VertexPtr>(b));
			if(a->m_dim == 1)
				return *(static_cast<EdgePtr>(a)) < *(static_cast<EdgePtr>(b));
			if(a->m_dim == 2)
				return *(static_cast<FacePtr>(a)) < *(static_cast<FacePtr>(b));

		}
		return 	a->m_dim < b->m_dim;
	}
};
typedef std::map<Simplex*, uint32_t, SimplexPtrComparator> SmplxToPPidMap;

class PPoint: public Vertex {
public:
	enum Type {
		POSITIVE, UNIVERSAL, NEGATIVE
	} m_type;

	PPoint(Vertex &vtx, Type t, uint32_t dim);
	PPoint(int, Type t, uint32_t dim);

	virtual ~PPoint();

	uint32_t m_id;
	uint32_t m_dim;
	Simplex* m_smplx;
	static uint32_t pointId;

	static uint32_t getId(Simplex* vtx, uint32_t dim);

	template<typename T>
	static uint32_t createPPoint(T smplx, std::map<uint32_t, PPoint*> &pointsMap, PPoint::Type type, uint32_t dim) {

		uint32_t id = getId(smplx->maxVertex(), dim);
		if (id)
			return id;

		PPoint *ppointMin = new PPoint(*(smplx->maxVertex()), type, dim);
		ppointMin->m_smplx = smplx;
		pointsMap[ppointMin->m_id] = ppointMin;
		m_vrtx2Id[smplx] = ppointMin->m_id;
		return ppointMin->m_id;

	}
	static SmplxToPPidMap m_vrtx2Id;

private:

};

typedef PPoint* PPointPtr;

#endif /* PPOINT_H_ */
