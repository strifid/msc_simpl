/*
 * CofacedFace.h
 *
 *  Created on: Dec 4, 2012
 *      Author: trifid
 */

#ifndef COFACEDEDGE_H_
#define COFACEDEDGE_H_

#include "Coface.h"
#include "opencv2/core/core.hpp"
#include "Vertex.h"
#include "Edge.h"

class CofacedEdge: public Coface {
public:
	CofacedEdge(VertexPtr vtx, EdgePtr face);
	virtual ~CofacedEdge();
	virtual void draw(Mat& img);
	VertexPtr m_vertex;
	EdgePtr m_edge;
};

typedef CofacedEdge* CofacedEdgePtr;

#endif /* COFACEDFACE_H_ */
