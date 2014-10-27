/*
 * BigDecimal.h
 *
 *  Created on: Sep 29, 2014
 *      Author: trifid
 */

#ifndef BIGDECIMAL_H_
#define BIGDECIMAL_H_
#include <stdint.h>
class BigDecimal {
public:
	BigDecimal(int m, uint32_t e);
	BigDecimal();
	virtual ~BigDecimal();

	int32_t getInt(){
		return m_man;
	}

	bool operator<(const BigDecimal& b) const;
	bool operator<=(const BigDecimal& b) const;

	bool operator>(const BigDecimal& b) const;
	bool operator>=(const BigDecimal& b) const;

	bool operator==(const BigDecimal& b) const;
	bool operator!=(const BigDecimal& b) const;
	void operator=(const BigDecimal& b);
	void set(int32_t m, uint32_t e);
	BigDecimal& operator+=(const BigDecimal& b);
	BigDecimal operator+(const BigDecimal& b);

protected:
	int32_t m_man;
	uint32_t m_exp;
};

#endif /* BIGDECIMAL_H_ */
