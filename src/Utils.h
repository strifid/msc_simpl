/*
 * Utils.h
 *
 *  Created on: Jul 21, 2013
 *      Author: trifid
 */

#ifndef MSUTILS_H_
#define MSUTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string>
#include <vector>

#include "Edge.h"
#include "SimplexStorage.h"
#include "Vertex.h"
#include "opencv2/core/core.hpp"

#include "Arc.h"
#include "ArcStorage.h"
#include "Image.h"

using cv::Mat;
class PPoint;
class Utils {
public:
	Utils();
	virtual ~Utils();

	template<typename T>
	static bool hasInVectorPtr(std::vector<T> v, T o) {
		for (size_t i = 0; i < v.size(); ++i) {
			if (*(v[i]) == *o)
				return true;
		}
		return false;
	}

	static std::string intToString(int32_t i);

	static EdgePtr getEdgeByVertex(SimplexStorage<EdgePtr>& edges, VertexPtr a, VertexPtr b);
	static void drawAscArc(Mat& img, AscArcPtr ascArc);
	static void drawAscArcStorage(Mat& img, AscArcStorage &ascArcStorage);
	static void drawDescArcStorage(Mat& img, DescArcStorage &descArcStorage);
	static bool ppairToSimplex(std::pair<PPoint*, PPoint*>& ppair, Vertex**, Edge**, Face**);

	static void drawAscArcStorageOrig(Image& img, AscArcStorage &ascArcStorage);
	static void drawDescArcStorageOrig(Image& img, DescArcStorage &descArcStorage);

	static uint32_t countMax(Image& img, AscArcStorage &ascArcStorage);
	static uint32_t  countMin(Image& img, DescArcStorage &descArcStorage);
	template <typename ArcStorageType>
	static uint32_t countSeddls(Image& img, ArcStorageType &arcStorage) {
		typename ArcStorageType::ArcsToSeddleMap::iterator it = arcStorage.m_arcsToSeddleMap.begin(), itEnd = arcStorage.m_arcsToSeddleMap.end();
		uint32_t seddle = 0;
		for (; it != itEnd; ++it){
			VertexPtr maxVtx = it->first->maxVertex();
			if(maxVtx->x < img.m_originalWidth && maxVtx->y < img.m_originalHeight ) {
				++seddle;
			}
		}
		return seddle;
	}

};

#endif /* UTILS_H_ */
