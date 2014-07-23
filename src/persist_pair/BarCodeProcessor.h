/*
 * BarCodeProcessor.h
 *
 *  Created on: Jul 21, 2014
 *      Author: trifid
 */

#ifndef BARCODEPROCESSOR_H_
#define BARCODEPROCESSOR_H_

#include "PersistPairProcessor.h"
class BarCode {
public:
	BarCode(PPointPtr p, uint32_t start) {
		m_start = start;
		m_id = p->m_id;
		m_dim = p->m_dim;
		m_stop = 0;
	}
	uint32_t m_start;
	uint32_t m_stop;
	uint32_t m_dim;
	uint32_t m_id;

};

std::ostream& operator<<(std::ostream& out, BarCode &bc);

typedef BarCode* BarCodePtr;

class BarCodeComparator {
public:
	bool operator()(const BarCodePtr a, const BarCodePtr b) const {
		return a->m_id < b->m_id;
	}

};

class BarCodeProcessor: public PersistPairProcessor {
public:

	BarCodeProcessor();
	virtual ~BarCodeProcessor();
	void computeBarCodes();

protected:
	void barCode(uint32_t point, uint32_t step);

	std::map<uint32_t, BarCode*> m_barCodes;
	BarCodePtr findBarCode(uint32_t id) {
		std::map<uint32_t, BarCode*>::iterator it = m_barCodes.find(id);
		if (it == m_barCodes.end())
			return NULL;
		return it->second;

	}

	std::vector<PPointPtr> getPpoints(std::vector<uint32_t> &ppIds);
};

#endif /* BARCODEPROCESSOR_H_ */
