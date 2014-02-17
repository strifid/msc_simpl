/*

 * Quad.cpp
 *
 *  Created on: Jan 16, 2013
 *      Author: trifid


#include "Quad.h"
#include "Image.h"

Quad::Quad(EdgePtr a, EdgePtr b, EdgePtr c, EdgePtr d) {

	EdgesSet tmpSet;
	tmpSet.insert(a);
	tmpSet.insert(b);
	tmpSet.insert(c);
	tmpSet.insert(d);

	m_edges.push_back(a);
	m_edges.push_back(b);
	m_edges.push_back(c);
	m_edges.push_back(d);

	EdgesSet::iterator it = tmpSet.begin();
	m_a = *it;
	m_b = *(++it);
	m_c = *(++it);
	m_d = *(++it);

	EdgePtr high = NULL;
	if (dynamic_cast<Simplex&>(*m_a) > dynamic_cast<Simplex&>(*m_b))
		high = m_a;
	else
		high = m_b;

	if (dynamic_cast<Simplex&>(*high) < dynamic_cast<Simplex&>(*m_c))
		high = m_c;

	if (dynamic_cast<Simplex&>(*high) < dynamic_cast<Simplex&>(*m_d))
		high = m_d;

	m_valueFirst = high->m_valueFirst;
	m_valueSecond = high->m_valueSecond;

	m_vertexes.insert(m_a->m_a);
	m_vertexes.insert(m_a->m_b);
	m_vertexes.insert(m_b->m_a);
	m_vertexes.insert(m_b->m_b);
	m_vertexes.insert(m_c->m_a);
	m_vertexes.insert(m_c->m_b);
	m_vertexes.insert(m_d->m_a);
	m_vertexes.insert(m_d->m_b);

	for (VertexesSet::iterator it = m_vertexes.begin(); it != m_vertexes.end(); it++)
		m_valueThird += (*it)->value();

}

Quad::~Quad() {
	// TODO Auto-generated destructor stub
}

bool Quad::operator <(const Quad & tr) const {
	if (*m_a != *tr.m_a)
		return *m_a < *tr.m_a;
	if (*m_b != *tr.m_b)
		return *m_b < *tr.m_b;
	if (*m_c != *tr.m_c)
		return *m_c < *tr.m_c;

	return *m_d < *tr.m_d;
}

bool Quad::operator !=(const Quad & tr) const {
	if (*m_a != *tr.m_a || *m_b != *tr.m_b || *m_b != *tr.m_b || *m_d != *tr.m_d)
		return true;

	return false;
}

EdgePtr Quad::findCommonEdge(Quad* quad) {
	for (size_t i = 0; i < m_edges.size(); ++i) {
		for (size_t j = 0; j < quad->m_edges.size(); ++j) {
			if (*(m_edges[i]) == *(quad->m_edges[j]))
				return m_edges[i];
		}
	}
	return NULL;
}

std::ostream & operator <<(std::ostream & out, Quad& vt) {
	out << "face: " << *vt.m_a << "-" << *vt.m_b << "-" << *vt.m_c << "-" << *vt.m_d;
	return out;
}

Point Quad::centralPoint() {

	VertexPtr leftTopConer = NULL;
	VertexPtr rightBottomConer = NULL;

	for (VertexesSet::iterator it = m_vertexes.begin(); it != m_vertexes.end(); ++it) {
		if (!leftTopConer)
			leftTopConer = *it;

		if (leftTopConer->x >= (*it)->x && leftTopConer->y >= (*it)->y)
			leftTopConer = *it;

	}

	Point b((leftTopConer->x * Image::m_enlargeFactor + Image::m_enlargeFactor * 1.5),
			(leftTopConer->y * Image::m_enlargeFactor + Image::m_enlargeFactor * 1.5));

	return b;
}

*/
