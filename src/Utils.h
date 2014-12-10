/*
 * Utils.h
 *
 *  Created on: Jul 21, 2013
 *      Author: trifid
 */

#ifndef MSUTILS_H_
#define MSUTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string>
#include <vector>

#include "Edge.h"
#include "SimplexStorage.h"
#include "Vertex.h"

class Utils {
public:
	Utils();
	virtual ~Utils();

	template<typename T>
	static bool hasInVectorPtr(std::vector<T> v, T o) {
		for (size_t i = 0; i < v.size(); ++i) {
			if (*(v[i]) == *o)
				return true;
		}
		return false;
	}

	static std::string intToString(int32_t i);

	static EdgePtr getEdgeByVertex(SimplexStorage<EdgePtr>& edges, VertexPtr a, VertexPtr b);
};

#endif /* UTILS_H_ */
