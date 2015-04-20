/*
 * $Revision: 1.4 $
 * 
 * last checkin:
 *   $Author: klein $ 
 *   $Date: 2007-11-14 16:26:58 +0100 (Mi, 14 Nov 2007) $ 
 ***************************************************************/
 
/** \file
 * \brief Implementation of class CPlanarSubClusteredST. 
 * Computes a (c-connected) spanning tree of a c-connected graph. 
 * 
 * \author Karsten Klein
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
 
 #include <ogdf/internal/cluster/CPlanarSubClusteredST.h>

//tests
#include <ogdf/basic/simple_graph_alg.h>


namespace ogdf {

void CPlanarSubClusteredST::call(const ClusterGraph &CG, EdgeArray<bool>& inST)
{

	//m_genDebug = 0;

	//initialize "call-global" info arrays
	m_allocCluster.init(CG.getGraph(), 0);
	//edge status
	m_edgeStatus.init(CG.getGraph(), 0);
	//edge to rep edge
	m_repEdge.init(CG, 0);
	//nodes and clusters to rep nodes
	m_cRepNode.init(CG, 0);
	m_vRepNode.init(CG, 0);

	inST.fill(false);

	//representationsgraphs for every cluster, on clustergraph
	ClusterArray<Graph*> l_clusterRepGraph(CG, 0); 
	computeRepresentationGraphs(CG, l_clusterRepGraph);

	//now we compute the spanning trees on the representation graphs
	//we should save the selection info on the original edge
	//are statically on the repgraphedges (we only have edge -> repedge 
	//information) but
	ClusterArray< EdgeArray<bool> > l_inTree(CG);

	cluster c;
	forall_clusters(c, CG)
	{
		l_inTree[c].init(*l_clusterRepGraph[c], false);
		//compute STs
		NodeArray<bool> visited(*l_clusterRepGraph[c], false);
		dfsBuildSpanningTree(l_clusterRepGraph[c]->firstNode(),
							 l_inTree[c],
							 visited);
	}//forallclusters


	OGDF_ASSERT(isConnected(CG.getGraph()));
	
//	//debug output
//	edge te;
//	int eCount = 0;
//
//	forall_clusters(c, CG)
//	{
//		edge tee;
//		forall_edges(tee,*l_clusterRepGraph[c])
//		{
//			if (l_inTree[c][tee])	
//			eCount++;
//		}
//	}//forallclusters
//	//cout << "Reported representation tree edges: " << eCount << "\n";
//
//
//	eCount = 0;
//	forall_edges(te, CG.getGraph())
//	{
//		if (l_inTree[m_allocCluster[te]][m_repEdge[te]])	
//			eCount++;
//	
//	}

	//compute the subclustered graph by constructing a spanning tree
	//using only the representation edges used in STs on the repgraphs
	NodeArray<bool> visited(CG.getGraph(), false);

	dfsBuildOriginalST(CG.getGraph().firstNode(),
					   l_inTree, 
					   inST,
					   visited);
	//unregister the edgearrays to avoid destructor failure after
	//representation graph deletion
	forall_clusters(c, CG)
	{
		l_inTree[c].init();
	}
	
	deleteRepresentationGraphs(CG, l_clusterRepGraph);
}//call


//spanning tree on input graph setting edge status and using
//repgraph spanning tree information
void CPlanarSubClusteredST::dfsBuildOriginalST(/*ClusterGraph& CG,*/
	node v,
	ClusterArray< EdgeArray<bool> > &treeEdges,    //edges in repgraph
	EdgeArray<bool>& inST,                         //original edges
	NodeArray<bool> &visited)
{

	//should check here if m_edgeStatus initialized

	visited[v] = true;

	edge e;
	forall_adj_edges(e,v)
	{
		//no selfloops
		node w = e->opposite(v);
		if (w == v) continue;
		//only repgraph ST edges are allowed
		//we should save the common cluster at the first computation above,
		//do this later when code works, otherwise running time m*m*c
		//cluster c1, c2;
		cluster allocCluster = m_allocCluster[e];
	
		OGDF_ASSERT(allocCluster != 0);

		//we should check here repedgegraph==graph
		if (! treeEdges[allocCluster][m_repEdge[e]]) continue;

		//(this part is always connected in original!)

		if(!visited[w]) {
			//treeEdges[e] = true;
			m_edgeStatus[e] |= 1; //e is in ST
			inST[e] = true;
			dfsBuildOriginalST(/*CG,*/ w, treeEdges, inST, visited);
		}
	}
}


//we should later provide a minimum st to allow weights on edges
void CPlanarSubClusteredST::dfsBuildSpanningTree(
	node v,
	EdgeArray<bool> &treeEdges,
	NodeArray<bool> &visited)
{
	OGDF_ASSERT(isConnected(*(v->graphOf())));
	visited[v] = true;

	edge e;
	forall_adj_edges(e,v)
	{
		node w = e->opposite(v);
		if(w == v) continue;

		if(!visited[w]) {
			treeEdges[e] = true;
		//	m_genDebug++; //debugonly
			dfsBuildSpanningTree(w,treeEdges,visited);
		}
	}
}


}//end namespace ogdf
