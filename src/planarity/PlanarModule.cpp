/*
 * $Revision: 1.5 $
 * 
 * last checkin:
 *   $Author: gutwenger $ 
 *   $Date: 2007-11-08 16:10:37 +0100 (Do, 08 Nov 2007) $ 
 ***************************************************************/
 
/** \file
 * \brief  Implementation of the Planar Module. 
 * 
 * Implements planarity test and planar embedding algorithm.
 * 
 * \author Sebastian Leipert
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


#include <ogdf/basic/basic.h>
#include <ogdf/basic/Array.h>
#include <ogdf/basic/NodeArray.h>
#include <ogdf/basic/SList.h>
#include <ogdf/basic/simple_graph_alg.h>
#include <ogdf/basic/extended_graph_alg.h>
#include <ogdf/internal/planarity/PlanarPQTree.h>
#include <ogdf/internal/planarity/PlanarLeafKey.h>
#include <ogdf/planarity/PlanarModule.h>
#include <ogdf/internal/planarity/EmbedPQTree.h>


namespace ogdf{

bool PlanarModule::planarityTest(Graph &G)
{
	bool ret = preparation(G,false);
	m_parallelEdges.init();
	m_isParallel.init();

	return ret;
}

bool PlanarModule::planarityTest(const Graph &G)
{
	Graph Gp(G); 
	bool ret = preparation(Gp,false);
	m_parallelEdges.init();
	m_isParallel.init();

	return ret;
}

// Prepares the planarity test and the planar embedding
// Parallel edges:  do not need to be ignored, they can be handled
// by the planarity test.
// Selfloops: need to be ignored.
bool PlanarModule::preparation(Graph  &G,bool embed)
{

	if (G.numberOfEdges() < 9 && !embed)
		return true;
	else if (G.numberOfEdges() < 3 && embed)
		return true;


	node v;
	edge e;

	SListPure<node> selfLoops;
	makeLoopFree(G,selfLoops);

	prepareParallelEdges(G);

	int  isolated = 0;
	forall_nodes(v,G)
		if (v->degree() == 0)
			isolated++;

	if (((G.numberOfNodes()-isolated) > 2) &&
		((3*(G.numberOfNodes()-isolated) -6) < (G.numberOfEdges() - m_parallelCount)))
		return false;



	bool planar = true;


	NodeArray<node> tableNodes(G,0);
	EdgeArray<edge> tableEdges(G,0);
	NodeArray<bool> mark(G,0);

	EdgeArray<int> componentID(G);


	// Determine Biconnected Components
	int bcCount = biconnectedComponents(G,componentID);

	// Determine edges per biconnected component
	Array<SList<edge> > blockEdges(0,bcCount-1);
	forall_edges(e,G)
	{
		blockEdges[componentID[e]].pushFront(e);
	} 

	// Determine nodes per biconnected component.
	Array<SList<node> > blockNodes(0,bcCount-1);
	int i;
	for (i = 0; i < bcCount; i++)
	{
		SListIterator<edge> it;
		for (it = blockEdges[i].begin(); it.valid(); ++it)
		{
			e = *it;
			if (!mark[e->source()])
			{
				blockNodes[i].pushBack(e->source());
				mark[e->source()] = true;
			}
			if (!mark[e->target()])
			{
				blockNodes[i].pushBack(e->target());
				mark[e->target()] = true;
			}
		}
		SListIterator<node> itn;
		for (itn = blockNodes[i].begin(); itn.valid(); ++itn)
		{
			v = *itn;
			if (mark[v])
				mark[v] = false;
			else
				OGDF_ASSERT(mark[v]); // v has been placed two times on the list.
		}

	}
	
	

	// Perform Planarity Test for every biconnected component
		
	if (bcCount == 1)
	{
		if (G.numberOfEdges() >= 2)
		{
			// Compute st-numbering
			NodeArray<int> numbering(G,0);
			int n = stNumber(G,numbering);
			OGDF_ASSERT_IF(dlConsistencyChecks,testSTnumber(G,numbering,n))

			EdgeArray<edge> backTableEdges(G,0);
			forall_edges(e,G)
				backTableEdges[e] = e;

			if (embed)
				planar = doEmbed(G,numbering,backTableEdges,backTableEdges);
			else
				planar = doTest(G,numbering);
		}

	}
	else
	{
		NodeArray<SListPure<adjEntry> > entireEmbedding(G);
		for (i = 0; i < bcCount; i++)
		{
			#ifdef OGDF_DEBUG 
			if(int(ogdf::debugLevel)>=int(dlHeavyChecks)){
				cout<<endl<<endl<<"-----------------------------------";
				cout<<endl<<endl<<"Component "<<i<<endl;}
			#endif

			Graph C;
		
			SListIterator<node> itn;
			for (itn = blockNodes[i].begin(); itn.valid(); ++ itn)
			{
				v = *itn;
				node w = C.newNode();
				tableNodes[v] = w;

				#ifdef OGDF_DEBUG 
				if(int(ogdf::debugLevel)>=int(dlHeavyChecks)){
					cout <<"Original: " << v << " New: " << w<< endl;}
				#endif

			}

			NodeArray<node> backTableNodes(C,0);
			if (embed)
			{
				for (itn = blockNodes[i].begin(); itn.valid(); ++ itn)
					backTableNodes[tableNodes[*itn]] = *itn;
			}

			SListIterator<edge> it;
			for (it = blockEdges[i].begin(); it.valid(); ++it)
			{
				e = *it;
				edge f = C.newEdge(tableNodes[e->source()],tableNodes[e->target()]);
				tableEdges[e] = f;
			}

			EdgeArray<edge> backTableEdges(C,0);
			for (it = blockEdges[i].begin(); it.valid(); ++it)
				backTableEdges[tableEdges[*it]] = *it;
		
			if (C.numberOfEdges() >= 2)
			{
				// Compute st-numbering
				NodeArray<int> numbering(C,0);
				int n = stNumber(C,numbering);
				OGDF_ASSERT_IF(dlConsistencyChecks,testSTnumber(C,numbering,n))

				if (embed)
					planar = doEmbed(C,numbering,backTableEdges,tableEdges);
				else
					planar = doTest(C,numbering);

				if (!planar)
					break;
			}

			if (embed)
			{
				forall_nodes(v,C)
				{
					node w = backTableNodes[v];
					adjEntry a;
					forall_adj(a,v)
					{
						edge e = backTableEdges[a->theEdge()];
						adjEntry adj = (e->adjSource()->theNode() == w)? 
										e->adjSource() : e->adjTarget();
						entireEmbedding[w].pushBack(adj);
					}
				}
			}




		}
		if (planar && embed)
		{
			forall_nodes(v,G)
				G.sort(v,entireEmbedding[v]);

			#ifdef OGDF_DEBUG 
			if (int(ogdf::debugLevel) >= int(dlHeavyChecks)){
				cout << endl << "New AdjLists of complete graph" << endl;
				forall_nodes(v,G){
					cout << "v = " << v << " : " << " ";
					SListIterator<adjEntry> it;
					for(it=entireEmbedding[v].begin();it.valid();it++)cout<<*it<<" ";
					cout << endl;adjEntry adj;
					forall_adj(adj,v)cout<<" "<<adj<<"["<<adj->theEdge()->index()<<"]";
					cout << endl;}}
			#endif

			// Test if embedding was determined correctly. 
			OGDF_ASSERT(G.representsCombEmbedding())
		}

	}

	while (!selfLoops.empty())
	{
		v = selfLoops.popFrontRet();
		G.newEdge(v,v);
	}

	if (planar && embed)
	{
		// Test if embedding was determined correctly. 
		OGDF_ASSERT(G.representsCombEmbedding())
	}

	return planar; 
}


// Performs a planarity test on a biconnected component
// of G. numbering contains an st-numbering of the component.
bool PlanarModule::doTest(Graph &G,NodeArray<int> &numbering)
{
	node v;
	bool planar = true;

	NodeArray<SListPure<PlanarLeafKey<indInfo*>* > > inLeaves(G);
	NodeArray<SListPure<PlanarLeafKey<indInfo*>* > > outLeaves(G);
	Array<node> table(G.numberOfNodes()+1);

	forall_nodes(v,G)
	{
		edge e;
		forall_adj_edges(e,v)
		{
			if (numbering[e->opposite(v)] > numbering[v]) 
				//sideeffect: loops are ignored
			{
				PlanarLeafKey<indInfo*>* L = OGDF_NEW PlanarLeafKey<indInfo*>(e);
				inLeaves[v].pushFront(L);
			}
		}
		table[numbering[v]] = v;
	}

	forall_nodes(v,G)
	{
		SListIterator<PlanarLeafKey<indInfo*>* > it;
		for (it = inLeaves[v].begin(); it.valid(); ++it)
		{
			PlanarLeafKey<indInfo*>* L = *it;
			outLeaves[L->userStructKey()->opposite(v)].pushFront(L);
		}
	}

	PlanarPQTree T;

	T.Initialize(inLeaves[table[1]]);
	for (int i = 2; i < G.numberOfNodes(); i++)
	{
		if (T.Reduction(outLeaves[table[i]]))
		{
			T.ReplaceRoot(inLeaves[table[i]]);
			T.emptyAllPertinentNodes();

		}
		else
		{
			planar = false;
			break;
		}
	}
	if (planar)
		T.emptyAllPertinentNodes();


	// Cleanup
	forall_nodes(v,G)
	{
		while (!inLeaves[v].empty())
		{
			PlanarLeafKey<indInfo*>* L = inLeaves[v].popFrontRet();
			delete L;	
		}
	}

	return planar;
}


// Performs a planarity test on a biconnected component
// of G and embedds it planar. 
// numbering contains an st-numbering of the component.
bool PlanarModule::doEmbed(Graph &G,
						   NodeArray<int>  &numbering,			
						   EdgeArray<edge> &backTableEdges,
						   EdgeArray<edge> &forwardTableEdges)
{
	node v;
	
	NodeArray<SListPure<PlanarLeafKey<indInfo*>* > > inLeaves(G);
	NodeArray<SListPure<PlanarLeafKey<indInfo*>* > > outLeaves(G);
	NodeArray<SListPure<edge> > frontier(G);
	NodeArray<SListPure<node> > opposed(G);
	NodeArray<SListPure<node> > nonOpposed(G);	
	Array<node> table(G.numberOfNodes()+1);
	Array<bool> toReverse(1,G.numberOfNodes()+1,false);

	forall_nodes(v,G)
	{
		edge e;

		forall_adj_edges(e,v)
		{
			if (numbering[e->opposite(v)] > numbering[v])
			{
				PlanarLeafKey<indInfo*>* L = OGDF_NEW PlanarLeafKey<indInfo*>(e);
				inLeaves[v].pushFront(L);
			}
		}
		table[numbering[v]] = v;
	}

	forall_nodes(v,G)
	{
		SListIterator<PlanarLeafKey<indInfo*>* > it;
		for (it = inLeaves[v].begin(); it.valid(); ++it)
		{
			PlanarLeafKey<indInfo*>* L = *it;
			outLeaves[L->userStructKey()->opposite(v)].pushFront(L);
		}
	}

	EmbedPQTree T;

	T.Initialize(inLeaves[table[1]]);
	int i;
	for (i = 2; i <= G.numberOfNodes(); i++)
	{
		if (T.Reduction(outLeaves[table[i]]))
		{
			#ifdef OGDF_DEBUG
				char aa[12]; ogdf::sprintf(aa,12,"%d.txt",i);
				if (int(ogdf::debugLevel)>=int(dlHeavyChecks))
					T.printOutCurrentTree("Before",aa);
			#endif

			T.ReplaceRoot(inLeaves[table[i]],frontier[table[i]],
						  opposed[table[i]],nonOpposed[table[i]],table[i]);
			T.emptyAllPertinentNodes();

			#ifdef OGDF_DEBUG
				if(i<G.numberOfNodes()&&int(ogdf::debugLevel)>=int(dlHeavyChecks))
					T.printOutCurrentTree("After",aa);
			#endif
		}
		else
		{
			// Cleanup
			forall_nodes(v,G)
			{
				while (!inLeaves[v].empty())
				{
					PlanarLeafKey<indInfo*>* L = inLeaves[v].popFrontRet();
					delete L;	
				}
			}
			return false;
		}
	}

	#ifdef OGDF_DEBUG
	if (int(ogdf::debugLevel) >= int(dlHeavyChecks)){
		for (i = 1; i <= G.numberOfNodes(); i++){		
			cout << "st-no.: " << i << " v = " << table[i] << " nonOpposed: ";
			SListIterator<node> it;
			for(it=nonOpposed[table[i]].begin();it.valid();it++)cout<<*it<<" ";
			cout << "Opposed: ";
			for(it=opposed[table[i]].begin();it.valid();it++)cout<<*it<<" ";
			cout << endl;}
		cout << endl << "Original Adjlists " << endl;
		forall_nodes(v,G){cout<<"v = "<<v<<" : "<<" ";
			adjEntry adj;
			forall_adj(adj,v)cout << adj << " ";
			cout << endl;}
		cout << endl << "New Lists " << endl;
		forall_nodes(v,G){cout<<"v = "<<v<<" : "<<" ";
			SListIterator<edge> it;
			for(it=frontier[v].begin();it.valid();it++)cout<<*it<<" ";
			cout << endl;}}
	#endif
		

	// Reverse adjacency lists if necessary
	// This gives an upward embedding
	for (i = G.numberOfNodes(); i >= 2; i--)
	{
		if (toReverse[i])
		{
			while (!nonOpposed[table[i]].empty())
			{
				v = nonOpposed[table[i]].popFrontRet();
				OGDF_ASSERT(!toReverse[numbering[v]])
				toReverse[numbering[v]] =  true;
			}
			frontier[table[i]].reverse();
		}
		else
		{
			while (!opposed[table[i]].empty())
			{
				v = opposed[table[i]].popFrontRet();
				OGDF_ASSERT(!toReverse[numbering[v]])
				toReverse[numbering[v]] =  true;
			}
		}
		nonOpposed[table[i]].clear();
		opposed[table[i]].clear();
	}

	#ifdef OGDF_DEBUG
	if (int(ogdf::debugLevel) >= int(dlHeavyChecks)){
		cout << endl << "New Lists after Reversing " << endl;
		forall_nodes(v,G){cout<<"v = "<<v<<" : "<<" ";
			SListIterator<edge> it;
			for(it=frontier[v].begin();it.valid();it++)cout<<*it<<" ";
			cout << endl;}}
	#endif
		
	// Compute the entire embedding
	NodeArray<SListPure<adjEntry> > entireEmbedding(G);
	forall_nodes(v,G)
	{
		while (!frontier[v].empty())
		{
			edge e = frontier[v].popFrontRet();
			entireEmbedding[v].pushBack(
				(e->adjSource()->theNode() == v)? e->adjSource() : e->adjTarget());
		}
	}

	#ifdef OGDF_DEBUG
	if (int(ogdf::debugLevel) >= int(dlHeavyChecks)){
		cout << endl << "New upward AdjLists " << endl;
		forall_nodes(v,G){cout<<"v = "<<v<<" : "<<" ";
			SListIterator<adjEntry> it;
			for(it=entireEmbedding[v].begin();it.valid();it++)cout<<*it<<" ";
			cout << endl;}}
	#endif


	NodeArray<bool> mark(G,false);
	NodeArray<SListIterator<adjEntry> > adjMarker(G,0);
	forall_nodes(v,G)
		adjMarker[v] = entireEmbedding[v].begin();
	v = table[G.numberOfNodes()];
	entireEmbed(G,entireEmbedding,adjMarker,mark,v);



	#ifdef OGDF_DEBUG
	if (int(ogdf::debugLevel) >= int(dlHeavyChecks)){
		cout << endl << "New complete AdjLists " << endl;
		forall_nodes(v,G){cout<<"v = "<<v<<" : "<<" ";
			SListIterator<adjEntry> it;
			for(it=entireEmbedding[v].begin();it.valid();it++)
				cout<<*it<<" ";cout << endl;}}
	#endif



	NodeArray<SListPure<adjEntry> > newEntireEmbedding(G);
	if (m_parallelCount > 0)
	{
		forall_nodes(v,G)
		{
			//adjEntry a;
			SListIterator<adjEntry> it;
			for(it=entireEmbedding[v].begin();it.valid();it++)
			{
				edge e = (*it)->theEdge(); // edge in biconnected component
				edge trans = backTableEdges[e]; // edge in original graph.
				if (!m_parallelEdges[trans].empty())
				{
					// This original edge is the reference edge
					// of a bundle of parallel edges

					ListIterator<edge> it;
					// If v is source of e, insert the parallel edges 
					// in the order stored in the list.
					if (e->adjSource()->theNode() == v)
					{
						adjEntry adj = e->adjSource();
						newEntireEmbedding[v].pushBack(adj);
						for (it = m_parallelEdges[trans].begin(); it.valid(); it++)
						{
							edge parallel = forwardTableEdges[*it];
							adjEntry adj = parallel->adjSource()->theNode() == v? 
										   parallel->adjSource() : parallel->adjTarget();
							newEntireEmbedding[v].pushBack(adj);
						}
					}
					else
					// v is target of e, insert the parallel edges 
					// in the opposite order stored in the list.
					// This keeps the embedding.
					{
						for (it = m_parallelEdges[trans].rbegin(); it.valid(); it--)
						{
							edge parallel = forwardTableEdges[*it];
							adjEntry adj = parallel->adjSource()->theNode() == v? 
										   parallel->adjSource() : parallel->adjTarget();
							newEntireEmbedding[v].pushBack(adj);
						}
						adjEntry adj = e->adjTarget();
						newEntireEmbedding[v].pushBack(adj);
					}
				}
				else if (!m_isParallel[trans])
					// normal non-multi-edge
				{
					adjEntry adj = e->adjSource()->theNode() == v? 
									e->adjSource() : e->adjTarget();
					newEntireEmbedding[v].pushBack(adj);
				}
				// else e is a multi-edge but not the reference edge
			}
		}
		#ifdef OGDF_DEBUG
		if (int(ogdf::debugLevel) >= int(dlHeavyChecks)){
			cout << endl << "New complete AdjLists with parallel edges corrected" << endl;
			forall_nodes(v,G){cout<<"v = "<<v<<" : "<<" ";
				SListIterator<adjEntry> it;
				for(it=newEntireEmbedding[v].begin();it.valid();it++)
					cout<<*it<<" ";cout << endl;}}
		#endif

		forall_nodes(v,G)
			G.sort(v,newEntireEmbedding[v]);
	}
	else
	{
		forall_nodes(v,G)
			G.sort(v,entireEmbedding[v]);
	}


	#ifdef OGDF_DEBUG
	if (int(ogdf::debugLevel) >= int(dlHeavyChecks)){
		cout << endl << "New Adjlists " << endl;
		forall_nodes(v,G){cout<<"v = "<<v<<" : "<<" ";
			adjEntry adj;forall_adj(adj,v)cout<<adj<<" ";
			cout<<endl;}}
	#endif


	// Test if embedding was determined correctly. 
	OGDF_ASSERT(G.representsCombEmbedding())

	//cleanup
	forall_nodes(v,G)
	{
		while (!inLeaves[v].empty())
		{
			PlanarLeafKey<indInfo*>* L = inLeaves[v].popFrontRet();
			delete L;	
		}
	}

	return true;
}

// Used by doEmbed. Computes an entire embedding from an
// upward embedding.
void PlanarModule::entireEmbed(Graph &G,
							   NodeArray<SListPure<adjEntry> > &entireEmbedding,
							   NodeArray<SListIterator<adjEntry> > &adjMarker,
							   NodeArray<bool> &mark,
							   node v)
{
	mark[v] = true;
	SListIterator<adjEntry> it;
	for (it = adjMarker[v]; it.valid(); ++it)
	{
		adjEntry a = *it;
		edge e = a->theEdge();
		adjEntry adj = (e->adjSource()->theNode() == v)? 
						e->adjTarget() : e->adjSource();
		node w = adj->theNode();
		entireEmbedding[w].pushFront(adj);
		if (!mark[w])
			entireEmbed(G,entireEmbedding,adjMarker,mark,w);
	}
}



void PlanarModule::prepareParallelEdges(Graph &G)
{

	edge e;

	// Stores for one reference edge all parallel edges.
	m_parallelEdges.init(G);
	// Is true for any multiedge, except for the reference edge.
	m_isParallel.init(G,false);
	getParallelFreeUndirected(G,m_parallelEdges);
	m_parallelCount = 0;
	forall_edges(e,G)
	{
		if (!m_parallelEdges[e].empty())
		{
			ListIterator<edge> it;
			for (it = m_parallelEdges[e].begin(); it.valid(); it++)
			{
				m_isParallel[*it] = true;
				m_parallelCount++;
			}
		}
	}
}





}