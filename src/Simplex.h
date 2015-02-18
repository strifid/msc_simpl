/*
 * Simplex.h
 *
 *  Created on: Dec 1, 2012
 *      Author: trifid
 */

#ifndef SIMPLEX_H_
#define SIMPLEX_H_
#include <stdint.h>
#include <iosfwd>
#include "BigDecimal.h"

class Simplex {
public:
	Simplex();
	virtual ~Simplex();

	bool operator==(const Simplex& vrtx) const;
	bool operator<(const Simplex& vrtx) const;
	bool operator>(const Simplex& vrtx) const;

	BigDecimal m_valueFirst;
	BigDecimal m_valueSecond;
	BigDecimal m_valueThird;

	uint32_t m_seqId;
	uint32_t m_dim;

	BigDecimal& value();
	void value(int32_t i, uint32_t e);
	int x;
	int y;

};

#endif /* SIMPLEX_H_ */
