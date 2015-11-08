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

bool tt = false;

void removeDuplicate(std::vector<FacePtr>& orig, std::vector<FacePtr>& ret) {

	if (orig.size() < 3) {
		ret = orig;
		return;
	}

	ret.push_back(orig[0]);
	FacePtr lastRemoved = NULL;
	for (int i = 2; i < orig.size(); i++) {
		if (!ret.empty() && (*(ret.back()) == *(orig[i]))) {
			lastRemoved = ret.back();
			ret.pop_back();
		} else {
			if (lastRemoved != NULL) {
				ret.push_back(lastRemoved);
				lastRemoved = NULL;
			}
			ret.push_back(orig[i - 1]);
		}
	}

}

void Utils::drawAscArc(Mat& img, AscArcPtr ascArc) {

	std::vector<FacePtr> arc;
	removeDuplicate(ascArc->m_arc, arc);

	for (size_t z = 0; z < arc.size() - 1; z++) {
		Point a = arc[z]->centralPoint();
		Point b = arc[z + 1]->centralPoint();
		checkBound(a, b);
		line(img, a, b, Scalar(0, 100, 255), 5, 8);
	}

	Point a = arc.front()->centralPoint();
	Point b = ascArc->m_arcBegin->centralPoint();
	checkBound(a, b);
	line(img, a, b, Scalar(0, 100, 255), 5, 8);

}

void Utils::drawAscArcStorage(Mat& img, AscArcStorage& ascArcStorage) {
	for (AscArcStorage::ArcsToCriticalMap::iterator it = ascArcStorage.m_arcsToCriticalMap.begin(); it != ascArcStorage.m_arcsToCriticalMap.end();
			it++) {

		if (tt) {

			for (int i = 0; i < it->second.size(); i++) {
				std::cout << "one arc: size " << it->second[i]->m_arc.size() << ": ";
				for (int j = 0; j < it->second[i]->m_arc.size(); ++j) {
					std::cout << it->second[i]->m_arc[j]->m_faceId << " ";
				}
				std::cout << std::endl;
			}

		}

		for (size_t i = 0; i < it->second.size(); ++i) {
			drawAscArc(img, it->second[i]);
		}

		it->first->draw(img, 200);
		circle(img,
				Point(it->first->maxVertex()->x * Image::m_enlargeFactor + Image::m_enlargeFactor,
						it->first->maxVertex()->y * Image::m_enlargeFactor + Image::m_enlargeFactor), 5, Scalar(0, 0, 255), 5);
	}
}

void removeDuplicate(std::vector<EdgePtr>& orig, std::vector<EdgePtr>& ret) {

	if (orig.size() < 2) {
		ret = orig;
		return;
	}

	ret.push_back(orig[0]);

	for (int i = 1; i < orig.size(); i++) {
		if (!ret.empty() && (*(ret.back()) == *(orig[i]))) {
			ret.pop_back();
		} else
			ret.push_back(orig[i]);
	}

}

void Utils::drawDescArcStorage(Mat& img, DescArcStorage& descArcStorage) {

	for (DescArcStorage::ArcsToCriticalMap::iterator it = descArcStorage.m_arcsToCriticalMap.begin(); it != descArcStorage.m_arcsToCriticalMap.end();
			it++) {
		for (size_t i = 0; i < it->second.size(); ++i) {

			std::vector<EdgePtr> arc;
			removeDuplicate(it->second[i]->m_arc, arc);

			for (size_t z = 0; z < arc.size(); z++) {
				arc[z]->draw(img, 5, Scalar(200, 255, 0));
			}
			it->second[i]->m_arcBegin->draw(img, 8, Scalar(200, 100, 0));

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
			std::vector<FacePtr> arc;
			removeDuplicate(it->second[i]->m_arc, arc);

			for (size_t j = 0; j < arc.size(); j++) {
				img.paintPixel(arc[j]->maxVertex(), Image::BLUE);
			}
			img.drawCircle((it->second[i]->m_arcEnd->maxVertex()), Scalar(0, 100, 255), 1, 2);
		}
	}
}

void Utils::drawDescArcStorageOrig(Image& img, DescArcStorage& descArcStorage) {

	for (DescArcStorage::ArcsToCriticalMap::iterator it = descArcStorage.m_arcsToCriticalMap.begin(); it != descArcStorage.m_arcsToCriticalMap.end();
			it++) {

		for (size_t i = 0; i < it->second.size(); ++i) {
			std::vector<EdgePtr> arc;
			removeDuplicate(it->second[i]->m_arc, arc);

			for (size_t j = 0; j < arc.size(); j++) {
				img.paintPixel(arc[j]->m_a, Image::RED);
				img.paintPixel(arc[j]->m_b, Image::RED);
			}
			img.drawCircle((it->second[i]->m_arcEnd->maxVertex()), Scalar(255, 150, 0), 1, 2);

			img.drawCircle((it->second[i]->m_arcBegin->maxVertex()), Scalar(0, 255, 0), 1, 2);

//			img.paintPixel(it->second[i]->m_arcEnd->maxVertex(), Image::GREEN);
		}
	}
}
