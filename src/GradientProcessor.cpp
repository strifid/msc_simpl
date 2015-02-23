#include "GradientProcessor.h"

#include <mt/utils/StrUtils.h>
#include <opencv2/core/types_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>

#include "CofacedEdge.h"
#include "CofacedFace.h"
#include "MsComplex.h"
#include "Utils.h"

using cv::waitKey;

GradientProcessor::GradientProcessor() :
		m_persistence(0), m_seqId(0) {
}
GradientProcessor::~GradientProcessor() {
}

VertexPtr GradientProcessor::findVertexByPixel(uint32_t x, uint32_t y) {
	Vertex vtx(x, y);
	std::set<VertexPtr, VertexPtrComparator>::iterator it = m_pix2vertex.find(&vtx);
	if (it != m_pix2vertex.end())
		return *it;
	std::cout << "ERROR: can't find vertex by pixel " << x << " " << y << std::endl;
	return NULL;
}

void GradientProcessor::findMaximums() {
	std::vector<FacePtr>& quads = m_faces.vector();
	for (size_t i = 0; i < quads.size(); i++) {
		if (m_processedSmplexes.find(quads[i]->m_seqId) == m_processedSmplexes.end()) {
			m_maximums.push_back(quads[i]);
		}
	}
}

void GradientProcessor::addVertex(VertexPtr vtx) {

	vtx->m_seqId = m_seqId++;
	m_vertexes.push(vtx);
	m_pix2vertex.insert(vtx);
	m_simplexRelations.push(vtx);

}

void GradientProcessor::addEdge(VertexPtr a, VertexPtr b) {
	if (a == NULL || b == NULL) {
		std::cout << "ERROR. can't create edge with NULL virtex" << std::endl;
		return;
	}

	EdgePtr edge = new Edge(a, b);
	edge->m_seqId = m_seqId++;
	m_edges.push(edge);
	m_simplexRelations.push(edge);
}

void GradientProcessor::addFace(VertexPtr a, VertexPtr b, VertexPtr c, VertexPtr d) {
	EdgePtr edgeAB = Utils::getEdgeByVertex(m_edges, a, b);
	EdgePtr edgeAC = Utils::getEdgeByVertex(m_edges, a, c);
	EdgePtr edgeBD = Utils::getEdgeByVertex(m_edges, b, d);
	EdgePtr edgeCD = Utils::getEdgeByVertex(m_edges, c, d);

	if (edgeAB == NULL || edgeAC == NULL || edgeBD == NULL || edgeCD == NULL) {
		std::cout << "ERROR: can't create face with NULL edge" << std::endl;
		return;
	}

	FacePtr face = new Face();
	face->addEdge(edgeAB);
	face->addEdge(edgeAC);
	face->addEdge(edgeBD);
	face->addEdge(edgeCD);

	face->m_seqId = m_seqId++;
	m_faces.push(face);
	m_simplexRelations.push(face);
}

bool GradientProcessor::loadData(const std::string & path, DataType type) {

	switch (type) {
	case FITS:
		m_img.initFits(path);
		break;
	case IMAGE:
		m_img.init(path);
		break;
	}
	std::cout << "load image: " << m_img.width() << "X" << m_img.height() << std::endl;
	return true;
}

void GradientProcessor::findMinimums() {

	m_vertexes.sort();
	Vertexes &vtxs = m_vertexes.vector();
	for (size_t i = 0; i < vtxs.size(); i++) {
		EdgePtr smallestEdge = NULL;
		VertexPtr neibVtx = vtxs.at(i);
		Edges *edges = m_simplexRelations.edges(neibVtx);
		if (edges->size() != 4)
			std::cout << "ERROR. not 4 faced edges for vertex " << *neibVtx << std::endl;
		for (size_t j = 0; j < edges->size(); ++j) {

			EdgePtr neibEdge = edges->at(j);
			if (m_processedSmplexes.find(neibEdge->m_seqId) == m_processedSmplexes.end()) {
				if (neibEdge->m_valueFirst == neibVtx->m_valueFirst) {
					if (smallestEdge == NULL)
						smallestEdge = neibEdge;
					else if (smallestEdge->m_valueSecond > neibEdge->m_valueSecond)
						smallestEdge = neibEdge;
				}
			}
		}

		if (smallestEdge == NULL) {
			m_minimums.insert(neibVtx);
		} else {
			m_processedSmplexes.insert(smallestEdge->m_seqId);
			m_processedSmplexes.insert(neibVtx->m_seqId);
			m_cofacesSimplexes.push_back((CofacePtr) new CofacedEdge(neibVtx, smallestEdge));
			m_edgeVertexGradientPair[neibVtx] = smallestEdge;
		}

	}
}

void GradientProcessor::findSeddles() {
	m_edges.sort();

	for (size_t i = 0; i < m_edges.vector().size(); i++) {
		FacePtr smallestTr = NULL;
		EdgePtr edge = m_edges.vector()[i];
		std::vector<FacePtr> *faces = m_simplexRelations.face(edge);

		if (m_processedSmplexes.find(edge->m_seqId) == m_processedSmplexes.end()) {
			for (size_t i = 0; i < faces->size(); ++i) {
				FacePtr neibFace = faces->at(i);
				if (m_processedSmplexes.find(neibFace->m_seqId) == m_processedSmplexes.end()) {
					if (neibFace->m_valueFirst == edge->m_valueFirst) {
						if (smallestTr == NULL)
							smallestTr = neibFace;
						else if (smallestTr->m_valueSecond == neibFace->m_valueSecond) {
							if (smallestTr->m_valueThird > neibFace->m_valueThird)
								smallestTr = neibFace;
						} else if (smallestTr->m_valueSecond > neibFace->m_valueSecond)
							smallestTr = neibFace;
					}
				}
			}

			if (smallestTr == NULL) {
				m_seddles.push_back(edge);
			} else {
				m_processedSmplexes.insert(smallestTr->m_seqId);
				m_processedSmplexes.insert(edge->m_seqId);
				CofacedFace* cc = new CofacedFace(edge, smallestTr);
				m_cofacesSimplexes.push_back((CofacePtr) cc);
				m_edgeFaceGradientPair[smallestTr] = edge;
			}
		}
	}
}
#include <persist_pair/PPoint.h>
#include <Utils.h>

void GradientProcessor::drawCmplx(const std::string& path, Drawer* drawer, bool show) {

	Mat drawField(m_img.height() * Image::m_enlargeFactor + Image::m_enlargeFactor * 2,
			m_img.width() * Image::m_enlargeFactor + 2 * Image::m_enlargeFactor, CV_8UC3, Scalar(255, 255, 255));

	for (int i = 0; i < m_img.height() + 1; ++i) {
		putText(drawField, Utils::intToString(i), Point(2, i * Image::m_enlargeFactor + Image::m_enlargeFactor), CV_FONT_NORMAL, 0.7, Scalar(0, 0, 0),
				1.3);
	}
	for (int i = 0; i < m_img.width() + 1; ++i) {
		putText(drawField, Utils::intToString(i), Point(i * Image::m_enlargeFactor + Image::m_enlargeFactor, 20), CV_FONT_NORMAL, 0.7,
				Scalar(0, 0, 0), 1.3);
	}

	drawer->draw(drawField);
	//		m_msCmplxStorage.draw(drawField);
	Edges& edges = m_edges.vector();
	for (size_t i = 0; i < edges.size(); i++) {
		if ((edges[i]->m_a->x == m_img.width() - 1 || edges[i]->m_b->x == m_img.width() - 1) && (edges[i]->m_a->x == 0 || edges[i]->m_b->x == 0)) {
			Point a((m_img.width()) * Image::m_enlargeFactor, (edges[i]->m_a->y + 1) * Image::m_enlargeFactor);
			Point b((m_img.width() + 1) * Image::m_enlargeFactor, (edges[i]->m_b->y + 1) * Image::m_enlargeFactor);
			line(drawField, a, b, Scalar(0, 0, 0), 1, 8);
		} else if ((edges[i]->m_a->y == m_img.height() - 1 || edges[i]->m_b->y == m_img.height() - 1)
				&& (edges[i]->m_a->y == 0 || edges[i]->m_b->y == 0)) {
			Point a((edges[i]->m_a->x + 1) * Image::m_enlargeFactor, (m_img.height()) * Image::m_enlargeFactor);
			Point b((edges[i]->m_b->x + 1) * Image::m_enlargeFactor, (m_img.height() + 1) * Image::m_enlargeFactor);
			line(drawField, a, b, Scalar(0, 0, 0), 1, 8);
		} else
			edges[i]->draw(drawField);
	}
	for (int i = 0; i < m_cofacesSimplexes.size(); ++i) {
		m_cofacesSimplexes[i]->draw(drawField);
	}

	for (size_t i = 0; i < m_vertexes.vector().size(); i++) {
		m_vertexes.vector().at(i)->draw(drawField);
	}

//	if (show) {
	imshow(path.c_str(), drawField);
	waitKey(0);
//	}
	/*
	 VtxDimToPPidMap::iterator it = PPoint::m_vrtx2Id.begin();
	 while (it != PPoint::m_vrtx2Id.end()) {
	 putText(drawField, mt::StrUtils::intToString(it->second),
	 Point(it->first.first->x * Image::m_enlargeFactor + Image::m_enlargeFactor, it->first.first->y * Image::m_enlargeFactor + Image::m_enlargeFactor),
	 CV_FONT_NORMAL, 0.7, Scalar(0, 0, 0), 1.3);
	 it++;
	 }*/

	imwrite(path.c_str(), drawField);
}

void GradientProcessor::drawCmplxOnTor(const std::string& path, AscArcStorage &aStorage, DescArcStorage &dStorage, bool show) {

	uint32_t torH = m_img.height();
	uint32_t torW = m_img.width();

	Mat drawField(torH * Image::m_enlargeFactor + Image::m_enlargeFactor * 2, torW * Image::m_enlargeFactor + 2 * Image::m_enlargeFactor, CV_8UC3,
			Scalar(255, 255, 255));

	for (int i = 0; i < torH + 1; ++i) {
		putText(drawField, Utils::intToString(i), Point(2, i * Image::m_enlargeFactor + Image::m_enlargeFactor), CV_FONT_NORMAL, 0.7, Scalar(0, 0, 0),
				1.3);
	}
	for (int i = 0; i < torW + 1; ++i) {
		putText(drawField, Utils::intToString(i), Point(i * Image::m_enlargeFactor + Image::m_enlargeFactor, 20), CV_FONT_NORMAL, 0.7,
				Scalar(0, 0, 0), 1.3);
	}

	Utils::drawAscArcStorage(drawField, aStorage);
	Utils::drawDescArcStorage(drawField, dStorage);

	//		m_msCmplxStorage.draw(drawField);
	Edges& edges = m_edges.vector();
	for (size_t i = 0; i < edges.size(); i++) {
		if ((edges[i]->m_a->x == torW - 1 || edges[i]->m_b->x == torW - 1) && (edges[i]->m_a->x == 0 || edges[i]->m_b->x == 0)) {
			Point a((torW) * Image::m_enlargeFactor, (edges[i]->m_a->y + 1) * Image::m_enlargeFactor);
			Point b((torW + 1) * Image::m_enlargeFactor, (edges[i]->m_b->y + 1) * Image::m_enlargeFactor);
			line(drawField, a, b, Scalar(0, 0, 0), 1, 8);
		} else if ((edges[i]->m_a->y == torH - 1 || edges[i]->m_b->y == torH - 1) && (edges[i]->m_a->y == 0 || edges[i]->m_b->y == 0)) {
			Point a((edges[i]->m_a->x + 1) * Image::m_enlargeFactor, (torH) * Image::m_enlargeFactor);
			Point b((edges[i]->m_b->x + 1) * Image::m_enlargeFactor, (torH + 1) * Image::m_enlargeFactor);
			line(drawField, a, b, Scalar(0, 0, 0), 1, 8);
		} else
			edges[i]->draw(drawField);
	}
	for (int i = 0; i < m_cofacesSimplexes.size(); ++i) {
		m_cofacesSimplexes[i]->draw(drawField);
	}

	for (size_t i = 0; i < m_vertexes.vector().size(); i++) {
		m_vertexes.vector().at(i)->draw(drawField);
	}

	show = true;
	if (show) {
		imshow(path.c_str(), drawField);
		waitKey(0);
	}
	/*
	 VtxDimToPPidMap::iterator it = PPoint::m_vrtx2Id.begin();
	 while (it != PPoint::m_vrtx2Id.end()) {
	 putText(drawField, mt::StrUtils::intToString(it->second),
	 Point(it->first.first->x * Image::m_enlargeFactor + Image::m_enlargeFactor, it->first.first->y * Image::m_enlargeFactor + Image::m_enlargeFactor),
	 CV_FONT_NORMAL, 0.7, Scalar(0, 0, 0), 1.3);
	 it++;
	 }*/

	imwrite(path.c_str(), drawField);
}

bool GradientProcessor::getAscendingManifold(std::vector<FacePtr>& arc) {

	if (arc.empty())
		return false;

	FacePtr faceMax = arc.back();
	for (size_t i = 0; i < arc.size() - 1; ++i) {
		if (!(*(arc.at(i)) != *faceMax)) {
			std::cout << "warning: find loop in ascending manifolds. loop point: " << *faceMax << std::endl;
			return false;
		}
	}

	EdgePtr gradPair = getGradientPair(faceMax);
	if (gradPair == NULL)
		return true;

	std::vector<FacePtr> *cofacets = m_simplexRelations.face(gradPair);
	if (cofacets == NULL) {
		std::cout << "can't find grad pair  " << std::endl;
		return false;
	}
	bool retVal = false;

	for (size_t i = 0; i < cofacets->size(); ++i) {
		if (*(cofacets->at(i)) != *faceMax) {
			arc.push_back(cofacets->at(i));
			retVal = getAscendingManifold(arc);
		}
	}
	return retVal;
}
void GradientProcessor::getDescendingManifold(Edges& arc, Vertexes& vtxs) {

	if (vtxs.empty())
		return;

	VertexPtr vtx = vtxs.back();

	for (size_t i = 0; i < vtxs.size() - 1; ++i) {
		if (*(vtxs.at(i)) == *vtx) {
			std::cout << "warning: find loop in descending manifolds. loop point: " << *vtx << std::endl;
			return;
		}
	}

	EdgePtr gradPair = getGradientPair(vtx);
	if (gradPair == NULL) {
		return;
	}

	if (gradPair->m_a == NULL || gradPair->m_b == NULL) {
		std::cout << "ERROR: grad pair vtx NULL" << std::endl;
		return;
	}

	VertexPtr nextVtx = gradPair->m_a;
	if (*vtx == *(gradPair->m_a))
		nextVtx = gradPair->m_b;

	arc.push_back(gradPair);
	vtxs.push_back(nextVtx);
	getDescendingManifold(arc, vtxs);

}

