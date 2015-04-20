/*
 * $Revision: 1.2 $
 * 
 * last checkin:
 *   $Author: klein $ 
 *   $Date: 2007-11-14 16:18:10 +0100 (Mi, 14 Nov 2007) $ 
 ***************************************************************/
 
/** \file
 * \brief Implements class FaceSinkGraph
 * 
 * \author Carsten Gutwenger
 * 
 * \par License:
 * This file is part of the Open Graph Drawing Framework (OGDF).
 * Copyright (C) 2005-2007
 * 
 * \par
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * Version 2 or 3 as published by the Free Software Foundation
 * and appearing in the files LICENSE_GPL_v2.txt and
 * LICENSE_GPL_v3.txt included in the packaging of this file.
 *
 * \par
 * In addition, as a special exception, you have permission to link
 * this software with the libraries of the COIN-OR Osi project
 * (http://www.coin-or.org/projects/Osi.xml), all libraries required
 * by Osi, and all LP-solver libraries directly supported by the
 * COIN-OR Osi project, and distribute executables, as long as
 * you follow the requirements of the GNU General Public License
 * in regard to all of the software in the executable aside from these
 * third-party libraries.
 * 
 * \par
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * \par
 * You should have received a copy of the GNU General Public 
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 * 
 * \see  http://www.gnu.org/copyleft/gpl.html
 ***************************************************************/


#include <ogdf/upward/FaceSinkGraph.h>


namespace ogdf {


// construction of face sink graph with cross references
FaceSinkGraph::FaceSinkGraph(
	const ConstCombinatorialEmbedding &E, // given embedding
	node s) :                             // single source
	m_pE            (&E),
	m_source        (s),
	m_T             (0),
	m_originalNode  (*this,0),
	m_originalFace  (*this,0),
	m_containsSource(*this,false)
{
	doInit();
}


void FaceSinkGraph::init(
	const ConstCombinatorialEmbedding &E, // given embedding
	node s)                               // single source
{
	m_pE     = &E;
	m_source = s;
	m_T      = 0;
	m_originalNode  .init(*this,0);
	m_originalFace  .init(*this,0);
	m_containsSource.init(*this,false);

	doInit();
}


void FaceSinkGraph::doInit()
{
	const ConstCombinatorialEmbedding &E = *m_pE;

	NodeArray<node> sinkSwitch(E,0); // corresponding node in F (if any)
	NodeArray<bool> isSinkSwitch(E,true);

	face f;
	forall_faces(f,E)
	{
		node faceNode = newNode();
		m_originalFace[faceNode] = f;

		SListPure<node> nodesInF;

		adjEntry adj1 = f->firstAdj(), adj = adj1;
		do {
			node v = adj->theNode();
			nodesInF.pushBack(v);

			if (v == m_source)
				m_containsSource[faceNode] = true;

			isSinkSwitch[adj->theEdge()->source()] = false;

			adj = adj->twin()->cyclicPred();
		} while (adj != adj1);

		SListConstIterator<node> it;
		for(it = nodesInF.begin(); it.valid(); ++it)
		{
			node v = *it;
			if(isSinkSwitch[v])	{
				if (sinkSwitch[v] == 0) {
					node vF = newNode();
					m_originalNode[vF] = v;
					sinkSwitch[v] = vF;
				}

				newEdge(faceNode,sinkSwitch[v]);
			}
		}

		for(it = nodesInF.begin(); it.valid(); ++it)
			isSinkSwitch[*it] = true;
	}
}




/* original:
void FaceSinkGraph::doInit()
{
	const ConstCombinatorialEmbedding &E = *m_pE;

	NodeArray<node> sinkSwitch(E,0); // corresponding node in F (if any)

	face f;
	forall_faces(f,E)
	{
		node faceNode = newNode();
		m_originalFace[faceNode] = f;

		adjEntry adj1 = f->firstAdj(), adj = adj1;
		adjEntry adjPred = adj->cyclicSucc();
		do {
			node v = adj->theNode();

			if (v == m_source)
				m_containsSource[faceNode] = true;

			// v is a sink-switch iff both adjacent edges (there are only two
			// in f since G is biconnected) are directed towards v
			if(adj->theEdge()->target() == v && 
				adjPred->theEdge()->target() == v)
			{
				if (sinkSwitch[v] == 0) {
					node vF = newNode();
					m_originalNode[vF] = v;
					sinkSwitch[v] = vF;
				}

				newEdge(faceNode,sinkSwitch[v]);
			}

			adjPred = adj->twin();
			adj = adjPred->cyclicPred();
		} while (adj != adj1);
	}
}*/


// checks if F is a forest with
//  1) there exactly one tree T containing no internal vertex of G
//  2) all other trees contain exactly one internal vertex of G
// a node in tree T is returned as representative
node FaceSinkGraph::checkForest()
{
	// representative of tree T (0 indicates none found yet)
	m_T = 0;

	// we perform a dfs traversal on F and check if there are backwards edges
	// (then F is not a forest)
	NodeArray<bool> visited(*this,false);

	node v;
	forall_nodes(v,*this)
	{
		if (visited[v]) continue;

		// number of internal vertices in current tree
		int nInternalVertices = 0;
		if (dfsCheckForest(v,0,visited,nInternalVertices) == 0)
			return 0;

		// either we have a unique tree with no internal vertices
		if(nInternalVertices == 0) {
			if(m_T)
				return 0;
			else
				m_T = v;

		// or we have exactly one internal vertex
		} else if (nInternalVertices != 1)
			return 0;
	}

	return m_T;
}


// performs dfs-traversal and checks for backwards edges
bool FaceSinkGraph::dfsCheckForest(
	node v,                              // current node
	node parent,                         // its parent in tree
	NodeArray<bool> &visited,            // not already visited ?
	int &nInternalVertices) // number of internal vertices of G in current tree
{
	visited[v] = true;

	// check if original node of v is an internal vertex in G
	node vOrig = m_originalNode[v];
	if(vOrig && vOrig->indeg() >= 1 && vOrig->outdeg() >= 1)
		++nInternalVertices;

	// iterate over all adjacent nodes of v different from parent
	adjEntry adj;
	forall_adj(adj,v)
	{
		node w = adj->twin()->theNode();

		if (w == parent) continue;
		if(visited[w]) return false;

		if(dfsCheckForest(w,v,visited,nInternalVertices) == false)
			return false;
	}

	return true;
}



// builds list of possible external faces (all faces in tree T containing
// the single source s) by a dfs traversal of T
void FaceSinkGraph::gatherExternalFaces(
	node v,                                // current node
	node parent,                           // its parent
	SList<face> &externalFaces)      // returns list of possible external faces
{
	if (m_containsSource[v])
		externalFaces.pushBack(m_originalFace[v]);

	// since we already know that T is a tree we can omit the visited array
	adjEntry adj;
	forall_adj(adj,v)
	{
		node w = adj->twin()->theNode();

		if (w != parent)
			gatherExternalFaces(w,v,externalFaces);
	}
}


node FaceSinkGraph::dfsFaceNodeOf(node v, node parent, face f1, face f2)
{
	face f = m_originalFace[v];
	if (m_containsSource[v] && (f == f1 || f == f2))
		return v;

	// since we already know that T is a tree we can omit the visited array
	adjEntry adj;
	forall_adj(adj,v)
	{
		node w = adj->twin()->theNode();

		if (w != parent) {
			node found = dfsFaceNodeOf(w,v,f1,f2);
			if (found != 0)
				return found;
		}
	}

	return 0;
}


// original variant of st-augmentation
// Inserts also new nodes representing faces into G.
void FaceSinkGraph::stAugmentation(
	node h,                       // node corresponding to external face
	Graph &G,                     // original graph (not const)
	SList<node> &augmentedNodes,  // list of augmented nodes
	SList<edge> &augmentedEdges)  // list of augmented edges
{
	SListPure<node> roots;
	node v;
	forall_nodes(v,*this) {
		node vOrig = m_originalNode[v];
		if (vOrig != 0 && vOrig->indeg() > 0 && vOrig->outdeg() > 0)
			roots.pushBack(v);
	}

	node vh = dfsStAugmentation(h,0,G,augmentedNodes,augmentedEdges);

	SListConstIterator<node> it;
	for(it = roots.begin(); it.valid(); ++it)
		dfsStAugmentation(*it,0,G,augmentedNodes,augmentedEdges);

	augmentedEdges.pushBack(G.newEdge(m_source,vh));

}


node FaceSinkGraph::dfsStAugmentation(
	node v,                      // current node
	node parent,                 // its parent
	Graph &G,                    // original graph (not const)
	SList<node> &augmentedNodes, // list of augmented nodes
	SList<edge> &augmentedEdges) // list of augmented edges
{
	bool isFace = (m_originalFace[v] != 0);
	node vf = 0;

	// since we already know that T is a tree we can omit the visited array
	adjEntry adj;
	forall_adj(adj,v)
	{
		node w = adj->twin()->theNode();

		if (w == parent) continue;

		if (isFace) {
			if (vf == 0) {
				vf = G.newNode();
				augmentedNodes.pushBack(vf);
				if (parent) {
					edge eParent = G.newEdge(vf,m_originalNode[parent]);
					augmentedEdges.pushBack(eParent);
				}
			}

			edge ew = G.newEdge(m_originalNode[w],vf);
			augmentedEdges.pushBack(ew);
		}

		dfsStAugmentation(w,v,G,augmentedNodes,augmentedEdges);
	}

	return vf;
}


// improved variant of st-augmentation
// Inserts also one new node representing the super sink into G.
void FaceSinkGraph::stAugmentation(
	node  h,                      // node corresponding to external face
	Graph &G,                     // original graph (not const)
	node  &superSink,             // super sink
	SList<edge> &augmentedEdges)  // list of augmented edges
{
	SListPure<node> roots;
	node v;
	forall_nodes(v,*this) {
		node vOrig = m_originalNode[v];
		if (vOrig != 0 && vOrig->indeg() > 0 && vOrig->outdeg() > 0)
			roots.pushBack(v);
	}


	superSink = dfsStAugmentation(h,0,G,augmentedEdges);

	SListConstIterator<node> it;
	for(it = roots.begin(); it.valid(); ++it)
		dfsStAugmentation(*it,0,G,augmentedEdges);

	augmentedEdges.pushBack(G.newEdge(m_source,superSink));

}


node FaceSinkGraph::dfsStAugmentation(
	node v,                      // current node
	node parent,                 // its parent
	Graph &G,                    // original graph (not const)
	SList<edge> &augmentedEdges) // list of augmented edges
{
	bool isFace = (m_originalFace[v] != 0);
	node vf = (parent != 0) ? m_originalNode[parent] : 0;

	// since we already know that T is a tree we can omit the visited array
	adjEntry adj;
	forall_adj(adj,v)
	{
		node w = adj->twin()->theNode();

		if (w == parent) continue;

		if (isFace) {
			if (vf == 0) {
				vf = G.newNode();
			}

			edge ew = G.newEdge(m_originalNode[w],vf);
			augmentedEdges.pushBack(ew);
		}

		dfsStAugmentation(w,v,G,augmentedEdges);
	}

	return vf;
}



} // end namespace ogdf

