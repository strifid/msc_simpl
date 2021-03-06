/*
 * Triangle.cpp
 *
 *  Created on: Nov 27, 2012
 *      Author: trifid
 */

#include "Triangle.h"
#include "Vertex.h"
#include "opencv2/core/core.hpp"
#include "Image.h"
#include "mt/utils/StrUtils.h"

using cv::Scalar;

void Face::addEdge(EdgePtr edge) {

	m_edges.insert(edge);

	if (m_valueFirst < edge->m_valueFirst) {
		m_valueFirst = edge->m_valueFirst;
		m_valueSecond = edge->m_valueSecond;
	}

	if (m_valueSecond < edge->m_valueSecond)
		m_valueSecond = edge->m_valueSecond;

	m_vertexes.insert(edge->m_a);
	m_vertexes.insert(edge->m_b);

	m_valueThird = BigDecimal();
	for (VertexesSet::iterator it = m_vertexes.begin(); it != m_vertexes.end(); it++)
		m_valueThird += (*it)->m_valueFirst;

}

EdgePtr Face::findCommonEdge(Face* quad) {
	for (EdgesSet::iterator itA = m_edges.begin(); itA != m_edges.end(); ++itA) {
		for (EdgesSet::iterator itB = quad->m_edges.begin(); itB != quad->m_edges.end(); ++itB) {
			if (*(*itA) == *(*itB))
				return *(itA);
		}
	}
	return NULL;
}

bool Face::operator <(const Face & tr) const {

	if (tr.m_edges.size() != m_edges.size())
		return tr.m_edges.size() < m_edges.size();

	EdgesSet::iterator itB = tr.m_edges.begin();
	EdgesSet::iterator itA = m_edges.begin();
	for (; itA != m_edges.end() && itB != m_edges.end(); itA++, itB++) {
		if (*itA != *itB)
			return *itA < *itB;
	}

	return *m_edges.rbegin() < *tr.m_edges.rbegin();

}

bool Face::operator ==(const Face & tr) const {
	return m_faceId == tr.m_faceId;
}

bool Face::operator !=(const Face & tr) const {

	return m_faceId != tr.m_faceId;

/*
	if (m_edges.size() != tr.m_edges.size())
		return false;
	return !std::equal(m_edges.begin(), m_edges.end(), tr.m_edges.begin());
*/

}

void Face::draw(Mat & img, uint32_t green) {
	std::vector<Point> tmp;

	VertexPtr leftTopCorner = getLeftTopConer();
	VertexPtr rightBottomCorner = getRightBottomConer();

	uint32_t tlx = leftTopCorner->x;
	uint32_t tly = leftTopCorner->y;

	if (leftTopCorner->x == 0 && rightBottomCorner->x == Image::m_width - 1) {
		tlx = rightBottomCorner->x;
		tly = rightBottomCorner->y - 1;
	}

	else if (leftTopCorner->y == 0 && rightBottomCorner->y == Image::m_height - 1) {
		tlx = rightBottomCorner->x - 1;
		tly = rightBottomCorner->y;
	}

	tmp.push_back(Point(tlx * Image::m_enlargeFactor + Image::m_enlargeFactor, tly * Image::m_enlargeFactor + Image::m_enlargeFactor));
	tmp.push_back(Point((tlx + 1) * Image::m_enlargeFactor + Image::m_enlargeFactor, tly * Image::m_enlargeFactor + Image::m_enlargeFactor));
	tmp.push_back(Point((tlx + 1) * Image::m_enlargeFactor + Image::m_enlargeFactor, (tly + 1) * Image::m_enlargeFactor + Image::m_enlargeFactor));
	tmp.push_back(Point(tlx * Image::m_enlargeFactor + Image::m_enlargeFactor, (tly + 1) * Image::m_enlargeFactor + Image::m_enlargeFactor));

	const Point* elementPoints[] = { &tmp[0] };
	int numberOfPoints = (int) tmp.size();

	fillPoly(img, elementPoints, &numberOfPoints, 1, Scalar(0, green, 255), 8);
}

VertexPtr Face::maxVertex() {
	if (m_vertexes.empty())
		return NULL;

	VertexPtr vtx = *(m_vertexes.begin());
	for (VertexesSet::iterator it = m_vertexes.begin(); it != m_vertexes.end(); it++) {
		if (vtx->value() < (*it)->value())
			vtx = *it;
	}

	return vtx;
}

VertexPtr Face::getRightBottomConer() {
	VertexPtr leftTopConer = NULL;
	for (VertexesSet::iterator it = m_vertexes.begin(); it != m_vertexes.end(); ++it) {
		if (!leftTopConer)
			leftTopConer = *it;

		if (leftTopConer->x <= (*it)->x && leftTopConer->y <= (*it)->y)
			leftTopConer = *it;

	}
	return leftTopConer;
}

VertexPtr Face::getLeftTopConer() {
	VertexPtr leftTopConer = NULL;
	for (VertexesSet::iterator it = m_vertexes.begin(); it != m_vertexes.end(); ++it) {
		if (!leftTopConer)
			leftTopConer = *it;

		if (leftTopConer->x >= (*it)->x && leftTopConer->y >= (*it)->y)
			leftTopConer = *it;

	}
	return leftTopConer;
}
Point Face::centralPoint() {

	VertexPtr leftTopConer = getLeftTopConer();
	VertexPtr rightBottomConer = getRightBottomConer();

	uint32_t centerX = leftTopConer->x * Image::m_enlargeFactor + Image::m_enlargeFactor * 1.5;
	if (leftTopConer->x == 0 && rightBottomConer->x == Image::m_width - 1)
		centerX = (Image::m_width - 1) * Image::m_enlargeFactor + Image::m_enlargeFactor * 1.5;

	uint32_t centerY = (leftTopConer->y * Image::m_enlargeFactor + Image::m_enlargeFactor * 1.5);
	if (leftTopConer->y == 0 && rightBottomConer->y == Image::m_height - 1) {
		centerY = (Image::m_height - 1) * Image::m_enlargeFactor + Image::m_enlargeFactor * 1.5;
	}

	Point b(centerX, centerY);

	return b;
}

std::ostream & operator <<(std::ostream & out, Face& vt) {
//	out << "face: " << *(vt.getLeftTopConer());
	out << "face: ";

	for (VertexesSet::iterator it = vt.m_vertexes.begin(); it != vt.m_vertexes.end(); it++) {
		out << *(*it) << " ";
	}
	return out;
}

