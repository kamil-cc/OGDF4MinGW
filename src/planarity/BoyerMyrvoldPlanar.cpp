/*
 * $Revision: 1.4 $
 * 
 * last checkin:
 *   $Author: gutwenger $ 
 *   $Date: 2007-11-09 12:12:41 +0100 (Fr, 09 Nov 2007) $ 
 ***************************************************************/
 
/** \file
 * \brief implementation of the class BoyerMyrvoldPlanar
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


#include <ogdf/internal/planarity/BoyerMyrvoldPlanar.h>
#include <ogdf/internal/planarity/BoyerMyrvoldInit.h>
#include <ogdf/internal/planarity/FindKuratowskis.h>

#ifdef FILE_OUTPUT
	#include <Output.h>
#endif

#ifdef OGDF_DEBUG
	#include <ogdf/basic/simple_graph_alg.h>
#endif

#define OUTPUT_FILE "../output.gml" // name of output files
#define ORIGINAL_FILE "../original.gml"
#define ORIGINAL_SPLITTET_FILE "../originalSplitted.gml"
#define OUTPUT_WALKUP_FILE "../walkup" //...nodenumber...
#define PLANAR_UML_FILE "../PlanarUMLEmbedding.gml"
#define PLANAR_TREE_FILE "../PlanarTreeEmbedding.gml"
#define MAGNIFY 20 // size of graphical output relative to nodesize in dfs-output

namespace ogdf {


// constructor
BoyerMyrvoldPlanar::BoyerMyrvoldPlanar(
					Graph& g,
					bool bundles,
					int embeddingGrade, // see enumeration enumEmbeddingGrade for options
					bool limitStructures, // limits number of structures to embeddingGrade
					SListPure<KuratowskiStructure>& output,
					bool randomDFSTree, // creates a random DFS-Tree, if true
					bool avoidE2Minors) // avoids multiple identical minors (type AE2/E2)
	  :	// ONLY FILE_OUTPUT: GraphAttributes for demonstrating purposes
		#ifdef FILE_OUTPUT
		m_ga(g,GraphAttributes::nodeGraphics |
				GraphAttributes::edgeGraphics |
				GraphAttributes::nodeLabel |
				GraphAttributes::edgeLabel |
				GraphAttributes::nodeColor |
				GraphAttributes::edgeColor |
				GraphAttributes::edgeStyle |
				GraphAttributes::nodeStyle),
		m_greenEmbeddedEdge(g,0),
		#endif
		
		m_g(g),
		m_bundles(bundles),
	  	m_embeddingGrade(embeddingGrade),
	  	m_limitStructures(limitStructures),
	  	m_randomDFSTree(randomDFSTree),
	  	m_avoidE2Minors(avoidE2Minors),
		
		// only DEBUG: statistics
		#ifdef OGDF_DEBUG
		nirvana(NULL),
		#endif
		
		// BoyerMyrvoldInit members
		m_realVertex(g,NULL),
		m_dfi(g,0),
		m_nodeFromDFI(-g.numberOfNodes(),g.numberOfNodes(),NULL),
		m_adjParent(g,NULL),
		m_leastAncestor(g), // doesn't need initialization
		m_edgeType(g,EDGE_UNDEFINED),
		m_lowPoint(g), // doesn't need initialization
		m_separatedDFSChildList(g),
		m_pNodeInParent(g), // doesn't need initialization
		
		// Walkup & Walkdown members
		m_visited(g,0),
		m_flipped(g,false),
		m_backedgeFlags(g),
		m_pertinentRoots(g),
		m_output(output)
{
	m_link[CCW].init(g,NULL);
	m_link[CW].init(g,NULL);
	m_beforeSCE[CCW].init(g,NULL);
	m_beforeSCE[CW].init(g,NULL);
	m_output.clear();
	// apply this only, if FIND-procedure will be called
	if (m_embeddingGrade > doNotFind) {
		m_pointsToRoot.init(g,NULL);
		m_visitedWithBackedge.init(g,0);
		m_highestSubtreeDFI.init(g); // doesn't need initialization
	}
	m_flippedNodes = 0;
};

// walk upon external face in the given direction, see getSucessorOnExternalFace
// the difference is, that all inactive vertices are skipped, i.e. the returned node
// is active in relation to the node with dfi v
// in the special case of degree-one nodes the direction is not changed
// info returns the dynamic nodetype of the endnode
node BoyerMyrvoldPlanar::activeSuccessor(node w, int& direction, const int& v, int& info) {
	OGDF_ASSERT(w!=NULL);
	OGDF_ASSERT(w->degree()>0);
	OGDF_ASSERT(m_link[CW][w]!=NULL && m_link[CCW][w]!=NULL);
	node next;
	adjEntry adj;
	
	#ifdef STATISTICS
	stat << "\nActiveSuccessor: " << m_dfi[w] << ": ";
	#endif
	
	do {
		adj = m_link[direction][w];
		next = adj->theNode();
		OGDF_ASSERT(next!=NULL);
		OGDF_ASSERT(next->degree()>0);
		OGDF_ASSERT(m_link[CW][next]!=NULL && m_link[CCW][next]!=NULL);
		
		if (w->degree() > 1)
			direction = adj==beforeShortCircuitEdge(next,CCW)->twin();
		w=next;
		info = infoAboutNode(next,v);
		
		#ifdef STATISTICS
		stat << m_dfi[next] << ", ";
		#endif
	} while (info==0); // until not inactive
	return next;
}

// merges adjEntries of virtual node w and associated real vertex x according to
// given outgoing directions x_dir and w_dir.
// j is the outgoing traversal direction of the current embedded node.
void BoyerMyrvoldPlanar::mergeBiconnectedComponent(StackPure<int>& stack, const int j) {
	OGDF_ASSERT(!stack.empty());
	
	const int w_dir = stack.pop(); // outgoing direction of w
	OGDF_ASSERT(!stack.empty());
	const int x_dir = stack.pop(); // outgoing direction of x
	OGDF_ASSERT(!stack.empty());
	int tmp = stack.pop();
	const node w = m_nodeFromDFI[tmp]; // virtual DFS-Successor of x
	const node w_child = m_nodeFromDFI[-tmp]; // real unique DFS-Child of bicomp with root w
	const node x = m_realVertex[w];
	
	// ONLY STATISTICS: show adjacency lists
	#ifdef STATISTICS
	stat << "\n\nMergeBicomp:\nbei dem Stack folgende 3 Elemente gepoppt: "
		<< m_dfi[w] << ", (x_dir=" << (x_dir==CCW ? "CCW" : "CW")
		<< ", w_dir=" << (w_dir==CCW ? "CCW)" : "CW)");
	stat << "\nAdjacency List of realVertex (" << m_dfi[x] << "): ";
	SListPure<adjEntry> adjacency;
	m_g.adjEntries(x,adjacency);
	SListIterator<adjEntry> it;
	for (it=adjacency.begin();it.valid();++it) {
		stat << m_dfi[(*it)->twin()->theNode()] << ", ";
	}
	stat << "\nAdjacency List of virtualVertex (" << m_dfi[w] << "): ";
	adjacency.clear();
	m_g.adjEntries(w,adjacency);
	for (it=adjacency.begin();it.valid();++it) {
		stat << m_dfi[(*it)->twin()->theNode()] << ", ";
	}
	stat << "\nstart merging...";
	#endif
	
	// set new external face neighbors and save adjEntry, where edges will be merged
	adjEntry mergeEntry;
	Direction dir;
	if (x_dir==CCW) {
		dir=before;
	} else dir=after;
	mergeEntry = beforeShortCircuitEdge(x,!x_dir)->twin();
	m_link[!x_dir][x] = m_link[!w_dir][w];
	m_beforeSCE[!x_dir][x] = m_beforeSCE[!w_dir][w];
	
	// merge real and virtual nodes, flip biconnected component root if neccesary
	OGDF_ASSERT(!m_flipped[w_child]);
	adjEntry adj = w->firstAdj();
	edge e;
	if (x_dir==w_dir) {
		// if not flipped
		if (dir==after) {
			mergeEntry=mergeEntry->cyclicSucc();
			dir=before;
		}
	} else {
		// if flipped:
		// set unique DFS-child of associated bicomp root node to "flipped"
		m_flipped[w_child] = true;
		++m_flippedNodes;
		if (dir==before) {
			mergeEntry = mergeEntry->cyclicPred();
			dir = after;
		}
	}
	
	// merge adjEntries
	adjEntry temp;
    while (adj != NULL) {
		temp = adj->succ();
        e = adj->theEdge();
        OGDF_ASSERT(e->source() != x && e->target() != x);
        // this allows also self-loops when moving adjacency entries
        if (e->source() == w) {
        	m_g.moveSource(e,mergeEntry,dir);
        } else m_g.moveTarget(e,mergeEntry,dir);
        adj = temp;
    }
    
	// ONLY STATISTICS: show traversal context and show adjacency lists
	#ifdef STATISTICS
	stat << "\nccw[" << m_dfi[x] << "]=" << m_dfi[m_link[CCW][x]->theNode()]
				<< ", CW[" << m_dfi[x] << "]=" << m_dfi[m_link[CW][x]->theNode()];
	stat << "\nmergeEntry: " << m_dfi[x] << "->"
				<< m_dfi[mergeEntry->twin()->theNode()];
	stat << "\ninsertDir=" << (dir==after ? "after" : "before");
	if (x_dir != w_dir) stat << "\nFlipping: set Flippinflag on node "
				<< m_dfi[w_child] << " and flip node " << m_dfi[w];
	stat << "\nAdjacency List of mergedVertex (" << m_dfi[x] << "): ";
	adjacency.clear();
	m_g.adjEntries(x,adjacency);
	for (it=adjacency.begin();it.valid();++it) stat <<
				m_dfi[(*it)->twin()->theNode()] << ", ";
	SListIterator<node> itt;
	stat << "\npertinent Roots of real vertex " << m_dfi[x] << ": ";
	for (itt=m_pertinentRoots[x].begin();itt.valid();++itt) {
		stat << m_dfi[*itt] << ", ";
	}
	stat << "(delete first item w=" << m_dfi[w] << ")";
	#endif
	
    // remove w from pertinent roots of x
    OGDF_ASSERT(!m_pertinentRoots[x].empty());
    OGDF_ASSERT(m_pertinentRoots[x].front() == w);
	m_pertinentRoots[x].popFront();
	
	// ONLY STATISTICS: show PertinentRoots List, show separatedDFSChildList,
	// which must not be empty
	#ifdef STATISTICS
	stat << "\nResult: "; 
	for (itt=m_pertinentRoots[x].begin();itt.valid();++itt) {
		stat << m_dfi[*itt] << ", ";
	}
	stat << "\nSeparatedDFSChildList of " << m_dfi[x] << ": ";
	ListIterator<node> ittt;
	for (ittt=m_separatedDFSChildList[x].begin();ittt.valid();++ittt) {
		stat << m_dfi[*ittt] << ", ";
	}
	stat << "(delete item " << -m_dfi[w] << ")";
	#endif
	
	// consider x's unique dfs-successor in pertinent bicomp:
	// remove this successor from separatedChildList of x using
	// saved pointer pNodeInParent in constant time
	OGDF_ASSERT(!m_separatedDFSChildList[x].empty());
	OGDF_ASSERT(m_pNodeInParent[w_child].valid());
	m_separatedDFSChildList[x].del(m_pNodeInParent[w_child]);

	// ONLY STATISTICS: show adjacency lists
	#ifdef STATISTICS
	stat << "\nResult: ";
	for (ittt=m_separatedDFSChildList[x].begin();ittt.valid();++ittt) {
		stat << m_dfi[*ittt] << ", ";
	}
	stat << "\ndelete virtual node: " << m_dfi[w];
	#endif
	
	// delete virtual vertex, it must not contain any edges any more
    OGDF_ASSERT(w->firstAdj()==NULL);
    m_nodeFromDFI[m_dfi[w]]=NULL;
    m_g.delNode(w);
}

// the same as mergeBiconnectedComponent, but without any embedding-related
// operations
void BoyerMyrvoldPlanar::mergeBiconnectedComponentOnlyPlanar(
		StackPure<int>& stack,
		const int j)
{
	OGDF_ASSERT(!stack.empty());
	
	const int w_dir = stack.pop(); // outgoing direction of w
	OGDF_ASSERT(!stack.empty());
	const int x_dir = stack.pop(); // outgoing direction of x
	OGDF_ASSERT(!stack.empty());
	int tmp = stack.pop();
	const node w = m_nodeFromDFI[tmp]; // virtual DFS-Successor of x
	const node w_child = m_nodeFromDFI[-tmp]; // real unique DFS-Child of bicomp with root w
	const node x = m_realVertex[w];

	// ONLY STATISTICS: show adjacency lists
	#ifdef STATISTICS
	stat << "\n\nMergeBicomp:\nbei dem Stack folgende 3 Elemente gepoppt: "
		<< m_dfi[w] << ", (x_dir=" << (x_dir==CCW ? "CCW" : "CW")
		<< ", w_dir=" << (w_dir==CCW ? "CCW)" : "CW)");
	stat << "\nAdjacency List of realVertex (" << m_dfi[x] << "): ";
	SListPure<adjEntry> adjacency;
	m_g.adjEntries(x,adjacency);
	SListIterator<adjEntry> it;
	for (it=adjacency.begin();it.valid();++it) {
		stat << m_dfi[(*it)->twin()->theNode()] << ", ";
	}
	stat << "\nAdjacency List of virtualVertex (" << m_dfi[w] << "): ";
	adjacency.clear();
	m_g.adjEntries(w,adjacency);
	for (it=adjacency.begin();it.valid();++it) {
		stat << m_dfi[(*it)->twin()->theNode()] << ", ";
	}
	stat << "\nstart merging...";
	#endif

	// set new external face neighbors and save adjEntry, where edges will be merged
	m_link[!x_dir][x] = m_link[!w_dir][w];
	m_beforeSCE[!x_dir][x] = m_beforeSCE[!w_dir][w];
	
	// merge real and virtual nodes, flipping is not necessary here
	OGDF_ASSERT(!m_flipped[w_child]);
	adjEntry adj = w->firstAdj();
	edge e;
	adjEntry temp;
    while (adj != NULL) {
		temp = adj->succ();
        e = adj->theEdge();
        OGDF_ASSERT(e->source() != x && e->target() != x);
        // this allows also self-loops when moving adjacency entries
        if (e->source() == w) {
        	m_g.moveSource(e,x);
        } else m_g.moveTarget(e,x);
        adj = temp;
    }

	// ONLY STATISTICS: show traversal context and show adjacency lists
	#ifdef STATISTICS
	stat << "\nccw[" << m_dfi[x] << "]=" << m_dfi[m_link[CCW][x]->theNode()]
				<< ", CW[" << m_dfi[x] << "]=" << m_dfi[m_link[CW][x]->theNode()];
	if (x_dir != w_dir) stat << "\nFlipping: set Flippinflag on node "
				<< m_dfi[w_child] << " and flip node " << m_dfi[w];
	stat << "\nAdjacency List of mergedVertex (" << m_dfi[x] << "): ";
	adjacency.clear();
	m_g.adjEntries(x,adjacency);
	for (it=adjacency.begin();it.valid();++it) stat <<
				m_dfi[(*it)->twin()->theNode()] << ", ";
	SListIterator<node> itt;
	stat << "\npertinent Roots of real vertex " << m_dfi[x] << ": ";
	for (itt=m_pertinentRoots[x].begin();itt.valid();++itt) {
		stat << m_dfi[*itt] << ", ";
	}
	stat << "(delete first item w=" << m_dfi[w] << ")";
	#endif

    // remove w from pertinent roots of x
    OGDF_ASSERT(!m_pertinentRoots[x].empty());
    OGDF_ASSERT(m_pertinentRoots[x].front() == w);
	m_pertinentRoots[x].popFront();

	// ONLY STATISTICS: show PertinentRoots List, show separatedDFSChildList,
	// which must not be empty
	#ifdef STATISTICS
	stat << "\nResult: "; 
	for (itt=m_pertinentRoots[x].begin();itt.valid();++itt) {
		stat << m_dfi[*itt] << ", ";
	}
	stat << "\nSeparatedDFSChildList of " << m_dfi[x] << ": ";
	ListIterator<node> ittt;
	for (ittt=m_separatedDFSChildList[x].begin();ittt.valid();++ittt) {
		stat << m_dfi[*ittt] << ", ";
	}
	stat << "(delete item " << -m_dfi[w] << ")";
	#endif
	
	// consider x's unique dfs-successor in pertinent bicomp:
	// remove this successor from separatedChildList of x using
	// saved pointer pNodeInParent in constant time
	OGDF_ASSERT(!m_separatedDFSChildList[x].empty());
	OGDF_ASSERT(m_pNodeInParent[w_child].valid());
	m_separatedDFSChildList[x].del(m_pNodeInParent[w_child]);

	// ONLY STATISTICS: show adjacency lists
	#ifdef STATISTICS
	stat << "\nResult: ";
	for (ittt=m_separatedDFSChildList[x].begin();ittt.valid();++ittt) {
		stat << m_dfi[*ittt] << ", ";
	}
	stat << "\ndelete virtual node: " << m_dfi[w];
	#endif
	
	// delete virtual vertex, it must not contain any edges any more
    OGDF_ASSERT(w->firstAdj()==NULL);
    m_nodeFromDFI[m_dfi[w]]=NULL;
    m_g.delNode(w);
}

// embeds backedges from node v with direction v_dir to node w
// with direction w_dir. i is the current embedded node.
void BoyerMyrvoldPlanar::embedBackedges(
		const node v,
		const int v_dir,
		const node w,
		const int w_dir,
		const int i)
{
	OGDF_ASSERT(!m_backedgeFlags[w].empty());
	OGDF_ASSERT(v!=NULL && w!=NULL);
	OGDF_ASSERT(m_link[CCW][v]!=NULL && m_link[CW][v]!=NULL);
	OGDF_ASSERT(m_link[CCW][w]!=NULL && m_link[CW][w]!=NULL);
	
	// ONLY STATISTICS: show information about backedges
	#ifdef STATISTICS
	SListPure<adjEntry> l;
	m_g.adjEntries(v,l);
	stat << "\n\nMove Backedge:\nAdjacency List of vertex " << m_dfi[v] << ": ";
	SListConstIterator<adjEntry> iit;
	for (iit=l.begin();iit.valid();++iit) {
		stat << m_dfi[(*iit)->twin()->theNode()] << ", ";
	}
	#endif
	
	// if one edge is a short circuit edge, compute the former underlying adjEntry
	// the adjEntry of v, used for inserting backedges
	adjEntry mergeEntryV = beforeShortCircuitEdge(v,v_dir)->twin();
	Direction insertv = (v_dir==CCW) ? after : before;
	// the adjEntry of w, used for inserting backedges
	adjEntry mergeEntryW = beforeShortCircuitEdge(w,!w_dir)->twin();
	Direction insertw = (w_dir==CCW) ? before : after;
	
	// the first backedge in the backedgeFlags-list will be
	// the new external face adjEntry
	edge e;
	SListConstIterator<adjEntry> it;
	// save first BackedgeEntry
	adjEntry firstBack = m_backedgeFlags[w].front();
	for (it = m_backedgeFlags[w].begin(); it.valid(); ++it) {
		// embed this backedge
		e = (*it)->theEdge();
		
		#ifdef OGDF_DEBUG
		stat << "\nbackedgeFlag[w] on node " << m_dfi[(*it)->theNode()] << " found";
		#endif
		#ifdef FILE_OUTPUT
		m_greenEmbeddedEdge[e]=1; // special embedded-green "#00FF00"
		m_ga.colorEdge(e)="#00FF00"; // special embedded-green
		#endif
		
		OGDF_ASSERT(w==e->source() || w==e->target());
		OGDF_ASSERT((*it)->theNode()==m_nodeFromDFI[i]);
		
		if (e->source() == w) {
			// insert backedge to v
			m_g.moveTarget(e,mergeEntryV,insertv);
			// insert backedge to w
			m_g.moveSource(e,mergeEntryW,insertw);
		} else {
			// insert backedge to v
			m_g.moveSource(e,mergeEntryV,insertv);
			// insert backedge to w
			m_g.moveTarget(e,mergeEntryW,insertw);
		}
	}
	
	// ONLY STATISTICS
	#ifdef STATISTICS
	stat << "\nmoveBackedge(s) from " << i << " to " << m_dfi[v] << ":";
	stat << "\nmergeEntry v: " << m_dfi[v] << "->" << m_dfi[mergeEntryV->twin()->theNode()];
	stat << "\ninsertV=" << (insertv==after ? "after" : "before");
	stat << "\nmergeEntry w: " << m_dfi[w] << "->" << m_dfi[mergeEntryW->twin()->theNode()];
	stat << "\ninsertW=" << (insertw==after ? "after" : "before");
	stat << "\nCCW-edge: " << m_dfi[w] << "->" << m_dfi[m_link[CCW][w]->theNode()];
	#endif
	
	// set external face link for this backedge and delete out-dated short
	// circuit links
	m_link[v_dir][v] = firstBack->twin();
	m_beforeSCE[v_dir][v]=NULL;
	m_link[!w_dir][w] = firstBack;
	m_beforeSCE[!w_dir][w]=NULL;
	
	// ONLY STATISTICS: give information about traversal context
	#ifdef STATISTICS
	stat << "\nCCW[" << m_dfi[v] << "]=" << m_dfi[m_link[CCW][v]->theNode()];
	stat << " CW[" << m_dfi[v] << "]=" << m_dfi[m_link[CW][v]->theNode()];
	stat << "\nCCW[" << m_dfi[w] << "]=" << m_dfi[m_link[CCW][w]->theNode()];
	stat << " CW[" << m_dfi[w] << "]=" << m_dfi[m_link[CW][w]->theNode()];
	m_g.adjEntries(v,l);
	stat << "\nAdjacency List of vertex " << m_dfi[v] << ": ";
	for (it=l.begin();it.valid();++it) {
		stat << m_dfi[(*it)->twin()->theNode()] << ", ";
	}
	m_g.adjEntries(w,l);
	stat << "\nAdjacency List of vertex " << m_dfi[w] << ": ";
	for (it=l.begin();it.valid();++it) {
		stat << m_dfi[(*it)->twin()->theNode()] << ", ";
	}
	#endif
	
	// decrease counter of backedges per bicomp
	if (m_embeddingGrade > doNotFind) {
		node bicompRoot = m_pointsToRoot[m_backedgeFlags[w].front()->theEdge()];
		m_visitedWithBackedge[bicompRoot] -= m_backedgeFlags[w].size();
		OGDF_ASSERT(m_visitedWithBackedge[bicompRoot] >= 0);
		#ifdef STATISTICS
		stat << "\n" << m_visitedWithBackedge[bicompRoot] << " backedges on bicomp ";
		stat << "with node " << m_dfi[bicompRoot] << " left...";
		#endif
	}
	
	// delete BackedgeFlags
	m_backedgeFlags[w].clear();
}

// the same as embedBackedges, but for the planar check without returned embedding
void BoyerMyrvoldPlanar::embedBackedgesOnlyPlanar(
		const node v,
		const int v_dir,
		const node w,
		const int w_dir,
		const int i)
{
	OGDF_ASSERT(!m_backedgeFlags[w].empty());
	OGDF_ASSERT(m_link[CCW][v]!=NULL && m_link[CW][v]!=NULL);
	OGDF_ASSERT(m_link[CCW][w]!=NULL && m_link[CW][w]!=NULL);
	
	// ONLY STATISTICS: show information about backedges
	#ifdef STATISTICS
	SListPure<adjEntry> l;
	m_g.adjEntries(v,l);
	stat << "\n\nMove Backedge:\nAdjacency List of vertex " << m_dfi[v] << ": ";
	SListConstIterator<adjEntry> iit;
	for (iit=l.begin();iit.valid();++iit) {
		stat << m_dfi[(*iit)->twin()->theNode()] << ", ";
	}
	#endif
	
	// the last backedge in the backedgeFlags-list will be
	// the new external face adjEntry
	edge e;
	SListIterator<adjEntry> it;
	// save last BackedgeEntry
	adjEntry lastBack = m_backedgeFlags[w].back();
	for(it=m_backedgeFlags[w].begin();it.valid();++it) {
		// embed backedge
		e = (*it)->theEdge();
		
		#ifdef OGDF_DEBUG
		stat << "\nbackedgeFlag[w] on node " << m_dfi[(*it)->theNode()] << " found";
		#endif
		#ifdef FILE_OUTPUT
		m_greenEmbeddedEdge[e]=1; // special embedded-green "#00FF00"
		m_ga.colorEdge(e)="#00FF00"; // special embedded-green
		#endif
		
		OGDF_ASSERT((*it)->theNode()==m_nodeFromDFI[i]);
		OGDF_ASSERT(w==e->source() || w==e->target());
		
		if (e->source() == w) {
			// insert backedge to v
			m_g.moveTarget(e,v);
		} else {
			// insert backedge to v
			m_g.moveSource(e,v);
		}
	}
	
	// ONLY STATISTICS
	#ifdef STATISTICS
	stat << "\nmoveBackedge(s) from " << i << " to " << m_dfi[v] << ":";
	stat << "\nCCW-edge: " << m_dfi[w] << "->" << m_dfi[m_link[CCW][w]->theNode()];
	#endif
	
	// set external face link for this backedge and delete out-dated short
	// circuit links
	m_link[v_dir][v] = lastBack->twin();
	m_beforeSCE[v_dir][v]=NULL;
	m_link[!w_dir][w] = lastBack;
	m_beforeSCE[!w_dir][w]=NULL;
	
	// ONLY STATISTICS: give information about traversal context
	#ifdef STATISTICS
	stat << "\nCCW[" << m_dfi[v] << "]=" << m_dfi[m_link[CCW][v]->theNode()];
	stat << " CW[" << m_dfi[v] << "]=" << m_dfi[m_link[CW][v]->theNode()];
	stat << "\nCCW[" << m_dfi[w] << "]=" << m_dfi[m_link[CCW][w]->theNode()];
	stat << " CW[" << m_dfi[w] << "]=" << m_dfi[m_link[CW][w]->theNode()];
	m_g.adjEntries(v,l);
	stat << "\nAdjacency List of vertex " << m_dfi[v] << ": ";
	for (it=l.begin();it.valid();++it) {
		stat << m_dfi[(*it)->twin()->theNode()] << ", ";
	}
	m_g.adjEntries(w,l);
	stat << "\nAdjacency List of vertex " << m_dfi[w] << ": ";
	for (it=l.begin();it.valid();++it) {
		stat << m_dfi[(*it)->twin()->theNode()] << ", ";
	}
	#endif
	
	// delete BackedgeFlags
	m_backedgeFlags[w].clear();
}

// create short circuit edge from node v with direction v_dir to node w with outgoing
// direction w_dir.
void BoyerMyrvoldPlanar::createShortCircuitEdge(
		const node v,
		const int v_dir,
		const node w,
		const int w_dir)
{
	adjEntry temp;
	// save former neighbors
	if (m_beforeSCE[v_dir][v]==NULL) m_beforeSCE[v_dir][v]=m_link[v_dir][v];
	if (m_beforeSCE[!w_dir][w]==NULL) m_beforeSCE[!w_dir][w]=m_link[!w_dir][w];
	// set new short circuit edge
	temp = m_beforeSCE[!w_dir][w]->twin();
	m_link[!w_dir][w] = m_beforeSCE[v_dir][v]->twin();
	m_link[v_dir][v] = temp;
}

// Walkup
// finds pertinent subgraph for descendant w of v.
// marks visited nodes with marker and returns the last traversed node.
node BoyerMyrvoldPlanar::walkup(
						const node v,
						const node w,
						const int marker,
						const edge back) {
	const int i = m_dfi[v];
	node x = w;
	node y = w;
	node temp;
	int x_dir = CW;
	int y_dir = CCW;
	
	// ONLY STATISTICS: show walkup information
	#ifdef STATISTICS
	stat << "\n------------------------------------------------------------";
	stat << "\nWalkup fuer v = " << i << ", backedge: " << i << "->" << m_dfi[w] << "\n";
	#endif
	
	while (m_visited[x]!=marker && m_visited[y]!=marker) {
		// ONLY STATISTICS: color nodes and give information about pertinent
		// and extern attributes
		#ifdef STATISTICS
		stat << "(" << m_dfi[x] << ": ";
		if (pertinent(x)) stat << "pert. ";
		if (externallyActive(x,i)) stat << "ext. ";
		stat << "," << m_dfi[y] << ": ";
		if (pertinent(y)) stat << "pert. ";
		if (externallyActive(y,i)) stat << "ext. ";
		stat << ")  ";
		#endif
		// ONLY FILE_OUTPUT: color nodes
		#ifdef FILE_OUTPUT
		m_ga.colorNode(x)="#00C000"; // darkgreen
		m_ga.colorNode(y)="#00FF00"; // lightgreen
		#endif
		
		m_visited[x] = marker;
		m_visited[y] = marker;
		if (m_embeddingGrade > doNotFind) {
			m_visitedWithBackedge[x] = back->index();
			m_visitedWithBackedge[y] = back->index();
		}
		
		// is x or y root vertex?
		if (m_realVertex[x] != NULL) {
			temp=x;
		} else if (m_realVertex[y] != NULL) {
			temp=y;
		} else temp=NULL;
		
		if (temp != NULL) {
			// put pertinent root into the list of its non-virtual vertex.
			// the insert-position is either front or back of the list, this
			// depends on the external activity of the pertinent root's
			// biconnected component.
			
			x = m_realVertex[temp];
			y = x;
			
			OGDF_ASSERT(m_visited[x]==marker || m_pertinentRoots[x].empty());
			// push pertinent root
			if (m_lowPoint[m_nodeFromDFI[-m_dfi[temp]]] < i) {
				m_pertinentRoots[x].pushBack(temp);
			} else m_pertinentRoots[x].pushFront(temp);
			// found v, finish walkup and return last traversed node
			if (x==v) {
				m_visited[x] = marker;
				return temp;
			}
		} else {
			// traverse to external face successors
			x = successorOnExternalFace(x,x_dir);
			y = successorOnExternalFace(y,y_dir);
		}
	}
	
	// return last traversted node
	if (m_visited[x]==marker) {
		return x;
	} else return y;
}

// Walkdown
// for DFS-child w of the current processed vertex v': embed all backedges
// to the virtual node v of v'
// returns 1, iff the embedding process found a stopping configuration
int BoyerMyrvoldPlanar::walkdown(
			const int i, // dfi of rootvertex v'
			const node v, // v is virtual node of v'
			FindKuratowskis& findKuratowskis)
{
	node w; // current node
	int j; // the current traversal direction out of v
	int w_dir; // direction of traversal of node w
	
	// variables for recognizing the right direction after descending to a bicomp
	node root,x,y;
	int x_dir,y_dir;
	
	StackPure<int> stack;
	int infoX, infoY; // gives information about the type of endnode in that direction
	node stopX = NULL;
	
	bool stoppingNodesFound = 0; // 0=false,1=true,2=break
	
	// in both directions
	// j=current outgoing direction of current embedded node v
	for (j = CCW; j <= CW; ++j) {
		w_dir = j;
		
		#ifdef STATISTICS
		//if (j==CW) cout << i << ",";
		stat << "\n------------------------------------------------------------";
		stat << "\nWalkdown fuer v = " << i << ", v' = " << m_dfi[v]
					<< " in Richtung: " << ((w_dir==CW)?"CW":"CCW");
		#endif
		
		w = successorOnExternalFace(v,w_dir);
		
		while (w != v) {
			// ONLY STATISTICS: give informatien about pertinent, inactive and
			// externally active nodes
			#ifdef STATISTICS
			stat << "\n\ntraversing node " << m_dfi[w] << ": ";
			infoX = infoAboutNode(w,i);
			switch (infoX) {
			case 0: stat << "inactive"; break;
			case 1: stat << "int. active."; break;
			case 2: stat << "pert. and extern"; break;
			case 3: stat << "extern"; break;
			}
			if (!m_pertinentRoots[w].empty()) {
				stat  << " with pertRoots: ";
				SListIterator<node> it;
				for (it = m_pertinentRoots[w].begin(); it.valid(); ++it) {
					stat << m_dfi[*it] << ", ";
				}
			}
			stat << " in Richtung " << (w_dir==CCW ? "CCW" : "CW");
			stat << " (-> " << m_dfi[constSuccessorOnExternalFace(w,w_dir)] << ")";
			#endif
			
			// assert, that CCW[] and CW[] return that adjEntry of the neighbor
			OGDF_ASSERT(beforeShortCircuitEdge(w,w_dir)->twin()->theNode()==w);
			
			// if backedgeFlag is set
			if (!m_backedgeFlags[w].empty()) {
				#ifdef STATISTICS
				stat << "\nBackedgeflag found on node: " << m_dfi[w];
				#endif
				
				if (m_embeddingGrade != doNotEmbed) {
					// compute entire embedding
					while (!stack.empty()) mergeBiconnectedComponent(stack,j);
					// embed the backedge
					embedBackedges(v,j,w,w_dir,i);
				} else {
					// compute only planarity, not the entire embedding
					while (!stack.empty()) mergeBiconnectedComponentOnlyPlanar(stack,j);
					// embed the backedge
					embedBackedgesOnlyPlanar(v,j,w,w_dir,i);
				}
			}
			
			// if pertinentRoots of w is not empty
			if (!m_pertinentRoots[w].empty()) {
				// append pertinent root of w and direction of entry in w to stack
				// y is root of pertinent child bicomp
				root = m_pertinentRoots[w].front();
				stack.push(m_dfi[root]);
				
				// append outgoing direction of entry in w to stack
				OGDF_ASSERT(w->degree() > 0);
				stack.push(w_dir);
				
				// ONLY STATISTICS: give information about traversal context
				#ifdef STATISTICS
				stat << "\n\nPertinentRoots of " << m_dfi[w] <<
						" isn't empty, first node=" << m_dfi[root] <<
						" try to append to stack:";
				stat << "\nFind active Successor: ";
				if (w_dir == CW) {
					stat << "CW appended for real node";
				} else stat << "CCW appended for real node";
				#endif
				
				// get active successor in pertinent bicomp
				x_dir=CCW;
				y_dir=CW;
				#ifdef STATISTICS
				stat << "\nActiveSuccessors CCW: ";
				#endif
				x = activeSuccessor(root,x_dir,i,infoX);
				#ifdef STATISTICS
				stat << "(Endrichtung=" << (x_dir==CCW ? "CCW)" : "CW)");
				stat << "\nActiveSuccessors CW: ";
				#endif
				y = activeSuccessor(root,y_dir,i,infoY);
				
				OGDF_ASSERT(x != root && y != root);
				createShortCircuitEdge(root,CCW,x,x_dir);
				createShortCircuitEdge(root,CW,y,y_dir);
				
				// ONLY STATISTICS: give information about pertinent, inactive
				// and externally active nodes
				#ifdef STATISTICS
				stat << "(Endrichtung=" << (y_dir==CCW ? "CCW)" : "CW)");
				stat << "\nx=";
				switch (infoX) {
				case 0: stat << "inactive"; break;
				case 1: stat << "int. active."; break;
				case 2: stat << "pert. and extern"; break;
				case 3: stat << "extern"; break;
				}
				stat << " y=";
				switch (infoY) {
				case 0: stat << "inactive"; break;
				case 1: stat << "int. active."; break;
				case 2: stat << "pert. and extern"; break;
				case 3: stat << "extern"; break;
				}
				#endif
				
				// push counterclockwise resp. clockwise active successor
				// in pertinent bicomp
				if (infoX == infoY) {
					// if both attributes are externally active and non-pertinent,
					// save stopping nodes
					if (infoX==3) {
						OGDF_ASSERT(x != y);
						if (m_embeddingGrade <= doNotFind) return true;
						
						// extract Kuratowskis
						stoppingNodesFound = 1;
						// check, if we have found enough kuratowski structures
						if (m_embeddingGrade > 0 &&
								findKuratowskis.getAllKuratowskis().size() >= m_embeddingGrade) {
							return 2;
						}
						findKuratowskis.addKuratowskiStructure(m_nodeFromDFI[i],root,x,y);
						
						// go to the pertinent starting node on father bicomp
						stack.pop(); // delete new w_dir from stack
						w = m_realVertex[m_nodeFromDFI[stack.pop()]]; // x itself
						// refresh pertinentRoots information
						m_pertinentRoots[w].popFront();
						
						#ifdef STATISTICS
						if (pertinent(w)) {
							cout << "\nThe other stopping vertex in this bicomp is pertinent!";
						}
						#endif
						
						// if more pertinent child bicomps exist on the same root,
						// let the walkdown either embed it or find a new kuratowski structure
						while (!stack.empty() && !pertinent(w)) {
							// last real root
							node lastActiveNode = w;
							
							// not in V-bicomp:
							// go to the unvisited active node on father bicomp
							w_dir = stack.pop(); // outgoing direction of w
							x_dir = stack.pop(); // outgoing direction of x
							w = m_nodeFromDFI[stack.top()]; // w, virtual node
							
							node otherActiveNode = m_link[!w_dir][w]->theNode();
							
							#ifdef STATISTICS
							stat << "\ngo up to node w=" << m_dfi[w] << ", link to node ";
							stat << m_dfi[otherActiveNode] << ", which is ";
							stat << (pertinent(otherActiveNode) ? "" : "non-") << "pertinent";
							#endif
							
							OGDF_ASSERT(otherActiveNode == constActiveSuccessor(w,!w_dir,i,infoX));
							OGDF_ASSERT(externallyActive(otherActiveNode,i));
							OGDF_ASSERT(lastActiveNode == m_link[w_dir][w]->theNode());
							if (pertinent(otherActiveNode)) {
								// push adapted information about actual bicomp in stack
								stack.push(x_dir);
								stack.push(!w_dir);
								// go on with walkdown on unvisited active node
								w_dir = !w_dir;
								break;
							} else {
								// delete old root
								stack.pop();
								// if there are two stopping vertices, that are not pertinent
								// there could be another kuratowski structure
								if (lastActiveNode != otherActiveNode &&
										wNodesExist(w,lastActiveNode,otherActiveNode)) {
									// check, if we have found enough kuratowski structures
									if (m_embeddingGrade > 0 &&
											findKuratowskis.getAllKuratowskis().size() >= m_embeddingGrade) {
										return 2;
									}
									// different stopping nodes:
									// try to extract kuratowski structure and put the two
									// stopping nodes in the right traversal order
									if (w_dir==CCW) {
										findKuratowskis.addKuratowskiStructure(m_nodeFromDFI[i],
														w,lastActiveNode,otherActiveNode);
									} else {
										findKuratowskis.addKuratowskiStructure(m_nodeFromDFI[i],
														w,otherActiveNode,lastActiveNode);
									}
								}
								
								// refresh pertinentRoots information
								w = m_realVertex[w]; // x
								m_pertinentRoots[w].popFront();
								w_dir = x_dir;
							}
						}
					}
					// if both attributes are the same: minimize flips
					else if (w_dir==CCW) {
						w = x;
						w_dir = x_dir;
						stack.push(CCW);
						
						#ifdef STATISTICS
						stat << "\nand outgoing CCW appended for virtual node";
						#endif
					} else {
						w = y;
						w_dir = y_dir;
						stack.push(CW);
						
						#ifdef STATISTICS
						stat << "\nand outgoing CW appended for virtual node";
						#endif
					}
				} else if (infoX <= infoY) {
					// push x
					w=x; w_dir=x_dir;
					stack.push(CCW);
					
					#ifdef STATISTICS
					stat << "\nand outgoing CCW appended for virtual node";
					#endif
				} else {
					// push y
					w=y; w_dir=y_dir;
					stack.push(CW);
					
					#ifdef STATISTICS
					stat << "\nand outgoing CW appended for virtual node";
					#endif
				}
				
			} else if (inactive(w,i)) {
				// w is an inactive vertex
				w = successorOnExternalFace(w,w_dir);
				
				#ifdef STATISTICS
				stat << "\n\nInactive Vertex found: " << m_dfi[w];
				if (w==v) stat << "\n" << i << " erreicht. stop.";
				#endif
				
			} else {
				// w must be a stopping vertex
				OGDF_ASSERT(externallyActive(w,i));
				OGDF_ASSERT(m_lowPoint[m_nodeFromDFI[-m_dfi[v]]] < i);
				#ifdef STATISTICS
				stat << "\n\nStopping Vertex found: " << m_dfi[w];
				if (stack.empty()) {
					stat << "\nEmbed short circuit edge from " << m_dfi[v]
							<< " to " << m_dfi[w];
				} else stat << "\nStack istn't empty";
				#endif
				
				// embed shortCircuitEdge
				/*if (stack.empty())*/ createShortCircuitEdge(v,j,w,w_dir);
				
				// only save single stopping nodes, if we don't have already one
				if (j==CCW) {
					stopX = w;
				} else if (w != stopX) {
					OGDF_ASSERT(stopX!=NULL);
					
					if (m_embeddingGrade <= doNotFind) return false;
					// check, if some backedges were not embedded (=> nonplanar)
					// note, that this is performed at most one time per virtual root
					if (m_visitedWithBackedge[v] > 0) {
						#ifdef STATISTICS
						stat << "\nThis structure is a non-minorA-structure: ";
						#endif
						
						// some backedges are left on this bicomp
						stoppingNodesFound = 1;
						// check, if we have found enough kuratowski structures
						if (m_embeddingGrade > 0 &&
								findKuratowskis.getAllKuratowskis().size() >= m_embeddingGrade) {
							return 2;
						}
						findKuratowskis.addKuratowskiStructure(m_nodeFromDFI[i],v,stopX,w);
					}
				}
				
				break; // while
			}
		} // while
		
		// if stack is non-empty, the graph is non-planar
		#ifdef STATISTICS
		if (!stack.empty()) stat << "\nAfter " << (j==CCW ? "CCW" : "CW")
								<< "-Walkdown occured a non-empty stack";
		#endif
		stack.clear();
	} // for
	
	return stoppingNodesFound;
}

// embed graph m_g node by node in descending DFI-order beginning with dfi i
bool BoyerMyrvoldPlanar::embed() {
	node w; // dfs-descendant of v
	node x;
	adjEntry adj;
	edge e;
	int i; // dfi of current node v
	bool nonplanar=false; // true, if graph is not planar
	
	FindKuratowskis findKuratowskis(this);
	
	#ifdef FILE_OUTPUT
	bool walkupDidSomething;
	#endif
	
	for (i = m_nodeFromDFI.high(); i >= 1; --i) {
		const node v = m_nodeFromDFI[i];
		
		// call Walkup
		// for all sources of backedges of v: find pertinent subgraph
		
		// ONLY DEBUG: quadratic time (!)
		// check, if all nodes are unvisited and no pertinentRoot exist
		#ifdef OGDF_DEBUG
		node z;
		forall_nodes(z,m_g) {
			if (m_visited[z]==i) {
				cerr << "\nWalkup: node was already visited: " << m_dfi[z] << ", marker=" << i;
			}
			if (!m_pertinentRoots[z].empty()) {
				cerr << "\nWalkup: pertinent Roots are not empty on node: " << m_dfi[z] << ", firstRoot=" << m_dfi[m_pertinentRoots[z].front()];
			}
			OGDF_ASSERT(m_visited[z]!=i && m_pertinentRoots[z].empty());
		}
		#endif
		#ifdef FILE_OUTPUT
		walkupDidSomething = false;
		#endif
		
		forall_adj(adj,v) {
			w = adj->twin()->theNode();
			e = adj->theEdge();
			if (m_dfi[w] > i && m_edgeType[e] == EDGE_BACK) {
				#ifdef FILE_OUTPUT
				m_ga.colorEdge(e)="#DDDD00"; // yellow
				walkupDidSomething = true;
				#endif
				m_backedgeFlags[w].pushBack(adj);
				
				x = walkup(v,w,i,e);
				if (m_embeddingGrade <= doNotFind) continue;
				
				// divide children bicomps
				if (m_realVertex[x] == v) {
					m_pointsToRoot[e] = x;
					// set backedgenumber to 1 on this root
					m_visitedWithBackedge[x] = 1;
					#ifdef STATISTICS
					stat << "\nfirst (1.) Backedge " << i << "->" << m_dfi[w] <<
							" points to bicomp with root " << m_dfi[x];
					#endif
				} else {
					x = m_pointsToRoot[m_visitedWithBackedge[x]];
					m_pointsToRoot[e] = x;
					// increase backedgenumber on this root
					OGDF_ASSERT(m_visitedWithBackedge[x]>=1);
					++m_visitedWithBackedge[x];
					#ifdef STATISTICS
					stat << "\nadditional (" << m_visitedWithBackedge[x] << ".) Backedge " << i
							<< "->" << m_dfi[w] << " points to bicomp with root " << m_dfi[x];
					#endif
				}
			}
		}
		
		// ONLY FILE_OUTPUT: output after each walkup to file
		#ifdef FILE_OUTPUT
		if (walkupDidSomething) {
			char buffer[255];
			std::sprintf(buffer,"%s_%d.gml",OUTPUT_WALKUP_FILE,i);
			Output::dfsTreeOutput(*this,m_ga,buffer,true);
			//dfsOutput(buffer,f,m_g,m_ga,m_g.numberOfNodes(),
			//			m_nodeFromDFI.high(),m_dfi,m_realVertex,MAGNIFY,false);
			walkupDidSomething = false;
		}
		#endif
		
		// call Walkdown
		// for every pertinent subtrees with children w of v as roots
		// embed all backedges to v
		SListPure<node>& pert(m_pertinentRoots[v]);
		while (!pert.empty()) {
			OGDF_ASSERT(pert.front()->degree()==1);
			int result = walkdown(i,pert.popFrontRet(),findKuratowskis);
			if (result == 2) {
				m_output = findKuratowskis.getAllKuratowskis();
				return false;
			} else if (result == 1) {
				// found stopping configuration
				nonplanar = true;
				if (m_embeddingGrade <= doNotFind) return false;
			}
		}
		
		// if !embed, check, if there are any backedges left
		if (m_embeddingGrade <= doNotFind) {
			forall_adj(adj,v) {
				if (m_edgeType[adj->theEdge()] == EDGE_BACK &&
						m_dfi[adj->twin()->theNode()] > m_dfi[v])
					return false; // nonplanar
			}
		}
		
		// ONLY DEBUG: quadratic time (!)
		// for testing purposes, do after each walkup and walkdown:
		OGDF_ASSERT(pert.empty());
		#ifdef OGDF_DEBUG
		if (!nonplanar) {
			for (int j = 1; j<=m_nodeFromDFI.high(); ++j) {
				node t = m_nodeFromDFI[j];
				OGDF_ASSERT(m_backedgeFlags[t].empty());
				OGDF_ASSERT(m_pertinentRoots[t].empty());
			}
		}
		#endif
	}
	
	// embed and flip bicomps, if necessary
	if (nonplanar) {
		m_output = findKuratowskis.getAllKuratowskis();
	} else postProcessEmbedding(); // flip graph and embed self-loops, etc.
	
	return !nonplanar;
}

// flips the whole graph and merges virtual with real nodes, if not already done
// purpose: create embedding, after a nonplanar graph was processed
void BoyerMyrvoldPlanar::mergeWholeNonPlanarGraph() {
	#ifdef OGDF_DEBUG
	node v;
	forall_nodes(v,m_g) OGDF_ASSERT(m_visited[v] != -1);
	#endif
	flipBicomp(1,-1,m_visited,true,true);
}

// merge unprocessed virtual nodes such as the dfs-roots
void BoyerMyrvoldPlanar::mergeUnprocessedNodes() {
	node v = m_g.firstNode();
	node w,next;
	adjEntry adj;
	edge e;
	while (v) {
		next = v->succ();
		if (m_dfi[v] < 0) {
			w = m_realVertex[v];
			adj = v->firstAdj();
			// copy all adjEntries to non-virtual node
			while (adj) {
				e = adj->theEdge();
				adj = adj->succ();
				if (e->source()==v) {
					m_g.moveSource(e,w);
				} else m_g.moveTarget(e,w);
			}
			m_nodeFromDFI[m_dfi[v]]=NULL;
			m_g.delNode(v);
		}
		v = next;
	}
}

// flips all nodes of the bicomp with unique real root-child c as necessary.
// in addition all connected components with dfs-root c without virtual
// nodes are allowed. this function can be used to reverse the flip, too!
// marker has to be an non-existing int in array visited.
// if wholeGraph ist true, all bicomps of all connected components will be traversed.
// if deleteFlipFlags ist true, the flipping flags will be deleted after flipping
void BoyerMyrvoldPlanar::flipBicomp(
					int c,
					int marker,
					NodeArray<int>& visited,
					bool wholeGraph,
					bool deleteFlipFlags)
{
	#ifdef STATISTICS
	stat << "\n\nFlipBicomp with unique rootchild " << c << ":";
	#endif
	if (m_flippedNodes == 0) {
		if (wholeGraph) mergeUnprocessedNodes();
		return;
	}
	
	StackPure<int> stack; // stack for dfs-traversal
	node v;
	int temp;
	adjEntry adj;
	
	// ONLY DEBUG: check, if array m_wasHere is initialized
	#ifdef OGDF_DEBUG
	forall_nodes(v,m_g) {
		if (visited[v] == marker) cout << "\nv=" << m_dfi[v] << " marker=" << marker;
		OGDF_ASSERT(visited[v] != marker);
	}
	#endif
	
	if (wholeGraph) {
		mergeUnprocessedNodes();
		for (int i = 1; i <= m_g.numberOfNodes(); ++i)
			stack.push(-i);
	}
	
	// flip bicomps, if the flipped-flag is set
	bool flip;
	stack.push(-c); // negative numbers: flip=false, otherwise flip=true
	while (!stack.empty()) {
		temp = stack.pop();
		if (temp < 0) {
			flip = false;
			v = m_nodeFromDFI[-temp];
		} else {
			flip = true;
			v = m_nodeFromDFI[temp];
		}
		if (wholeGraph) {
			if (visited[v]==marker) continue;
			// mark visited nodes
			visited[v] = marker;
		}
		
		// ONLY STATISTICS
		#ifdef STATISTICS
		stat << " " << m_dfi[v];
		#endif
		
		// flip adjEntries of node, if necessary
		if (m_flipped[v]) {
			flip = (flip==true) ? false : true;
			#ifdef STATISTICS
			stat << "(flip=" << (flip==true ? "true" : "false") << ")";
			#endif
			
			// don't do this, if all flips on nodes of this bicomp will be reversed
			if (deleteFlipFlags) {
				m_flipped[v] = false;
				--m_flippedNodes;
				OGDF_ASSERT(m_flippedNodes >= 0);
			}
		}
		if (flip) {
			m_g.reverseAdjEdges(v);
			if (deleteFlipFlags) {
				adjEntry tmp = m_link[CCW][v];
				m_link[CCW][v] = m_link[CW][v];
				m_link[CW][v] = tmp;
				
				tmp = m_beforeSCE[CCW][v];
				m_beforeSCE[CCW][v] = m_beforeSCE[CW][v];
				m_beforeSCE[CW][v] = tmp;
			}
		}
		
		// go along the dfs-edges
		forall_adj(adj,v) {
			temp = m_dfi[adj->twin()->theNode()];
			OGDF_ASSERT(m_edgeType[adj->theEdge()] != EDGE_UNDEFINED);
			if (temp > m_dfi[v] && m_edgeType[adj->theEdge()]==EDGE_DFS) {
				stack.push(flip ? temp : -temp);
			}
		}
	}
}

// postprocess the embedding, so that all unprocessed virtual vertices are
// merged with their non-virtual counterpart. Furthermore all bicomps
// are flipped, if necessary and parallel edges and self-loops are embedded.
void BoyerMyrvoldPlanar::postProcessEmbedding() {
	StackPure<int> stack; // stack for dfs-traversal
	node v,w;
	adjEntry adj;
	int temp;
	
	// ONLY DEBUG: check, if array visited is initialized
	#ifdef OGDF_DEBUG
	forall_nodes(v,m_g) OGDF_ASSERT(m_visited[v] != -1);
	#endif
	
	mergeUnprocessedNodes();
	
	#ifdef STATISTICS
	stat << "\n\nPostProcessing: ";
	#endif
	
	// flip bicomps, if the flipped-flag is set, i.e. postprocessing in
	// reverse dfi-order
	bool flip;
	for(int i=1; i<=m_g.numberOfNodes(); ++i) {
		if (m_visited[m_nodeFromDFI[i]] == -1) continue;
		stack.push(-i); // negative numbers: flip=false, otherwise flip=true
		
		while (!stack.empty()) {
			temp = stack.pop();
			if (temp < 0) {
				flip=false;
				v = m_nodeFromDFI[-temp];
			} else {
				flip=true;
				v = m_nodeFromDFI[temp];
			}
			if (m_visited[v]==-1) continue;
			// mark visited nodes with visited[v]==-1
			m_visited[v] = -1;
			
			// ONLY STATISTICS
			#ifdef STATISTICS
			stat << m_dfi[v];
			if (m_flipped[v]) stat << "(cFlipFlag=true)";
			stat << " ";
			#endif
			
			// flip adjEntries of node, if necessary
			if (m_flipped[v]) {
				m_flipped[v] = false;
				flip=(flip==true) ? false : true;
			}
			if (flip) m_g.reverseAdjEdges(v);
			
			#ifdef FILE_OUTPUT
			if (m_flipped[v]) {
				m_ga.styleNode(v) = GraphAttributes::esDot;
			} else m_ga.styleNode(v) = GraphAttributes::esSolid;
			#endif
			
			adj=v->firstAdj();
			while (adj) {
				w = adj->twin()->theNode();
				temp = m_edgeType[adj->theEdge()];
				if (temp==EDGE_DFS) {
					// go along the dfs-edges
					stack.push(flip ? m_dfi[w] : -m_dfi[w]);
					adj=adj->succ();
				} else if (temp==EDGE_SELFLOOP) {
					// embed self-loops
					m_g.moveAdjBefore(adj->twin(),adj);
					adj=adj->succ();
				} else if (temp==EDGE_DFS_PARALLEL &&
							m_adjParent[v]!=NULL &&
							w == m_adjParent[v]->theNode()) {
					// embed edges that are parallel to dfs-edges
					// it is only possible to deal with the parallel edges to the
					// parent, since children nodes could be flipped later
					adjEntry tmp = adj->succ();
					m_g.moveAdjAfter(adj,m_adjParent[v]->twin());
					m_g.moveAdjBefore(adj->twin(),m_adjParent[v]);
					adj = tmp;
				} else adj=adj->succ();
			}
		}
	}
	
	// ONLY DEBUG: print adjacency lists and check self-loops and
	// parallel edges for correct embedding
	#ifdef OGDF_DEBUG
	stat << "\n\nAdjacency Lists:";
	forall_nodes(v,m_g) {
		stat << "\n" << m_dfi[v] << ": ";
		adjEntry adj,prev,prevParent;
		forall_adj(adj,v) {
			w = adj->twin()->theNode();
			stat << m_dfi[w] << "[" << (adj->theEdge()->index()) << "]" << " ";
			// check, if adjEntries of every self-loop are consecutive
			if (w==v) {
				OGDF_ASSERT(adj->twin() == adj->cyclicSucc() ||
							adj->twin() == adj->cyclicPred());
			} else if (m_edgeType[adj->theEdge()]==EDGE_DFS_PARALLEL &&
						m_adjParent[v]!=NULL &&
						w == m_adjParent[v]->theNode()) {
				// check, if adjEntries of every parallel dfs-edge are consecutive
				prev = adj->cyclicPred();
				prevParent = adj->twin()->cyclicSucc();
				OGDF_ASSERT(prev == prevParent->twin());
				OGDF_ASSERT((m_edgeType[prev->theEdge()]==EDGE_DFS_PARALLEL &&
							m_edgeType[prevParent->theEdge()]==EDGE_DFS_PARALLEL) ||
							(m_edgeType[prev->theEdge()]==EDGE_DFS &&
							m_edgeType[prevParent->theEdge()]==EDGE_DFS));
			}
		}
	}
	#endif
}

// tests Graph m_g for planarity
// if graph should be embedded, a planar embedding or a kuratowski subdivision
// of m_g is returned in addition, depending on whether m_g is planar
bool BoyerMyrvoldPlanar::start() {
	BoyerMyrvoldInit bmi(this);
	bmi.computeDFS();
	bmi.computeLowPoints();
	bmi.computeDFSChildLists();
	
	// ONLY STATISTICS: show preprocessed values with bucketsort by dfi-numbers
	#ifdef STATISTICS
	SList<node> all;
	m_g.allNodes(all);
	BucketDFI bDFI(m_dfi);
	all.bucketSort(bDFI);
	SListConstIterator<node> it;
	node v;
	for(it=all.begin(); it.valid(); ++it) {
		v = *it;
		if (m_dfi[v] > 0) {
			// real vertex
			stat << "\n" << v->index() << ")\t";
			stat << " DFI: " << m_dfi[v] << "\t";
			if (m_adjParent[v] == NULL)  {
				stat << "        \t";
			} else {
				stat << "Prnt: ";
				stat << m_dfi[m_adjParent[v]->theNode()] << "\t";
			}
			stat << "LA.: " << m_leastAncestor[v] << "\t";
			if (m_link[CW][v] == NULL || m_link[CCW][v] == NULL) {
				stat << "      \t       \t";
			} else {
				stat << "CW: ";
				stat << m_dfi[m_link[CW][v]->theNode()] << "\t";
				stat << "CCW: ";
				stat << m_dfi[m_link[CCW][v]->theNode()] << "\t";
			}
			stat << "LowPnt: " << m_lowPoint[v] << "\t";
			if (m_embeddingGrade > doNotFind)
				stat << "HighDFI: " << m_highestSubtreeDFI[v] << "\t";
			stat << "DFSChild: ";
			ListConstIterator<node> itt;
			for(itt=m_separatedDFSChildList[v].begin(); itt.valid(); ++itt) {
				stat << m_dfi[*itt] <<" ";
			}
		} else {
			// virtual vertex
			stat << "\n" << v->index() << ")\t";
			stat << "DFI: " << m_dfi[v] << "\t";
			stat << "        \t      \t";
			if (m_link[CW][v] == NULL || m_link[CCW][v] == NULL) {
				stat << "      \t       \t";
			} else {
				stat << "CW: ";
				stat << m_dfi[m_link[CW][v]->theNode()] << " \t";
				stat << "CCW: ";
				stat << m_dfi[m_link[CCW][v]->theNode()] << "\t";
			}
		}
	}
	#endif
	
	// ONLY FILE_OUTPUT: save original and original splitted graph to file
	#ifdef FILE_OUTPUT
	Output::dfsTreeOutput(*this,m_ga,ORIGINAL_FILE,false);
	Output::dfsTreeOutput(*this,m_ga,ORIGINAL_SPLITTET_FILE,true);
	//dfsOutput(ORIGINAL_SPLITTET_FILE,false,m_g,m_ga,m_g.numberOfNodes(),
	//			m_nodeFromDFI.high(),m_dfi,m_realVertex,MAGNIFY,false);
	#endif
	
	// call the embedding procedure
	bool planar = embed();
	
	// ONLY FILE_OUTPUT: save output graph to file
	#ifdef FILE_OUTPUT
	Output::dfsTreeOutput(*this,m_ga,OUTPUT_FILE,true);
	/*dfsOutput(OUTPUT_FILE,false,m_g,m_ga,m_g.numberOfNodes(),
					m_nodeFromDFI.high(),m_dfi,m_realVertex,MAGNIFY,false);*/
	
	// if planar and connected, save a planar embedding in PLANAR_EMBEDDING_FILE
	if (planar && m_embeddingGrade != doNotEmbed) {
		if (!isConnected(m_g) || !isLoopFree(m_g)) {
			#ifdef STATISTICS
			stat << "\n\nNo PlanarUML-Output: Graph has to be connected and loop-free.";
			#endif
		} else Output::planarUMLOutput(m_g,m_ga,PLANAR_UML_FILE,m_dfi,m_nodeFromDFI);
	}
	#endif
	
	return planar;
};


}
