/*
 * $Revision: 1.3 $
 * 
 * last checkin:
 *   $Author: gutwenger $ 
 *   $Date: 2007-11-08 16:10:37 +0100 (Do, 08 Nov 2007) $ 
 ***************************************************************/
 
/** \file
 * \brief Declaration of class GridLayoutMapped which extends GridLayout
 *        by a grid mapping mechanism.
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


#ifndef OGDF_GRID_LAYOUT_MAPPED_H
#define OGDF_GRID_LAYOUT_MAPPED_H


#include <ogdf/basic/GridLayout.h>

//scaling to allow correct edge anchors
const int l_gridScale = 2;


namespace ogdf {

	class PlanRep;
	class PlanRepUML;
	class OrthoRep;


//---------------------------------------------------------
// GridLayoutMapped
// extends GridLayout by a grid mapping mechanism
//---------------------------------------------------------
class GridLayoutMapped : public GridLayout
{
public:
	// construction (determines mapping factor)
	GridLayoutMapped(const PlanRep &PG,
		const OrthoRep &OR,
		double separation,
		double cOverhang,
		int fineness = 4);


	// writes grid layout to layout using re-mapping
	void remap(Layout &drawing);

	// transforms real coordinates to grid coordinates
	int toGrid(double x) const {
		return l_gridScale*int(m_fMapping * x + 0.5);
	}

	// transforms grid coordinates to real coordinates
	double toDouble(int i) const {
		//OGDF_ASSERT(i % l_gridScale == 0)
		return (i/l_gridScale) / m_fMapping;
	}


	const NodeArray<int> &width() const { return m_gridWidth; }
	// returns a reference to the array storing grid widths of nodes
	NodeArray<int> &width() { return m_gridWidth; }

	const NodeArray<int> &height() const { return m_gridHeight; }
	// returns a reference to the array storing grid heights of nodes
	NodeArray<int> &height() { return m_gridHeight; }

	const int &width(node v) const { return m_gridWidth[v]; }
	// returns grid width of node v
	int &width(node v) { return m_gridWidth[v]; }

	const int &height(node v) const { return m_gridWidth[v]; }
	// returns grid height of node v
	int &height(node v) { return m_gridWidth[v]; }


private:
	NodeArray<int> m_gridWidth;  // grid width of nodes
	NodeArray<int> m_gridHeight; // grid heights of nodes

	const PlanRep *m_pPG;     // planarized representation of grid layout
	double m_fMapping;           // mapping factor
};


} // end namespace ogdf


#endif
