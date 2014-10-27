/*
 * GradientProcessor.h
 *
 *  Created on: Nov 15, 2012
 *      Author: trifid
 */

#ifndef GRADIENTPROCESSOR_H_
#define GRADIENTPROCESSOR_H_

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "Coface.h"
#include "Edge.h"
#include "Image.h"
#include "MsComplexStorage.h"
#include "Pixel.h"
#include "SimplexRelations.h"
#include "SimplexStorage.h"
#include "Triangle.h"
#include "Vertex.h"
#include "opencv2/core/core.hpp"

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
	bool loadFitsData(const std::string& path);

	GradientProcessor();
	virtual ~GradientProcessor();

	int32_t findVertex() {
		for (int x = 0; x < m_img.width(); x++) {
			for (int y = 0; y < m_img.height(); y++) {
				VertexPtr vtx = new Vertex(x, y);
				vtx->value(m_img.value(Pixel(x, y)), x * m_img.width() + y);

				addVertex(vtx);
			}
		}

		return 0;
	}

	virtual int32_t findEdges() = 0;
	virtual int32_t findFaces() = 0;

	std::string m_gradFieldFile;
	std::string m_outputFile;
	uint32_t m_persistence;

protected:

	void findMaximums();
	void findSeddles();
	void findMinimums();

	void addFace(FacePtr tr);
	void addVertex(VertexPtr vtx);
	void addEdge(EdgePtr face);
	VertexPtr findVertexByPixel(const Pixel & pxl);

	void drawGradientField();

	void drawComplexesOnOriginal();
	void normalizeField();

	void getDescendingManifold(Edges& arc, Vertexes& vtxs);
	bool getAscendingManifold(std::vector<FacePtr>& arc);
	EdgePtr getGradientPair(FacePtr tr) {
		typename std::map<FacePtr, EdgePtr, SimplexComparator<FacePtr> >::iterator it = m_edgeFaceGradientPair.find(tr);
		if (it == m_edgeFaceGradientPair.end())
			return NULL;
		return it->second;
	}

	EdgePtr getGradientPair(VertexPtr face) {
		std::map<VertexPtr, EdgePtr>::iterator it = m_edgeVertexGradientPair.find(face);
		if (it == m_edgeVertexGradientPair.end())
			return NULL;
		return it->second;
	}

	Image m_img;
//	std::set<Pixel, PixelComparator> m_processedPixels;
	std::set<VertexPtr, VertexPtrComparator> m_pix2vertex;

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

	std::map<FacePtr, EdgePtr, SimplexComparator<FacePtr> > m_edgeFaceGradientPair;

	std::map<VertexPtr, EdgePtr> m_edgeVertexGradientPair;

	Faces m_ascending;
	Edges m_descending;
	Vertexes m_descendingVtxs;

	MsComplexStorage m_msCmplxStorage;

	void drawCmplx(const std::string& path, Drawer* drawer, bool show = false);
};

#endif /* GRADIENTPROCESSOR_H_ */
