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

class Edge: public Simplex {
public:
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

	virtual int32_t value() {
		if (!m_a)
			return 0;
		return m_a->value();
	}

	VertexPtr maxVertex(){
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
