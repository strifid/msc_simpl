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
#include "SimplexRelations.h"
#include "SimplexStorage.h"
#include "Triangle.h"
#include "Vertex.h"
#include "opencv2/core/core.hpp"
#include "ArcStorage.h"
using cv::Mat;

class VertexComponets {
public:
	Edges m_faces;
	Faces m_triangles;
};
typedef VertexComponets* VertexComponetsPtr;

class GradientProcessor {
public:

	enum DataType{
		FITS, IMAGE
	};

	bool loadData(const std::string& path, DataType type);

	GradientProcessor();
	virtual ~GradientProcessor();

	std::string m_gradFieldFile;
	std::string m_outputFile;
	uint32_t m_persistence;

protected:

	void findMaximums();
	void findSeddles();
	void findMinimums();

	void addFace(VertexPtr a, VertexPtr b, VertexPtr c, VertexPtr d);
	void addVertex(VertexPtr vtx);
	void addEdge(VertexPtr a, VertexPtr b);
	VertexPtr findVertexByPixel(uint32_t x, uint32_t y);

	void drawCmplxOnTor(const std::string& path, AscArcStorage &aStorage, DescArcStorage &dStorage, bool show);


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
