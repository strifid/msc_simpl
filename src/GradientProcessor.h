/*
 * GradientProcessor.h
 *
 *  Created on: Nov 15, 2012
 *      Author: trifid
 */

#ifndef GRADIENTPROCESSOR_H_
#define GRADIENTPROCESSOR_H_

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
#include "CofacedTriangle.h"
#include "ArcStorage.h"
#include "MsComplex.h"
#include "CofacedFace.h"
#include "opencv2/highgui/highgui.hpp"
#include <iosfwd>
#include <iosfwd>
#include <iostream>
#include <fstream>
#include "mt/utils/StrUtils.h"

using cv::imread;
using cv::Scalar;
using cv::waitKey;
using cv::imwrite;

using cv::Mat;

class VertexComponets {
public:
	Edges m_faces;
	Faces m_triangles;
};
typedef VertexComponets* VertexComponetsPtr;

class GradientProcessor: public mt::Logable {
public:

	bool loadImageData(const std::string& path);
	GradientProcessor() :
			m_seqId(0) {
	}
	virtual ~GradientProcessor() {
	}

	int32_t findVertex() {
		for (int x = 0; x < m_img.width(); x++) {
			for (int y = 0; y < m_img.height(); y++) {
				VertexPtr vtx = new Vertex(x, y);
				vtx->value(m_img.value(Pixel(x, y)));
				addVertex(vtx);
			}
		}

		return 0;
	}

	virtual int32_t findEdges() = 0;
	virtual int32_t findFaces() = 0;

	void run();

	std::string m_gradFieldFile;
	std::string m_outputFile;
	uint32_t m_persistence;

protected:

	void findMaximums() {
		std::vector<FacePtr>& quads = m_faces.vector();
		for (size_t i = 0; i < quads.size(); i++) {
			if (m_processedSmplexes.find(quads[i]->m_seqId) == m_processedSmplexes.end()) {
				m_maximums.push_back(quads[i]);
			}
		}
	}

	void findSeddles();
	void findMinimums();

	void addFace(FacePtr tr);
	void addVertex(VertexPtr vtx);
	void addEdge(EdgePtr face);
	VertexPtr findVertexByPixel(const Pixel & pxl) {
		std::map<Pixel, VertexPtr, PixelComparator>::iterator it = m_pix2vertex.find(pxl);
		if (it != m_pix2vertex.end())
			return it->second;
		return NULL;
	}


	void drawGradientField();

	void drawComplexesOnOriginal() {

		if (m_outputFile.empty())
			return;

		m_msCmplxStorage.drawOriginal(m_img);

		m_img.saveAs(m_outputFile, true);
		std::cout << "save file in " << m_outputFile << std::endl;

	}

	Image m_img;
	std::set<Pixel, PixelComparator> m_processedPixels;
	std::map<Pixel, VertexPtr, PixelComparator> m_pix2vertex;

	SimplexStorage<VertexPtr> m_vertexes;
	SimplexStorage<EdgePtr> m_edges;
	SimplexStorage<FacePtr> m_faces;

	SimplexRelations m_simplexRelations;

	std::set<uint32_t> m_processedSmplexes;

	VertexesSet m_minimums;
	Edges m_seddles;
	std::vector<FacePtr> m_maximums;
	Cofaces m_cofacesSimplexes;

	uint32_t m_seqId;
	void normalizeField();

	void getDescendingManifold(Edges& arc, Vertexes& vtxs);
	bool getAscendingManifold(std::vector<FacePtr>& arc);

	std::map<FacePtr, EdgePtr, SimplexComparator<FacePtr> > m_edgeFaceGradientPair;
	EdgePtr getGradientPair(FacePtr tr) {
		typename std::map<FacePtr, EdgePtr, SimplexComparator<FacePtr> >::iterator it = m_edgeFaceGradientPair.find(tr);
		if (it == m_edgeFaceGradientPair.end())
			return NULL;
		return it->second;
	}

	std::map<VertexPtr, EdgePtr> m_edgeVertexGradientPair;
	EdgePtr getGradientPair(VertexPtr face) {
		std::map<VertexPtr, EdgePtr>::iterator it = m_edgeVertexGradientPair.find(face);
		if (it == m_edgeVertexGradientPair.end())
			return NULL;
		return it->second;
	}

	Faces m_ascending;
	Edges m_descending;
	Vertexes m_descendingVtxs;

	MsComplexStorage m_msCmplxStorage;
};

#endif /* GRADIENTPROCESSOR_H_ */
