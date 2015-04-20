/*
 * $Revision: 1.4 $
 * 
 * last checkin:
 *   $Author: gutwenger $ 
 *   $Date: 2007-11-13 21:44:33 +0100 (Di, 13 Nov 2007) $ 
 ***************************************************************/
 
/** \file
 * \brief Provides a simple, dfs-based algorithm for
 * biconnectivity augmentation.
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

#ifndef OGDF_DFS_MAKE_BICONNECTED_H
#define OGDF_DFS_MAKE_BICONNECTED_H



#include <ogdf/module/AugmentationModule.h>

namespace ogdf {

/**
 * \brief Implementation of a DFS-based algorithm for biconnectivity augmentation.
 *
 * The class \a DfsMakeBiconnected implements an augmentation algorithms
 * that augments a graph to a biconnected graph. In addition, if the graph was
 * planar before augmentation, the resulting graph will be biconnected and
 * planar.
 * The algorithm simply uses DFS and, whenever a cut vertex is discovered,
 * a new edge is added.
 */

class DfsMakeBiconnected : public AugmentationModule {
public:
	//! Creates an instance of DFS-based biconnectivity augmentation.
	DfsMakeBiconnected() { }

	// destruction
	~DfsMakeBiconnected() { }

protected:
	//! Implements the algorithm call.
	void doCall(Graph& G, List<edge> &L);
};

} // end namespace ogdf

#endif
