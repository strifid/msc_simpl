/*
 * Utils.cpp
 *
 *  Created on: Jul 21, 2013
 *      Author: trifid
 */

#include "Utils.h"

Utils::Utils() {
	// TODO Auto-generated constructor stub

}

Utils::~Utils() {
	// TODO Auto-generated destructor stub
}

std::string Utils::intToString(int32_t i) {
	char buf[16];
	sprintf(buf, "%d", i);
	return buf;
}

EdgePtr Utils::getEdgeByVertex(SimplexStorage<EdgePtr>& edges, VertexPtr a, VertexPtr b) {
	if (a == NULL || b == NULL)
		return NULL;
	Edge tmpEdgeAB(a, b);
	return edges.getSimplex(&tmpEdgeAB);
}
