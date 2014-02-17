/*

 * Quad.h
 *
 *  Created on: Jan 16, 2013
 *      Author: trifid


#ifndef QUAD_H_
#define QUAD_H_
#include "opencv2/core/core.hpp"
#include "Edge.h"
#include "Triangle.h"
#include <iosfwd>

using cv::Mat;

class Quad: public Face {
public:
	Quad(EdgePtr a, EdgePtr b, EdgePtr c, EdgePtr d);
	virtual ~Quad();

	bool operator<(const Quad& tr) const;
	bool operator!=(const Quad& tr) const;

	EdgePtr findCommonEdge(Quad* quad);
	EdgePtr m_d;

	Point centralPoint();
};

typedef Quad* QuadPtr;
typedef std::vector<QuadPtr> Quads;

std::ostream& operator<<(std::ostream& out, Quad &vt);

#endif  QUAD_H_
*/
