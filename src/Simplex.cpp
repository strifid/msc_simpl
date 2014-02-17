/*
 * Simplex.cpp
 *
 *  Created on: Dec 1, 2012
 *      Author: trifid
 */

#include "Simplex.h"
#include <iosfwd>

Simplex::Simplex() :
		m_valueFirst(0), m_valueSecond(0), m_valueThird(0), m_seqId(0) {
}

Simplex::~Simplex() {
}

bool Simplex::operator ==(const Simplex & vrtx) const {
	if (m_valueFirst == vrtx.m_valueFirst && m_valueSecond == vrtx.m_valueSecond && m_valueThird == vrtx.m_valueThird)
		return true;
	return false;
}

bool Simplex::operator <(const Simplex & vrtx) const {
	if (m_valueFirst != vrtx.m_valueFirst)
		return m_valueFirst < vrtx.m_valueFirst;
	if (m_valueSecond != vrtx.m_valueSecond)
		return m_valueSecond < vrtx.m_valueSecond;

	return m_valueThird < vrtx.m_valueThird;

}

bool Simplex::operator >(const Simplex & vrtx) const {
	if (m_valueFirst != vrtx.m_valueFirst)
		return m_valueFirst > vrtx.m_valueFirst;
	if (m_valueSecond != vrtx.m_valueSecond)
		return m_valueSecond > vrtx.m_valueSecond;

	return m_valueThird > vrtx.m_valueThird;

}


#include <vector>
#include "opencv2/core/core.hpp"
#include "Pixel.h"
#include <set>
#include "Subset.h"
#include <map>
#include "Simplex.h"

/*
std::ostream & operator <<(std::ostream & out, Simplex vt) {

	out << "value: " << vt.m_valueFirst << "+ e*" << vt.m_valueSecond << "+ e^2*" << vt.m_valueThird;
	return out;

}
*/

