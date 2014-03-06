#include "GradientProcessor.h"

#include "mt/utils/StrUtils.h"
#include "CofacedEdge.h"
#include "CofacedFace.h"
#include "opencv2/highgui/highgui_c.h"

#include "opencv2/highgui/highgui.hpp"

using cv::waitKey;

GradientProcessor::GradientProcessor() :
		m_persistence(0), m_seqId(0) {
}
GradientProcessor::~GradientProcessor() {
}

void GradientProcessor::drawComplexesOnOriginal() {

	if (m_outputFile.empty())
		return;

	m_msCmplxStorage.drawOriginal(m_img);

	m_img.saveAs(m_outputFile, true);
	std::cout << "save file in " << m_outputFile << std::endl;

}

VertexPtr GradientProcessor::findVertexByPixel(const Pixel & pxl) {
	std::map<Pixel, VertexPtr, PixelComparator>::iterator it = m_pix2vertex.find(pxl);
	if (it != m_pix2vertex.end())
		return it->second;
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


void GradientProcessor::normalizeField() {
	bool run;
	uint32_t counter = 0;
	do {
		run = false;
		for (int x = 0; x < m_img.width(); ++x) {
			for (int y = 0; y < m_img.height(); ++y) {
				Pixels pxls = m_img.getAllAround(Pixel(x, y));
				for (size_t i = 0; i < pxls.size(); i++) {
					if (m_img.value(pxls[i]) == m_img.value(Pixel(x, y))) {
						m_img.setValue(Pixel(x, y), m_img.value(Pixel(x, y)) + 1);
						run = true;
						if (counter > 10)
							return;
					}
				}
			}
		}
		counter++;

	} while (run);
	std::cout << "\nnormalized by " << counter << " tries" << std::endl;

}

void GradientProcessor::addVertex(VertexPtr vtx) {

	vtx->m_seqId = m_seqId++;
	m_vertexes.push(vtx);
	m_pix2vertex[*(dynamic_cast<Pixel*>(vtx))] = vtx;
	m_simplexRelations.push(vtx);

}

void GradientProcessor::addEdge(EdgePtr face) {

	if ((face->m_a->value()) == (face->m_b->value()))
		return;
	face->m_seqId = m_seqId++;
	m_edges.push(face);
	m_simplexRelations.push(face);
}

void GradientProcessor::addFace(FacePtr tr) {
	tr->m_seqId = m_seqId++;
	m_faces.push(tr);
	m_simplexRelations.push(tr);
}

bool GradientProcessor::loadFitsData(const std::string & path) {

	m_img.initFits(path);
	std::cout << "load image: " << m_img.width() << "X" << m_img.height() << std::endl;
	return true;
}

bool GradientProcessor::loadImageData(const std::string & path) {
	m_img.init(path);
	std::cout << "load image: " << m_img.width() << "X" << m_img.height() << std::endl;
	return true;
}

void GradientProcessor::findMinimums() {

	m_vertexes.sort();
	Vertexes &vtxs = m_vertexes.vector();
	for (size_t i = vtxs.size() - 1; i > 0; i--) {
		//std::cout << "val: " << vtxs[i]->value() << std::endl;

		EdgePtr smallestFace = NULL;
		VertexPtr neibVtx = vtxs.at(i);
		Edges *faces = m_simplexRelations.edges(neibVtx);
		for (size_t j = 0; j < faces->size(); ++j) {
			EdgePtr neibFace = faces->at(j);
			if (m_processedSmplexes.find(neibFace->m_seqId) == m_processedSmplexes.end()) {
				if (neibFace->m_valueFirst == neibVtx->m_valueFirst) {
					if (smallestFace == NULL)
						smallestFace = neibFace;
					else if (smallestFace->m_valueSecond > neibFace->m_valueSecond)
						smallestFace = neibFace;
				}
			}
		}

		if (smallestFace == NULL) {
			m_minimums.insert(neibVtx);
		} else {
			m_processedSmplexes.insert(smallestFace->m_seqId);
			m_processedSmplexes.insert(neibVtx->m_seqId);
			m_cofacesSimplexes.push_back((CofacePtr) new CofacedEdge(neibVtx, smallestFace));
			m_edgeVertexGradientPair[neibVtx] = smallestFace;
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

void GradientProcessor::drawGradientField() {
	if (m_gradFieldFile.empty())
		return;
	Mat drawField(m_img.height() * Image::m_enlargeFactor + Image::m_enlargeFactor * 2,
			m_img.width() * Image::m_enlargeFactor + 2 * Image::m_enlargeFactor, CV_8UC3, Scalar(255, 255, 255));

	for (int i = 0; i < m_img.height(); ++i) {
		putText(drawField, mt::StrUtils::intToString(i), Point(2, i * Image::m_enlargeFactor + Image::m_enlargeFactor), CV_FONT_NORMAL, 0.7,
				Scalar(0, 0, 0), 1.3);
	}
	for (int i = 0; i < m_img.width(); ++i) {
		putText(drawField, mt::StrUtils::intToString(i), Point(i * Image::m_enlargeFactor + Image::m_enlargeFactor, 20), CV_FONT_NORMAL,
				0.7, Scalar(0, 0, 0), 1.3);
	}

	m_msCmplxStorage.draw(drawField);

	Edges& edges = m_edges.vector();
	for (size_t i = 0; i < edges.size(); i++) {
		edges[i]->draw(drawField);
	}

	/*
	 for (size_t i = 0; i < m_maximums.size(); i++) {
	 circle(
	 drawField,
	 Point(m_maximums[i]->maxVertex()->x * Image::m_enlargeFactor + Image::m_enlargeFactor,
	 m_maximums[i]->maxVertex()->y * Image::m_enlargeFactor + Image::m_enlargeFactor), 6,
	 Scalar(0, 0, 255), 5);
	 }
	 */

	/*for (typename VertexesSet::iterator it = m_minimums.begin(); it != m_minimums.end(); ++it) {
	 circle(drawField,
	 Point((*it)->x * Image::m_enlargeFactor + Image::m_enlargeFactor, (*it)->y * Image::m_enlargeFactor + Image::m_enlargeFactor), 6,
	 Scalar(255, 255, 0), 5);
	 }
	 */

	/*
	 for (size_t i = 0; i < m_seddles.size(); ++i) {
	 m_seddles[i]->draw(drawField, 3, Scalar(0, 255, 0));
	 }
	 */

	for (int i = 0; i < m_cofacesSimplexes.size(); ++i) {
		m_cofacesSimplexes[i]->draw(drawField);
	}

	for (size_t i = 0; i < m_vertexes.vector().size(); i++) {
		m_vertexes.vector().at(i)->draw(drawField);
	}

	imshow(m_img.imageName().c_str(), drawField);
	imwrite(m_gradFieldFile.c_str(), drawField);
	waitKey(0);
}

bool GradientProcessor::getAscendingManifold(std::vector<FacePtr>& arc) {

	if (arc.empty())
		return false;

	FacePtr triangle = arc.back();

	EdgePtr gradPair = getGradientPair(triangle);
	if (gradPair == NULL)
		return true;

	std::vector<FacePtr> *cofacets = m_simplexRelations.face(gradPair);
	if (cofacets == NULL) {
		std::cout << "can't find grad pair  " << std::endl;
		return false;
	}
	bool retVal = false;

	for (size_t i = 0; i < cofacets->size(); ++i) {
		if (*(cofacets->at(i)) != *triangle) {
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

	EdgePtr gradPair = getGradientPair(vtx);
	if (gradPair == NULL)
		return;

	VertexPtr nextVtx = gradPair->m_a;
	if (*vtx == *(gradPair->m_a))
		nextVtx = gradPair->m_b;

	arc.push_back(gradPair);
	vtxs.push_back(nextVtx);
	getDescendingManifold(arc, vtxs);
}
