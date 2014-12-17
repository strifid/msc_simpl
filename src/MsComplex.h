/*
 * MsComlex.h
 *
 *  Created on: Jan 1, 2013
 *      Author: trifid
 */

#ifndef MSCOMLEX_H_
#define MSCOMLEX_H_
#include "Triangle.h"
#include "Edge.h"
#include "Vertex.h"
#include "Arc.h"
#include "Image.h"
#include "opencv2/core/core.hpp"

#include "Drawer.h"
#include "SimplexStorage.h"

using cv::circle;

class MsComplex: public Drawer {
public:

	MsComplex() {
	}
	virtual ~MsComplex() {
	}

	virtual void draw(Mat& img);
	FacePtr m_max;
	Edges m_seddles;
	VertexPtr m_min;

	DescArcs m_dArcs;
	std::vector<Arc<FacePtr, FacePtr> *> m_aArcs;

	bool operator ==(MsComplex &cmplx) {
		if (*(m_max) == *(cmplx.m_max) && *(m_min) == *(cmplx.m_min))
			return true;
		return false;
	}

	bool operator <(MsComplex &cmplx) {
		if (*(m_max) == *(cmplx.m_max))
			return *(m_min) < *(cmplx.m_min);
		return *(m_max) < *(cmplx.m_max);
	}

	void drawOriginal(Image& img);

	std::string toString() {

		std::stringstream out;
		out << "MAX: " << *(m_max->maxVertex()) << " MIN " << *m_min;

		for (size_t i = 0; i < m_seddles.size(); i++) {
			out << " SADDLE_" << i << ": " << *m_seddles[i];
		}

		for (size_t i = 0; i < m_dArcs.size(); i++) {
			out << " DARC_" << i << ": begin: " << *(m_dArcs[i]->m_arcBegin) << " end " << *(m_dArcs[i]->m_arcEnd);
		}

		return out.str();
	}

};

typedef MsComplex* MsComplexPtr;
typedef std::set<MsComplex*, SimplexComparator<MsComplex*> > MsComplexesSet;

#endif /* MSCOMLEX_H_ */
