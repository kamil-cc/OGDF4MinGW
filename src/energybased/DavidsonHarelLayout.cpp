/*
 * $Revision: 1.3 $
 * 
 * last checkin:
 *   $Author: gutwenger $ 
 *   $Date: 2007-11-12 15:53:04 +0100 (Mo, 12 Nov 2007) $ 
 ***************************************************************/
 
/** \file
 * \brief Implementation fo DavidsonHarelLayout
 * 
 * This is the frontend for the DavidsonHarel optimization 
 * function. It adds the energy functions to the problem and 
 * sets their weights. It also contains functions for setting 
 * and returning the parameters that influence the quality of
 * the solution and the speed of the optimization process.
 * 
 * \author Rene Weiskircher
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

#include <ogdf/energybased/DavidsonHarelLayout.h>
#include <ogdf/internal/energybased/Repulsion.h>
#include <ogdf/internal/energybased/Attraction.h>
#include <ogdf/internal/energybased/Overlap.h>
#include <ogdf/internal/energybased/Planarity.h>
#include <ogdf/internal/energybased/PlanarityGrid.h>


#define DEFAULT_REPULSION_WEIGHT 1e15
#define DEFAULT_ATTRACTION_WEIGHT 1e-9
#define DEFAULT_OVERLAP_WEIGHT 1000
#define DEFAULT_PLANARITY_WEIGHT 10000
#define DEFAULT_ITERATIONS 0
#define DEFAULT_START_TEMPERATURE 200

namespace ogdf {

struct InputValueInvalid : Exception { InputValueInvalid() : Exception(__FILE__, __LINE__) {}};
struct WeightLessThanZeroException : InputValueInvalid {};
struct IterationsNonPositive : InputValueInvalid {};
struct TemperatureNonPositive : InputValueInvalid {};


DavidsonHarelLayout::DavidsonHarelLayout()
{
	m_repulsionWeight = DEFAULT_REPULSION_WEIGHT;
	m_attractionWeight = DEFAULT_ATTRACTION_WEIGHT;
	m_nodeOverlapWeight = DEFAULT_OVERLAP_WEIGHT;
	m_planarityWeight = DEFAULT_PLANARITY_WEIGHT;
	m_numberOfIterations = DEFAULT_ITERATIONS;
	m_startTemperature = DEFAULT_START_TEMPERATURE;
	m_stepping = DavidsonHarel::tpFree;
	m_speed    = sppMedium;
	m_multiplier = 2.0;
	m_crossings = false;
}


void DavidsonHarelLayout::fixSettings(SettingsParameter sp)
{
	double r, a, p, o;
	switch (sp) {
	case spStandard: {r = 900; a = 250; o = 1450; p = 300; m_crossings = false;} 
			         break;
    case spRepulse:  {r = 9000; a = 250; o = 1450; p = 300; m_crossings = false;} 
			         break;
	case spPlanar:   {r = 900; a = 250; o = 1450; p = 3000; m_crossings = true;} 
			         break;
	default: THROW_PARAM(AlgorithmFailureException, afcIllegalParameter); break;
	}//switch
	setRepulsionWeight(r);
	setAttractionWeight(a);
	setNodeOverlapWeight(o);
	setPlanarityWeight(p);
}//fixSettings


void DavidsonHarelLayout::setSpeed(SpeedParameter sp)
{
  m_speed = sp; 
  m_numberOfIterations = 0;
}//setSpeed


void DavidsonHarelLayout::setStepping(DavidsonHarel::TuningParameter tp)
{
  m_stepping = tp;
}//setStepping


void DavidsonHarelLayout::setRepulsionWeight(double w)
{
	if(w < 0) throw WeightLessThanZeroException();
	else m_repulsionWeight = w;
}


void DavidsonHarelLayout::setAttractionWeight(double w)
{
	if(w < 0) throw WeightLessThanZeroException();
	else m_attractionWeight = w;
}


void DavidsonHarelLayout::setNodeOverlapWeight(double w)
{
	if(w < 0) throw WeightLessThanZeroException();
	else m_nodeOverlapWeight = w;
}


void DavidsonHarelLayout::setPlanarityWeight(double w)
{
	if(w < 0) throw WeightLessThanZeroException();
	else m_planarityWeight = w;
}


void DavidsonHarelLayout::setNumberOfIterations(int w)
{
	if(w < 0) throw IterationsNonPositive();
	else m_numberOfIterations = w;
}


void DavidsonHarelLayout::setStartTemperature (int w)
{
	if(w < 0) throw TemperatureNonPositive();
	else m_startTemperature = w;
}


//this sets the parameters of the class DavidsonHarel, adds the energy functions and
//starts the optimization process
void DavidsonHarelLayout::call(GraphAttributes &AG)
{
	// all edges straight-line
	AG.clearAllBends();

	DavidsonHarel dh;
	Repulsion rep(AG);
	Attraction atr(AG);
	Overlap over(AG);
	Planarity plan(AG);
	//PlanarityGrid plan(AG);
	//PlanarityGrid2 plan(AG);
	//NodeIntersection ni(AG);

	//repeat the constructors work...
	atr.reinitializeEdgeLength(m_multiplier);

	dh.addEnergyFunction(&rep,m_repulsionWeight);
	dh.addEnergyFunction(&atr,m_attractionWeight);
	dh.addEnergyFunction(&over,m_nodeOverlapWeight);
	if (m_crossings) dh.addEnergyFunction(&plan,m_planarityWeight);
	//dh.addEnergyFunction(&ni,2000.0);

	//dh.setNumberOfIterations(m_numberOfIterations);
	//dh.setStartTemperature(m_startTemperature);
	const Graph& G = AG.constGraph();
	if (m_numberOfIterations == 0)
	{
		switch (m_speed)  //todo: function setSpeedParameters
		{
		  case sppFast: {
				          m_numberOfIterations = max(75, 3*G.numberOfNodes()); 
						  m_startTemperature = 400;
						} break;
		  case sppMedium: {
				            m_numberOfIterations = 10*G.numberOfNodes(); 
							m_startTemperature = 800;
						  } 
				          break;
		  case sppHQ: {
				        m_numberOfIterations = 25*G.numberOfNodes(); //should be: isolate
						m_startTemperature = 2500;
					  } 
					  break;
		  default: THROW_PARAM(AlgorithmFailureException, afcIllegalParameter); break;
		}//switch
	}//if

	dh.setNumberOfIterations(m_numberOfIterations);
	dh.setStartTemperature(m_startTemperature);
	dh.call(AG, m_stepping);
}

} // namespace ogdf
