/*
 * Utils.h
 *
 *  Created on: Jul 21, 2013
 *      Author: trifid
 */

#ifndef MSUTILS_H_
#define MSUTILS_H_
#include <vector>
#include <stddef.h>

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

};

#endif /* UTILS_H_ */
