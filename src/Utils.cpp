/*
 * Utils.cpp
 *
 *  Created on: Jul 21, 2013
 *      Author: trifid
 */

#include "Utils.h"
#include "Image.h"
Utils::Utils() {
	// TODO Auto-generated constructor stub

}

Utils::~Utils() {
	// TODO Auto-generated destructor stub
}

std::string Utils::intToString(int32_t i) {
	char buf[16];
	sprintf(buf, "%d", i);
	return buf;
}

EdgePtr Utils::getEdgeByVertex(SimplexStorage<EdgePtr>& edges, VertexPtr a, VertexPtr b) {
	if (a == NULL || b == NULL)
		return NULL;
	Edge tmpEdgeAB(a, b);
	return edges.getSimplex(&tmpEdgeAB);
}

void checkBound(Point&a, Point& b) {
	if (a.x < Image::m_enlargeFactor * 2 && b.x > Image::m_enlargeFactor * 4) {
		a.x = (Image::m_width + 1.5) * Image::m_enlargeFactor;
	}
	if (b.x < Image::m_enlargeFactor * 2 && a.x > Image::m_enlargeFactor * 4) {
		b.x = (Image::m_width + 1.5) * Image::m_enlargeFactor;
	}

	if (a.y < Image::m_enlargeFactor * 2 && b.y > Image::m_enlargeFactor * 4) {
		a.y = (Image::m_height + 1.5) * Image::m_enlargeFactor;
	}
	if (b.y < Image::m_enlargeFactor * 2 && a.y > Image::m_enlargeFactor * 4) {
		b.y = (Image::m_height + 1.5) * Image::m_enlargeFactor;
	}

}

void Utils::drawAscArc(Mat& img, AscArcPtr ascArc) {
	for (size_t z = 0; z < ascArc->m_arc.size() - 1; z++) {
		Point a = ascArc->m_arc[z]->centralPoint();
		Point b = ascArc->m_arc[z + 1]->centralPoint();
		checkBound(a, b);
		line(img, a, b, Scalar(0, 100, 255), 5, 8);
	}

	Point a = ascArc->m_arc.front()->centralPoint();
	Point b = ascArc->m_arcBegin->centralPoint();
	checkBound(a, b);
	line(img, a, b, Scalar(0, 100, 255), 5, 8);

}

void Utils::drawAscArcStorage(Mat& img, AscArcStorage& ascArcStorage) {
	for (AscArcStorage::ArcsToCriticalMap::iterator it = ascArcStorage.m_arcsToCriticalMap.begin(); it != ascArcStorage.m_arcsToCriticalMap.end();
			it++) {
		for (size_t i = 0; i < it->second.size(); ++i) {
			drawAscArc(img, it->second[i]);
		}

		it->first->draw(img, 200);
		circle(img,
				Point(it->first->maxVertex()->x * Image::m_enlargeFactor + Image::m_enlargeFactor,
						it->first->maxVertex()->y * Image::m_enlargeFactor + Image::m_enlargeFactor), 5, Scalar(0, 0, 255), 5);
	}
}

void Utils::drawDescArcStorage(Mat& img, DescArcStorage& descArcStorage) {

	for (DescArcStorage::ArcsToCriticalMap::iterator it = descArcStorage.m_arcsToCriticalMap.begin(); it != descArcStorage.m_arcsToCriticalMap.end();
			it++) {
		for (size_t i = 0; i < it->second.size(); ++i) {
			for (size_t z = 0; z < it->second[i]->m_arc.size(); z++) {
				it->second[i]->m_arc[z]->draw(img, 5, Scalar(200, 255, 0));
				it->second[i]->m_arcBegin->draw(img, 8, Scalar(200, 100, 0));

			}
		}
		circle(img,
				Point(it->first->x * Image::m_enlargeFactor + Image::m_enlargeFactor, it->first->y * Image::m_enlargeFactor + Image::m_enlargeFactor),
				5, Scalar(255, 200, 0), 7);

	}

}

#include "persist_pair/PPoint.h"

bool Utils::ppairToSimplex(std::pair<PPointPtr, PPointPtr>& ppair, Vertex** vtx, Edge** edge, Face** face) {

	if (ppair.first->m_dim == PPoint::NEGATIVE || ppair.second->m_dim == PPoint::NEGATIVE) {
		if (ppair.first->m_dim == PPoint::NEGATIVE) {
			*face = static_cast<FacePtr>(ppair.first->m_smplx);
			*edge = static_cast<EdgePtr>(ppair.second->m_smplx);
		} else if (ppair.second->m_dim == PPoint::NEGATIVE) {
			*face = static_cast<FacePtr>(ppair.second->m_smplx);
			*edge = static_cast<EdgePtr>(ppair.first->m_smplx);
		}
		return true;

	}

	else if (ppair.first->m_dim == PPoint::POSITIVE || ppair.second->m_dim == PPoint::POSITIVE) {
		if (ppair.first->m_dim == PPoint::POSITIVE) {
			*vtx = static_cast<VertexPtr>(ppair.first->m_smplx);
			*edge = static_cast<EdgePtr>(ppair.second->m_smplx);
		} else if (ppair.second->m_dim == PPoint::POSITIVE) {
			*vtx = static_cast<VertexPtr>(ppair.second->m_smplx);
			*edge = static_cast<EdgePtr>(ppair.first->m_smplx);
		}
	}
	return false;
}

void Utils::drawAscArcStorageOrig(Image& img, AscArcStorage& ascArcStorage) {
	for (AscArcStorage::ArcsToCriticalMap::iterator it = ascArcStorage.m_arcsToCriticalMap.begin(); it != ascArcStorage.m_arcsToCriticalMap.end();
			it++) {
		for (size_t i = 0; i < it->second.size(); ++i) {
			for (size_t j = 0; j < it->second[i]->m_arc.size(); j++) {
				img.paintPixel(it->second[i]->m_arc[j]->maxVertex(), Image::BLUE);
			}
			img.paintPixel(it->second[i]->m_arcEnd->maxVertex(), Image::GREEN);
		}
	}
}

void Utils::drawDescArcStorageOrig(Image& img, DescArcStorage& descArcStorage) {

	for (DescArcStorage::ArcsToCriticalMap::iterator it = descArcStorage.m_arcsToCriticalMap.begin(); it != descArcStorage.m_arcsToCriticalMap.end();
			it++) {
		for (size_t i = 0; i < it->second.size(); ++i) {
			for (size_t j = 0; j < it->second[i]->m_arc.size(); j++) {
				img.paintPixel(it->second[i]->m_arc[j]->m_a, Image::RED);
				img.paintPixel(it->second[i]->m_arc[j]->m_b, Image::RED);
			}
			img.paintPixel(it->second[i]->m_arcEnd->maxVertex(), Image::GREEN);
		}
	}
}
