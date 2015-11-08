/*
 * ArcStorage.h
 *
 *  Created on: Jan 1, 2013
 *      Author: trifid
 */

#ifndef ARCSTORAGE_H_
#define ARCSTORAGE_H_

//#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/mem_fun.hpp>
//#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index_container.hpp>
#include <opencv2/core/core.hpp>
#include <stddef.h>
#include <vtkCellArray.h>
#include <vtkIdList.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyLine.h>
#include <vtkSmartPointer.h>
#include <vtkType.h>
#include <vtkXMLPolyDataWriter.h>
#include <stdint.h>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <vector>

#include "Arc.h"
#include "Edge.h"
//#include "MsComplex.h"
//#include "MsComplexStorage.h"
#include "SimplexStorage.h"
#include "Triangle.h"
#include "Vertex.h"
class Utils;
class MsComplex;

template<typename ArcPtr, typename CriticalPtr>
class MultiMapElement {

public:
	std::vector<ArcPtr> m_arcs;

	CriticalPtr critical() {
		return m_critical;
	}

	EdgePtr seddle() {
		return m_seddle;
	}

	void critical(CriticalPtr cp) {
		m_critical = cp;
	}

	void seddle(EdgePtr e) {
		m_seddle = e;
	}

protected:
	CriticalPtr m_critical;
	EdgePtr m_seddle;

};

template<typename ArcPtr, typename CriticalPtr>
class ArcStorage {
public:
	ArcStorage() {
	}
	virtual ~ArcStorage() {
	}

	void printAll(const std::string& prefix) {

		uint32_t totalArcs = 0;
		for (typename ArcsToSeddleMap::iterator it = m_arcsToSeddleMap.begin(); it != m_arcsToSeddleMap.end(); it++) {
			totalArcs += it->second.size();
			if (it->second.size() != 2) {
				std::cout << "ERROR: " << prefix << " storage incorrect arcs for seddle " << *(it->first) << " arcs: " << it->second.size()
						<< std::endl;
			}
		}
		std::cout << "total " << prefix << " arcs by seddle: " << totalArcs << std::endl;

		uint32_t totalArcsByMax = 0;
		for (typename ArcsToCriticalMap::iterator it = m_arcsToCriticalMap.begin(); it != m_arcsToCriticalMap.end(); it++) {
			totalArcsByMax += it->second.size();
			/*
			 std::vector<ArcPtr>& arcs = it->second;
			 for (size_t i = 0; i < arcs.size(); i++) {
			 std::cout << "arc begin: " << *(arcs[i]->m_arcBegin) << " end: " << *(arcs[i]->m_arcEnd->maxVertex()) << std::endl;
			 }
			 */
		}
		std::cout << "total " << prefix << " arcs by critical: " << totalArcsByMax << std::endl;

	}

	bool hasCommonSeddle(CriticalPtr a, CriticalPtr b) {
		typename ArcsToCriticalMap::iterator itA = m_arcsToCriticalMap.find(a);
		typename ArcsToCriticalMap::iterator itB = m_arcsToCriticalMap.find(b);

		if (itA != m_arcsToCriticalMap.end() && itB != m_arcsToCriticalMap.end()) {
			for (size_t i = 0; i < itA->second.size(); ++i) {
				for (size_t j = 0; j < itB->second.size(); ++j) {
					if (*(itA->second[i]->m_arcBegin) == *(itB->second[j]->m_arcBegin)) {
						return true;
					}
				}
			}
		}

		return false;

	}

	void addArc(ArcPtr arc) {
		m_arcsToCriticalMap[arc->m_arcEnd].push_back(arc);
		m_arcsToSeddleMap[arc->m_arcBegin].push_back(arc);
	}

	std::vector<ArcPtr>* seddles(CriticalPtr tr) {
		typename ArcsToCriticalMap::iterator it = m_arcsToCriticalMap.find(tr);
		if (it != m_arcsToCriticalMap.end())
			return &(it->second);
		return NULL;

	}

	std::vector<ArcPtr> * critical(EdgePtr seddle) {
		typename ArcsToSeddleMap::iterator it = m_arcsToSeddleMap.find(seddle);
		if (it != m_arcsToSeddleMap.end())
			return &(it->second);
		return NULL;

	}

	void registerArcByNewCritical(ArcPtr arc) {
		m_arcsToCriticalMap[arc->m_arcEnd].push_back(arc);
	}

	ArcPtr getArc(EdgePtr seddle, CriticalPtr crit) {
		if (seddle == NULL || crit == NULL) {
			std::cout << "ERROR. can't find simplex for ppair" << std::endl;
			return NULL;
		}

		std::vector<ArcPtr>* arcs = critical(seddle);
		if (arcs == NULL){
//			std::cout << "ERROR. can't find arcs for seddle " << *seddle << std::endl;
			return NULL;
		}

		for (size_t i = 0; i < arcs->size(); i++) {
			if (*(arcs->at(i)->m_arcEnd) == *(crit))
				return arcs->at(i);
		}
		std::cout << "ERROR. can't find arc by critical " << *crit  << std::endl;

		return NULL;
	}

	bool isInStorage(ArcPtr arc) {

		if (getArc(arc->m_arcBegin, arc->m_arcEnd) == NULL)
			return false;
		return true;
	}

	void erasePersistentPair(ArcPtr arc) {
		m_arcsToCriticalMap.erase(arc->m_arcEnd);
		m_arcsToSeddleMap.erase(arc->m_arcBegin);

	}

	void erase(EdgePtr seddle) {

		typename ArcsToSeddleMap::iterator it = m_arcsToSeddleMap.find(seddle);
		if (it == m_arcsToSeddleMap.end())
			return;
		std::vector<ArcPtr> arcs = it->second;

		for (size_t i = 0; i < arcs.size(); i++) {
			typename ArcsToCriticalMap::iterator it = m_arcsToCriticalMap.find(arcs[i]->m_arcEnd);
			if (it != m_arcsToCriticalMap.end()) {
				for (typename std::vector<ArcPtr>::iterator vit = it->second.begin(); vit != it->second.end(); ++vit) {
					if (*((*vit)->m_arcBegin) == *(seddle)) {
						it->second.erase(vit);
						break;
					}
				}
			}
		}

		m_arcsToSeddleMap.erase(it);
	}

	void eraseArc(ArcPtr arc) {
		typename ArcsToCriticalMap::iterator toMaxMapIt = m_arcsToCriticalMap.find(arc->m_arcEnd);

		bool removeSmth = false;
		if (toMaxMapIt != m_arcsToCriticalMap.end()) {
			for (typename std::vector<ArcPtr>::iterator arcIt = toMaxMapIt->second.begin(); arcIt != toMaxMapIt->second.end(); ++arcIt) {
				if (*arc == *(*arcIt)) {
					toMaxMapIt->second.erase(arcIt);
					removeSmth = true;
					break;
				}
			}
		}

		typename ArcsToSeddleMap::iterator toSaddleMapIt = m_arcsToSeddleMap.find(arc->m_arcBegin);
		if (toSaddleMapIt != m_arcsToSeddleMap.end()) {
			for (typename std::vector<ArcPtr>::iterator arcIt = toSaddleMapIt->second.begin(); arcIt != toSaddleMapIt->second.end(); ++arcIt) {
				if (*arc == *(*arcIt)) {
					toSaddleMapIt->second.erase(arcIt);
					removeSmth = true;
					break;
				}
			}
		}

		if(!removeSmth){
			std::cout << "WARNING: can't remove arc " << std::endl;
		}

	}

	ArcPtr getSecondLeg(EdgePtr sdl) {
		std::vector<ArcPtr>* arcs = critical(sdl);
		if (!arcs) {
			return NULL;
		}

		if (arcs->empty()) {
			return NULL;
		}

		if (arcs->size() != 1) {
			std::cout << "WARNING: have more than one lag for minimum " << *sdl << std::endl;
		}
		return arcs->at(0);

	}

	typedef std::map<EdgePtr, std::vector<ArcPtr>, EdgesComparator> ArcsToSeddleMap;
	typedef std::map<CriticalPtr, std::vector<ArcPtr>, SimplexComparator<CriticalPtr> > ArcsToCriticalMap;

	ArcsToSeddleMap& arcsToSeddleMap() {
		return m_arcsToSeddleMap;
	}

/*
	void saveArcsInVtp(const std::string& path) {

		vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
		vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();

		for (typename ArcsToSeddleMap::iterator it = m_arcsToSeddleMap.begin(); it != m_arcsToSeddleMap.end(); it++) {
			std::vector<ArcPtr> &arcs = it->second;
			for (size_t i = 0; i < arcs.size(); ++i) {
				ArcPtr arc = arcs[i];
				vtkSmartPointer<vtkPolyLine> polyLine = vtkSmartPointer<vtkPolyLine>::New();
				polyLine->GetPointIds()->SetNumberOfIds(arc->m_arc.size() + 2);

				vtkIdType id = points->InsertNextPoint(arc->m_arcBegin->maxVertex()->x, arc->m_arcBegin->maxVertex()->y, 0);
				polyLine->GetPointIds()->SetId(0, id);
				uint32_t j = 1;
				for (; j < arc->m_arc.size() + 1; ++j) {
					VertexPtr vtx = arc->m_arc[j - 1]->maxVertex();
					id = points->InsertNextPoint(vtx->x, vtx->y, 0);
//					std::cout << "inset pt x: " <<  vtx->x << " y: " << vtx->y<< std::endl;
					polyLine->GetPointIds()->SetId(j, id);
				}
				id = points->InsertNextPoint(arc->m_arcEnd->maxVertex()->x, arc->m_arcEnd->maxVertex()->y, 0);
				polyLine->GetPointIds()->SetId(j, id);

				cells->InsertNextCell(polyLine);
			}
		}
		vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
		polyData->SetPoints(points);

		polyData->SetLines(cells);
		vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
		writer->SetFileName(path.c_str());
		writer->SetInput(polyData);
		writer->SetDataModeToAscii();
		writer->Write();

	}
*/

	ArcsToSeddleMap m_arcsToSeddleMap;
	ArcsToCriticalMap m_arcsToCriticalMap;

protected:

	MsComplex* completeComplex(Arc<FacePtr, FacePtr> * aArc1, DescArcPtr dArc1, std::vector<Arc<FacePtr, FacePtr> *>* aArcs, DescArcs* dArcs);

	typedef boost::multi_index_container<MultiMapElement<ArcPtr, CriticalPtr>*,
			boost::multi_index::indexed_by<
					boost::multi_index::ordered_unique<
							boost::multi_index::mem_fun<MultiMapElement<ArcPtr, CriticalPtr>, CriticalPtr,
									&MultiMapElement<ArcPtr, CriticalPtr>::critical>, SimplexComparator<CriticalPtr> >,
					boost::multi_index::ordered_unique<
							boost::multi_index::mem_fun<MultiMapElement<ArcPtr, CriticalPtr>, EdgePtr, &MultiMapElement<ArcPtr, CriticalPtr>::seddle>,
							SimplexComparator<EdgePtr> > > > ArcsMap;

//	ArcsMap m_map;

	Arc<FacePtr, FacePtr> * findAscArcMax(std::vector<Arc<FacePtr, FacePtr> *> &ascArcs);

};

typedef ArcStorage<AscArcPtr, FacePtr> AscArcStorage;
typedef ArcStorage<DescArcPtr, VertexPtr> DescArcStorage;

#endif /* ARCSTORAGE_H_ */
