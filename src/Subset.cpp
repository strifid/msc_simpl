/*
 * Subset.cpp
 *
 *  Created on: Jul 8, 2012
 *      Author: trifid
 */

#include "Subset.h"
#include "Image.h"
#include <iostream>

Subset::Subset() {
	// TODO Auto-generated constructor stub
	MT_LOGINIT(Subset)
}

Subset::~Subset() {
	// TODO Auto-generated destructor stub
}

void Subset::addPoint(const Pixel & pt) {
	MT_LOGDEBUG("add point " << pt.toString() << " into subset")
	m_points[pt] = true;
}

PointsMap & Subset::points() {
	return m_points;
}

bool Subset::isInSubset(const Pixel & pt) {
	PointsMap::iterator it = m_points.find(pt);
	if (it != m_points.end())
		return true;
	return false;
}

Pixels Subset::getContour() {

	Pixels contour;

	for (PointsMap::iterator it = m_points.begin(); it != m_points.end(); it++) {
//todo remove bondary pixel from contour
		if (m_parentImage->isBoundaryPixel(it->first)) {
			contour.push_back(it->first);
		} else {
			Pixels pixCont = m_parentImage->getAllAround(it->first);
			for (size_t i = 0; i < pixCont.size(); i++) {
				if (!isInSubset(pixCont[i])) {
					contour.push_back(it->first);
					break;
				}
			}
		}
	}
	std::string contourstr;
	for (size_t i = 0; i < contour.size(); ++i) {
		contourstr += contour[i].toString() + " ";
	}
	MT_LOGDEBUG("contour is: " << contourstr);

	return contour;
}

void Subset::findWraps() {

	m_wrap0.points().clear();
	m_wrapUp.points().clear();
	m_wrapDown.points().clear();
	m_bd.points().clear();

//todo data copying. remove


	Pixels contour = getContour();
	for (size_t i = 0; i < contour.size(); i++) {

		Pixels aroundPxls = m_parentImage->getAllAround(contour[i]);
		for (size_t j = 0; j < aroundPxls.size(); j++) {
			if (!isInSubset(aroundPxls[j])) {
				switch (m_parentImage->comparePixels(aroundPxls[j], m_center)) {
				case 0:
					m_wrap0.addPoint(aroundPxls[j]);
					break;
				case 1:
					m_wrapUp.addPoint(aroundPxls[j]);
					break;
				case -1:
					m_wrapDown.addPoint(aroundPxls[j]);
					break;
				}

				m_bd.addPoint(aroundPxls[j]);
			}
		}
	}
}

void Subset::parentImage(Image *img) {
	m_parentImage = img;
}

bool Subset::isInSubsetAndWraps(const Pixel& pt) {
	if (isInSubset(pt))
		return true;

	for (size_t i = 0; i < 4; i++) {
		if (m_wraps[i].isInWrap(pt))
			return true;
	}
	return false;
}

Pixels& Subset::oneConnectedPixels() {

	m_oneConnectedPixels.clear();

	Pixels contour = getContour();
	for (size_t i = 0; i < contour.size(); i++) {
		Pixels aroundPxls = m_parentImage->getOneConnectedAround(contour[i]);
		for (size_t j = 0; j < aroundPxls.size(); j++) {

			if (!isInSubset(aroundPxls[j])) {
				m_oneConnectedPixels.push_back(aroundPxls[j]);

			}
		}
	}
	return m_oneConnectedPixels;

}

