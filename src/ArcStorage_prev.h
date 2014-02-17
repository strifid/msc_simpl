/*
 * ArcStorage.h
 *
 *  Created on: Jan 1, 2013
 *      Author: trifid
 */

#ifdef NEVERUSE_H_
#define ARCSTORAGE_H_

#include "Edge.h"
#include "Arc.h"
#include "MsComplex.h"
#include "Triangle.h"
#include "Vertex.h"
#include "MsComplexStorage.h"

template<typename FacePtr>
class ArcStorage {
public:
	ArcStorage() {
	}
	virtual ~ArcStorage() {
	}

	void printAll() {

		uint32_t totalArcs = 0;
		for (typename AscArcsToSeddleMap::iterator it = m_ascArcsToSeddlesMap.begin(); it != m_ascArcsToSeddlesMap.end(); it++) {
			totalArcs+= it->second.size();
			if(it->second.size() > 2){
				std::cout<< "incorrect arcs for seddle " << *(it->first)  << " arcs: " << it->second.size() << std::endl;
			}
		}
		std::cout << "total asc arcs by seddle: " << totalArcs << std::endl;


		uint32_t totalArcsByMax = 0;
		for (typename AscArcsToMaxMap::iterator it = m_ascArcsToMaxMap.begin(); it != m_ascArcsToMaxMap.end(); it++) {
			totalArcsByMax+= it->second.size();
		}
		std::cout << "total asc arcs by max: " << totalArcsByMax << std::endl;

		if(totalArcsByMax != totalArcs)
			exit(0);

	}

	bool hasCommonSeddle(VertexPtr a, VertexPtr b) {
		DescArcsToMinMap::iterator itA = m_descArcsToMinMap.find(a);
		DescArcsToMinMap::iterator itB = m_descArcsToMinMap.find(b);

		if (itA != m_descArcsToMinMap.end() && itB != m_descArcsToMinMap.end()) {
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

	bool hasCommonSeddle(FacePtr a, FacePtr b) {
		typename AscArcsToMaxMap::iterator itA = m_ascArcsToMaxMap.find(a);
		typename AscArcsToMaxMap::iterator itB = m_ascArcsToMaxMap.find(b);

		if (itA != m_ascArcsToMaxMap.end() && itB != m_ascArcsToMaxMap.end()) {
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

	void addAscArc(EdgePtr saddle, std::vector<FacePtr> &arc, FacePtr max);
	void addDescArc(EdgePtr saddle, Edges &arc, VertexPtr min);

	std::vector<Arc<FacePtr, FacePtr> *>* seddles(FacePtr tr);
	DescArcs* seddles(VertexPtr tr);
	std::vector<Arc<FacePtr, FacePtr> *>* max(EdgePtr face);
	DescArcs* min(EdgePtr face);

	void connectArcs(MsComplexStorage<FacePtr>& msComplexes, uint32_t persist);

	void registerDescArcByNewMin(DescArcPtr arc) {
		m_descArcsToMinMap[arc->m_arcEnd].push_back(arc);
	}

	void registerAscArcByNewMax(AscArcPtr arc) {
		m_ascArcsToMaxMap[arc->m_arcEnd].push_back(arc);
	}

	void eraseDescPersistentPair(DescArcPtr arc) {
		m_descArcsToMinMap.erase(arc->m_arcEnd);
		m_descArcsToSeddlesMap.erase(arc->m_arcBegin);
		typename AscArcsToSeddleMap::iterator it = m_ascArcsToSeddlesMap.find(arc->m_arcBegin);
		if (it == m_ascArcsToSeddlesMap.end())
			return;

		for (size_t i = 0; i < it->second.size(); i++) {
			eraseAscArc(it->second[i]);
		}
		m_ascArcsToSeddlesMap.erase(arc->m_arcBegin);

	}

	void eraseAscPersistentPair(AscArcPtr arc) {
		m_ascArcsToMaxMap.erase(arc->m_arcEnd);
		m_ascArcsToSeddlesMap.erase(arc->m_arcBegin);
		typename DescArcsToSeddleMap::iterator it = m_descArcsToSeddlesMap.find(arc->m_arcBegin);
		if (it == m_descArcsToSeddlesMap.end())
			return;

		for (size_t i = 0; i < it->second.size(); i++) {
			eraseDescArc(it->second[i]);
		}

		m_descArcsToSeddlesMap.erase(arc->m_arcBegin);

	//	printAll();
	}

	void eraseDescArc(DescArcPtr arc) {
		DescArcsToMinMap::iterator toMinMapIt = m_descArcsToMinMap.find(arc->m_arcEnd);
		if (toMinMapIt != m_descArcsToMinMap.end()) {

			for (DescArcs::iterator arcIt = toMinMapIt->second.begin(); arcIt != toMinMapIt->second.end(); ++arcIt) {
				if (*(arc->m_arcBegin) == *((*arcIt)->m_arcBegin)) {
					toMinMapIt->second.erase(arcIt);
					break;
				}
			}
		}

		DescArcsToSeddleMap::iterator toSaddleMapIt = m_descArcsToSeddlesMap.find(arc->m_arcBegin);
		if (toSaddleMapIt != m_descArcsToSeddlesMap.end()) {
			for (DescArcs::iterator arcIt = toSaddleMapIt->second.begin(); arcIt != toSaddleMapIt->second.end(); ++arcIt) {
				if (*(arc->m_arcEnd) == *((*arcIt)->m_arcEnd)) {
					toSaddleMapIt->second.erase(arcIt);
//					std::cout << "find and delete arc to seddle beg " << *(arc->m_arcBegin) << " end "  << *(arc->m_arcEnd)  << std::endl;
					break;
				}
			}
		}

	}

	void eraseAscArc(AscArcPtr arc) {
		typename AscArcsToMaxMap::iterator toMaxMapIt = m_ascArcsToMaxMap.find(arc->m_arcEnd);

		if (toMaxMapIt != m_ascArcsToMaxMap.end()) {

			for (AscArcs::iterator arcIt = toMaxMapIt->second.begin(); arcIt != toMaxMapIt->second.end(); ++arcIt) {
				if (*(arc->m_arcBegin) == *((*arcIt)->m_arcBegin)) {
					toMaxMapIt->second.erase(arcIt);
					break;
				}
			}
		}

		typename AscArcsToSeddleMap::iterator toSaddleMapIt = m_ascArcsToSeddlesMap.find(arc->m_arcBegin);
		if (toSaddleMapIt != m_ascArcsToSeddlesMap.end()) {
			for (AscArcs::iterator arcIt = toSaddleMapIt->second.begin(); arcIt != toSaddleMapIt->second.end(); ++arcIt) {
				if (*(arc->m_arcEnd) == *((*arcIt)->m_arcEnd)) {
					toSaddleMapIt->second.erase(arcIt);
					break;
				}
			}
		}

	}

	DescArcPtr getSecondDescLeg(EdgePtr sdl) {
		DescArcs* arcs = min(sdl);
		if (!arcs) {
			return NULL;
		}

		if (arcs->empty()) {
			return NULL;
		}

		if (arcs->size() != 1){
			std::cout << "WARNING: have more than one lag for minimum " << *sdl << std::endl;
		}
		return arcs->at(0);

	}

	AscArcPtr getSecondAscLeg(EdgePtr sdl) {
		AscArcs* arcs = max(sdl);
		if (!arcs) {
			return NULL;
		}

		if (arcs->empty()) {
			return NULL;
		}

		if (arcs->size() != 1)
			std::cout << "WARNING: have more than one lag for max " << *sdl << std::endl;
		return arcs->at(0);

	}

private:

	MsComplex<FacePtr>* completeComplex(Arc<FacePtr, FacePtr> * aArc1, DescArcPtr dArc1,
			std::vector<Arc<FacePtr, FacePtr> *>* aArcs, DescArcs* dArcs);

	typedef std::map<VertexPtr, DescArcs, VertexPtrComparator> DescArcsToMinMap;
	typedef std::map<FacePtr, std::vector<Arc<FacePtr, FacePtr> *>, SimplexComparator<FacePtr> > AscArcsToMaxMap;
	typedef std::map<EdgePtr, std::vector<Arc<FacePtr, FacePtr> *>, EdgesComparator> AscArcsToSeddleMap;
	typedef std::map<EdgePtr, DescArcs, EdgesComparator> DescArcsToSeddleMap;

	AscArcsToMaxMap m_ascArcsToMaxMap;
	DescArcsToMinMap m_descArcsToMinMap;

	AscArcsToSeddleMap m_ascArcsToSeddlesMap;
	DescArcsToSeddleMap m_descArcsToSeddlesMap;

	Arc<FacePtr, FacePtr> * findAscArcMax(std::vector<Arc<FacePtr, FacePtr> *> &ascArcs);

};

template<typename FacePtr>
void ArcStorage<FacePtr>::addAscArc(EdgePtr saddle, std::vector<FacePtr> &line, FacePtr max) {
	Arc<FacePtr, FacePtr> *arc = new Arc<FacePtr, FacePtr>(saddle, line, max);
	m_ascArcsToMaxMap[max].push_back(arc);
	m_ascArcsToSeddlesMap[saddle].push_back(arc);
}

template<typename FacePtr>
void ArcStorage<FacePtr>::addDescArc(EdgePtr saddle, Edges &line, VertexPtr min) {
	DescArc *arc = new DescArc(saddle, line, min);
	m_descArcsToMinMap[min].push_back(arc);
	m_descArcsToSeddlesMap[saddle].push_back(arc);
}

template<typename FacePtr>
std::vector<Arc<FacePtr, FacePtr> *> * ArcStorage<FacePtr>::seddles(FacePtr tr) {
	typename AscArcsToMaxMap::iterator it = m_ascArcsToMaxMap.find(tr);
	if (it != m_ascArcsToMaxMap.end())
		return &(it->second);
	return NULL;
}

template<typename FacePtr>
DescArcs * ArcStorage<FacePtr>::seddles(VertexPtr vt) {
	DescArcsToMinMap::iterator it = m_descArcsToMinMap.find(vt);
	if (it != m_descArcsToMinMap.end())
		return &(it->second);
	return NULL;
}

template<typename FacePtr>
std::vector<Arc<FacePtr, FacePtr> *> * ArcStorage<FacePtr>::max(EdgePtr face) {
	typename AscArcsToSeddleMap::iterator it = m_ascArcsToSeddlesMap.find(face);
	if (it != m_ascArcsToSeddlesMap.end())
		return &(it->second);
	return NULL;

}

template<typename FacePtr>
MsComplex<FacePtr>* ArcStorage<FacePtr>::completeComplex(Arc<FacePtr, FacePtr> * aArc1, DescArcPtr dArc1,
		std::vector<Arc<FacePtr, FacePtr> *> *aArcs, DescArcs *dArcs) {

	if (!dArcs || !aArc1 || !dArc1 || !aArcs)
		return NULL;

	Arc<FacePtr, FacePtr> * aArc2 = NULL;
	DescArcPtr dArc2 = NULL;
	bool hasCommonSaddle = false;
	EdgePtr commonSaddle;

	for (size_t i = 0; i < aArcs->size(); ++i) {
		aArc2 = aArcs->at(i);
		if (*aArc2 == *aArc1)
			continue;

		for (size_t z = 0; z < dArcs->size(); ++z) {
			dArc2 = dArcs->at(z);
			if (*dArc2 == *dArc1)
				continue;

			if (*(aArc2->m_arcBegin) == *(dArc2->m_arcBegin)) {
				hasCommonSaddle = true;
				commonSaddle = aArc2->m_arcBegin;
				break;
			}
		}

		if (hasCommonSaddle)
			break;
	}

	MsComplex<FacePtr>* msCompl = NULL;

	if (hasCommonSaddle) {
		msCompl = new MsComplex<FacePtr>();
		msCompl->m_seddles.push_back(commonSaddle);
		msCompl->m_aArcs.push_back(aArc1);
		msCompl->m_aArcs.push_back(aArc2);
		msCompl->m_dArcs.push_back(dArc1);
		msCompl->m_dArcs.push_back(dArc2);
	}

	return msCompl;
}

template<typename FacePtr>
DescArcs *ArcStorage<FacePtr>::min(EdgePtr face) {
	typename DescArcsToSeddleMap::iterator it = m_descArcsToSeddlesMap.find(face);
	if (it != m_descArcsToSeddlesMap.end())
		return &(it->second);
	return NULL;
}

template<typename FacePtr>
Arc<FacePtr, FacePtr> * ArcStorage<FacePtr>::findAscArcMax(std::vector<Arc<FacePtr, FacePtr> *> & ascArcs) {
	if (ascArcs.size() < 1)
		return NULL;

	Arc<FacePtr, FacePtr> * maxArc = ascArcs[0];

	for (size_t i = 1; i < ascArcs.size(); i++) {
		if (maxArc->m_arcEnd->maxVertex()->value() < ascArcs[i]->m_arcEnd->maxVertex()->value())
			maxArc = ascArcs[i];
	}

	return maxArc;
}

template<typename FacePtr>
void ArcStorage<FacePtr>::connectArcs(MsComplexStorage<FacePtr>& msComplexes, uint32_t persist) {

	for (typename AscArcsToSeddleMap::iterator it = m_ascArcsToSeddlesMap.begin(); it != m_ascArcsToSeddlesMap.end(); it++) {

		DescArcs* descArcs = min(it->first);
		std::vector<Arc<FacePtr, FacePtr> *>* ascArcs = &(it->second);
		if (!descArcs)
			continue;

		for (size_t i = 0; i < ascArcs->size(); i++) {
			FacePtr max = ascArcs->at(i)->m_arcEnd;
			std::vector<Arc<FacePtr, FacePtr> *>* aArcs = seddles(max);

			for (size_t z = 0; z < descArcs->size(); z++) {

				//todo somthing strange
				VertexPtr min = descArcs->at(z)->m_arcEnd;

				if (max->m_valueFirst < min->m_valueFirst + persist)
					continue;

				MsComplex<FacePtr> *msComplex = completeComplex(ascArcs->at(i), descArcs->at(z), aArcs, seddles(min));
				if (msComplex) {
					msComplex->m_max = max;
					msComplex->m_min = min;
					msComplex->m_seddles.push_back(it->first);
					if (msComplexes.isExist(msComplex))
						delete msComplex;
					else
						msComplexes.addComplex(msComplex);
				}
			}
		}
	}

}

#endif /* ARCSTORAGE_H_ */
