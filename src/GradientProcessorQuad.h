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
#include "MsComplexEditor.h"
#include "persist_pair/PPoint.h"
#include "Utils.h"
using cv::Mat;

typedef ArcStorage<AscArcPtr, FacePtr> AscArcStorage;
typedef ArcStorage<DescArcPtr, VertexPtr> DescArcStorage;

class GradientProcessorQuad: public GradientProcessor {
public:

	GradientProcessorQuad();
	virtual ~GradientProcessorQuad();

	void run();

protected:

	MsComplexEditor m_editor;

	void simplify(uint32_t persistence);

	template<typename ArcPtr, typename CriticalPtr, typename CriticalForDelPtr, typename ArcForDelPtr>
	void simplifyArcs(std::vector<CriticalPtr> criticals, ArcStorage<ArcPtr, CriticalPtr> &arcsStorage,
			ArcStorage<ArcForDelPtr, CriticalForDelPtr> &arcsStorageForDel, uint32_t persistence, bool asc = true) {

		for (size_t criticalIndx = 0; criticalIndx < criticals.size(); ++criticalIndx) {
			std::vector<ArcPtr> cantRemove;
			bool hasTodo = true;
			do {
				ArcPtr lowestPersistentPairAsc = findPersistentPair<CriticalPtr, ArcPtr>(criticals[criticalIndx],
						arcsStorage.seddles(criticals[criticalIndx]), persistence, cantRemove, asc);
				if (lowestPersistentPairAsc) {
					if (!removePersistentPair<ArcPtr, CriticalPtr, FacePtr>(lowestPersistentPairAsc, arcsStorage, asc)) {
						cantRemove.push_back(lowestPersistentPairAsc);
					} else {
						arcsStorageForDel.erase(lowestPersistentPairAsc->m_arcBegin);
					}
				} else
					hasTodo = false;
			} while (hasTodo);
		}
	}

	template<typename Simplex, typename ArcPtr>
	ArcPtr findPersistentPair(Simplex face, std::vector<ArcPtr> *arcs, uint32_t persistence, std::vector<ArcPtr> &cantRemove, bool asc = true) {
		if (arcs == NULL)
			return NULL;
		ArcPtr maxPersistPair = NULL;
		for (size_t i = 0; i < arcs->size(); ++i) {
			ArcPtr arc = arcs->at(i);
			bool isPerPair = false;
			if (asc)
				isPerPair = arc->m_arcBegin->maxVertex()->value().getInt() + persistence > face->maxVertex()->value().getInt();
			else
				isPerPair = arc->m_arcBegin->maxVertex()->value().getInt() < face->maxVertex()->value().getInt() + persistence;

			if (isPerPair) {
				if (Utils::hasInVectorPtr<ArcPtr>(cantRemove, arcs->at(i)))
					continue;

				if (maxPersistPair != NULL && maxPersistPair->m_arcEnd->maxVertex()->value() < arcs->at(i)->m_arcEnd->maxVertex()->value())
					maxPersistPair = arcs->at(i);
				else
					maxPersistPair = arcs->at(i);
			}
		}
		return maxPersistPair;
	}

	//todo remove faceptr type from template
	template<typename ArcPtr, typename CriticalPtr, typename FacePtr>
	bool removePersistentPair(ArcPtr lowestPersistentPair, ArcStorage<ArcPtr, CriticalPtr> &arcStorage, bool asc) {
		if (lowestPersistentPair->m_arcBegin->maxVertex()->x == 25 && lowestPersistentPair->m_arcBegin->maxVertex()->y == 98) {
			std::cout << "start delete" << std::endl;
		}

		arcStorage.eraseArc(lowestPersistentPair);
		std::vector<ArcPtr> *arcs = arcStorage.seddles(lowestPersistentPair->m_arcEnd);
		ArcPtr secondLeg = arcStorage.getSecondLeg(lowestPersistentPair->m_arcBegin);
		if (!secondLeg || *(secondLeg->m_arcEnd) == *(lowestPersistentPair->m_arcEnd)) {
			arcStorage.addArc(lowestPersistentPair);

			if (lowestPersistentPair->m_arcBegin->maxVertex()->x == 25 && lowestPersistentPair->m_arcBegin->maxVertex()->y == 98) {
				std::cout << "no second leg" << std::endl;
			}
			return false;
		}

		if (lowestPersistentPair->m_arcBegin->maxVertex()->x == 25 && lowestPersistentPair->m_arcBegin->maxVertex()->y == 98) {
			std::cout << "sec leg ok" << std::endl;
		}

//			std::cout << "find second leg beg: " << *(secondLeg->m_arcBegin) << " end: " << *(secondLeg->m_arcEnd) << std::endl;

/*
		if (arcStorage.hasCommonSeddle(secondLeg->m_arcEnd, lowestPersistentPair->m_arcEnd)) {
			arcStorage.addArc(lowestPersistentPair);
//			std::cout << "find second leg beg: " << *(secondLeg->m_arcBegin) << " end: " << *(secondLeg->m_arcEnd) << std::endl;
			if (lowestPersistentPair->m_arcBegin->maxVertex()->x == 25 && lowestPersistentPair->m_arcBegin->maxVertex()->y == 98) {
				std::cout << "has common seddle" << std::endl;
			}

			return false;
		}
*/

		if (arcs != NULL) {

/*
			for (size_t i = 0; i < arcs->size(); ++i) {
				ArcPtr arc = arcs->at(i);
				if (lowestPersistentPair->hasCommon(arc->m_arc)) {
					arcStorage.addArc(lowestPersistentPair);
					if (lowestPersistentPair->m_arcBegin->maxVertex()->x == 25 && lowestPersistentPair->m_arcBegin->maxVertex()->y == 98) {
						std::cout << "11111" << std::endl;
					}

					return false;
				}
			}
*/
			for (size_t i = 0; i < arcs->size(); ++i) {
				ArcPtr arc = arcs->at(i);
				arc->addFirstLeg(lowestPersistentPair);
				/*
				 if(asc)
				 arc->m_arc.push_back(lowestPersistentPair->m_arcEnd);
				 */

				arc->addSecondLeg(secondLeg);
				arcStorage.registerArcByNewCritical(arc);

				/*
				 if (asc) {
				 if (arc->m_arcBegin->maxVertex()->value() > arc->m_arcEnd->maxVertex()->value())
				 std::cout << "WARNING: asc arc beg greater than end" << std::endl;
				 } else {
				 if (arcs->at(i)->m_arcBegin->maxVertex()->value() < arcs->at(i)->m_arcEnd->maxVertex()->value())
				 std::cout << "WARNING: desc arc beg smaller than end" << std::endl;
				 }

				 std::cout << "new arc: " << *(arcs->at(i)->m_arcBegin->maxVertex()) << " end: " << *(arcs->at(i)->m_arcEnd->maxVertex()) << std::endl;
				 */
			}
			arcStorage.eraseArc(secondLeg);
		}else{
			if (lowestPersistentPair->m_arcBegin->maxVertex()->x == 25 && lowestPersistentPair->m_arcBegin->maxVertex()->y == 98) {
				std::cout << "no arc" << std::endl;
			}
		}

		//	std::cout << std::endl;
		arcStorage.erasePersistentPair(lowestPersistentPair);
		return true;

	}

	//bool removeDescPersistentPair(DescArcPtr arc);

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

	void saveMaxInVtk();
	void saveMinInVtk();

	void createArcStorageForSimpl();

private:
	void printInfo();
	void findAscArcs();
	void findDescArcs();
	void simplifyPpairs(std::vector<std::pair<PPointPtr, PPointPtr> >& pps);
}
;

#endif /* GRADIENTPROCESSOR_H_ */
