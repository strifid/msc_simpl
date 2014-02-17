/*
 * Utils.h
 *
 *  Created on: Feb 20, 2013
 *      Author: trifid
 */

#ifndef UTILS_H_
#define UTILS_H_
#include "opencv2/core/core.hpp"
#include "Quad.h"
#include <vector>
#include "MsComplex.h"

using cv::Point;

class MsComplexEditor {
public:

	void process(MsComplex *cmplx);
	bool save(const std::string& path);
	void saveEditMap(const std::string& path);

	Vertexes &getAscArcTrace(const std::vector<FacePtr>& ascArc, VertexPtr max);
	Vertexes& getDescArcTrace(const std::vector<EdgePtr>& ascArc);

protected:

	typedef std::vector<Point> Points;

	std::set<Arc<FacePtr, FacePtr>*> m_processedAscArcs;
	std::set<DescArc*> m_processedDescArcs;

	//member to avoid extra vector coping
	Vertexes m_pointsReturnVector;

	std::map<std::pair<int, int>, int> m_generateField;

	void addArcPoints(Vertexes& points, uint32_t max, uint32_t min);

};

#endif /* UTILS_H_ */
