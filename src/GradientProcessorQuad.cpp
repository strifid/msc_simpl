#include "GradientProcessorQuad.h"

/*
 #include <mt/common/Singleton.h>
 #include <mt/common/SmartPtr.h>
 #include <mt/logger/Logger.h>
 #include <mt/logger/LoggerFactory.h>
 #include <mt/thread/AutoMutex.h>
 #include <mt/thread/Mutex.h>
 #include <mt/utils/StrUtils.h>
 */
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
}

GradientProcessorQuad::~GradientProcessorQuad() {
}

int32_t GradientProcessorQuad::findEdges() {

	for (int x = 0; x < m_img.width(); x++) {
		for (int y = 0; y < m_img.height(); y++) {

			VertexPtr a = findVertexByPixel(Pixel(x, y));
			VertexPtr b = findVertexByPixel(Pixel(x + 1 == m_img.width() ? 0 : x + 1, y));
			if (b == NULL)
				std::cout << " b can't find for " << x << " " << y << std::endl;
			VertexPtr c = findVertexByPixel(Pixel(x, y + 1 == m_img.height() ? 0 : y + 1));
			if (c == NULL)
				std::cout << "can't find for " << x << " " << y << std::endl;

			EdgePtr face = new Edge(a, b);
			addEdge(face);
			face = new Edge(a, c);
			addEdge(face);

		}
	}
	return 0;
}

int32_t GradientProcessorQuad::findFaces() {

	for (int x = 0; x < m_img.width(); x++) {
		for (int y = 0; y < m_img.height(); y++) {
			VertexPtr a = findVertexByPixel(Pixel(x, y));
			VertexPtr b = findVertexByPixel(Pixel(x + 1 == m_img.width() ? 0 : x + 1, y));
			VertexPtr c = findVertexByPixel(Pixel(x, y + 1 == m_img.height() ? 0 : y + 1));
			VertexPtr d = findVertexByPixel(Pixel(x + 1 == m_img.width() ? 0 : x + 1, y + 1 == m_img.height() ? 0 : y + 1));

			Edge tmpEdgeAB(a, b);
			EdgePtr edgeAB = m_edges.getSimplex(&tmpEdgeAB);

			Edge tmpEdgeAC(a, c);
			EdgePtr edgeAC = m_edges.getSimplex(&tmpEdgeAC);

			Edge tmpEdgeBD(b, d);
			EdgePtr edgeBD = m_edges.getSimplex(&tmpEdgeBD);

			Edge tmpEdgeCD(c, d);
			EdgePtr edgeCD = m_edges.getSimplex(&tmpEdgeCD);
			if (edgeAB == NULL || edgeAC == NULL || edgeBD == NULL || edgeCD == NULL) {
				std::cout << "ERROR: can't find edges" << std::endl;
				continue;
			}

			FacePtr face = new Face();
			face->addEdge(edgeAB);
			face->addEdge(edgeAC);
			face->addEdge(edgeBD);
			face->addEdge(edgeCD);
			addFace(face);

		}
	}

	return 0;
}

void GradientProcessorQuad::addDescArc(VertexPtr vtx, EdgePtr seddle) {
	Vertexes vtxs;
	Edges edges;
	vtxs.push_back(vtx);
	edges.push_back(seddle);

//	std::cout << "try find desc arc for " << *vtx << " begin: " << *seddle << std::endl;
	getDescendingManifold(edges, vtxs);

//	std::cout << "vertexes: " << vtxs.size() << " edges " << edges.size() << " last vtx: " << *(vtxs.back()) << " seddle " << *(seddle)<< std::endl ;
	VertexesSet::iterator it = m_minimums.find(vtxs.back());
	if (it == m_minimums.end()) {
		std::cout << "ERROR. can't find desc manyfold for " << *(vtxs.back()) << std::endl;
	} else {
		DescArcPtr descArc = new Arc<EdgePtr, VertexPtr>(seddle, edges, vtxs.back());
		m_descArcsStorage.addArc(descArc);
	}
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
			AscArcPtr arc = m_ascArcsStorageForSimpl.getArc(edge, face);
			if (arc == NULL) {
				std::cout << "ERROR. can't find arc for ppair: max: " << *(face->maxVertex()) << " seddle " << *edge << std::endl;
				continue;
			}
			std::cout << "ppair: max: " << *(face->maxVertex()) << " seddle " << *edge << std::endl;

			if (removePersistentPair<AscArcPtr, FacePtr, FacePtr>(arc, m_ascArcsStorageForSimpl, true)) {
				m_descArcsStorageForSimpl.erase(arc->m_arcBegin);
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
			DescArcPtr arc = m_descArcsStorageForSimpl.getArc(edge, vtx);
			if (arc == NULL) {
				std::cout << "ERROR. can't find arc for ppair: min: " << *(vtx) << " seddle " << *edge << std::endl;
				continue;
			}
			std::cout << "ppair: max: " << *(vtx) << " seddle " << *edge << std::endl;

			if (removePersistentPair<DescArcPtr, VertexPtr, FacePtr>(arc, m_descArcsStorageForSimpl, false)) {
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

	DescArcs oneConnected;
	for (DescArcStorage::ArcsToCriticalMap::iterator it = m_descArcsStorage.m_arcsToCriticalMap.begin();
			it != m_descArcsStorage.m_arcsToCriticalMap.end(); ++it) {
		if (it->second.size() == 1) {
			oneConnected.push_back(it->second.at(0));
			std::cout << "has onlye one arc. critical " << *(it->first) << std::endl;
		}
	}

	for (size_t i = 0; i < oneConnected.size(); ++i) {
		m_descArcsStorage.eraseArc(oneConnected[i]);
		m_ascArcsStorage.erase(oneConnected[i]->m_arcBegin);
	}

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
//			ppProc.init(m_ascArcsStorage, m_descArcsStorage);

			ppProc.findPairs();
			std::vector<std::pair<PPointPtr, PPointPtr> >& pps = ppProc.filter(m_persistence);

			simplifyPpairs(pps);
			connectArcs(m_ascArcsStorageForSimpl, m_descArcsStorageForSimpl);
//			connectArcs(m_ascArcsStorage, m_descArcsStorage);

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

		for (size_t z = 0; z < dArcs->size(); ++z) {
			dArc2 = dArcs->at(z);
			if (*dArc2 == *dArc1)
				continue;

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
