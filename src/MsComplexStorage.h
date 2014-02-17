/*
 * MsComplexStorage.h
 *
 *  Created on: Jan 8, 2013
 *      Author: trifid
 */

#ifndef MSCOMPLEXSTORAGE_H_
#define MSCOMPLEXSTORAGE_H_
#include "MsComplex.h"
#include "SimplexStorage.h"
#include "opencv2/core/core.hpp"
#include <ostream>
#include <iosfwd>
#include <fstream>

class MsComplexStorage {
public:
	MsComplexStorage() {
	}
	virtual ~MsComplexStorage() {
	}

	void addComplex(MsComplex*cmpl) {
		m_complexByMax[cmpl->m_max].push_back(cmpl);
		for (size_t i = 0; i < cmpl->m_seddles.size(); i++) {
			m_complexBySaddle[cmpl->m_seddles[i]].push_back(cmpl);
		}
		m_complexesSet.insert(cmpl);
	}
	void removeComplex(MsComplex *cmpl);

	bool isExist(MsComplex* cmpl) {
		typename MsComplexesSet::iterator it = m_complexesSet.find(cmpl);
		if (it != m_complexesSet.end())
			return true;
		return false;
	}
	std::vector<MsComplex*> * find(EdgePtr edge) {
		typename ComplexesBySaddleMap::iterator it = m_complexBySaddle.find(edge);
		if (it != m_complexBySaddle.end())
			return &(it->second);
		return NULL;
	}

	std::vector<MsComplex*> * find(FacePtr face) {
		typename ComplexesByMaxMap::iterator it = m_complexByMax.find(face);
		if (it != m_complexByMax.end())
			return &(it->second);
		return NULL;
	}

	void drawComplex(MsComplex* cmplx) {
		m_msComplexesForDrawing.push_back(cmplx);
	}
	void drawAll() {

/*
		size_t i = 0;
		for (MsComplexesSet::iterator it = m_complexesSet.begin(); it != m_complexesSet.end(); it++) {
			if (i == 3)
				m_msComplexesForDrawing.push_back(*it);

			i++;
		}
*/

		m_msComplexesForDrawing.insert(m_msComplexesForDrawing.begin(), m_complexesSet.begin(), m_complexesSet.end());
	}

	void drawOriginal(Image& img) {
		for (typename MsComplexesSet::iterator it = m_complexesSet.begin(); it != m_complexesSet.end(); it++) {
			(*it)->drawOriginal(img);
		}
	}

	size_t size() {
		return m_complexesSet.size();
	}

	void clear() {
		m_complexByMax.clear();
		m_complexBySaddle.clear();
		m_complexesSet.clear();
	}

	void saveCoords(const std::string& str) {
		/*
		 std::ofstream coordFile;
		 coordFile.open(path.c_str());
		 coordFile << "x\ty\tz" << std::endl;

		 for (std::map<std::pair<int, int>, int>::iterator it = m_generateField.begin(); it != m_generateField.end(); it++) {
		 //		if (it->second < 255 && it->second >= 0)
		 coordFile << it->first.first << "\t" << it->first.second << "\t" << it->second << std::endl;

		 }
		 coordFile.close();
		 */

	}

	void saveCriticalPoints(const std::string& path) {
		std::ofstream coordFile;

		VertexesSet vtxs;
		for (MsComplexesSet::iterator it = m_complexesSet.begin(); it != m_complexesSet.end(); it++) {
			vtxs.insert((*it)->m_max->maxVertex());
			vtxs.insert((*it)->m_min);
			for (size_t i = 0; i < (*it)->m_seddles.size(); ++i) {
				vtxs.insert((*it)->m_seddles[i]->maxVertex());
			}
		}
		coordFile.open(path.c_str());
		coordFile << "x\ty\tz" << std::endl;
		for (VertexesSet::iterator it = vtxs.begin(); it != vtxs.end(); it++) {
			coordFile << (*it)->x << "\t" << (*it)->y << "\t" << (*it)->value() << std::endl;
		}
		coordFile.close();
	}

	void draw(Mat & mat) {
		for (size_t i = 0; i < m_msComplexesForDrawing.size(); i++) {
			m_msComplexesForDrawing[i]->draw(mat);
		}
	}

	MsComplexesSet & complexesSet() {
		return m_complexesSet;
	}

protected:

	typedef std::map<FacePtr, std::vector<MsComplex*>, SimplexComparator<FacePtr> > ComplexesByMaxMap;
	ComplexesByMaxMap m_complexByMax;
	typedef std::map<EdgePtr, std::vector<MsComplex*>, SimplexComparator<EdgePtr> > ComplexesBySaddleMap;
	ComplexesBySaddleMap m_complexBySaddle;
	std::vector<MsComplex*> m_msComplexesForDrawing;

	MsComplexesSet m_complexesSet;
};

#endif /* MSCOMPLEXSTORAGE_H_ */
