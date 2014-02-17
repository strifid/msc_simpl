/*
 * SimplexStorage.h
 *
 *  Created on: Dec 25, 2012
 *      Author: trifid
 */

#ifndef SIMPLEXSTORAGE_H_
#define SIMPLEXSTORAGE_H_
#include "Simplex.h"
#include <vector>
#include <set>
#include <algorithm>

template<typename SimplexPtr> struct SimplexComparator {
	bool operator()(const SimplexPtr a, const SimplexPtr b) const {
		return *a < *b;
	}
};

template<typename SimplexPtr> struct SimplexValueComparator {
	bool operator()(const SimplexPtr a, const SimplexPtr b) const {
		return *(dynamic_cast<Simplex*>(a)) < *(dynamic_cast<Simplex*>(b));
	}
};

template<class SimplexPtr>
class SimplexStorage {
public:
	SimplexStorage() {
	}

	virtual ~SimplexStorage() {
	}

	void push(SimplexPtr simplex) {
		m_simplexes.push_back(simplex);
		m_simplexesSet.insert(simplex);
	}

	std::vector<SimplexPtr>& vector() {
		return m_simplexes;
	}

	void sort(){
		struct SimplexValueComparator<SimplexPtr> myobj;
		std::sort (m_simplexes.begin(), m_simplexes.end(), myobj);

	}

	bool exist(SimplexPtr simplex) {
		typename std::set<SimplexPtr, SimplexComparator<SimplexPtr> >::iterator it = m_simplexesSet.find(simplex);
		if (it != m_simplexesSet.end())
			return true;
		return false;
	}

	SimplexPtr getSimplex(const SimplexPtr simplex) {
		typename  std::set<SimplexPtr, SimplexComparator<SimplexPtr> >::iterator it = m_simplexesSet.find(simplex);
		if (it != m_simplexesSet.end())
			return *it;
		return NULL;
	}

private:
	std::vector<SimplexPtr> m_simplexes;
	std::set<SimplexPtr, SimplexComparator<SimplexPtr> > m_simplexesSet;

};

#endif /* SIMPLEXSTORAGE_H_ */
