/*
 * $Revision: 1.2 $
 * 
 * last checkin:
 *   $Author: gutwenger $ 
 *   $Date: 2007-11-08 16:10:37 +0100 (Do, 08 Nov 2007) $ 
 ***************************************************************/
 
/** \file
 * \brief Declaration of CPlanarSubClusteredST class.
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

#ifdef _MSC_VER
#pragma once
#endif

#ifndef OGDF_CPLANAR_SUBCLUSTERED_ST_H
#define OGDF_CPLANAR_SUBCLUSTERED_ST_H


#include <ogdf/cluster/ClusterGraph.h>
#include <ogdf/cluster/ClusterArray.h>
#include <ogdf/basic/EdgeArray.h>

namespace ogdf {

//! Constructs a c-planar subclustered spanning tree of the input by setting edgearray values
class CPlanarSubClusteredST
{

public:

	CPlanarSubClusteredST() {};

	//! sets values in inST according to membership in c-planar STGraph
	virtual void call(const ClusterGraph& CG, EdgeArray<bool>& inST);

private:

	//! builds spanning tree on original graph out of repgraphS STs
	void dfsBuildOriginalST(/*ClusterGraph& CG,*/
	node v,
	ClusterArray< EdgeArray<bool> > &treeEdges,    //edges in repgraph
	EdgeArray<bool>& inST,                         //original edges
	NodeArray<bool> &visited);
	//builds spanning tree on graph of node v in treeEdges
	void dfsBuildSpanningTree(node v, 
				 			  EdgeArray<bool> &treeEdges,
							  NodeArray<bool> &visited);

	//! constructs for every cluster graph representing its
	//! main structure (children & their connections)
	//! only insert nodes here 
	void constructRepresentationGraphNodes(const ClusterGraph& CG,
										   Graph& g, 
									       cluster c
										   )
	{
		
		//insert nodes for all child cluster
		ListConstIterator<cluster> it;
		for (it = c->cBegin(); it.valid(); it++) 
		{
			node v = g.newNode();
			m_cRepNode[*it] = v;
		}//for
		//insert nodes for all node entries in c
		ListConstIterator<node> itn;
		for (itn = c->nBegin(); itn.valid(); itn++) 
		{
			node v = g.newNode();
			m_vRepNode[*itn] = v;
		}//for
	}//constructRepresentationGraphNodes

	//! insert rep edges for all edges in clustergraph
	void constructRepresentationGraphEdges(const ClusterGraph& CG,
										   ClusterArray<Graph*>& RepGraph)
	{
		edge e;
		forall_edges(e, CG.getGraph())
		{
			//insert representation in RepGraph[allocation cluster]
			//defined by lowest common ancestor of end points
			node u = e->source();
			node v = e->target();
			cluster uAncestor, vAncestor;
			cluster allocCluster = 
				CG.commonClusterLastAncestors(u,v, uAncestor, vAncestor);
			m_allocCluster[e] = allocCluster;
			//now derive the real ancestors (maybe the nodes themselves from
			//the supplied clusters

			//Case1: both nodes in same cluster => connect the nodes in the 
			//cluster representation graph
			if (uAncestor == vAncestor)
			{
				edge eNew = RepGraph[uAncestor]->newEdge(
								m_vRepNode[u], m_vRepNode[v]);
				m_repEdge[e] = eNew;
			}//if
			else 
			{
				OGDF_ASSERT(!((uAncestor == CG.rootCluster())&&
					(vAncestor == CG.rootCluster())))
				//now only one node can be directly in rootcluster
				//this case now seems to fall together with else else...
				if (uAncestor == CG.rootCluster())
				{
					OGDF_ASSERT(false);
					edge eNew = RepGraph[uAncestor]->newEdge(
								m_vRepNode[u], m_cRepNode[vAncestor]);
					m_repEdge[e] = eNew;
				}//if u in rootcluster
				else if (vAncestor == CG.rootCluster())
				{
					OGDF_ASSERT(false);
					edge eNew = RepGraph[vAncestor]->newEdge(
								m_cRepNode[uAncestor], m_vRepNode[v]);
					m_repEdge[e] = eNew;
				}//if v in rootcluster
				else
				{
					OGDF_ASSERT(allocCluster != 0)
					//now create edge in lowest common cluster
					//if ancestor is cluster(v), then representant is node
					node v1, v2;
					v1 = ( (uAncestor == 0) ? m_vRepNode[u] : 
								m_cRepNode[uAncestor]);
					v2 = ( (vAncestor == 0) ? m_vRepNode[v] : 
								m_cRepNode[vAncestor]);
					edge eNew = RepGraph[allocCluster]->newEdge(v1, v2);
					m_repEdge[e] = eNew;
				}
			}//else

		}//foralledges
		//m_repEdge
	}//constructRepresentationGraphEdges

	void computeRepresentationGraphs(const ClusterGraph& CG, 
									 ClusterArray<Graph*>& RepGraph)
	{
		cluster c;
		forall_clusters(c, CG)
		{
			RepGraph[c] = new Graph;
			constructRepresentationGraphNodes(CG, *RepGraph[c], c);
		}//forallclusters
		constructRepresentationGraphEdges(CG, RepGraph);
	}//computeRepresentationGraphs

	void deleteRepresentationGraphs(const ClusterGraph& CG, 
									 ClusterArray<Graph*>& RepGraph)
	{
		cluster c;
		forall_clusters(c, CG)
		{
			if (RepGraph[c])
				delete RepGraph[c];
		}//forallclusters
	
	}//deleteRepresentationGraphs

    //****************************************************
	//data fields

	//! store status of original edge: in subclustered graph? also used to check spanning tree
	EdgeArray<int> m_edgeStatus;

	//! store the allocation cluster to avoid multiple computation
	EdgeArray<cluster> m_allocCluster;
	//! store the representation edge
	EdgeArray<edge> m_repEdge;
	//! store the representation nodes for nodes and clusters
	ClusterArray<node> m_cRepNode;
	NodeArray<node> m_vRepNode;
	//pointer to input ClusterPlanRep
	//set in call, not to be used anywhere else
	//m_pCPR;
	//int m_genDebug; //only for debugging

};//cplanarsubclusteredST

} // end namespace ogdf


#endif
