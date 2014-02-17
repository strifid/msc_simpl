/*
 * Utils.cpp
 *
 *  Created on: Feb 20, 2013
 *      Author: trifid
 */

#include "MsComplexEditor.h"
#include <iosfwd>
#include <iostream>
#include <fstream>

Vertexes& MsComplexEditor::getAscArcTrace(const std::vector<FacePtr>& ascArc, VertexPtr max) {

	m_pointsReturnVector.clear();
	for (size_t i = 0; i < ascArc.size() - 1; ++i) {
		EdgePtr commonEdge = ascArc[i]->findCommonEdge(ascArc[i + 1]);
		if (commonEdge) {
			if (*max != *(commonEdge->m_a))
				m_pointsReturnVector.push_back(commonEdge->m_a);
		} else {

			/*
			 std::cout << "can't find common edge for " << *(ascArc[i]->maxVertex()) << " end " << *(ascArc[i + 1]->maxVertex()) << " max " << *max << std::endl;
			 std::cout << "a " << *(ascArc[i]) << std::endl;
			 std::cout << "b " << *(ascArc[i + 1]) << std::endl;

			 std::cout << "arc size: " << ascArc.size() << std::endl;
			 for (size_t i = 0; i < ascArc.size() - 1; ++i) {
			 std::cout << *(ascArc[i]) << std::endl;
			 }
			 exit(1);
			 */
		}
	}
	return m_pointsReturnVector;
}

Vertexes& MsComplexEditor::getDescArcTrace(const std::vector<EdgePtr>& descArc) {

	m_pointsReturnVector.clear();
	if (descArc.empty())
		return m_pointsReturnVector;

	for (size_t i = 0; i < descArc.size() - 1; ++i) {
		VertexPtr vtx = descArc[i]->findCommonVertex(descArc[i + 1]);
		if (vtx != NULL)
			m_pointsReturnVector.push_back(vtx);
		else
			std::cout << "can't find common vertex for " << *(descArc[i]) << " end " << *(descArc[i + 1]) << std::endl;
	}
	return m_pointsReturnVector;
}

void MsComplexEditor::process(MsComplex* cmplx) {

	for (size_t j = 0; j < cmplx->m_aArcs.size(); ++j) {
		AscArcPtr ascArc = cmplx->m_aArcs[j];
		if (m_processedAscArcs.find(ascArc) == m_processedAscArcs.end()) {
			Vertexes& points = getAscArcTrace(ascArc->m_arc, cmplx->m_max->maxVertex());
			addArcPoints(points, cmplx->m_max->maxVertex()->value(), ascArc->m_arcBegin->m_a->value());
			m_generateField[std::make_pair(cmplx->m_max->maxVertex()->x, cmplx->m_max->maxVertex()->y)] = cmplx->m_max->maxVertex()->value();
			m_processedAscArcs.insert(ascArc);
		}
	}

	for (size_t j = 0; j < cmplx->m_dArcs.size(); ++j) {
		DescArcPtr descArc = cmplx->m_dArcs[j];
		if (m_processedDescArcs.find(descArc) == m_processedDescArcs.end()) {
			Vertexes& points = getDescArcTrace(descArc->m_arc);
			addArcPoints(points, descArc->m_arcBegin->m_a->value(), cmplx->m_min->value());
			m_generateField[std::make_pair(cmplx->m_min->x, cmplx->m_min->y)] = cmplx->m_min->value();
			m_processedDescArcs.insert(descArc);
		}
	}

}

bool MsComplexEditor::save(const std::string& path) {

	return true;
}

void MsComplexEditor::addArcPoints(Vertexes& points, uint32_t max, uint32_t min) {
	std::stringstream out;

	if (points.empty())
		return;

	if (points.size() < 3) {
		for (size_t i = 0; i < points.size(); ++i) {
			m_generateField[std::make_pair(points[i]->x, points[i]->y)] = points[i]->value();
		}
		return;
	}

	if (max < min) {
		std::cout << "asc arc: min greater than max" << std::endl;
		return;
	}
	double step = (double)(max - min) / (points.size() + 1);

	for (size_t i = 0; i < points.size(); ++i) {
		uint32_t value = min + step * (i + 1);
		out << " x " << points[i]->x << " y " << points[i]->y << " value: " << value;
		m_generateField[std::make_pair(points[i]->x, points[i]->y)] = value;
	}
}

void MsComplexEditor::saveEditMap(const std::string& path) {

	std::ofstream coordFile;
	coordFile.open(path.c_str());
	coordFile << "x\ty\tz" << std::endl;

	for (std::map<std::pair<int, int>, int>::iterator it = m_generateField.begin(); it != m_generateField.end(); it++) {
		//if (it->second < 255 && it->second >= 0)
		coordFile << it->first.first << "\t" << it->first.second << "\t" << it->second << std::endl;

	}
	coordFile.close();
}

