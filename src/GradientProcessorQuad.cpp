#include "GradientProcessorQuad.h"

#include <mt/common/Singleton.h>
#include <mt/common/SmartPtr.h>
#include <mt/logger/Logger.h>
#include <mt/logger/LoggerFactory.h>
#include <mt/thread/AutoMutex.h>
#include <mt/thread/Mutex.h>
#include <mt/utils/StrUtils.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <vtkCellArray.h>
#include <vtkIdList.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>
#include <vtkXMLPolyDataWriter.h>
#include <algorithm>
#include <iosfwd>
#include <iostream>
#include <map>
#include <utility>
#include <vector>

#include "Arc.h"
#include "MsComplexStorage.h"
#include "persist_pair/BarCodeProcessor.h"
#include "persist_pair/PersistPairProcessor.h"
#include "Pixel.h"

//#include "VtpWriter.h"

std::ostream& operator<<(std::ostream& out, DescArcPtr a) {
	out << "begin: " << *(a->m_arcBegin) << ", end: " << *(a->m_arcEnd) << " arc: ";
	for (size_t i = 0; i < a->m_arc.size(); ++i) {
		out << *(a->m_arc[i]) << " ";
	}
	return out;
}

std::ostream& operator<<(std::ostream& out, AscArcPtr a) {
	out << "begin: " << *(a->m_arcBegin) << ", end: " << *(a->m_arcEnd) << " arc: ";
	for (size_t i = 0; i < a->m_arc.size(); ++i) {
		out << *(a->m_arc[i]) << " ";
	}
	return out;
}

GradientProcessorQuad::GradientProcessorQuad() {
	MT_LOGINIT(GradientProcessorQuad)
}

GradientProcessorQuad::~GradientProcessorQuad() {
}

int32_t GradientProcessorQuad::findEdges() {

	std::vector<VertexPtr>& vertexes = m_vertexes.vector();
	for (size_t i = 0; i < vertexes.size(); i++) {
		Pixels pxls = m_img.getOneConnectedAround(vertexes[i]->x, vertexes[i]->y);

		if (pxls.size() > 4) {
			std::cout << "ERROR: vtx: " << *(vertexes[i]) << " has one connected " << pxls.size() << std::endl;
		}
		for (size_t z = 0; z < pxls.size(); z++) {
			VertexPtr vertexB = findVertexByPixel(pxls[z]);
			if (vertexB == NULL)
				continue;

			EdgePtr face = new Edge(vertexes[i], vertexB);
			if (m_edges.exist(face)) {
				delete face;
				continue;
			}

			addEdge(face);
		}
	}

	return 0;
}

int32_t GradientProcessorQuad::findFaces() {

	for (size_t i = 0; i < m_vertexes.vector().size(); i++) {

		VertexPtr a = m_vertexes.vector()[i];
		VertexPtr b = findVertexByPixel(Pixel(a->x + 1, a->y));
		VertexPtr c = findVertexByPixel(Pixel(a->x, a->y + 1));
		VertexPtr d = findVertexByPixel(Pixel(a->x + 1, a->y + 1));

		if (!b || !c || !d)
			continue;

		Edge tmpEdgeAB(a, b);
		EdgePtr edgeAB = m_edges.getSimplex(&tmpEdgeAB);

		Edge tmpEdgeAC(a, c);
		EdgePtr edgeAC = m_edges.getSimplex(&tmpEdgeAC);

		Edge tmpEdgeBD(b, d);
		EdgePtr edgeBD = m_edges.getSimplex(&tmpEdgeBD);

		Edge tmpEdgeCD(c, d);
		EdgePtr edgeCD = m_edges.getSimplex(&tmpEdgeCD);
		if (edgeAB == NULL || edgeAC == NULL || edgeBD == NULL || edgeCD == NULL) {
			std::cout << "WARNING: can't find edges" << std::endl;
			continue;
		}

		FacePtr face = new Face();
		face->addEdge(edgeAB);
		face->addEdge(edgeAC);
		face->addEdge(edgeBD);
		face->addEdge(edgeCD);
		addFace(face);

	}

	return 0;
}

void GradientProcessorQuad::addDescArc(VertexPtr vtx, EdgePtr seddle) {
	Vertexes vtxs;
	Edges faces;
	vtxs.push_back(vtx);
	faces.push_back(seddle);

//	std::cout << "try find desc arc for " << *vtx << " begin: " << *seddle << std::endl;
	getDescendingManifold(faces, vtxs);
	DescArcPtr descArc = new Arc<EdgePtr, VertexPtr>(seddle, faces, vtxs.back());
	m_descArcsStorage.addArc(descArc);

}

void GradientProcessorQuad::printInfo() {
	std::cout << "vertexes: " << m_vertexes.vector().size() << std::endl;
	std::cout << "faces: " << m_edges.vector().size() << std::endl;
	std::cout << "faces: " << m_faces.vector().size() << std::endl;
	std::cout << "critical points:" << std::endl << "minimums: " << m_minimums.size() << std::endl;
	std::cout << "saddles: " << m_seddles.size() << std::endl;
	std::cout << "maximums: " << m_maximums.size() << std::endl;
}

void GradientProcessorQuad::findAscArcs() {
	for (size_t i = 0; i < m_seddles.size(); i++) {
		Faces *tr = m_simplexRelations.face(m_seddles[i]);
		if (tr == NULL) {
			std::cout << "ERROR. have no any cofaced faces to seddle" << std::endl;
			continue;
		}
		for (size_t z = 0; z < tr->size(); z++) {
			Faces arc;
			arc.push_back(tr->at(z));
			if (getAscendingManifold(arc)) {
				AscArcPtr ascArc = new Arc<FacePtr, FacePtr>(m_seddles[i], arc, arc.back());
				m_ascArcsStorage.addArc(ascArc);
			} else
				std::cout << "WARNING can't find asc manifold for " << *(tr->at(z)) << std::endl;
		}
	}
	m_ascArcsStorage.printAll("asc");
}

void GradientProcessorQuad::findDescArcs() {
	for (size_t i = 0; i < m_seddles.size(); i++) {
		addDescArc(m_seddles[i]->m_a, m_seddles[i]);
		addDescArc(m_seddles[i]->m_b, m_seddles[i]);
	}

	m_descArcsStorage.printAll("desc");

}

void GradientProcessorQuad::simplifyPpairs(std::vector<std::pair<PPointPtr, PPointPtr> >& pps) {
	createArcStorageForSimpl();

	for (size_t i = 0; i < pps.size(); ++i) {
		/*
		 if (pps[i].first->m_id == 17 || pps[i].second->m_id == 17)
		 std::cout << "delete !!!!!!!!! first " << pps[i].first->m_id << " sec " << pps[i].second->m_id << " first dim " << pps[i].first->m_dim
		 << std::endl;
		 */
		if (pps[i].first->m_smplx == NULL || pps[i].second->m_smplx == NULL) {
			std::cout << "ERROR: smplx in ppair is NULL" << std::endl;
			exit(0);
		}

		if (pps[i].first->m_dim == PPoint::NEGATIVE || pps[i].second->m_dim == PPoint::NEGATIVE) {
			FacePtr face = NULL;
			EdgePtr edge = NULL;
			if (pps[i].first->m_dim == PPoint::NEGATIVE) {
				face = static_cast<FacePtr>(pps[i].first->m_smplx);
			} else if (pps[i].first->m_dim == PPoint::UNIVERSAL) {
				edge = static_cast<EdgePtr>(pps[i].first->m_smplx);
			}
			if (pps[i].second->m_dim == PPoint::NEGATIVE) {
				face = static_cast<FacePtr>(pps[i].second->m_smplx);
			} else if (pps[i].second->m_dim == PPoint::UNIVERSAL) {
				edge = static_cast<EdgePtr>(pps[i].second->m_smplx);
			}
			if (face == NULL || edge == NULL) {
				std::cout << "ERROR. can't find simplex for ppair" << std::endl;
				continue;
			}
			AscArcPtr arc = m_ascArcsStorage.getArc(edge, face);
			if (arc == NULL) {
				continue;
			}

			if (removePersistentPair<AscArcPtr, FacePtr, FacePtr>(arc, m_ascArcsStorageForSimpl, true)) {
				m_descArcsStorageForSimpl.erase(arc->m_arcBegin);
				/*
				 std::cout << "erase from AscArc storage arc begin " << *(arc->m_arcBegin->maxVertex()) << " end: " << *(arc->m_arcEnd->maxVertex())
				 << std::endl;
				 */

				/*
				 std::cout << "sedle is " << *(arc->m_arcBegin) << " vertex val is : a=" <<arc->m_arcBegin->m_a->value() <<  " vertex val is : b=" <<arc->m_arcBegin->m_b->value()
				 << "max vertex val is "<< arc->m_arcBegin->maxVertex()->value() << std::endl;
				 */

			}
		} else if (pps[i].first->m_dim == PPoint::POSITIVE || pps[i].second->m_dim == PPoint::POSITIVE) {
			VertexPtr vtx = NULL;
			EdgePtr edge = NULL;
			if (pps[i].first->m_dim == PPoint::POSITIVE) {
				vtx = static_cast<VertexPtr>(pps[i].first->m_smplx);
			} else if (pps[i].first->m_dim == PPoint::UNIVERSAL) {
				edge = static_cast<EdgePtr>(pps[i].first->m_smplx);
			}
			if (pps[i].second->m_dim == PPoint::POSITIVE) {
				vtx = static_cast<VertexPtr>(pps[i].second->m_smplx);
			} else if (pps[i].second->m_dim == PPoint::UNIVERSAL) {
				edge = static_cast<EdgePtr>(pps[i].second->m_smplx);
			}
			if (vtx == NULL || edge == NULL) {
				std::cout << "ERROR. can't find simplex for ppair" << std::endl;
				continue;
			}
			DescArcPtr arc = m_descArcsStorage.getArc(edge, vtx);
			if (arc == NULL) {
				continue;
			}
			/*
			 if (pps[i].first->m_id == 15 || pps[i].second->m_id == 15)
			 std::cout << "pp.second " << pps[i].second->m_id << " erase from DescArc storage arc begin " << *(arc->m_arcBegin->maxVertex())
			 << " end: " << *(arc->m_arcEnd->maxVertex()) << std::endl;

			 */
			if (removePersistentPair<DescArcPtr, VertexPtr, FacePtr>(arc, m_descArcsStorageForSimpl, false)) {
				/*		if (pps[i].first->m_id == 17 || pps[i].second->m_id == 17)
				 std::cout << "pp.second " << pps[i].second->m_id << " erase from DescArc storage arc begin " << *(arc->m_arcBegin->maxVertex())
				 << " end: " << *(arc->m_arcEnd->maxVertex()) << std::endl;
				 */
				m_ascArcsStorageForSimpl.erase(arc->m_arcBegin);

			}
		} else {
			std::cout << "ERROR: can't identify asc or desc " << std::endl;
		}

	}
}

void GradientProcessorQuad::run() {
//	normalizeField();

	findVertex();
	findEdges();
	findFaces();

	findMinimums();
	findSeddles();
	findMaximums();

	findAscArcs();
	findDescArcs();

	printInfo();
	connectArcs(m_ascArcsStorage, m_descArcsStorage);

	size_t cmplxN;
	size_t iii = 0;

	if (m_persistence) {
		do {
			iii++;
			cmplxN = m_msCmplxStorage.complexesSet().size();

			std::cout << "mscs: " << cmplxN << std::endl;

			PersistPairProcessor ppProc;
			ppProc.init(m_msCmplxStorage.complexesSet());
			ppProc.findPairs();
			std::vector<std::pair<PPointPtr, PPointPtr> >& pps = ppProc.filter(m_persistence);

			simplifyPpairs(pps);
			connectArcs(m_ascArcsStorageForSimpl, m_descArcsStorageForSimpl);

		} while (cmplxN != m_msCmplxStorage.complexesSet().size());
	}

	std::cout << "try: " << iii << std::endl;

	m_msCmplxStorage.drawAll();
//	cout << "find ppairs: " << ppProc.ppairs.size() << std::endl;
	drawComplexesOnOriginal();

	drawGradientField();

	std::cout << "mscs: " << m_msCmplxStorage.complexesSet().size() << std::endl;

	BarCodeProcessor proc;
	proc.init(m_msCmplxStorage.complexesSet());
	proc.computeBarCodes(m_gradFieldFile);

}

struct FacePtrComparator {
	bool operator()(FacePtr i, FacePtr j) {
		return (i->maxVertex()->value() < j->maxVertex()->value());
	}
} FacePtrComparator;

struct VertexPtrComparatorForSort {
	bool operator()(VertexPtr i, VertexPtr j) {
		return (i->value() > j->value());
	}
} vertexPtrComparator;


MsComplex* GradientProcessorQuad::completeComplex(AscArcPtr aArc1, DescArcPtr dArc1, AscArcs *aArcs, DescArcs *dArcs) {

	if (!dArcs || !aArc1 || !dArc1 || !aArcs)
		return NULL;

	AscArcPtr aArc2 = NULL;
	DescArcPtr dArc2 = NULL;
	bool hasCommonSaddle = false;
	EdgePtr commonSaddle;

	for (size_t i = 0; i < aArcs->size(); ++i) {
		aArc2 = aArcs->at(i);
		if (*aArc2 == *aArc1)
			continue;

		/*
		 if (aArc2->hasCommon(aArc1->m_arc))
		 continue;
		 */

		for (size_t z = 0; z < dArcs->size(); ++z) {
			dArc2 = dArcs->at(z);
			if (*dArc2 == *dArc1)
				continue;

			/*
			 if (dArc2->hasCommon(dArc1->m_arc))
			 continue;
			 */

			if (*(aArc2->m_arcBegin) == *(dArc2->m_arcBegin)) {
				hasCommonSaddle = true;
				commonSaddle = aArc2->m_arcBegin;
				break;
			}
		}

		if (hasCommonSaddle)
			break;
	}

	MsComplex *msCompl = NULL;

	if (hasCommonSaddle) {
		msCompl = new MsComplex();
		msCompl->m_seddles.push_back(commonSaddle);
		msCompl->m_aArcs.push_back(aArc1);
		msCompl->m_aArcs.push_back(aArc2);
		msCompl->m_dArcs.push_back(dArc1);
		msCompl->m_dArcs.push_back(dArc2);
	}

	return msCompl;
}

void GradientProcessorQuad::createArcStorageForSimpl() {

	MsComplexesSet &msStrg = m_msCmplxStorage.complexesSet();

	for (MsComplexesSet::iterator it = msStrg.begin(); it != msStrg.end(); it++) {

		MsComplexPtr msc = *it;
		for (size_t i = 0; i < msc->m_aArcs.size(); i++) {
			if (msc->m_aArcs.size() != 2)
				std::cout << "ERROR. more than 2 ascArc in MSC" << std::endl;
			if (!m_ascArcsStorageForSimpl.isInStorage(msc->m_aArcs[i]))
				m_ascArcsStorageForSimpl.addArc(msc->m_aArcs[i]);
		}

		for (size_t i = 0; i < msc->m_dArcs.size(); i++) {
			if (msc->m_dArcs.size() != 2)
				std::cout << "ERROR. more than 2 ascArc in MSC" << std::endl;
			if (!m_descArcsStorageForSimpl.isInStorage(msc->m_dArcs[i]))
				m_descArcsStorageForSimpl.addArc(msc->m_dArcs[i]);
		}
	}

}

/*
void GradientProcessorQuad::connectArcs(AscArcStorage& ascArc, DescArcStorage& descArc) {

	m_msCmplxStorage.clear();
	AscArcStorage::ArcsToSeddleMap &arcsToSeddleMap = ascArc.arcsToSeddleMap();

	for (typename AscArcStorage::ArcsToSeddleMap::iterator it = arcsToSeddleMap.begin(); it != arcsToSeddleMap.end(); it++) {

		DescArcs* descArcs = descArc.critical(it->first);
		AscArcs* ascArcs = &(it->second);
		if (!descArcs)
			continue;

		for (size_t i = 0; i < ascArcs->size(); i++) {
			FacePtr max = ascArcs->at(i)->m_arcEnd;
			AscArcs* aArcs = ascArc.seddles(max);

			for (size_t z = 0; z < descArcs->size(); z++) {
				//todo somthing strange
				VertexPtr min = descArcs->at(z)->m_arcEnd;

				if (max->m_valueFirst < min->m_valueFirst)
					continue;

				MsComplex *msComplex = completeComplex(ascArcs->at(i), descArcs->at(z), aArcs, descArc.seddles(min));
				if (msComplex) {
					msComplex->m_max = max;
					msComplex->m_min = min;
					msComplex->m_seddles.push_back(it->first);
					if (m_msCmplxStorage.isExist(msComplex))
						delete msComplex;
					else
						m_msCmplxStorage.addComplex(msComplex);
				}
			}
		}
	}

}
*/

void GradientProcessorQuad::connectArcs(AscArcStorage& ascArc, DescArcStorage& descArc) {

	m_msCmplxStorage.clear();
	DescArcStorage::ArcsToSeddleMap &arcsToSeddleMap = descArc.arcsToSeddleMap();

	for (typename DescArcStorage::ArcsToSeddleMap::iterator it = arcsToSeddleMap.begin(); it != arcsToSeddleMap.end(); it++) {

		DescArcs* descArcs = &(it->second);
		AscArcs* ascArcs = ascArc.critical(it->first);
		if (!ascArcs)
			continue;

		for (size_t i = 0; i < ascArcs->size(); i++) {
			FacePtr max = ascArcs->at(i)->m_arcEnd;
			AscArcs* aArcs = ascArc.seddles(max);

			for (size_t z = 0; z < descArcs->size(); z++) {
				//todo somthing strange
				VertexPtr min = descArcs->at(z)->m_arcEnd;

				if (max->m_valueFirst < min->m_valueFirst)
					continue;

				MsComplex *msComplex = completeComplex(ascArcs->at(i), descArcs->at(z), aArcs, descArc.seddles(min));
				if (msComplex) {
					msComplex->m_max = max;
					msComplex->m_min = min;
					msComplex->m_seddles.push_back(it->first);
					if (m_msCmplxStorage.isExist(msComplex))
						delete msComplex;
					else
						m_msCmplxStorage.addComplex(msComplex);
				}
			}
		}
	}

	m_msCmplxStorage.printInfo(m_maximums);
}
