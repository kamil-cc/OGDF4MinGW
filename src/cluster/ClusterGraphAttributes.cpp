/*
 * $Revision: 1.4 $
 * 
 * last checkin:
 *   $Author: gutwenger $ 
 *   $Date: 2007-11-08 16:10:37 +0100 (Do, 08 Nov 2007) $ 
 ***************************************************************/
 
/** \file
 * \brief Implement class ClusterGraphAttributes
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


#include <ogdf/cluster/ClusterGraphAttributes.h>
#include <ogdf/cluster/ClusterArray.h>
#include <ogdf/fileformats/GmlParser.h>



namespace ogdf {


ClusterGraphAttributes::ClusterGraphAttributes(ClusterGraph& cg,
											   long initAttributes)
: GraphAttributes(cg.getGraph(), initAttributes | edgeType | nodeType |
		nodeGraphics | edgeGraphics), m_clusterTemplate(cg), m_pClusterGraph(&cg)
//we should initialize m__clusterinfo here
{
	//should we always fill the cluster infos here?
}//constructor

//reinitialize graph
void ClusterGraphAttributes::init(ClusterGraph &cg, long initAttributes)
{
	m_pClusterGraph = &cg;
	m_clusterInfo.clear();
/*	if (&cg == m_pClusterGraph) 
	{
		GraphAttributes::initAttributes(initAttributes);
		return;
	}//if same graph
	*/
	//need to initialize GraphAttributes with getGraph()
	GraphAttributes::init(cg.getGraph(), initAttributes /*| edgeType | nodeType |
		nodeGraphics | edgeGraphics*/);
}


//
// calculates the bounding box of the graph including clusters
const DRect ClusterGraphAttributes::boundingBox() const
{
	DRect bb = GraphAttributes::boundingBox();
	double minx = bb.p1().m_x;
	double miny = bb.p1().m_y;
	double maxx = bb.p2().m_x;
	double maxy = bb.p2().m_y;

	cluster c;
	forall_clusters(c,*m_pClusterGraph)
	{
		if(c == m_pClusterGraph->rootCluster())
			continue;

		double x1 = clusterXPos(c);
		double y1 = clusterYPos(c);
		double x2 = x1 + clusterWidth(c);
		double y2 = y1 + clusterHeight(c);

        if (x1 < minx) minx = x1;
        if (x2 > maxx) maxx = x2;
        if (y1 < miny) miny = y1;
        if (y2 > maxy) maxy = y2;
	}

    return DRect(minx, miny, maxx, maxy);
}


void ClusterGraphAttributes::writeGML(const char *fileName)
{
	ofstream os(fileName);
	writeGML(os);
}

void ClusterGraphAttributes::writeGML(ostream &os)
{
	NodeArray<int>    nId(*m_pGraph);

	int nextId = 0;

	os.setf(ios::showpoint);

	GraphAttributes::writeGML(os);

	// set index string for cluster entries
	node v;
	forall_nodes(v,*m_pGraph) 
	{
		nId[v] = nextId++;
	}

	// output the cluster information
	String indent = "\0";
	nextId = 1;
	writeGraphWinCluster(os, nId, nextId,
		m_pClusterGraph->rootCluster(), indent);
}


// recursively write the cluster structure in GML
void ClusterGraphAttributes::writeCluster(ostream &os,
								NodeArray<int> &nId,
								ClusterArray<int> & cId,
								int &nextId,
								cluster c,
								String indent)
{
	String newindent = indent;
	newindent+="  ";
	os << indent << "cluster [\n";
	os << indent << "id " << (cId[c] = nextId++) << "\n";
	ListConstIterator<cluster> it;
	for (it = c->cBegin(); it.valid(); it++) 
		writeCluster(os,nId,cId,nextId,*it,newindent);
	ListConstIterator<node> itn;
	for (itn = c->nBegin(); itn.valid(); itn++) 
		os << indent << "node " << nId[*itn] << "\n";
	os << indent << "]\n"; // cluster
}

// recursively write the cluster structure in GraphWin GML
void ClusterGraphAttributes::writeGraphWinCluster(
	ostream        &os,
	NodeArray<int> &nId,
	int            &nextId,
	cluster        c,
	String         indent
	)
{
	String newindent = indent;
	newindent+="  ";

	if (c == m_pClusterGraph->rootCluster())
		os << indent << "rootcluster [\n";
	else
	{
		os << indent << "cluster [\n";
		os << indent << "id " << c->index() << "\n";
		
		const String &templStr = m_clusterTemplate[c];
		if(templStr.length() > 0) {
			// GDE extension: Write cluster template and custom attribute
			os << "template ";
			writeLongString(os, templStr);
			os << "\n";

			os << "label ";
			writeLongString(os, clusterLabel(c));
			os << "\n";

		} else {
			os << indent << "label \"" << clusterLabel(c) << "\"\n"; 
		}

		os << indent << "graphics [\n";

			double shiftPos;
			shiftPos = clusterYPos(c->index());

			os << indent << "x " << clusterXPos(c->index()) << "\n";
			os << indent << "y " << shiftPos/*clusterYPos(c->index())*/ << "\n";

			os << indent << "width " << clusterWidth(c->index()) << "\n";
			os << indent << "height " << clusterHeight(c->index()) << "\n";
			os << indent << "fill \""  << clusterFillColor(c->index()) << "\"\n";
			os << indent << "pattern "  << clusterFillPattern(c->index()) << "\n";

			//border line styles
			os << indent << "color \"" << clusterColor(c) << "\"\n";
			os << indent << "lineWidth " << clusterLineWidth(c) << "\n";
			//save space by defaulting
			if (clusterLineStyle(c) != esSolid)
				os << indent << "stipple " << clusterLineStyle(c) << "\n";

			os << indent << "style \"rectangle\"\n";

		os << indent << "]\n"; //graphics

	}

	// write contained clusters
	ListConstIterator<cluster> it;
	for (it = c->cBegin(); it.valid(); it++) 
		writeGraphWinCluster(os, nId, nextId, *it, newindent);

	// write contained nodes
	ListConstIterator<node> itn;
	for (itn = c->nBegin(); itn.valid(); itn++) 
		os << indent << "vertex \"" << nId[*itn] << "\"\n";

	os << indent << "]\n"; // cluster
}

//++++++++++++++++++++++++++++++++++++++++++++
//reading graph, attributes, cluster structure
bool ClusterGraphAttributes::readClusterGML(const char* fileName, 
								ClusterGraph& CG, 
								Graph& G)
{

	ifstream is(fileName);
	if (!is)
		return false; // couldn't open file

	return readClusterGML(is, CG, G);
}
bool ClusterGraphAttributes::readClusterGML(istream& is, 
								ClusterGraph& CG, 
								Graph& G)
{

	bool result;
	GmlParser gml(is);
	if (gml.error())
		return false;

	result = gml.read(G,*this);

	if (!result) return false;

	return readClusterGraphGML(CG, G, gml);
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//read Cluster Graph with Attributes, base graph G, from fileName
bool ClusterGraphAttributes::readClusterGraphGML(const char* fileName, 
												 ClusterGraph& CG, 
												 Graph& G,
												 GmlParser& gml)
{
	ifstream is(fileName);
	if (!is)
		return false; // couldn't open file

	return readClusterGraphGML(CG, G, gml);
}


//read from input stream
bool ClusterGraphAttributes::readClusterGraphGML( 
	 ClusterGraph& CG, 
	 Graph& G,
	 GmlParser& gml)
{	
	return gml.readAttributedCluster(G, CG, *this);
}


ostream &operator<<(ostream &os, ogdf::cluster c)
{
	if (c) os << c->index(); else os << "nil";
	return os;
}


} // end namespace ogdf

