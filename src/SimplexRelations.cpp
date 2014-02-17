#include "SimplexRelations.h"

void SimplexRelations::push(VertexPtr vtx) {
	m_vertexRelations[vtx] = new Relations();
}


void SimplexRelations::push(EdgePtr edge) {
	Relations *rl = new Relations();
	rl->m_vtxs.push_back(edge->m_a);
	rl->m_vtxs.push_back(edge->m_b);

	m_edgeRelations[edge] = rl;

	rl = find(edge->m_a);
	if (rl)
		rl->m_edges.push_back(edge);
	rl = find(edge->m_b);
	if (rl)
		rl->m_edges.push_back(edge);

}


void SimplexRelations::push(FacePtr tr) {


	for (EdgesSet::iterator it = tr->m_edges.begin(); it != tr->m_edges.end(); it++) {
		Relations *rl = find(*it);
		if (rl)
			rl->m_faces.push_back(tr);
	}
}


Vertexes* SimplexRelations::vertexes(EdgePtr edge) {
	typename EdgeRelationsMap::iterator it = m_edgeRelations.find(edge);
	if (it == m_edgeRelations.end()) {
		return NULL;
	}
	return &(it->second->m_vtxs);
}


std::vector<FacePtr> * SimplexRelations::face(EdgePtr face) {
	typename EdgeRelationsMap::iterator it = m_edgeRelations.find(face);
	if (it == m_edgeRelations.end()) {
		return NULL;
	}
	return &(it->second->m_faces);

}


Edges * SimplexRelations::edges(VertexPtr vtx) {
	typename VertexRelationsMap::iterator it = m_vertexRelations.find(vtx);
	if (it == m_vertexRelations.end()) {
		return NULL;
	}
	return &(it->second->m_edges);
}

