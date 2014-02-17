/*
 * CofacedTriangle.h
 *
 *  Created on: Dec 4, 2012
 *      Author: trifid
 */

#ifndef COFACEDTRIANGLE_H_
#define COFACEDTRIANGLE_H_

#include "Coface.h"
#include "Edge.h"
#include "Triangle.h"
#include "opencv2/core/core.hpp"
#include "Image.h"

template <typename FacePtr>
class CofacedFace: public Coface {
public:
	CofacedFace(EdgePtr face, FacePtr tr){
		m_edge = face;
		m_face = tr;
	}
	virtual ~CofacedFace(){}
	virtual void draw(Mat& img){

		int thickness = 3;
		int lineType = 8;
		Point a = m_edge->centralPoint();
		Point b = m_face->centralPoint();

		drawArrow(img, a, b);

	}
	EdgePtr m_edge;
	FacePtr m_face;
};

#endif /* COFACEDTRIANGLE_H_ */
