/*
 * Face.h
 *
 *  Created on: Nov 24, 2012
 *      Author: trifid
 */

#ifndef FACE_H_
#define FACE_H_
#include <vector>
#include <set>
#include "Vertex.h"
#include "Simplex.h"
#include "opencv2/core/core.hpp"
#include <iostream>
#include <opencv/cv.h>
using namespace cv;

class Edge: public Simplex {
public:
	uint32_t m_faceId;
	Edge();
	Edge(VertexPtr a, VertexPtr b);
	virtual ~Edge();
	bool operator<(const Edge& face) const;
	bool operator==(const Edge& face) const;
	bool operator!=(const Edge& face) const;

	Point centralPoint();
	void draw(Mat& img, int32_t thickness = 1, Scalar color = Scalar(0, 0, 0)) const;
	VertexPtr m_a;
	VertexPtr m_b;

	VertexPtr getDifferentVertex(Edge* edge);
	VertexPtr findCommonVertex(Edge* edge);

	virtual BigDecimal& value() {
		if (!m_a) {
			std::cout << "ERROR!! value in Edge return 0" << std::endl;
		}
		return m_a->value();
	}

	VertexPtr maxVertex() const {
		return m_a->value() > m_b->value() ? m_a : m_b;
	}
protected:
};

typedef Edge* EdgePtr;

struct EdgesComparator {
	bool operator()(const EdgePtr a, const EdgePtr b) const {
		return *a < *b;
	}
};

std::ostream& operator<<(std::ostream& out, Edge &vt);

typedef std::vector<EdgePtr> Edges;
typedef std::set<EdgePtr, EdgesComparator> EdgesSet;

#endif /* FACE_H_ */
