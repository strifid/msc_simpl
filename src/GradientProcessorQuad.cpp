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

//#include "VtpWriter.h"

GradientProcessorQuad::GradientProcessorQuad() {
}

GradientProcessorQuad::~GradientProcessorQuad() {
}

int32_t GradientProcessorQuad::findSimplexes() {

	uint32_t torW = m_img.width();
	uint32_t torH = m_img.height();

	for (int x = 0; x < m_img.width(); x++) {
		for (int y = 0; y < m_img.height(); y++) {
			VertexPtr vtx = new Vertex(x, y);
			vtx->value(m_img.value(x, y), x * (m_img.width() * 2 - 2) + y);
			addVertex(vtx);
		}
	}

	for (int x = 0; x < torW; x++) {
		for (int y = 0; y < torH; y++) {
			VertexPtr a = findVertexByPixel(x, y);
			VertexPtr b = findVertexByPixel(x + 1 == torW ? 0 : x + 1, y);
			VertexPtr c = findVertexByPixel(x, y + 1 == torH ? 0 : y + 1);
			addEdge(a, b);
			addEdge(a, c);
		}
	}
	for (int x = 0; x < torW; x++) {
		for (int y = 0; y < torH; y++) {
			VertexPtr a = findVertexByPixel(x, y);
			VertexPtr b = findVertexByPixel(x + 1 == torW ? 0 : x + 1, y);
			VertexPtr c = findVertexByPixel(x, y + 1 == torH ? 0 : y + 1);
			VertexPtr d = findVertexByPixel(x + 1 == torW ? 0 : x + 1, y + 1 == torH ? 0 : y + 1);
			addFace(a, b, c, d);
		}
	}

	return 0;
}

void GradientProcessorQuad::addDescArc(VertexPtr vtx, EdgePtr seddle) {
	Vertexes vtxs;
	Edges edges;
	vtxs.push_back(vtx);
	edges.push_back(seddle);

	getDescendingManifold(edges, vtxs);

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
		if (tr == NULL || tr->size() != 2) {
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

bool GradientProcessorQuad::simplifyPpairs(std::vector<std::pair<PPointPtr, PPointPtr> >& pps) {

	for (size_t i = 0; i < pps.size(); ++i) {
		if (pps[i].first->m_smplx == NULL || pps[i].second->m_smplx == NULL) {
			std::cout << "ERROR: smplx in ppair is NULL" << std::endl;
			exit(0);
		}

		FacePtr face = NULL;
		EdgePtr edge = NULL;
		VertexPtr vtx = NULL;

		if (Utils::ppairToSimplex(pps[i], &vtx, &edge, &face)) {
			AscArcPtr arc = m_ascArcsStorage.getArc(edge, face);
			if (removePersistentPair<AscArcPtr, FacePtr>(arc, m_ascArcsStorage, true)) {
				m_descArcsStorage.erase(arc->m_arcBegin);
				continue;
			} else
				std::cout << "can't remove AscArc: " << *(face) << " seddle " << *edge << std::endl;
		} else {
			DescArcPtr arc = m_descArcsStorage.getArc(edge, vtx);
			if (removePersistentPair<DescArcPtr, VertexPtr>(arc, m_descArcsStorage, false)) {
				m_ascArcsStorage.erase(arc->m_arcBegin);
				continue;
			} else
				std::cout << "can't remove DescArc: " << *vtx << " seddle " << *edge << std::endl;
		}
	}
	return false;
}

void GradientProcessorQuad::run() {

	findSimplexes();

	findMinimums();
	findSeddles();
	findMaximums();

	findAscArcs();
	findDescArcs();

	printInfo();

	/*
	 connectArcs(m_ascArcsStorage, m_descArcsStorage);
	 std::cout << "cmplx: " << m_msCmplxStorage.complexesSet().size() << std::endl;
	 */

	/*
	 Edge edge(findVertexByPixel(0, 28), findVertexByPixel(0, 27));
	 std::vector<AscArcPtr> *ret = m_ascArcsStorage.critical(&edge);
	 std::cout << "has " << ret->size() << std::endl;

	 for (size_t i = 0; i < ret->size(); i++) {
	 std::cout << "size " << ret->at(i)->m_arc.size() << ". start " << *(ret->at(i)->m_arcBegin) << " end with " << *(ret->at(i)->m_arcEnd)
	 << std::endl;
	 }
	 */

	size_t iii = 0;

	uint32_t ppsSize = 0;
	do {

		iii = ppsSize;
		PersistPairProcessor ppProc;
		ppProc.init(m_ascArcsStorage, m_descArcsStorage);

		ppProc.findPairs();
		std::vector<std::pair<PPointPtr, PPointPtr> >& pps = ppProc.filter(m_persistence);
		ppsSize = pps.size();
		simplifyPpairs(pps);

	} while (iii != ppsSize);

	drawComplexesOnOriginal();
	drawGradientField();

	/*
	 BarCodeProcessor proc;
	 proc.init(m_ascArcsStorage, m_descArcsStorage);
	 proc.computeBarCodes(m_gradFieldFile);
	 */

}

void GradientProcessorQuad::drawGradientField() {
	if (m_gradFieldFile.empty())
		return;

	drawCmplxOnTor(m_gradFieldFile + "_all.jpg", m_ascArcsStorage, m_descArcsStorage, false);

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

void GradientProcessorQuad::drawComplexesOnOriginal() {

	if (m_outputFile.empty())
		return;

	Utils::drawAscArcStorageOrig(m_img, m_ascArcsStorage);
	Utils::drawDescArcStorageOrig(m_img, m_descArcsStorage);
	m_img.saveAs(m_outputFile, true);
	std::cout << "save file in " << m_outputFile << std::endl;

}
