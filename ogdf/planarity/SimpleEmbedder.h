/*
 * $Revision: 1.5 $
 * 
 * last checkin:
 *   $Author: gutwenger $ 
 *   $Date: 2007-11-15 13:46:15 +0100 (Do, 15 Nov 2007) $ 
 ***************************************************************/
 
/** \file
 * \brief A simple embedder algorithm.
 * 
 * \author Thorsten Kerkhof (thorsten.kerkhof@udo.edu)
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

#ifndef OGDF_SIMPLE_EMBEDDER_H
#define OGDF_SIMPLE_EMBEDDER_H

#include <ogdf/module/EmbedderModule.h>
#include <ogdf/planarity/PlanarModule.h>
#include <ogdf/basic/CombinatorialEmbedding.h>
#include <ogdf/planarity/PlanRep.h>

namespace ogdf {

//! Planar graph embedding by using PlanarModule.
class SimpleEmbedder : public EmbedderModule
{
public:
	// construction / destruction
	SimpleEmbedder() { }
	~SimpleEmbedder() { }

	/**
   * \brief Call embedder algorithm.
   * \param G is the original graph. Its adjacency list is changed by the embedder.
	 * \param adjExternal is an adjacency entry on the external face and is set by the embedder.
   */
	void call(PlanRep& PG, adjEntry& adjExternal);

private:
	/**
   * \brief Find best suited external face according to certain criteria.
   * \param PG is a planar representation of the original graph.
	 * \param E is a combinatorial embedding of the original graph.
   * \return Best suited external face.
   */
	face findBestExternalFace(const PlanRep& PG,
	                          const CombinatorialEmbedding& E);

};

} // end namespace ogdf

#endif
