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

struct VertexPtrDimComparator {
	bool operator()(const std::pair<VertexPtr, uint32_t> &a, const std::pair<VertexPtr, uint32_t> &b) const {
		if (*(a.first) == *(b.first))
			return a.second < b.second;
		return *(a.first) < *(b.first);
	}
};
typedef std::map<std::pair<VertexPtr, uint32_t>, uint32_t, VertexPtrDimComparator> VtxDimToPPidMap;

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

	static uint32_t getId(VertexPtr vtx, uint32_t dim);

	template<typename T>
	static uint32_t createPPoint(T smplx, std::map<uint32_t, PPoint*> &pointsMap, PPoint::Type type, uint32_t dim) {

		uint32_t id = getId(smplx->maxVertex(), dim);
		if (id)
			return id;

		PPoint *ppointMin = new PPoint(*(smplx->maxVertex()), type, dim);
		ppointMin->m_smplx = smplx;
		pointsMap[ppointMin->m_id] = ppointMin;
		m_vrtx2Id[std::make_pair(smplx->maxVertex(), dim)] = ppointMin->m_id;
		return ppointMin->m_id;

	}
	static VtxDimToPPidMap m_vrtx2Id;

private:

};

typedef PPoint* PPointPtr;

#endif /* PPOINT_H_ */
