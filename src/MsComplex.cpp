#include "MsComplex.h"
#include "opencv2/core/core.hpp"

#include "Edge.h"
using cv::line;

void MsComplex::draw(Mat &img) {

//	if (m_max->maxVertex()->x == 6 && m_max->maxVertex()->y == 10 && m_min->x == 14) {
	for (size_t i = 0; i < m_aArcs.size(); i++) {

		for (size_t z = 0; z < m_aArcs[i]->m_arc.size() - 1; z++) {

//			Face
//			m_aArcs[i]->m_arc[z]->draw(img, 200);
			line(img, m_aArcs[i]->m_arc[z]->centralPoint(), m_aArcs[i]->m_arc[z+1]->centralPoint(), Scalar(0, 100, 255), 5, 8);

		}

		line(img, m_aArcs[i]->m_arc.front()->centralPoint(), m_aArcs[i]->m_arcBegin->centralPoint(), Scalar(0, 100, 255), 5, 8);

		m_aArcs[i]->m_arcEnd->draw(img, 200);

	}

	for (size_t i = 0; i < m_dArcs.size(); i++) {
		for (size_t z = 0; z < m_dArcs[i]->m_arc.size(); z++) {
			m_dArcs[i]->m_arc[z]->draw(img, 5, Scalar(200, 255, 0));
		}
	}

	for (size_t i = 0; i < m_seddles.size(); i++) {
		m_seddles[i]->draw(img, 8, Scalar(200, 100, 0));
	}
	circle(img, Point(m_min->x * Image::m_enlargeFactor + Image::m_enlargeFactor, m_min->y * Image::m_enlargeFactor + Image::m_enlargeFactor), 5,
			Scalar(255, 200, 0), 7);

	circle(img,
			Point(m_max->maxVertex()->x * Image::m_enlargeFactor + Image::m_enlargeFactor,
					m_max->maxVertex()->y * Image::m_enlargeFactor + Image::m_enlargeFactor), 5, Scalar(0, 0, 255), 5);

//	}
}

void MsComplex::drawOriginal(Image & img) {

	for (size_t i = 0; i < m_aArcs.size(); i++) {
		for (size_t z = 0; z < m_aArcs[i]->m_arc.size(); z++) {
			img.paintPixel(m_aArcs[i]->m_arc[z]->maxVertex(), Image::BLUE);
		}
	}

	for (size_t i = 0; i < m_dArcs.size(); i++) {
		for (size_t z = 0; z < m_dArcs[i]->m_arc.size(); z++) {
			img.paintPixel(m_dArcs[i]->m_arc[z]->m_a, Image::RED);
			img.paintPixel(m_dArcs[i]->m_arc[z]->m_b, Image::RED);
		}
	}
	img.paintPixel(m_min, Image::GREEN);
	img.paintPixel(m_max->maxVertex(), Image::GREEN);
}

