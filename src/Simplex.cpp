/*
 * Simplex.cpp
 *
 *  Created on: Dec 1, 2012
 *      Author: trifid
 */

#include "Simplex.h"
#include <iosfwd>

Simplex::Simplex() :
		m_seqId(0) {
}

Simplex::~Simplex() {
}

/*
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
*/

BigDecimal& Simplex::value() {
	return m_valueFirst;
}


void Simplex::value(int32_t i, uint32_t e) {
	m_valueFirst.set(i, e);
}

#include <vector>
#include "opencv2/core/core.hpp"
#include <set>
#include <map>
#include "Simplex.h"

/*
 std::ostream & operator <<(std::ostream & out, Simplex vt) {

 out << "value: " << vt.m_valueFirst << "+ e*" << vt.m_valueSecond << "+ e^2*" << vt.m_valueThird;
 return out;

 }
 */

