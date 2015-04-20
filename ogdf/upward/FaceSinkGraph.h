/*
 * $Revision: 1.3 $
 * 
 * last checkin:
 *   $Author: klein $ 
 *   $Date: 2007-11-14 16:15:44 +0100 (Mi, 14 Nov 2007) $ 
 ***************************************************************/
 
/** \file
 * \brief Declaration of class FaceSinkGraph
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

#ifdef _MSC_VER
#pragma once
#endif

#ifndef OGDF_FACE_SINK_GRAPH_H
#define OGDF_FACE_SINK_GRAPH_H


#include <ogdf/basic/CombinatorialEmbedding.h>
#include <ogdf/basic/NodeArray.h>
#include <ogdf/basic/SList.h>


namespace ogdf {


class FaceSinkGraph : public Graph
{
public:
	//! constructor (we assume that the original graph is biconnected!)
	FaceSinkGraph(const ConstCombinatorialEmbedding &E, node s);

	//! default constructor (dummy)
	FaceSinkGraph() : m_pE(0) { }


	void init(const ConstCombinatorialEmbedding &E, node s);


	//! return a reference to the original graph G
	const Graph &originalGraph() const {
		return *m_pE;
	}

	//! returns a reference to the embedding E of the original graph G
	const ConstCombinatorialEmbedding  &originalEmbedding() const {
		return *m_pE;
	}

	//! returns the sink-switch in G corresponding to node v in the face-sink
	//! graph, 0 if v corresponds to a face
	node originalNode(node v) const {
		return m_originalNode[v];
	}

	//! returns the face in E corresponding to node v in the face-sink
	//! graph, 0 if v corresponds to a sink-switch
	face originalFace(node v) const {
		return m_originalFace[v];
	}

	// returns true iff node v in the face-sink graph corresponds to a
	// face in E containing the source
	bool containsSource(node v) const {
		return m_containsSource[v];
	}


	//! checks if the face-sink graph is a forest with
	//!  1) there is exactly one tree T containing no internal vertex of G
	//!  2) all other trees contain exactly one internal vertex of G
	//! a node in tree T is returned as representative
	node checkForest();

	//! returns the list of faces f in E such that there exists an upward-planar
	//! drawing realizing E with f as external face
	void possibleExternalFaces(SList<face> &externalFaces) {
		gatherExternalFaces(m_T,0,externalFaces);
	}


	node faceNodeOf(edge e) {
		return dfsFaceNodeOf(m_T,0,
			m_pE->rightFace(e->adjSource()),m_pE->rightFace(e->adjTarget()));
	}


	node faceNodeOf(face f) {
		return dfsFaceNodeOf(m_T,0,f,0);
	}


	//! augments G to an st-planar graph (original implementation)
	/** introduces also new nodes into G corresponding to face-nodes in face sink graph)
	 */
	void stAugmentation(
		node h,                       // node corresponding to external face
		Graph &G,                     // original graph (not const)
		SList<node> &augmentedNodes,  // list of augmented nodes
		SList<edge> &augmentedEdges); // list of augmented edges

	//! augments G to an st-planar graph 
	/** (introduces only one new node as super sink into G)
	 */
	void stAugmentation(
		node  h,                      // node corresponding to external face
		Graph &G,                     // original graph (not const)
		node  &superSink,             // super sink
		SList<edge> &augmentedEdges); // list of augmented edges


private:
	//! constructs face-sink graph
	void doInit();

	//! performs dfs-traversal and checks for backwards edges
	bool dfsCheckForest(
		node v,                   // current node
		node parent,              // its parent in tree
		NodeArray<bool> &visited, // not already visited ?
		// number of internal vertices of G in current tree
		int &nInternalVertices);

	//! builds list of possible external faces 
	/** all faces in tree T containing
	 * the single source s) by a dfs traversal of T
	 */
	void gatherExternalFaces(
		node v,                      // current node
		node parent,                 // its parent
		SList<face> &externalFaces); // returns list of possible external faces

	node dfsFaceNodeOf(node v, node parent,face f1, face f2);

	node dfsStAugmentation(
		node v,                       // current node
		node parent,                  // its parent
		Graph &G,                     // original graph (not const)
		SList<node> &augmentedNodes,  // list of augmented nodes
		SList<edge> &augmentedEdges); // list of augmented edges

	node dfsStAugmentation(
		node v,                       // current node
		node parent,                  // its parent
		Graph &G,                     // original graph (not const)
		SList<edge> &augmentedEdges); // list of augmented edges

	
	//! associated embedding of graph G
	const ConstCombinatorialEmbedding *m_pE;
	node m_source; //!< the single source
	node m_T;      //!< representative of unique tree T

	NodeArray<node> m_originalNode;   //!< original node in G
	NodeArray<face> m_originalFace;   //!< original face in E
	NodeArray<bool> m_containsSource; //!< contains face node the source ?
}; // class FaceSinkGraph


} // end namespace ogdf


#endif
