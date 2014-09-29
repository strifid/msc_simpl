/*
 * BigDecimal.cpp
 *
 *  Created on: Sep 29, 2014
 *      Author: trifid
 */

#include "BigDecimal.h"

BigDecimal::BigDecimal() {
	m_man = 0;
	m_exp = 0;
}

BigDecimal::~BigDecimal() {
	// TODO Auto-generated destructor stub
}

bool BigDecimal::operator <(const BigDecimal& b) const {
	if(m_man == b.m_man)
		return m_exp < b.m_exp;
	return m_man < b.m_man;
}

bool BigDecimal::operator >(const BigDecimal& b) const {
	if(m_man == b.m_man)
		return m_exp > b.m_exp;
	return m_man > b.m_man;
}

bool BigDecimal::operator ==(const BigDecimal& b) const {
	if(m_man == b.m_man)
		return m_exp == b.m_exp;
	return false;
}

BigDecimal::BigDecimal(int m, uint32_t e) {
	m_man = m;
	m_exp = e;
}

void BigDecimal::operator =(int i) {
	m_man = i;

}

bool BigDecimal::operator !=(const BigDecimal& b) const {
	return ! operator==(b);
}

BigDecimal& BigDecimal::operator +=(const BigDecimal& b) {

	m_man += b.m_man;
	m_exp += b.m_exp;
	return *this;

}
