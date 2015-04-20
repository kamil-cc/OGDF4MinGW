/*
 * $Revision: 1.3 $
 * 
 * last checkin:
 *   $Author: klein $ 
 *   $Date: 2007-11-13 15:48:51 +0100 (Di, 13 Nov 2007) $ 
 ***************************************************************/
 
/** \file
 * \brief Abstract base class for structures on graphs, that need
 *        to be informed about graph changes (e.g. cluster graphs).
 *
 * Follows the observer pattern: graphs are observable 
 * objects that can inform observers on changes made to their
 * structure.
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

#ifndef OGDF_GRAPH_STRUCTURE_H
#define OGDF_GRAPH_STRUCTURE_H


#include <ogdf/basic/List.h>
#include <ogdf/basic/Graph.h>

namespace ogdf {

//
// in embedded graphs, adjacency lists are given in clockwise order.
//


//class Graph;
//class node;
//class edge;


//----------------------------------------------------------
// GraphObserver
//----------------------------------------------------------
/**
 * \brief Abstract Base class for classes that need to keep track 
 * of changes in the graph like addition/deletion of nodes 
 * or edges.
 * derived classes have to overload nodeDeleted, nodeAdded
 * edgeDeleted, edgeAdded
 * these functions should be called by Graph before (delete)
 * 
 */
 
class GraphObserver {
	friend class Graph;

public:
	//! Constructs instance of GraphObserver class
	GraphObserver() : m_pGraph(0) {}
	/**
	 *\brief Constructs instance of GraphObserver class
	 * \param G is the graph to be watched
	 */ 
	GraphObserver(const Graph* G) : m_pGraph(G) 
	{
		m_itGList = G->registerStructure(this);
	}//constructor
	//! Destroys the instance, unregisters it from watched graph
	virtual ~GraphObserver() 
	{
		if (m_pGraph) m_pGraph->unregisterStructure(m_itGList);
	}//destructor

	//! Associates observer instance with graph \a G
	void reregister(const Graph *pG) {
		//small speedup: check if == m_pGraph
		if (m_pGraph) m_pGraph->unregisterStructure(m_itGList);
		if ((m_pGraph = pG) != 0) m_itGList = pG->registerStructure(this);
	}

	//! Called by watched graph when a node is deleted
	//! Has to be implemented by derived classes
	virtual void nodeDeleted(node v) = 0;
	//! Called by watched graph when a node is added
	//! Has to be implemented by derived classes
	virtual void nodeAdded(node v)   = 0;
	//! Called by watched graph when an edge is deleted
	//! Has to be implemented by derived classes
	virtual void edgeDeleted(edge e) = 0;
	//! Called by watched graph when an edge is added
	//! Has to be implemented by derived classes
	virtual void edgeAdded(edge e)   = 0;
	//! Called by watched graph when it is reinitialized
	//! Has to be implemented by derived classes
	virtual void reInit()            = 0; 
	//! Called by watched graph when its clear function is called
	//! Has to be implemented by derived classes
	virtual void cleared()           = 0;

	const Graph*  getGraph() const {return m_pGraph;}

protected:
	const Graph* m_pGraph; //! watched graph
	ListIterator<GraphObserver*> m_itGList; //! List entry in graphs list of all registered graphobservers


};

} //end namespace ogdf

#endif
