/*
 * $Revision: 1.3 $
 * 
 * last checkin:
 *   $Author: klein $ 
 *   $Date: 2007-11-14 16:18:10 +0100 (Mi, 14 Nov 2007) $ 
 ***************************************************************/
 
/** \file
 * \brief Declaration of the class BoyerMyrvoldPlanar
 * 
 * \author Jens Schmidt
 * 
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

#ifndef OGDF_BoyerMyrvoldPlanar_H
#define OGDF_BoyerMyrvoldPlanar_H

#include <ogdf/basic/Graph_d.h>
#include <ogdf/basic/NodeArray.h>
#include <ogdf/basic/Stack.h>
#include <ogdf/basic/List.h>
#include <ogdf/basic/SList.h>

// ONLY FILEOUTPUT: enable GraphAttributes and output-features
#ifdef FILE_OUTPUT
	#include <ogdf/basic/GraphAttributes.h>
#endif

// ONLY DEBUG: statistics mode
#ifdef STATISTICS
	#define stat cout
	#ifndef OGDF_DEBUG
		#error
	#endif
#else
	#ifdef OGDF_DEBUG
		#define stat nirvana
	#endif
#endif


namespace ogdf {

//! Directions for clockwise and counterclockwise traversal
enum enumDirection {
	CCW=0,
	CW=1
};

//! Type of edge
/** @param 0 undefined
 * @param 1 selfloop
 * @param 2 backedge
 * @param 3 DFS-edge
 * @param 4 parallel DFS-edge
 * @param 5 deleted backedge
 */
enum enumEdgeType {
	EDGE_UNDEFINED=0,
	EDGE_SELFLOOP=1,
	EDGE_BACK=2,
	EDGE_DFS=3,
	EDGE_DFS_PARALLEL=4,
	EDGE_BACK_DELETED=5
};

class KuratowskiStructure;
class FindKuratowskis;

//! This class implements the extended BoyerMyrvold planarity embedding algorithm
class BoyerMyrvoldPlanar {
		friend class BoyerMyrvold;
		friend class BoyerMyrvoldInit;
		friend class FindKuratowskis;
		friend class ExtractKuratowskis;
		
		#ifdef FILE_OUTPUT
		friend class Output;
		#endif
		
	public:
		//! Constructor, for parameters see BoyerMyrvold
		BoyerMyrvoldPlanar(
					Graph& g,
					bool bundles,
					int m_embeddingGrade,
					bool limitStructures,
					SListPure<KuratowskiStructure>& output,
					bool randomDFSTree,
					bool avoidE2Minors);
		//! Destructor
		~BoyerMyrvoldPlanar() { };
		
		//! Starts the embedding algorithm
		bool start();
		
		//! Denotes the different embedding options
		enum enumEmbeddingGrade {
			doNotEmbed=-3, // and not find any kuratowski subdivisions
			doNotFind=-2, // but embed
			doFindUnlimited=-1, // and embed
			doFindZero=0 // and embed
		};
		
		//! Flips all nodes of the bicomp with unique, real, rootchild c as necessary
		/** @param c is the unique rootchild of the bicomp
		 * @param marker is the value which marks nodes as visited
		 * @param visited is the array containing visiting information
		 * @param wholeGraph Iff true, all bicomps of all connected components will be traversed
		 * @param deleteFlipFlags Iff true, the flipping flags will be deleted after flipping
		 */
		void flipBicomp(
				int c,
				int marker,
				NodeArray<int>& visited,
				bool wholeGraph,
				bool deleteFlipFlags);
		
		//! Flips the whole graph and merges virtual with real nodes, if not already done
		void mergeWholeNonPlanarGraph();
		
		#ifdef FILE_OUTPUT
		//! GraphAttributes for demonstrating purposes
		GraphAttributes m_ga;
		//! contains all embedded backedges
		EdgeArray<int> m_greenEmbeddedEdge;
		#endif
		
	protected:
		/***** Methods for Walkup and Walkdown ******/
		
		//! Checks whether node \a w is pertinent. \a w has to be non-virtual.
		inline bool pertinent(const node& w) {
			OGDF_ASSERT(w!=NULL);
			if (m_dfi[w] <= 0) return false;
			return (!m_backedgeFlags[w].empty() || !m_pertinentRoots[w].empty());
		}
		
		//! Checks whether real node \a w is internally active while embedding node with DFI \a v
		inline bool internallyActive(const node& w, const int& v) {
			OGDF_ASSERT(w!=NULL);
			if (m_dfi[w] <= 0) return false;
			return (pertinent(w) && !externallyActive(w,v));
		}
		
		//! Checks whether real node \a w is externally active while embedding node with DFI \a v
		inline bool externallyActive(const node& w, const int& v) {
			OGDF_ASSERT(w!=NULL);
			if (m_dfi[w] <= 0) return false;
			if (m_leastAncestor[w] < v) return true;
			if (m_separatedDFSChildList[w].empty()) return false;
			return (m_lowPoint[m_separatedDFSChildList[w].front()] < v);
		}
		
		//! Checks whether real node \a w is inactive while embedding node with DFI \a v
		inline bool inactive(const node& w, const int& v) {
			OGDF_ASSERT(w!=NULL);
			if (m_dfi[w] <= 0) return true;
			if (!m_backedgeFlags[w].empty() || !m_pertinentRoots[w].empty()
				|| m_leastAncestor[w] < v) return false;
			if (m_separatedDFSChildList[w].empty()) return true;
			return (m_lowPoint[m_separatedDFSChildList[w].front()] >= v);
		}
		
		//! Checks all dynamic information about a node \a w while embedding node with DFI \a v
		/** @param[out] 0 = inactive
		 * @param[out] 1 = internallyActive
		 * @param[out] 2 = pertinent and externallyActive
		 * @param[out] 3 = externallyActive and not pertinent
		 */
		inline int infoAboutNode(const node& w, const int& v) {
			OGDF_ASSERT(w!=NULL);
			if (m_dfi[w] <= 0) return 0;
			if (!m_pertinentRoots[w].empty() || !m_backedgeFlags[w].empty()) {
				// pertinent
				if (m_leastAncestor[w] < v) return 2;
				if (m_separatedDFSChildList[w].empty()) return 1;
				return (m_lowPoint[m_separatedDFSChildList[w].front()] < v
						? 2 : 1);
			} else {
				// not pertinent
				if (m_leastAncestor[w] < v) return 3;
				if (m_separatedDFSChildList[w].empty()) return 0;
				return (m_lowPoint[m_separatedDFSChildList[w].front()] < v
						? 3 : 0);
			}
		}
		
		//! Walks upon external face in the given \a direction starting at \a w
		/** If none of the bicomps has been flipped then CW = clockwise and
		 * CCW = counterclockwise holds. In general, the traversaldirection could have
		 * been changed due to flipped components. If this occurs, the
		 * traversaldirection is flipped.
		 */
		inline node successorOnExternalFace(const node& w, int& direction) {
			OGDF_ASSERT(w!=NULL);
			OGDF_ASSERT(w->degree()>0);
			OGDF_ASSERT(m_link[CW][w]!=NULL && m_link[CCW][w]!=NULL);
			adjEntry adj = m_link[direction][w];
			OGDF_ASSERT(adj->theNode()!=NULL);
			
			if (w->degree() > 1) direction =
					adj==beforeShortCircuitEdge(adj->theNode(),CCW)->twin();
			OGDF_ASSERT(direction || adj==beforeShortCircuitEdge(adj->theNode(),CW)->twin());
			return adj->theNode();
		}
		
		//! Walks upon external face in given \a direction avoiding short circuit edges
		inline node successorWithoutShortCircuit(const node& w, int& direction) {
			OGDF_ASSERT(w!=NULL);
			OGDF_ASSERT(w->degree()>0);
			OGDF_ASSERT(m_link[CW][w]!=NULL && m_link[CCW][w]!=NULL);
			adjEntry adj = beforeShortCircuitEdge(w,direction);
			OGDF_ASSERT(adj->theNode()!=NULL);
			
			if (w->degree() > 1) direction =
					adj==beforeShortCircuitEdge(adj->theNode(),CCW)->twin();
			OGDF_ASSERT(direction || adj==beforeShortCircuitEdge(adj->theNode(),CW)->twin());
			return adj->theNode();
		}
		
		//! Returns the successornode on the external face in given \a direction
		/** \a direction is not changed.
		 */
		inline node constSuccessorOnExternalFace(const node& v, const int& direction) {
			OGDF_ASSERT(v!=NULL);
			OGDF_ASSERT(v->degree()>0);
			return m_link[direction][v]->theNode();
		}
		
		//! Walks upon external face in \a direction avoiding short circuit edges
		/** \a direction is not changed.
		 */
		inline node constSuccessorWithoutShortCircuit(const node& v, const int& direction) {
			OGDF_ASSERT(v!=NULL);
			OGDF_ASSERT(v->degree()>0);
			return beforeShortCircuitEdge(v,direction)->theNode();
		}
		
		//! Returns underlying former adjEntry, if a short circuit edge in \a direction of \a v exists
		/** Otherwise the common edge is returned. In every case the returned adjEntry
		 * points to the targetnode.
		 */
		inline adjEntry beforeShortCircuitEdge(const node& v, const int& direction) {
			OGDF_ASSERT(v!=NULL);
			
			//ONLY STATISTICS: short circuit edge information
			/*#ifdef STATISTICS
			stat << "\n" << m_dfi[v] << ", dir=" << (direction==CCW ? "CCW" : "CW")
					<< ", CCW=" << m_dfi[m_link[CCW][v]->theNode()]
					<< ", CW=" << m_dfi[m_link[CW][v]->theNode()];
			stat << " before_ccw: " << (m_beforeSCE[CCW][v]!=NULL ? m_dfi[m_beforeSCE[CCW][v]->theNode()] : -1) <<
				" before_cw: " << (m_beforeSCE[CW][v]!=NULL ? m_dfi[m_beforeSCE[CW][v]->theNode()] : -1);
			#endif*/
			
			return (m_beforeSCE[direction][v]==NULL) ? m_link[direction][v] : m_beforeSCE[direction][v];
		}
		
		//! Walks upon external face in the given \a direction starting at \a w until an active vertex is reached
		/** Returns dynamical typeinformation \a info of that endvertex.
		 */
		node activeSuccessor(node w, int& direction, const int& v, int& info);
		
		//! Walks upon external face in the given \a direction (without changing it) until an active vertex is reached
		/** Returns dynamical typeinformation \a info of that endvertex. But does not change the \a direction.
		 */
		inline node constActiveSuccessor(node w, int direction, const int& v, int& info) {
			return activeSuccessor(w,direction,v,info);
		}
		
		//! Checks, if one ore more wNodes exist between the two stopping vertices \a stopx and \a stopy
		/** The node \a root is root of the bicomp containing the stopping vertices
		 */
		inline bool wNodesExist(node root, const node& stopx, const node& stopy) {
			OGDF_ASSERT(root != stopx && root != stopy && stopx != stopy);
			int dir = CCW;
			bool between = false;
			#ifdef STATISTICS
			stat << "\ncheck on wNodesExist: ";
			#endif
			while (root != NULL) {
				#ifdef STATISTICS
				stat << m_dfi[root] << " ";
				#endif
				root = successorOnExternalFace(root,dir);
				if (between && pertinent(root)) {
					#ifdef STATISTICS
					stat << " wNode found";
					#endif
					return true;
				}
				if (root == stopx || root == stopy) {
					if (between) {
						#ifdef STATISTICS
						stat << " no wNode found";
						#endif
						return false;
					}
					between = true;
				}
			}
			#ifdef STATISTICS
			stat << " no wNode found";
			#endif
			return false;
		}
		
		//! Prints informations about node \a v
		inline void printNodeInfo(node v) {
			cout << "\nprintNodeInfo(" << m_dfi[v] << "): ";
			cout << "CCW=" << m_dfi[constSuccessorOnExternalFace(v,CCW)];
			cout << ",CW=" << m_dfi[constSuccessorOnExternalFace(v,CW)];
			cout << "\tCCWBefore=" << m_dfi[constSuccessorWithoutShortCircuit(v,CCW)];
			cout << ",CWBefore=" << m_dfi[constSuccessorWithoutShortCircuit(v,CW)];
			cout << "\tadjList: ";
			adjEntry adj;
			for (adj = v->firstAdj(); adj; adj = adj->succ()) {
				cout << adj->twin()->theNode() << " ";
			}
		}
		
		//! Merges the last two biconnected components saved in \a stack while embedding them
		/** \a j is the outgoing traversal direction of the current node to embed
		 */
		void mergeBiconnectedComponent(StackPure<int>& stack, const int j);
		
		//! Merges the last two biconnected components saved in \a stack without embedding them
		/** \a j is the outgoing traversal direction of the current node to embed
		 */
		void mergeBiconnectedComponentOnlyPlanar(StackPure<int>& stack, const int j);
		
		//! Embeds backedges from node \a v with direction \a v_dir to node \a w with direction \a w_dir
		/** \a i is the DFI of current embedded node.
		 */
		void embedBackedges(const node v, const int v_dir,
							const node w, const int w_dir, const int i);
		
		//! Links (not embed) backedges from node \a v with direction \a v_dir to node \a w with direction \a w_dir
		/** \a i is the DFI of current embedded node.
		 */
		void embedBackedgesOnlyPlanar(const node v, const int v_dir,
									const node w, const int w_dir, const int i);
		
		//! Creates a short circuit edge from node \a v with direction \a v_dir to node \a w with direction \a w_dir
		void createShortCircuitEdge(const node v, const int v_dir,
									const node w, const int w_dir);
		
		//! Walkup: Builds the pertinent subgraph for the backedge \a back.
		/** \a back is the backedge between nodes \a v and \a w. \a v is the current node to embed.
		 * All visited nodes are marked with value \a marker. The Function returns the last traversed node.
		 */
		node walkup(const node v, const node w, const int marker, const edge back);
		
		//! Walkdown: Embeds all backedges with DFI \a i as targetnode to node \a v
		/** @param i is the DFI of the current vertex to embed
		 * @param v is the virtual node being the root of the bicomp attached to \a i
		 * @param findKuratowskis collects information in order to extract Kuratowski Subdivisions later
		 * @param[out] 1, iff the embedding process found a stopping configuration
		 */
		int walkdown(const int i, const node v, FindKuratowskis& findKuratowskis);
		
		//! Merges unprocessed virtual nodes such as the dfs-roots with their real counterpart
		void mergeUnprocessedNodes();
		
		//! Postprocessing of the graph, so that all virtual vertices are embedded and all bicomps are flipped
		/** In addition, embedding steps for parallel edges and self-loops are implemented.
		 */
		void postProcessEmbedding();
		
		//! Starts the embedding phase, which embeds \a m_g node by node in descending DFI-order.
		/** Returns true, if graph is planar, false otherwise.
		 */
		bool embed();
		
		
		/***** Members ******/
		//! Input graph, which can be altered
		Graph& m_g;
		
		//! Some parameters... see BoyerMyrvold for further options
		const bool m_bundles;
		const int m_embeddingGrade;
		const bool m_limitStructures;
		const bool m_randomDFSTree;
		const bool m_avoidE2Minors;
		
		//! The whole number of bicomps, which have to be flipped
		int m_flippedNodes;
		
		#ifdef OGDF_DEBUG
		//! If not in statistics mode, delete all statistics
		std::ostream nirvana;
		#endif
		
		/***** Members from Boyer Myrvold-Init ******/
		//! Link to non-virtual vertex of a virtual Vertex.
		/** A virtual vertex has negative DFI of the DFS-Child of related non-virtual Vertex
		 */
		NodeArray<node> m_realVertex;
		
		//! The one and only DFI-NodeArray
		NodeArray<int> m_dfi;
		
		//! Returns appropriate node from given DFI
		Array<node> m_nodeFromDFI;
		
		//! Links to opposite adjacency entries on external face in clockwise resp. ccw order
		/** m_link[0]=CCW, m_link[1]=CW
		 */
		NodeArray<adjEntry> m_link[2];
		
		//! Links for short circuit edges.
		/** If short circuit edges are introduced, the former adjEntries to the neighbors
		 * have to be saved here for embedding and merging purposes. If there is no
		 * short circuit edge, this adjEntry is NULL.
		 */
		NodeArray<adjEntry> m_beforeSCE[2];
		
		//! The adjEntry which goes from DFS-parent to current vertex
		NodeArray<adjEntry> m_adjParent;
		
		//! The DFI of the least ancestor node over all backedges
		/** If no backedge exists, the least ancestor is the DFI of that node itself
		 */
		NodeArray<int> m_leastAncestor;
		
		//! Contains the type of each \a edge
		/** @param 0 = EDGE_UNDEFINED
		 * @param 1 = EDGE_SELFLOOP
		 * @param 2 = EDGE_BACK
		 * @param 3 = EDGE_DFS
		 * @param 4 = EDGE_DFS_PARALLEL
		 * @param 5 = EDGE_BACK_DELETED
		 */
		EdgeArray<int> m_edgeType;
		
		//! The lowpoint of each \a node
		NodeArray<int> m_lowPoint;
		
		//! The highest DFI in a subtree with \a node as root
		NodeArray<int> m_highestSubtreeDFI;
		
		//! A list to all separated DFS-children of \a node
		/** The list is sorted by lowpoint values (in linear time)
		*/
		NodeArray<ListPure<node> > m_separatedDFSChildList;
		
		//! Pointer to \a node contained in the DFSChildList of his parent, if exists.
		/** If node isn't in list or list doesn't exist, the pointer is set to NULL.
		*/
		NodeArray<ListIterator<node> > m_pNodeInParent;
		
		/***** Members for Walkup and Walkdown ******/
		//! This Array keeps track of all vertices that are visited by current walkup
		NodeArray<int> m_visited;
		
		//! Identifies the rootnode of the child bicomp the given backedge points to
		EdgeArray<node> m_pointsToRoot;
		
		//! Keeps track of all vertices that are visited by the walkup through a specific backedge
		/** This is done in order to refer to the unique child-bicomp of v.
		 */
		NodeArray<int> m_visitedWithBackedge;
		
		//! Iff true, the node is the root of a bicomp which has to be flipped.
		/** The DFS-child of every bicomp root vertex is unique. if a bicomp
		 * is flipped, this DFS-child is marked to check whether the bicomp
		 * has to be flipped or not.
		 */
		NodeArray<bool> m_flipped;
		
		//! Holds information, if node is the source of a backedge.
		/** This information refers to the adjEntries on the targetnode
		 * and is used during the walkdown
		 */
		NodeArray<SListPure<adjEntry> > m_backedgeFlags;
		
		//! List of virtual bicomp roots, that are pertinent to the current embedded node
		NodeArray<SListPure<node> > m_pertinentRoots;
		
		//! Data structure for the kuratowski subdivisions, which will be returned
		SListPure<KuratowskiStructure>& m_output;
};

// ONLY STATISTICS: Bucket Function for text-based output in linear time
#ifdef STATISTICS
//! BucketFunction for DFI buckets
/** Parameter DFI may not be deleted until destruction of this class.
 */
class BucketDFI : public BucketFunc<node> {
	public:
		BucketDFI(const NodeArray<int>& dfi) :m_dfi(dfi) { };
		
		//! This function has to be derived from BucketFunc and gets the buckets from lowPoint-Array
		int getBucket(const node& v) { return m_dfi[v]; }
		
	private:
		//! This is stored to be able to get the buckets
		const NodeArray<int>& m_dfi;
};
#endif


}

#endif
