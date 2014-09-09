/*
 * PPoint.h
 *
 *  Created on: Sep 28, 2013
 *      Author: trifid
 */

#ifndef PPOINT_H_
#define PPOINT_H_
#include "Pixel.h"
#include "Vertex.h"

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
	static std::map<VertexPtr, uint32_t, VertexPtrComparator> m_vrtx2Id;

	static uint32_t getId(VertexPtr vtx);
private:
};

typedef PPoint* PPointPtr;

#endif /* PPOINT_H_ */
