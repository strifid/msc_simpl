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

	void addComplex(MsComplex*cmpl);

	bool isExist(MsComplex* cmpl) ;

	void drawComplex(MsComplex* cmplx);

	std::vector<MsComplex*>& getComplxesForDrawing() ;
	void drawAll() {
		m_msComplexesForDrawing.insert(m_msComplexesForDrawing.begin(), m_complexesSet.begin(), m_complexesSet.end());
	}

	void drawOriginal(Image& img) ;

	size_t size() {
		return m_complexesSet.size();
	}

	void clear() ;

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

	void saveCriticalPoints(const std::string& path);

	virtual void draw(Mat & mat) ;

	MsComplexesSet & complexesSet() {
		return m_complexesSet;
	}

	void printInfo(std::vector<FacePtr>& v);
protected:

	typedef std::map<FacePtr, std::vector<MsComplex*>, SimplexComparator<FacePtr> > ComplexesByMaxMap;
	ComplexesByMaxMap m_complexByMax;
	typedef std::map<EdgePtr, std::vector<MsComplex*>, SimplexComparator<EdgePtr> > ComplexesBySaddleMap;
	ComplexesBySaddleMap m_complexBySaddle;
	std::vector<MsComplex*> m_msComplexesForDrawing;

	MsComplexesSet m_complexesSet;
};


#endif /* MSCOMPLEXSTORAGE_H_ */
