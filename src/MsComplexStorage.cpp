/*
 * MsComplexStorage.cpp
 *
 *  Created on: Dec 17, 2014
 *      Author: trifid
 */

#include "MsComplexStorage.h"

void MsComplexStorage::addComplex(MsComplex* cmpl) {
	m_complexByMax[cmpl->m_max].push_back(cmpl);
	for (size_t i = 0; i < cmpl->m_seddles.size(); i++) {
		m_complexBySaddle[cmpl->m_seddles[i]].push_back(cmpl);
	}
	m_complexesSet.insert(cmpl);
}

bool MsComplexStorage::isExist(MsComplex* cmpl) {
	typename MsComplexesSet::iterator it = m_complexesSet.find(cmpl);
	if (it != m_complexesSet.end())
		return true;
	return false;
}

void MsComplexStorage::saveCriticalPoints(const std::string& path) {

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

void MsComplexStorage::draw(Mat& mat) {
	for (size_t i = 0; i < m_msComplexesForDrawing.size(); i++) {
		m_msComplexesForDrawing[i]->draw(mat);
	}
}

void MsComplexStorage::drawComplex(MsComplex* cmplx) {
	m_msComplexesForDrawing.push_back(cmplx);
}

std::vector<MsComplex*>& MsComplexStorage::getComplxesForDrawing() {
	return m_msComplexesForDrawing;
}

void MsComplexStorage::drawOriginal(Image& img) {
	for (typename MsComplexesSet::iterator it = m_complexesSet.begin(); it != m_complexesSet.end(); it++) {
		(*it)->drawOriginal(img);
	}
}

void MsComplexStorage::clear() {
	m_complexByMax.clear();
	m_complexBySaddle.clear();
	m_complexesSet.clear();
}

void MsComplexStorage::printInfo(std::vector<FacePtr>& v) {
	std::cout << "m_complexesSet size " << m_complexesSet.size() << std::endl;
	std::cout << "m_complexByMax size " << m_complexByMax.size() << std::endl;
	std::cout << "m_complexBySaddle size " << m_complexBySaddle.size() << std::endl;
}
