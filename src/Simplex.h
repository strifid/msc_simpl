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

class Simplex {
public:
	Simplex();
	virtual ~Simplex();

	bool operator==(const Simplex& vrtx) const;
	bool operator<(const Simplex& vrtx) const;
	bool operator>(const Simplex& vrtx) const;

	int32_t m_valueFirst;
	int32_t m_valueSecond;
	int32_t m_valueThird;

	uint32_t m_seqId;

	virtual int32_t value() = 0;
};

//std::ostream & operator <<(std::ostream & out, Simplex vt);

#endif /* SIMPLEX_H_ */
