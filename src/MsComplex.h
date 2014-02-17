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
#include "SimplexStorage.h"

using cv::circle;

class MsComplex {
public:

	MsComplex() {
	}
	virtual ~MsComplex() {
	}

	void draw(Mat& img);
	FacePtr m_max;
	Edges m_seddles;
	VertexPtr m_min;

	DescArcs m_dArcs;
	std::vector<Arc<FacePtr, FacePtr> *> m_aArcs;

	void organizeArcs() {

		for (size_t i = 0; i < m_dArcs.size(); ++i) {
			DescArcPtr arc = m_dArcs[i];

		}

	}

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

	bool isPersistent(uint32_t persistent);
	EdgePtr findCommonSeddle(Edges& seddles) {

		EdgePtr commonSaddle = NULL;
		uint32_t commonSaddleCnt = 0;
		for (size_t i = 0; i < m_seddles.size(); ++i) {
			for (size_t j = 0; j < seddles.size(); ++j) {
				if (*(m_seddles[i]) == *(seddles[j])) {
					commonSaddle = m_seddles[i];
					commonSaddleCnt++;
				}
			}
		}

		if (commonSaddleCnt > 1)
			return NULL;
		return commonSaddle;
	}

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

	bool concatByMax(MsComplex* cmplx) {
		EdgePtr commonSeddle = findCommonSeddle(cmplx->m_seddles);
		if (!commonSeddle)
			return false;

		for (typename std::vector<Arc<FacePtr, FacePtr> *>::iterator it = m_aArcs.begin(); it != m_aArcs.end(); it++) {
			if (*((*it)->m_arcBegin) == *commonSeddle) {
				m_aArcs.erase(it);
				break;
			}
		}

		for (typename std::vector<Arc<FacePtr, FacePtr> *>::iterator it = cmplx->m_aArcs.begin(); it != cmplx->m_aArcs.end(); it++) {
			if (*((*it)->m_arcBegin) != *commonSeddle) {
				m_aArcs.insert(m_aArcs.begin(), *it);
				break;
			}
		}

		DescArcPtr commonArc = NULL;
		for (size_t i = 0; i < m_dArcs.size(); ++i) {
			if (*(m_dArcs[i]->m_arcBegin) == *(commonSeddle)) {
				commonArc = m_dArcs[i];
				break;
			}
		}

		if (commonArc == NULL) {
			std::cout << "can't find common for " << *commonSeddle << std::endl;
			return false;
		}

		DescArcPtr commonNextArc = NULL;
		for (size_t i = 0; i < cmplx->m_dArcs.size(); ++i) {
			if (*(cmplx->m_dArcs[i]->m_arcBegin) == *(commonSeddle)) {
				commonNextArc = cmplx->m_dArcs[i];
				break;
			}
		}
		if (commonNextArc == NULL) {
			std::cout << "can't find next common" << std::endl;
			return false;
		}

		commonArc->m_arc.insert(commonArc->m_arc.begin(), commonSeddle);

		commonArc->m_arc.insert(commonArc->m_arc.begin(), commonNextArc->m_arc.rbegin(), commonNextArc->m_arc.rend());

		DescArcPtr arcBehindMin = NULL;
		for (size_t i = 0; i < cmplx->m_dArcs.size(); ++i) {
			if (*(cmplx->m_dArcs[i]->m_arcBegin) != *(commonSeddle)) {
				arcBehindMin = cmplx->m_dArcs[i];
				break;
			}
		}
		if (arcBehindMin == NULL) {
			std::cout << "can't find behind" << std::endl;
			return false;
		}

		commonArc->m_arc.insert(commonArc->m_arc.begin(), arcBehindMin->m_arc.rbegin(), arcBehindMin->m_arc.rend());

		commonArc->m_arcBegin = arcBehindMin->m_arcBegin;

		std::stringstream outStrm;
		for (int i = 0; i < commonArc->m_arc.size(); ++i) {
			outStrm << *(commonArc->m_arc[i]) << " ";
		}

//		std::cout << "total arc: " << outStrm.str() << std::endl;
		for (std::vector<EdgePtr>::iterator it = m_seddles.begin(); it != m_seddles.end(); it++) {
			if (*(*it) == *commonSeddle) {
				m_seddles.erase(it);
				break;
			}
		}

		for (std::vector<EdgePtr>::iterator it = cmplx->m_seddles.begin(); it != cmplx->m_seddles.end(); it++) {
			if (*(*it) != *commonSeddle) {
				m_seddles.push_back(*it);
				break;
			}
		}

		return true;
	}
};

typedef std::set<MsComplex*, SimplexComparator<MsComplex*> > MsComplexesSet;

#endif /* MSCOMLEX_H_ */
