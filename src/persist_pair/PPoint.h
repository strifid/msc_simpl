/*
 * PPoint.h
 *
 *  Created on: Sep 28, 2013
 *      Author: trifid
 */

#ifndef PPOINT_H_
#define PPOINT_H_
#include "Pixel.h"
#include "Vertex.h"

class PPoint: public Vertex {
public:
	enum Type {
		POSITIVE,
		NEGATIVE,
		UNIVERSAL
	} m_type;


	PPoint(int x, int y, int value, Type t);

	virtual ~PPoint();


	uint32_t m_id;
	uint32_t m_dim;

private:
	static uint32_t pointId;
};

typedef PPoint* PPointPtr;

#endif /* PPOINT_H_ */
