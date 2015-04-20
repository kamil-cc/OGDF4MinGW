/*
 * $Revision: 1.2 $
 * 
 * last checkin:
 *   $Author: gutwenger $ 
 *   $Date: 2007-11-08 16:10:37 +0100 (Do, 08 Nov 2007) $ 
 ***************************************************************/
 
/** \file
 * \brief Declaration of interface for minor-monotone edge
 *        insertion algorithms.
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

#ifndef OGDF_MM_EDGE_INSERTION_MODULE_H
#define OGDF_MM_EDGE_INSERTION_MODULE_H


#include <ogdf/planarity/PlanRepExpansion.h>
#include <ogdf/basic/Module.h>


namespace ogdf {

/**
 * \brief Interface for minor-monotone edge insertion algorithms.
 *
 * \see MMSubgraphPlanarizer
 */
class MMEdgeInsertionModule : public Module {
public:
	//! The postprocessing methods.
	enum RemoveReinsertType {
		rrNone,        //!< No postprocessing.
		rrInserted,    //!< Postprocessing only with the edges that have to be inserted.
		rrMostCrossed, //!< Postprocessing with the edges involved in the most crossings.
		rrAll,         //!< Postproceesing with all edges and all node splits.
		rrIncremental  //!< Full postprocessing after each edge insertion.
	};

	//! Initializes a minor-monotone edge insertion module.
	MMEdgeInsertionModule() { }

	// destruction
	virtual ~MMEdgeInsertionModule() { }

	/**
	 * \brief Inserts all edges in \a origEdges into \a PG.
	 *
	 * @param PG is the input planarized expansion and will also receive the result.
	 * @param origEdges is the list of original edges (edges in the original graph
	 *        of \a PG) that have to be inserted.
	 * \return the status of the result.
	 */
	ReturnType call(PlanRepExpansion &PG, const List<edge> &origEdges) {
		return doCall(PG, origEdges);
	}

protected:
	/**
	 * \brief Actual algorithm call that has to be implemented by derived classes.
	 *
	 * @param PG is the input planarized expansion and will also receive the result.
	 * @param origEdges is the list of original edges (edges in the original graph
	 *        of \a PG) that have to be inserted.
	 */
	virtual ReturnType doCall(PlanRepExpansion &PG,
		const List<edge> &origEdges) = 0;


	OGDF_MALLOC_NEW_DELETE
};

} // end namespace ogdf

#endif
