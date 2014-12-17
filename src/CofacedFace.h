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

//template <typename FacePtr>
class CofacedFace: public Coface {
public:
	CofacedFace(EdgePtr face, FacePtr tr) {
		m_edge = face;
		m_face = tr;
	}
	virtual ~CofacedFace() {
	}
	virtual void draw(Mat& img);
	EdgePtr m_edge;
	FacePtr m_face;
};

inline void CofacedFace::draw(Mat& img) {

	int thickness = 3;
	int lineType = 8;
	Point a = m_edge->centralPoint();
	Point b = m_face->centralPoint();

	if(m_edge->m_a->x == 0 && m_edge->m_b->x == 0 && b.x > Image::m_enlargeFactor*2 ){
		a.x += Image::m_enlargeFactor*(Image::m_width) ;
	}

	if(m_edge->m_a->y == 0 && m_edge->m_b->y == 0 && b.y > Image::m_enlargeFactor*2 ){
		a.y += Image::m_enlargeFactor*(Image::m_height) ;
	}

	drawArrow(img, a, b);

}

#endif /* COFACEDTRIANGLE_H_ */
