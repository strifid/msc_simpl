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

class MsComplexStorage: public Drawer {
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

	std::vector<MsComplex*>& getComplxesForDrawing() {
		return m_msComplexesForDrawing;
	}
	void drawAll() {
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

		VertexesSet vtxs;
		for (MsComplexesSet::iterator it = m_complexesSet.begin(); it != m_complexesSet.end(); it++) {
			vtxs.insert((*it)->m_max->maxVertex());
			vtxs.insert((*it)->m_min);
			for (size_t i = 0; i < (*it)->m_seddles.size(); ++i) {
				vtxs.insert((*it)->m_seddles[i]->maxVertex());
			}
		}
		std::ofstream coordFile;

		coordFile.open(path.c_str());
		coordFile << "x\ty\tz" << std::endl;
		for (VertexesSet::iterator it = vtxs.begin(); it != vtxs.end(); it++) {
			coordFile << (*it)->x << "\t" << (*it)->y << "\t" << (*it)->value().getInt() << std::endl;
		}
		coordFile.close();
	}

	virtual void draw(Mat & mat) {
		for (size_t i = 0; i < m_msComplexesForDrawing.size(); i++) {
			m_msComplexesForDrawing[i]->draw(mat);
		}
	}

	MsComplexesSet & complexesSet() {
		return m_complexesSet;
	}



	void printInfo(std::vector<FacePtr>& v ){

		std::cout << "m_complexesSet size " << m_complexesSet.size() << std::endl;
		std::cout << "m_complexByMax size " << m_complexByMax.size() << std::endl;
		std::cout << "m_complexBySaddle size " << m_complexBySaddle.size() << std::endl;

		std::set<AscArcPtr> aSet;
		std::set<DescArcPtr> dSet;
		SimplexStorage<FacePtr> max;
		for(MsComplexesSet::iterator it = m_complexesSet.begin() ; it != m_complexesSet.end(); it++){
			aSet.insert((*it)->m_aArcs[0]);
			aSet.insert((*it)->m_aArcs[1]);
			dSet.insert((*it)->m_dArcs[0]);
			dSet.insert((*it)->m_dArcs[1]);
			max.push((*it)->m_max);
		}

		for (size_t i = 0; i < v.size(); ++i) {
			if( !max.exist(v[i]))
				std::cout << "can't find any complex for max " << *(v[i])<< std::endl;

		}
		std::cout << "aSet " << aSet.size() << " dSet " << dSet.size() << std::endl;
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
