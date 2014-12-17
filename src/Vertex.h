/*
 * Vertex.h
 *
 *  Created on: Nov 15, 2012
 *      Author: trifid
 */

#ifndef VERTEX_H_
#define VERTEX_H_

#include <vector>
#include "opencv2/core/core.hpp"
#include <set>
#include <map>
#include "Simplex.h"

using cv::Mat;
using cv::Scalar;

class Vertex: public Simplex {
public:
	Vertex(int x, int y);
	Vertex(int x, int y, int value);

	virtual ~Vertex();
	bool operator==(const Vertex& vrtx) const;
	bool operator!=(const Vertex& vrtx) const;

	bool operator<(const Vertex& vrtx) const;
	void draw(Mat& img, Scalar color = Scalar(0, 0, 0));

	Vertex* maxVertex() {
		return this;
	}
	int x;
	int y;

};

typedef Vertex* VertexPtr;

struct VertexPtrComparator {
	bool operator()(const VertexPtr a, const VertexPtr b) const {
		return *a < *b;
	}
};

std::ostream& operator<<(std::ostream& out, const Vertex &vt);

typedef std::vector<VertexPtr> Vertexes;
typedef std::set<VertexPtr, VertexPtrComparator> VertexesSet;

#endif /* VERTEX_H_ */
