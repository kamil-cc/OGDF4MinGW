/*
 * $Revision: 1.3 $
 * 
 * last checkin:
 *   $Author: gutwenger $ 
 *   $Date: 2007-11-09 12:12:41 +0100 (Fr, 09 Nov 2007) $ 
 ***************************************************************/
 
/** \file
 * \brief implementation of the class BoyerMyrvoldInit
 * 
 * \author Jens Schmidt
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


#include <ogdf/internal/planarity/BoyerMyrvoldInit.h>


namespace ogdf {


// constructor
BoyerMyrvoldInit::BoyerMyrvoldInit(BoyerMyrvoldPlanar* pBM)
	  : m_g(pBM->m_g),
		
		// ONLY FILE_OUTPUT: GraphAttributes for demonstrating purposes
		#ifdef FILE_OUTPUT
		m_ga(pBM->m_ga),
		#endif
		
		// initialize Members of BoyerMyrvoldPlanar
		m_embeddingGrade(pBM->m_embeddingGrade),
		m_randomDFSTree(pBM->m_randomDFSTree),
		
		m_realVertex(pBM->m_realVertex),
		m_dfi(pBM->m_dfi),
		m_nodeFromDFI(pBM->m_nodeFromDFI),
		m_link(pBM->m_link),
		m_adjParent(pBM->m_adjParent),
		m_leastAncestor(pBM->m_leastAncestor),
		m_edgeType(pBM->m_edgeType),
		m_lowPoint(pBM->m_lowPoint),
		m_highestSubtreeDFI(pBM->m_highestSubtreeDFI),
		m_separatedDFSChildList(pBM->m_separatedDFSChildList),
		m_pNodeInParent(pBM->m_pNodeInParent)
{
	OGDF_ASSERT(pBM != NULL);
	OGDF_ASSERT(m_embeddingGrade <= BoyerMyrvoldPlanar::doNotFind ||
				m_highestSubtreeDFI.graphOf() == &m_g);
};

// start DFS-traversal
void BoyerMyrvoldInit::computeDFS() {
	StackPure<adjEntry> stack;
	int nextDFI = 1;
	const int numberOfNodes = m_g.numberOfNodes();
	node v,w,next,parentNode;
	adjEntry adj,prnt;
	edge e;
	
	// ONLY DEBUG: check that dfi's are initialized
	#ifdef OGDF_DEBUG
	forall_nodes(v,m_g) OGDF_ASSERT(m_dfi[v] == 0);
	#endif
	
	// get random dfs-tree, if wanted
	if (m_randomDFSTree) {
		SListPure<node> list;
		SListPure<adjEntry> adjList;
		SListIterator<node> it;
		// permute nodelist
		m_g.allNodes(list);
		list.permute();
		for (it = list.begin(); it.valid(); ++it) {
			node& v(*it);
			// permute adjEntries
			if (v->degree() == 0) {
				m_dfi[v] = nextDFI;
				m_leastAncestor[v] = nextDFI;
				m_nodeFromDFI[nextDFI] = v;
				++nextDFI;
			} else {
				adjList.clear();
				m_g.adjEntries(v,adjList);
				adjList.permute();
				m_g.sort(v,adjList);
				stack.push(v->firstAdj());
			}
		}
	} else {
		for (next = m_g.firstNode(); next; next = next->succ())
			if (next->degree() == 0) {
				m_dfi[next] = nextDFI;
				m_leastAncestor[next] = nextDFI;
				m_nodeFromDFI[nextDFI] = next;
				++nextDFI;
			} else stack.push(next->firstAdj());
	}
	
	while (nextDFI <= numberOfNodes) {
		OGDF_ASSERT(!stack.empty());
		prnt = stack.pop();
		v = prnt->theNode();
		// check, if node v was visited before.
		if (m_dfi[v] != 0) continue;
		// parentNode=NULL on first node on connected component
		parentNode = prnt->twin()->theNode();
		if (m_dfi[parentNode] == 0) parentNode = NULL;
		
		// if not, mark node as visited and initialize NodeArrays
		m_dfi[v] = nextDFI;
		m_leastAncestor[v] = nextDFI;
		m_nodeFromDFI[nextDFI] = v;
		++nextDFI;
		
		// push all adjacent nodes onto stack
		forall_adj(adj,v) {
			e = adj->theEdge();
			if (adj == prnt && parentNode != NULL) continue;
			
			// check for self-loops and dfs- and dfs-parallel edges
			w = adj->twin()->theNode();
			if (m_dfi[w] == 0) {
				// only FILE_OUTPUT: fix orientation of dfs-edges to create
				// directed tree-output
				#ifdef FILE_OUTPUT
				if (e->adjTarget() == adj) {
					cout << "\nchanged the orientation of an dfs-edge ";
					cout << m_dfi[v] << "->(node with index " << w->index();
					cout << ") for tree-based output purposes";
					m_g.reverseEdge(e);
				}
				#endif
				
				m_edgeType[e] = EDGE_DFS;
				m_adjParent[w] = adj;
				m_link[CW][w] = adj;
				m_link[CCW][w] = adj;
				
				// found new dfs-edge: preorder
				stack.push(adj->twin());
			} else if (w == v) {
				// found self-loop
				m_edgeType[e] = EDGE_SELFLOOP;
			} else {
				// node w already has been visited and is an dfs-ancestor of v
				OGDF_ASSERT(m_dfi[w] < m_dfi[v]);
				if (w == parentNode) {
					// found parallel edge of dfs-parent-edge
					m_edgeType[e] = EDGE_DFS_PARALLEL;
				} else {
					// found backedge
					m_edgeType[e] = EDGE_BACK;
					// set least Ancestor
					if (m_dfi[w] < m_leastAncestor[v])
						m_leastAncestor[v] = m_dfi[w];
				}
			}
		}
	}
	
	// ONLY DEBUG: check, if DFIs are set and least ancestors have a lower dfi,
	// also validate DFI-parents
	#ifdef OGDF_DEBUG
	forall_nodes(v,m_g) {
		OGDF_ASSERT(m_dfi[v] > 0 && m_dfi[v] <= m_g.numberOfNodes());
		OGDF_ASSERT(m_leastAncestor[v] <= m_dfi[v]);
		if (m_adjParent[v] != NULL) {
			OGDF_ASSERT(m_dfi[m_adjParent[v]->theNode()] > 0);
			OGDF_ASSERT(m_dfi[m_adjParent[v]->theNode()] <= m_g.numberOfNodes());
			OGDF_ASSERT(m_dfi[m_adjParent[v]->twin()->theNode()] >
					m_dfi[m_adjParent[v]->theNode()]);
		}
	}
	#endif
	#ifdef FILE_OUTPUT
	forall_edges(e,m_g) {
		// color edges in GraphAttributes
		switch (m_edgeType[e]) {
		case EDGE_UNDEFINED:
			m_ga.colorEdge(e)="#A0FFFF"; // sky
			//m_ga.labelEdge(e)="0";
			cerr << "\nERROR: unclassified edges after BoyerMyrvoldInit::computeDFS";
			break;
		case EDGE_SELFLOOP:
			m_ga.colorEdge(e)="#FF00FF"; // pink
			//m_ga.labelEdge(e)="1";
			break;
		case EDGE_BACK:
			m_ga.colorEdge(e)="#FF0000"; // red
			//m_ga.labelEdge(e)="2";
			break;
		case EDGE_DFS:
			m_ga.colorEdge(e)="#000000"; // black
			//m_ga.labelEdge(e)="3";
			break;
		case EDGE_DFS_PARALLEL:
			m_ga.colorEdge(e)="#808080"; // darkgrey
			//m_ga.labelEdge(e)="4";
			break;
		}
	}
	#endif
}

// creates a virtual vertex of vertex father and embeds it as
// root in the biconnected child component containing of one edge
void BoyerMyrvoldInit::createVirtualVertex(const adjEntry father)
{
	// check that adjEntry is valid
	OGDF_ASSERT(father != NULL);
	
	// create new virtual Vertex and copy properties from non-virtual node
	const node virt = m_g.newNode();
	m_realVertex[virt] = father->theNode();
	m_dfi[virt] = -m_dfi[father->twin()->theNode()];
	m_nodeFromDFI[m_dfi[virt]] = virt;
	
	// set links for traversal of bicomps
	m_link[CW][virt] = father->twin();
	m_link[CCW][virt] = father->twin();
	
	//stat << "the edge: " << m_dfi[father->theNode()] << ", " << m_dfi[father->theEdge()->opposite(father->theNode())] << "\n";
	//stat << "outgoing: " << (father->theEdge()->source()==father->theNode()) <<"\n\n";
	
	// move edge to new virtual Vertex
	edge e = father->theEdge();
	if (e->source() == father->theNode()) {
		// e is outgoing edge
		m_g.moveSource(e,virt);
	} else {
		// e is ingoing edge
		m_g.moveTarget(e,virt);
	}
}

// calculates the lowpoints
void BoyerMyrvoldInit::computeLowPoints()
{
	node w;
	adjEntry adj,lastAdj;
	
	for (int i = m_g.numberOfNodes(); i >= 1; --i) {
		const node v = m_nodeFromDFI[i];
		
		// initialize lowpoints with least Ancestors and highpoints with dfi of node
		m_lowPoint[v] = m_leastAncestor[v];
		if (m_embeddingGrade > BoyerMyrvoldPlanar::doNotFind) m_highestSubtreeDFI[v] = i;
		
		// set the lowPoint of v by minimizing over its children lowPoints
		// create virtual vertex for each child
		adj = v->firstAdj();
		while (adj) {
			lastAdj = adj;
			adj = adj->succ();
			
			// avoid self-loops, parallel- and backedges
			if (m_edgeType[lastAdj->theEdge()] != EDGE_DFS) continue;
			w = lastAdj->twin()->theNode();
			
			// avoid parent dfs-node
			if (m_dfi[w] <= i) continue;
			
			// set lowPoints and highpoints
			if (m_lowPoint[w] < m_lowPoint[v]) m_lowPoint[v] = m_lowPoint[w];
			if (m_embeddingGrade > BoyerMyrvoldPlanar::doNotFind &&
									m_highestSubtreeDFI[w] > m_highestSubtreeDFI[v])
				m_highestSubtreeDFI[v] = m_highestSubtreeDFI[w];
			
			// create virtual vertex for each dfs-child
			createVirtualVertex(lastAdj);
		}
	}
	
	// ONLY DEBUG: check if all lowPoints are set
	#ifdef OGDF_DEBUG
	for (int i=1; i<=m_nodeFromDFI.high(); ++i) {
		node v = m_nodeFromDFI[i];
		OGDF_ASSERT(m_lowPoint[v] > 0);
		OGDF_ASSERT(m_lowPoint[v] <= m_nodeFromDFI.high());
		if (m_embeddingGrade > BoyerMyrvoldPlanar::doNotFind) {
			OGDF_ASSERT(m_highestSubtreeDFI[v] > 0);
			OGDF_ASSERT(m_highestSubtreeDFI[v] <= m_nodeFromDFI.high());
		}
	}
	#endif
}

// compute the separated DFS children for all nodes in ascending order of
// their lowpoint values in linear time
void BoyerMyrvoldInit::computeDFSChildLists() {
	// Bucketsort by lowpoint values
	BucketLowPoint blp(m_lowPoint);
	
	// copy all non-virtual nodes in a list and sort them with Bucketsort
	SListPure<node> allNodes;
	node v;
	forall_nodes(v,m_g) if (m_dfi[v]>0) allNodes.pushBack(v);
	allNodes.bucketSort(1,m_nodeFromDFI.high(),blp);
	
	// build DFS-child list
	SListConstIterator<node> it;
	for (it = allNodes.begin(); it.valid(); ++it) {
		v = *it;
		OGDF_ASSERT(m_dfi[v]>0);
		
		// if node is not root: insert node after last element of parent's DFSChildList
		// to achieve constant time deletion later:
		// set a pointer for each node to predecessor of his representative in the list
		if (m_adjParent[v] != NULL) {
			OGDF_ASSERT(m_realVertex[m_adjParent[v]->theNode()]!=NULL);
			
			m_pNodeInParent[v] = m_separatedDFSChildList[m_realVertex[m_adjParent[v]->theNode()]].pushBack(v);
			
			OGDF_ASSERT(m_pNodeInParent[v].valid());
			OGDF_ASSERT(v == *m_pNodeInParent[v]);
		} else m_pNodeInParent[v] = NULL;
	}
	
	// ONLY DEBUG: check if DFSChildList is sorted by ascending lowPoint values
	#ifdef OGDF_DEBUG
	int test=0;
	for (it = allNodes.begin(); it.valid(); ++it) {
		OGDF_ASSERT(m_lowPoint[*it] >= test);
		test = m_lowPoint[*it];
	}
	#endif
}

}

