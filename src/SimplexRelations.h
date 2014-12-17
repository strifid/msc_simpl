/*
 * SimplexRelations.h
 *
 *  Created on: Dec 25, 2012
 *      Author: trifid
 */

#ifndef SIMPLEXRELATIONS_H_
#define SIMPLEXRELATIONS_H_
#include "Vertex.h"
#include "Edge.h"
#include "Triangle.h"
#include <map>
#include "SimplexStorage.h"

class SimplexRelations {
public:

	SimplexRelations() {
	}
	virtual ~SimplexRelations() {
	}
	void push(VertexPtr vtx);
	void push(EdgePtr vtx);
	void push(FacePtr vtx);

	Vertexes* vertexes(EdgePtr face);
	Edges* edges(VertexPtr face);

	std::vector<FacePtr>* face(EdgePtr face);

private:

	class Relations {
	public:
		Vertexes m_vtxs;
		Edges m_edges;
		Faces m_faces;
	};

	Relations* find(VertexPtr vtx) {
		typename VertexRelationsMap::iterator it = m_vertexRelations.find(vtx);
		if (it == m_vertexRelations.end()) {
			return NULL;
		}
		return it->second;
	}

	Relations* find(EdgePtr face) {
		typename EdgeRelationsMap::iterator it = m_edgeRelations.find(face);
		if (it == m_edgeRelations.end()) {
			return NULL;
		}
		return it->second;
	}

	typedef std::map<VertexPtr, Relations*, VertexPtrComparator> VertexRelationsMap;
	VertexRelationsMap m_vertexRelations;
	typedef std::map<EdgePtr, Relations*, EdgesComparator> EdgeRelationsMap;
	EdgeRelationsMap m_edgeRelations;

};



#endif /* SIMPLEXRELATIONS_H_ */
