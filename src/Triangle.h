/*
 * Triangle.h
 *
 *  Created on: Nov 27, 2012
 *      Author: trifid
 */

#ifndef TRIANGLE_H_
#define TRIANGLE_H_
#include "Edge.h"
#include <vector>
#include "opencv2/core/core.hpp"

class Face: public Simplex {
public:
	uint32_t m_faceId;
	Face() :
			Simplex() {
		m_dim = 2;
		m_faceId = 0;
	}
	virtual ~Face() {
	}

	bool operator<(const Face& tr) const;
	bool operator!=(const Face& tr) const;
	bool operator==(const Face& tr) const;

	void addEdge(EdgePtr edge);
	VertexesSet m_vertexes;
	EdgesSet m_edges;

	void draw(Mat& img, uint32_t green = 0);
	EdgePtr findCommonEdge(Face* quad);

	VertexPtr maxVertex();
	virtual int32_t value() {

		/*
		 if (!m_a)
		 return 0;
		 return m_a->value();
		 */
		return 0;
	}

	virtual Point centralPoint();
	VertexPtr getLeftTopConer();
	VertexPtr getRightBottomConer();

protected:

};

std::ostream & operator <<(std::ostream & out, Face& vt);

typedef Face* FacePtr;
typedef std::vector<FacePtr> Faces;
//typedef std::set<FacePtr> FacesSet;

#endif /* TRIANGLE_H_ */
