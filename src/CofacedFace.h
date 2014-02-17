/*
 * CofacedFace.h
 *
 *  Created on: Dec 4, 2012
 *      Author: trifid
 */

#ifndef COFACEDFACE_H_
#define COFACEDFACE_H_

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
	EdgePtr m_face;
};

typedef CofacedEdge* CofacedFacePtr;

#endif /* COFACEDFACE_H_ */
