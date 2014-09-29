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

	int32_t value();
	void value(int32_t i);

};

//std::ostream & operator <<(std::ostream & out, Simplex vt);

#endif /* SIMPLEX_H_ */
