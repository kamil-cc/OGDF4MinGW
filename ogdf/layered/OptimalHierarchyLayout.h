/*
 * $Revision: 1.5 $
 * 
 * last checkin:
 *   $Author: gutwenger $ 
 *   $Date: 2007-11-13 16:15:38 +0100 (Di, 13 Nov 2007) $ 
 ***************************************************************/
 
/** \file
 * \brief Declaration and implementation of the optimal third
 *        phase of the Sugiyama algorithm.
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

#ifndef OGDF_OPTIMAL_HIERARCHY_LAYOUT_H
#define OGDF_OPTIMAL_HIERARCHY_LAYOUT_H



#include <ogdf/module/HierarchyLayoutModule.h>


namespace ogdf {


//! The LP-based hierarchy layout algorithm.
/**
 * OptimalHierarchyLayout implements a hierarchy layout algorithm that is based
 * on an LP-formulation. It is only available if OGDF is compiled with LP-solver
 * support (e.g., Coin).
 *
 * The used model avoids Spaghetti-effect like routing of edges by using
 * long vertical segments as in FastHierarchyLayout. An additional balancing
 * can be used which balances the successors below a node.
 * 
 * <H3>Optional parameters</H3>
 *
 * <table>
 *   <tr>
 *     <th><i>Option</i><th><i>Type</i><th><i>Default</i><th><i>Description</i>
 *   </tr><tr>
 *     <td><i>nodeDistance</i><td>double<td>3.0
 *     <td>The minimal allowed x-distance between nodes on a layer.
 *   </tr><tr>
 *     <td><i>layerDistance</i><td>double<td>3.0
 *     <td>The minimal allowed y-distance between layers.
 *   </tr><tr>
 *     <td><i>fixedLayerDistance</i><td>bool<td>false
 *     <td>If set to true, the distance between neighboured layers is always
 *     layerDistance; otherwise the distance is adjusted (increased) to improve readability.
 *   </tr><tr>
 *     <td><i>weightSegments</i><td>double<td>2.0
 *     <td>The weight of edge segments connecting to vertical segments.
 *   </tr><tr>
 *     <td><i>weightBalancing</i><td>double<td>0.1
 *     <td>The weight for balancing successors below a node; 0.0 means no balancing.
 *   </tr>
 * </table>
 */
class OptimalHierarchyLayout : public HierarchyLayoutModule
{
#ifndef OGDF_LP_SOLVER
protected:
	void doCall(const Hierarchy& H, GraphCopyAttributes &AGC) {
		THROW_PARAM(LibraryNotSupportedException, lnscCoin);
	}

#else

public:
	//! Creates an instance of optimal hierarchy layout.
	OptimalHierarchyLayout();

	//! Copy constructor.
	OptimalHierarchyLayout(const OptimalHierarchyLayout &);

	// destructor
	~OptimalHierarchyLayout() { }


	//! Assignment operator.
	OptimalHierarchyLayout &operator=(const OptimalHierarchyLayout &);


	/**
	 *  @name Optional parameters
	 *  @{
	 */

	//! Returns the minimal allowed x-distance between nodes on a layer.
	double nodeDistance() const {
		return m_nodeDistance;
	}

	//! Sets the minimal allowed x-distance between nodes on a layer to \a x.
	void nodeDistance(double x) {
		if(x >= 0)
			m_nodeDistance = x;
	}

	//! Returns the minimal allowed y-distance between layers.
	double layerDistance() const {
		return m_layerDistance;
	}

	//! Sets the minimal allowed y-distance between layers to \a x.
	void layerDistance(double x) {
		if(x >= 0)
			m_layerDistance = x;
	}

	//! Returns the current setting of option <i>fixedLayerDistance</i>.
	/**
	 * If set to true, the distance is always layerDistance; otherwise
	 * the distance is adjusted (increased) to improve readability.
	 */
	bool fixedLayerDistance() const {
		return m_fixedLayerDistance;
	}

	//! Sets the option <i>fixedLayerDistance</i> to \a b.
	void fixedLayerDistance(bool b) {
		m_fixedLayerDistance = b;
	}

	//! Returns the weight of edge segments connecting to vertical segments.
	double weightSegments() const {
		return m_weightSegments;
	}

	//! Sets the weight of edge segments connecting to vertical segments to \a w.
	void weightSegments(double w) {
		if(w > 0.0 && w <= 100.0)
			m_weightSegments = w;
	}

	//! Returns the weight for balancing successors below a node; 0.0 means no balancing.
	double weightBalancing() const {
		return m_weightBalancing;
	}

	//! Sets the weight for balancing successors below a node to \a w; 0.0 means no balancing.
	void weightBalancing(double w) {
		if(w >= 0.0 && w <= 100.0)
			m_weightBalancing = w;
	}

	//! @}

protected:
	//! Implements the algorithm call.
	void doCall(const Hierarchy& H,GraphCopyAttributes &AGC);

private:
	void computeXCoordinates(
		const Hierarchy& H,
		GraphCopyAttributes &AGC);
	void computeYCoordinates(
		const Hierarchy& H,
		GraphCopyAttributes &AGC);

	// options
	double m_nodeDistance;  //!< The minimal distance between nodes.
	double m_layerDistance; //!< The minimal distance between layers.
	bool   m_fixedLayerDistance; //!< Use fixed layer distances?

	double m_weightSegments;  //!< The weight of edge segments.
	double m_weightBalancing; //!< The weight for balancing.

#endif
};

}


#endif
