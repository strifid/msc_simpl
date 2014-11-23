/*
 * GradientProcessor.h
 *
 *  Created on: Nov 15, 2012
 *      Author: trifid
 */

#ifndef GRADIENTPROCESSORQUAD_H_
#define GRADIENTPROCESSORQUAD_H_

#include <string>
#include "opencv2/core/core.hpp"
#include "Vertex.h"
#include <set>
#include "Image.h"
#include "Edge.h"
#include "Triangle.h"
#include "Coface.h"
#include "SimplexStorage.h"
#include "SimplexRelations.h"
#include "CofacedFace.h"
#include "ArcStorage.h"
#include "MsComplex.h"
#include "GradientProcessor.h"
#include "Quad.h"
#include "persist_pair/PPoint.h"
#include "Utils.h"
using cv::Mat;

class GradientProcessorQuad: public GradientProcessor {
public:

	GradientProcessorQuad();
	virtual ~GradientProcessorQuad();

	void run();

protected:

	void simplify(uint32_t persistence);

	//todo remove faceptr type from template
	template<typename ArcPtr, typename CriticalPtr, typename FacePtr>
	bool removePersistentPair(ArcPtr lowestPersistentPair, ArcStorage<ArcPtr, CriticalPtr> &arcStorage, bool asc) {
		arcStorage.eraseArc(lowestPersistentPair);
		std::vector<ArcPtr> *arcs = arcStorage.seddles(lowestPersistentPair->m_arcEnd);
		ArcPtr secondLeg = arcStorage.getSecondLeg(lowestPersistentPair->m_arcBegin);

		if (!secondLeg || *(secondLeg->m_arcEnd) == *(lowestPersistentPair->m_arcEnd)) {
			arcStorage.addArc(lowestPersistentPair);
			return false;
		}

		if (arcs != NULL) {
			std::vector<ArcPtr> *secondLegArcs = arcStorage.seddles(secondLeg->m_arcEnd);
			if (secondLegArcs) {
				for (size_t i = 0; i < arcs->size(); ++i) {
					for (size_t j = 0; j < secondLegArcs->size(); ++j) {
						if (*(arcs->at(i)->m_arcBegin) == *(secondLegArcs->at(j)->m_arcBegin)) {
							std::cout << "ERROR. cna't simpl. " << *(lowestPersistentPair->m_arcEnd) << " and seddle "
									<< *(lowestPersistentPair->m_arcBegin) << std::endl;
							arcStorage.addArc(lowestPersistentPair);
							return false;
						}
					}
				}

			}
			for (size_t i = 0; i < arcs->size(); ++i) {
				ArcPtr arc = arcs->at(i);
				arc->addFirstLeg(lowestPersistentPair);
				arc->addSecondLeg(secondLeg);
				arcStorage.registerArcByNewCritical(arc);
			}
			arcStorage.eraseArc(secondLeg);
		}

		arcStorage.erasePersistentPair(lowestPersistentPair);
		return true;

	}

	typedef std::vector<MsComplex*> MsComplexes;
	typedef MsComplex* MsComplexPtr;

	virtual int32_t findEdges();
	virtual int32_t findFaces();

	AscArcStorage m_ascArcsStorage;
	DescArcStorage m_descArcsStorage;

	AscArcStorage m_ascArcsStorageForSimpl;
	DescArcStorage m_descArcsStorageForSimpl;

	void connectArcs(AscArcStorage& ascArc, DescArcStorage& descArc);
	MsComplex* completeComplex(AscArcPtr aArc1, DescArcPtr dArc1, AscArcs *aArcs, DescArcs *dArcs);

	void addDescArc(VertexPtr vtx, EdgePtr seddle);

	void createArcStorageForSimpl();

private:
	void printInfo();
	void findAscArcs();
	void findDescArcs();
	void simplifyPpairs(std::vector<std::pair<PPointPtr, PPointPtr> >& pps);
};

#endif /* GRADIENTPROCESSOR_H_ */
