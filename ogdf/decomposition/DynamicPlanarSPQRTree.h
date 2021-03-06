/*
 * $Revision: 1.1 $
 * 
 * last checkin:
 *   $Author: klein $ 
 *   $Date: 2007-11-12 15:54:55 +0100 (Mo, 12 Nov 2007) $ 
 ***************************************************************/
 
/** \file
 * \brief Declaration of class DynamicPlanarSPQRTree.
 * 
 * \author Jan Papenfu&szlig;
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


#ifndef OGDF_DYNAMIC_PLANAR_SPQR_TREE_H
#define OGDF_DYNAMIC_PLANAR_SPQR_TREE_H


#include <ogdf/DynamicSPQRTree.h>
#include <ogdf/decomposition/PlanarSPQRTree.h>


namespace ogdf {


template<class T, class I> class Array;
template<class A, class B> class Tuple2;


//---------------------------------------------------------
// DynamicPlanarSPQRTree
// extension of class DynamicSPQRTree for support of embedded graphs
//---------------------------------------------------------

//! SPQR-trees of planar graphs.
/**
 * The class DynamicPlanarSPQRTree maintains the triconnected components of a
 * planar biconnected graph G and represents all possible embeddings
 * of G. Each skeleton graph is embedded.
 *
 * The current embeddings of the skeletons define an embedding of G.
 * There are two basic operations for obtaining another embedding
 * of G: reverse(v), which flips the skeleton of an R-node v
 * around its poles, and swap(v,e_1,e_2), which exchanges the
 * positions of the edges e_1 and e_2 in the skeleton of a P-node v.
 */

class DynamicPlanarSPQRTree : public DynamicSPQRTree, public PlanarSPQRTree
{
public:

	// constructors

	//! Creates an SPQR tree \a T for planar graph \a G rooted at the first edge of \a G.
	/**
	 * If \a isEmbedded is set to true, \a G must represent a combinatorial 
	 * embedding, i.e., the counter-clockwise order of the adjacency entries
	 * around each vertex defines an embedding.
	 * \pre \a G is planar and biconnected and contains at least 3 nodes,
	 *      or \a G has exactly 2 nodes and at least 3  edges.
	 */
	DynamicPlanarSPQRTree(Graph &G, bool isEmbedded = false) :
		DynamicSPQRTree(G)
	{
		PlanarSPQRTree::init(isEmbedded);
	}

	//! Creates an SPQR tree \a T for planar graph \a G rooted at edge \a e.
	/**
	 * If \a isEmbedded is set to true, \a G must represent a combinatorial 
	 * embedding, i.e., the counter-clockwise order of the adjacency entries
	 * around each vertex defines an embedding.
	 * \pre \a e is an edge in \a G, and \a G is planar and biconnected and
	 * contains at least 3 nodes, or \a G has exactly 2 nodes and at least 3
	 * edges.
	 */
	DynamicPlanarSPQRTree(Graph &G, edge e, bool isEmbedded = false) :
		DynamicSPQRTree(G,e)
	{
		PlanarSPQRTree::init(isEmbedded);
	}
};


} // end namespace ogdf


#endif
