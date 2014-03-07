#include "GradientProcessorQuad.h"

#include <mt/common/Singleton.h>
#include <mt/common/SmartPtr.h>
#include <mt/logger/Logger.h>
#include <mt/logger/LoggerFactory.h>
#include <mt/thread/AutoMutex.h>
#include <mt/thread/Mutex.h>
#include <stddef.h>
#include <sys/types.h>
#include <vtkCellArray.h>
#include <vtkIdList.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>
#include <vtkXMLPolyDataWriter.h>
//#include "opencv2/core/core.hpp"
//#include "opencv2/highgui/highgui.hpp"
#include <algorithm>
#include <stdint.h>
//#include <fstream>
//#include <iosfwd>
#include <iostream>
#include <map>
//#include <set>
#include <utility>
#include <vector>

#include "Arc.h"
//#include "CofacedFace.h"
//#include "Edge.h"
//#include "MsComplexEditor.h"
#include "MsComplexStorage.h"
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
		Pixels pxls = m_img.getOneConnectedAround(dynamic_cast<Pixel&>(*vertexes[i]));
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

void GradientProcessorQuad::run() {
	normalizeField();

	findVertex();
	findEdges();
	findFaces();

	findMinimums();
	findSeddles();
	findMaximums();

	for (size_t i = 0; i < m_seddles.size(); i++) {
		Faces *tr = m_simplexRelations.face(m_seddles[i]);
		if (tr == NULL)
			continue;
		for (size_t z = 0; z < tr->size(); z++) {
			Faces arc;
			arc.push_back(tr->at(z));
			if (getAscendingManifold(arc)) {
				AscArcPtr ascArc = new Arc<FacePtr, FacePtr>(m_seddles[i], arc, arc.back());
				m_ascArcsStorage.addArc(ascArc);
			}
		}
	}

	for (size_t i = 0; i < m_seddles.size(); i++) {
		addDescArc(m_seddles[i]->m_a, m_seddles[i]);
		addDescArc(m_seddles[i]->m_b, m_seddles[i]);
	}

	std::cout << "vertexes: " << m_vertexes.vector().size() << std::endl;
	std::cout << "faces: " << m_edges.vector().size() << std::endl;
	std::cout << "faces: " << m_faces.vector().size() << std::endl;

	std::cout << "critical points:" << std::endl << "minimums: " << m_minimums.size() << std::endl;
	std::cout << "saddles: " << m_seddles.size() << std::endl;
	std::cout << "maximums: " << m_maximums.size() << std::endl;

	connectArcs(m_ascArcsStorage, m_descArcsStorage);
	std::cout << "mscs: " << m_msCmplxStorage.complexesSet().size() << std::endl;

	PersistPairProcessor ppProc;
	ppProc.init(m_msCmplxStorage.complexesSet());
	ppProc.findPairs();

	/*
	 if (m_persistence) {
	 createArcStorageForSimpl();
	 for (size_t i = 0; i < 10; ++i) {

	 simplify(m_persistence);
	 connectArcs(m_ascArcsStorageForSimpl, m_descArcsStorageForSimpl);
	 createArcStorageForSimpl();

	 std::cout << "after simpl with p= " << m_persistence << " mscs: " << m_msCmplxStorage.complexesSet().size() << std::endl;
	 }
	 }
	 */

	if (!m_outputFile.empty())
		m_msCmplxStorage.saveCriticalPoints(m_outputFile + ".txt");

	m_msCmplxStorage.drawAll();
	cout << "find ppairs: " << ppProc.ppairs.size() << std::endl;

	drawGradientField();
	drawComplexesOnOriginal();

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

void GradientProcessorQuad::removeDescArc(int vx, int vy, int sx, int sy) {

	VertexPtr vtx = new Vertex(vx, vy);

	std::vector<DescArcPtr> *v = m_descArcsStorageForSimpl.seddles(vtx);
	if (v == NULL) {
		std::cout << "err can't find x2 y0 min" << std::endl;
		return;
	}

	DescArcPtr sm;
	for (size_t i = 0; i < v->size(); ++i) {
		if (v->at(i)->m_arcBegin->m_a->x == sx && v->at(i)->m_arcBegin->m_a->y == sy) {
			sm = v->at(i);
		}
	}
	if (!sm) {
		std::cout << "can't find desc arc for min(" << vx << ", " << vy << ") seddle_max " << sx << ", " << sy << std::endl;
		return;
	}

	if (!removePersistentPair<DescArcPtr, VertexPtr, FacePtr>(sm, m_descArcsStorageForSimpl, false)) {
		std::cout << "can't remove desc arc" << std::endl;
	}

}

void GradientProcessorQuad::removeAscArc(int vx, int vy, int sx, int sy) {

	VertexPtr ta = new Vertex(vx, vy);
	VertexPtr tb = new Vertex(vx + 1, vy);
	VertexPtr tc = new Vertex(vx, vy + 1);
	VertexPtr td = new Vertex(vx + 1, vy + 1);

	VertexPtr a = m_vertexes.getSimplex(ta);
	VertexPtr b = m_vertexes.getSimplex(tb);
	VertexPtr c = m_vertexes.getSimplex(tc);
	VertexPtr d = m_vertexes.getSimplex(td);

	Edge tmpEdgeBD(b, d);
	EdgePtr edgeBD = m_edges.getSimplex(&tmpEdgeBD);
	if (edgeBD == NULL) {
		std::cout << "WARNING: can't find edges bd" << std::endl;
		return;
	}

	Edge tmpEdgeCD(c, d);
	EdgePtr edgeCD = m_edges.getSimplex(&tmpEdgeCD);
	if (edgeCD == NULL) {
		std::cout << "WARNING: can't find edges cd" << std::endl;
		return;
	}

	Edge tmpEdgeAB(b, a);
	EdgePtr edgeAB = m_edges.getSimplex(&tmpEdgeAB);
	if (edgeAB == NULL) {
		std::cout << "WARNING: can't find edges ab " << tmpEdgeAB << std::endl;
		return;
	}

	Edge tmpEdgeAC(c, a);
	EdgePtr edgeAC = m_edges.getSimplex(&tmpEdgeAC);
	if (edgeAC == NULL) {
		std::cout << "WARNING: can't find edges ac" << std::endl;
		return;
	}

	FacePtr max = new Face();
	max->addEdge(edgeAB);
	max->addEdge(edgeAC);
	max->addEdge(edgeBD);
	max->addEdge(edgeCD);

	std::vector<AscArcPtr> *v = m_ascArcsStorageForSimpl.seddles(max);
	if (v == NULL) {
		std::cout << "err can't find anything for max " << *max << std::endl;
		return;
	}

	AscArcPtr sm = NULL;
	std::cout << "ssdfsdfsd" << std::endl;
	for (size_t i = 0; i < v->size(); ++i) {
		std::cout << "sx " << v->at(i)->m_arcBegin->m_a->x << " sy " << v->at(i)->m_arcBegin->m_a->y << std::endl;

		if (v->at(i)->m_arcBegin->m_a->x == sx && v->at(i)->m_arcBegin->m_a->y == sy) {
			sm = v->at(i);
		}
	}
	if (!sm) {
		std::cout << "can't find asc arc for max(" << vx << ", " << vy << ") seddle_max " << sx << ", " << sy << std::endl;
		return;
	}

	removePersistentPair<AscArcPtr, FacePtr, FacePtr>(sm, m_ascArcsStorageForSimpl, true);

}

void GradientProcessorQuad::simplify(uint32_t persistence) {

	std::sort(m_maximums.begin(), m_maximums.end(), FacePtrComparator);
	simplifyArcs<AscArcPtr, FacePtr, VertexPtr, DescArcPtr>(m_maximums, m_ascArcsStorageForSimpl, m_descArcsStorageForSimpl, persistence,
			true);

	Vertexes mins;
	for (VertexesSet::iterator it = m_minimums.begin(); it != m_minimums.end(); ++it) {
		mins.push_back(*it);
	}
	std::sort(mins.begin(), mins.end(), vertexPtrComparator);
	simplifyArcs<DescArcPtr, VertexPtr, FacePtr, AscArcPtr>(mins, m_descArcsStorageForSimpl, m_ascArcsStorageForSimpl, persistence, false);

	std::cout << "\tsimplification done. persistance " << persistence << std::endl;

}

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

#include <vtkPolyVertex.h>

void GradientProcessorQuad::saveMaxInVtk() {

	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();

	for (size_t i = 0; i < m_maximums.size(); i++) {
		VertexPtr max = m_maximums[i]->maxVertex();
		vtkIdType id = points->InsertNextPoint(max->x, m_img.height() - max->y - 1, 0);
		vtkSmartPointer<vtkPolyVertex> polyVtx = vtkSmartPointer<vtkPolyVertex>::New();
		polyVtx->GetPointIds()->SetNumberOfIds(1);
		polyVtx->GetPointIds()->SetId(0, id);
		cells->InsertNextCell(polyVtx);
	}

	vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
	polyData->SetPoints(points);

	polyData->SetVerts(cells);
	vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
	writer->SetFileName("max.vtp");
	writer->SetInput(polyData);
	writer->SetDataModeToAscii();
	writer->Write();

}

void GradientProcessorQuad::saveMinInVtk() {

	vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();

	for (VertexesSet::iterator it = m_minimums.begin(); it != m_minimums.end(); it++) {
		VertexPtr max = *it;
		vtkIdType id = points->InsertNextPoint(max->x, m_img.height() - max->y - 1, 0);
		vtkSmartPointer<vtkPolyVertex> polyVtx = vtkSmartPointer<vtkPolyVertex>::New();
		polyVtx->GetPointIds()->SetNumberOfIds(1);
		polyVtx->GetPointIds()->SetId(0, id);
		cells->InsertNextCell(polyVtx);
	}

	vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
	polyData->SetPoints(points);

	polyData->SetVerts(cells);
	vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
	writer->SetFileName("min.vtp");
	writer->SetInput(polyData);
	writer->SetDataModeToAscii();
	writer->Write();

}

