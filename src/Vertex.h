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
#include "Pixel.h"
#include <set>
#include "Subset.h"
#include <map>
#include "Simplex.h"

using cv::Mat;
using cv::Scalar;

class Vertex: public Subset, public Pixel, public Simplex {
public:
	Vertex(int x, int y);

	virtual ~Vertex();
	uint32_t locateSubset(Image& img);

	/*
	 std::set<Pixel, PixelComparator> &pixels() {
	 return m_pixels;
	 }
	 */

	bool operator==(const Vertex& vrtx) const;
	bool operator<(const Vertex& vrtx) const;

	int32_t value() {
		return m_value;
	}
	void value(int32_t i) {
		m_value = i;
		m_valueFirst = m_value;
	}

	void draw(Mat& img, Scalar color = Scalar(0, 0, 0));

	Vertex* maxVertex() {
		return this;
	}

protected:
//	std::set<Pixel, PixelComparator> m_pixels;
	Pixels getAllAround(const Pixel& pt, uint32_t width, uint32_t height);
	int32_t m_value;

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
