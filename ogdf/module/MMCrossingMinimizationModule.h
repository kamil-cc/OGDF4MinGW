/*
 * $Revision: 1.5 $
 * 
 * last checkin:
 *   $Author: gutwenger $ 
 *   $Date: 2007-11-08 16:10:37 +0100 (Do, 08 Nov 2007) $ 
 ***************************************************************/
 
/** \file
 * \brief Declaration of MMCrossingMinimization Module, an interface
 * for minor-monotone crossing minimization algorithms.
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

#ifndef OGDF_MM_CROSSING_MINIMIZATION_MODULE_H
#define OGDF_MM_CROSSING_MINIMIZATION_MODULE_H



#include <ogdf/planarity/PlanRepExpansion.h>
#include <ogdf/basic/Module.h>
#include <ogdf/basic/Logger.h>
#include <ogdf/basic/exceptions.h>


namespace ogdf {

/**
 * \brief Interface for minor-monotone crossing minimization algorithms.
 *
 */
class MMCrossingMinimizationModule : public Module
{
public:
	//! Initializes a minor-monotone crossing minimization module.
	MMCrossingMinimizationModule() { m_nodeSplits = 0; }

	// destruction
	virtual ~MMCrossingMinimizationModule() { }

	/**
	 * \brief Computes a planarized representation of an expansion of the input graph.
	 *
	 * @param PG represents the input graph as well as the computed planarized
	 *        expansion after the call. \a PG has to be initialzed as a
	 *        PlanRepExpansion of the input graph and is modified to obatain the planarized
	 *        representation (nodes are eventually expanded by splitting the node and
	 *        crossings are replaced by dummy vertices with degree four).
	 * @param cc is the number of the connected component in \a PG that is considered.
	 * @param crossingNumber is assigned the number of crossings.
	 * \return the status of the result.
	 */
	ReturnType call(PlanRepExpansion &PG,
			int cc,
			int&  crossingNumber) {
		return doCall(PG, cc, crossingNumber, m_nodeSplits, m_splittedNodes);
	};

	/**
	 * \brief Performs minor-monotone crossing minimization on \a G.
	 *
	 * @param G is the input graph.
	 * @param cr is assigned the number of crossings.
	 * \return the status of the result.
	 */
	ReturnType call(const Graph &G, int &cr);

	/**
	 * \brief Performs minor-monotone crossing minimization on \a G for given splittable nodes.
	 *
	 * @param G is the input graph.
	 * @param splittableNodes is the list of nodes that are allowed to be split.
	 * @param cr is assigned the number of crossings.
	 * \return the status of the result.
	 */
	ReturnType call(const Graph &G, const List<node> &splittableNodes, int &cr);

	/**
	 * \brief Returns the number of required node splits after the call.
	 */
	int numberOfNodeSplits() const { return m_nodeSplits; }

	int numberOfSplittedNodes() const { return m_splittedNodes; }

protected:
	/**
	 * \brief Actual algorithm call that needs to be implemented by derived classed.
	 *
	 * @param PG represents the input graph as well as the computed planarized expansion
	 *        after the call. \a PG is initialized as a PlanRepExpansion of the input
	 *        graph and needs to be modified to obatain the planarized representation
	 *        (crossings are replaced by dummy vertices with degree four).
	 * @param cc is the number of the connected component in \a PG that is considered.
	 * @param crossingNumber needs to be assigned the number of crossings.
	 * @param numNS needs to be assigned the required number of node splits.
	 * @param numSN needs to be assigned the number of splitted nodes.
	 * \return the status of the result.
	 */
	virtual ReturnType doCall(PlanRepExpansion &PG,
		int cc,
		int& crossingNumber,
		int& numNS,
		int& numSN) = 0;

private:
	int m_nodeSplits;    //!< The number of required node splits.
	int m_splittedNodes; //!< The number of nodes that are split.

	OGDF_MALLOC_NEW_DELETE
};

} // end namespace ogdf

#endif
