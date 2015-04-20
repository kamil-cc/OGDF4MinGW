/*
 * $Revision: 1.3 $
 * 
 * last checkin:
 *   $Author: klein $ 
 *   $Date: 2007-11-12 15:59:17 +0100 (Mo, 12 Nov 2007) $ 
 ***************************************************************/
 
/** \file
 * \brief Declares class DavidsonHarelLayout, which is a front-end
 * for the fDavidsonHarel class.
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



#ifdef _MSC_VER
#pragma once
#endif

#ifndef OGDF_DAVIDSON_HAREL_LAYOUT_H
#define OGDF_DAVIDSON_HAREL_LAYOUT_H


#include <ogdf/module/LayoutModule.h>
#include <ogdf/energybased/DavidsonHarel.h>


namespace ogdf {


//! The Davidson-Harel layout algorithm. 
/**
 * The implementation used in DavidsonHarelLayout is based on
 * the following publication:
 *
 * Ron Davidson, David Harel: <i>Drawing Graphs Nicely Using Simulated Annealing</i>.
 * ACM Transactions on Graphics 15(4), pp. 301-331, 1996.
 */
class DavidsonHarelLayout : public LayoutModule
{
public:
	//! Easy way to set fixed costs
	enum SettingsParameter {spStandard, spRepulse, spPlanar}; //tuning of costs

	//! Easy way to set temperature and iterations
	enum SpeedParameter {sppFast, sppMedium, sppHQ};

	//! Creates an instance of Davidson-Harel layout.
	DavidsonHarelLayout();

	~DavidsonHarelLayout(){}

	//! Calls the layout algorithm for graph attributes \a GA.
	void call(GraphAttributes &GA);

	//! Fixes the cost values to special configurations.
    void fixSettings(SettingsParameter sp);

	//! set the fineness of the candidate disc radisu
	void setStepping(DavidsonHarel::TuningParameter tp);

	//! More convenient way of setting the speed of the algorithm.
	/**
	 * Influences number of iterations per temperature step, starting
	 * temperature, and cooling factor.
	 */
	void setSpeed(SpeedParameter sp);

	//! Sets the preferred edge length multiplier for attraction.
	/**
	 * This is bad design, cause you dont need to have an attraction function,
	 * DH is purely modular and independent with its cost functions.
	 */
	void setPreferredEdgeLengthMultiplier(double multi) {m_multiplier = multi;}

	//! Sets the weight for the energy function \a Repulsion.
	void setRepulsionWeight(double w);

	//! Returns the weight for the energy function \a Repulsion.
	double getRepulsionWeight() const {return m_repulsionWeight;}

	//! Sets the weight for the energy function \a Attraction.
	void setAttractionWeight(double);

	//! Returns the weight for the energy function \a Attraction.
	double getAttractionWeight() const {return m_attractionWeight;}

	//! Sets the weight for the energy function \a NodeOverlap.
	void setNodeOverlapWeight(double);

	//! Returns the weight for the energy function \a NodeOverlap.
	double getNodeOverlapWeight() const {return m_nodeOverlapWeight;}

	//! Sets the weight for the energy function \a Planarity.
	void setPlanarityWeight(double);

	//! Returns the weight for the energy function \a Planarity.
	double getPlanarityWeight() const {return m_planarityWeight;}

	//! Sets the starting temperature to \a t.
	void setStartTemperature(int t);

	//! Returns the starting temperature.
	int getStartTemperature() const {return m_startTemperature;}

	//! Sets the number of iterations per temperature step to \a steps.
	void setNumberOfIterations(int steps);

	//! Returns the number of iterations per temperature step.
	int getNumberOfIterations() const {return m_numberOfIterations;}

private:
	double m_repulsionWeight;   //!< The weight for repulsion energy.
	double m_attractionWeight;  //!< The weight for attraction energy.
	double m_nodeOverlapWeight; //!< The weight for node overlap energy.
	double m_planarityWeight;   //!< The weight for edge crossing energy.
	int m_startTemperature;     //!< The temperature at the start of the optimization.
	int m_numberOfIterations;   //!< The number of iterations per temperature step.
	DavidsonHarel::TuningParameter m_stepping; //!< Defines if worst solutions can be chosen.
	SpeedParameter m_speed;     //!< You can override this by manually setting iter=0.
	double m_multiplier;        //!< By default, number of iterations per temperature step is number of vertices multiplied by multiplier.
	bool m_crossings;           //!< Should crossings be computed?
};

}
#endif
