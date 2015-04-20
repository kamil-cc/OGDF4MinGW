/*
 * $Revision: 1.3 $
 * 
 * last checkin:
 *   $Author: klein $ 
 *   $Date: 2007-11-14 16:18:10 +0100 (Mi, 14 Nov 2007) $ 
 ***************************************************************/
 
/** \file
 * \brief Implements simple labeling algorithm
 * 
 * \author Joachim Kupke
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


#include <ogdf/labeling/ELabelPosSimple.h>

namespace ogdf {


ELabelPosSimple::ELabelPosSimple() :
    m_absolut(true),
    m_marginDistance(0.2),
    m_edgeDistance(0.2),
    m_midOnEdge(true)
{
}

ELabelPosSimple::~ELabelPosSimple()
{
}

// liefert das Segment der Kantenlinie, auf dem point liegt
static DLine segment(DPolyline &bends, DPoint &point)
{
    ListConstIterator<DPoint> iter;
    DPoint from, to;

    from = *(iter = bends.begin());
    for (++iter; iter.valid(); ++iter) {
        to = *iter;
        DLine line(from, to);
        if (line.contains(point))
            break;
        from = to;
    }

    if (from == to)
        THROW_PARAM(AlgorithmFailureException, afcLabel);

    return DLine(from, to);
}

// liefert den Punkt neben dem Segment (links oder rechts, siehe 'left'), der orthogonal von
// 'p' die Entfernung 'newLen' von diesem Segment hat.
static DPoint leftOfSegment(const DLine &segment, const DPoint &p, double newLen, bool left = true)
{
    DVector v;
    if (p == segment.start())
        v = segment.end() - p;
    else
        v = p - segment.start();

    DVector newPos;
    if (left) newPos = ++v;
    else      newPos = --v;

    // newPos hat immer L?nge != 0
    newPos = (newPos * newLen) / newPos.length();

    return p + newPos;
}

void ELabelPosSimple::call(GraphAttributes &ug, ELabelInterface<double> &eli)
{
    //ug.addNodeCenter2Bends();
    edge e;
    forall_edges(e, ug.constGraph()) {
        EdgeLabel<double> &el = eli.getLabel(e);
        DPolyline       bends = ug.bends(e);

        bends.normalize();

        if (bends.size() < 2)
            THROW_PARAM(AlgorithmFailureException, afcLabel);

        double frac;

        if (m_absolut) {
            double len = bends.length();
            if (len == 0.0)
                frac = 0.0;
            else
                frac = m_marginDistance / len;

        }
        else {
            frac = m_marginDistance;
        }

        if (frac < 0.0) frac = 0.0;
        if (frac > 0.4) frac = 0.4;

        // hole Positionen auf der Kante
        DPoint midPoint   = bends.position(0.5);
        DPoint startPoint = bends.position(frac);
        DPoint endPoint   = bends.position(1.0 - frac);

        // hole die beteiligten Segmente
        DLine startLine = segment(bends, startPoint);
        DLine endLine   = segment(bends, endPoint);
        DLine midLine   = segment(bends, midPoint);

        // berechne die Labelpositionen
        if (el.usedLabel(elEnd1)) {
            DPoint np = leftOfSegment(startLine, startPoint, m_edgeDistance, true);
            el.setX(elEnd1, np.m_x);
            el.setY(elEnd1, np.m_y);
        }

        if (el.usedLabel(elMult1)) {
            DPoint np = leftOfSegment(startLine, startPoint, m_edgeDistance, false);
            el.setX(elMult1, np.m_x);
            el.setY(elMult1, np.m_y);
        }

        if (el.usedLabel(elName)) {
            DPoint np = m_midOnEdge ? midPoint : leftOfSegment(midLine, midPoint, m_edgeDistance, true);
            el.setX(elName, np.m_x);
            el.setY(elName, np.m_y);
        }

        if (el.usedLabel(elEnd2)) {
            DPoint np = leftOfSegment(endLine, endPoint, m_edgeDistance, true);
            el.setX(elEnd2, np.m_x);
            el.setY(elEnd2, np.m_y);
        }

        if (el.usedLabel(elMult2)) {
            DPoint np = leftOfSegment(endLine, endPoint, m_edgeDistance, false);
            el.setX(elMult2, np.m_x);
            el.setY(elMult2, np.m_y);
        }
    }
}

}
