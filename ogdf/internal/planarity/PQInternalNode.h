/*
 * $Revision: 1.5 $
 * 
 * last checkin:
 *   $Author: klein $ 
 *   $Date: 2007-11-14 16:18:10 +0100 (Mi, 14 Nov 2007) $ 
 ***************************************************************/
 
/** \file
 * \brief Declaration and implementation of the class PQInternalNode.
 *
 * \author Sebastian Leipert
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


#ifndef OGDF_INTERNALNODE_H
#define OGDF_INTERNALNODE_H



#include <ogdf/internal/planarity/PQNode.h>

namespace ogdf {


/**
 * The class template PQInternalNode is used to present 
 * P-nodes and Q-nodes in the PQ-Tree. This implementation does not
 * provide different classes for both, P- and Q-nodes, although this might 
 * seem necessary in the first place. The reason why this is not done, is 
 * supported by the fact that the maintainance of both nodes in the tree
 * is similar and using the same class for P- and Q-nodes
 * makes the application of the templates by
 * Booth and Lueker much easier.
 *
 * The template class PQInternalNode offers the possibility of using
 * four different kinds of constructors, depending on the usage of
 * the different possible information classes PQInternalKey<T,X,Y>
 * and PQNodeKey<T,X,Y>.
 *
 * In all four cases the constructor expects an integer value \a count,
 * setting the value of the variable \a m_identificationNumber in the base class,
 * an integer value \a type setting the variable \a m_type of
 * PQInternalNode and an integer value \a status setting the variable \a m_status of
 * PQInternalNode.
 *
 * Besides, the constructors accept additional information of type
 * PQNodeKey and PQInternalKey.
 * This information is not necessary when allocating an element of type
 * PQInternalNode and results in the four constructors that handle all cases. 
 *
 * Using a constructor with the \a infoPtr storing the adress of
 * an element of type PQNodeKey automatically sets
 * the \a m_nodePointer (see basicKey) of this element of type
 * PQNodeKey to the newly allocated PQInternalNode. See also
 * PQNode since this is done in the base class.
 *
 * Using a constructor with the PQInternalKeyPtr storing the adress of
 * an element of type PQInternalKey automatically sets
 * the \a m_nodePointer (see basicKey) of this element of type
 * PQInternalKey to the newly allocated PQInternalNode.
*/

template<class T,class X,class Y>
class PQInternalNode : public PQNode<T,X,Y> {

public:

	PQInternalNode(int count,int typ, int stat,
                 PQInternalKey<T,X,Y>* internalPtr,
                 PQNodeKey<T,X,Y>* infoPtr)
		:PQNode<T,X,Y>(count,infoPtr)
	{
		m_type = typ;
		m_status = stat;
		m_mark = UNMARKED;

		m_pointerToInternal = internalPtr;
		internalPtr->setNodePointer(this);
	};

	PQInternalNode(int count,int typ, int stat,
                 PQInternalKey<T,X,Y>* internalPtr)
		:PQNode<T,X,Y>(count)
	{
		m_type   = typ;
		m_status = stat;
		m_mark   = UNMARKED;
		m_pointerToInternal = internalPtr;
		internalPtr->setNodePointer(this);
	};

	PQInternalNode(int count,int typ, int stat,
				 PQNodeKey<T,X,Y>* infoPtr)
		:PQNode<T,X,Y>(count,infoPtr)
	{
		m_type   = typ;
		m_status = stat;
		m_mark   = UNMARKED;
		m_pointerToInternal = 0;
	};

	PQInternalNode(int count,int typ, int stat)
		:PQNode<T,X,Y>(count)
	{
		m_type = typ;
		m_status = stat;
		m_mark = UNMARKED;
		m_pointerToInternal = 0;
   
	};

	/**
	 * The destructor does not delete any
	 * accompanying information class as PQLeafKey,
	 * PQNodeKey and PQInternalKey.
	 * This has been avoided, since applications may need the existence of
	 * these information classes after the corresponding node has been
	 * deleted. If the deletion of an accompanying information class should
	 * be performed with the deletion of a node, either derive a new class
	 * with an appropriate destructor, or make use of the function
	 * CleanNode() of the class template PQTree.
	*/
    ~PQInternalNode(){};


	//! Returns 0. An element of type PQInternalNode does not have a PQLeafKey.
	virtual PQLeafKey<T,X,Y>*  getKey() const {return 0;};

	/**
	 * Accepts only pointers \a pointerToKey = 0.
	 * The function setKey() is designed to set a
	 * specified pointer variable in a derived class
	 * of PQNode to the adress stored in \a pointerToKey that is
	 * of type PQLeafKey. The class template PQInternalNode does not store
	 * informations of type PQLeafKey.  
     *
	 * setKey() ignores the informations as long as
	 * \a pointerToKey = 0. The return value then is 1.
	 * In case that \a pointerToKey != 0, the return value is 0.
	 */
	virtual bool setKey(PQLeafKey<T,X,Y>* pointerToKey)
	{
		if (pointerToKey != 0)
			return false;
		else
			return true;
	};

	//! Returns a pointer to the PQInternalKey information.
    virtual PQInternalKey<T,X,Y>* getInternal() const {return m_pointerToInternal;};

	/**
	 * setInternal() sets the pointer variable \a m_pointerToInternal to the
	 * specified adress of \a pointerToInternal that is of type PQInternalKey.
	 * Observe that \a pointerToInternal has
	 * to be instantiated by the client. The function setInternal() does
	 * not instantiate the corresponding variable in the derived class.
	 * Nevertheless, using this function will automatically set the \a m_nodePointer of the
	 * element of type PQInternalKey to this PQInternalNode.
	 * The return value is always 1 unless \a pointerInternal was
	 * equal to 0.
	 */
    virtual bool setInternal(PQInternalKey<T,X,Y>* pointerToInternal)
	{
		m_pointerToInternal = pointerToInternal;
		if (pointerToInternal != 0)
		{
			m_pointerToInternal->setNodePointer(this);
			return true;
		}
		else
			return false;
	};

	//! Returns the variable \ m_mark.
	/**
	 * The variable \a m_mark describes the designation used in
	 * the first pass of Booth and Luekers algorithm called Bubble(). A
	 * P- or Q-node is either marked \b BLOCKED, \b UNBLOCKED or
	 * \b QUEUED (see PQNode).
	 */
	virtual int   mark() const {return m_mark;};          
	
	//! Sets the variable \ m_mark.
	virtual void  mark(int m) {m_mark = m;};

	//! Returns the variable \a m_status in the derived class PQInternalNode.
	/**
	 * The functions manage the status of a node in the PQ-tree. A status is
	 * any kind of information of the current situation in the frontier of
	 * a node (the frontier of a node are all descendant leaves of the
	 * node). A status can be anything such as \b EMPTY, \b FULL or
	 * \b PARTIAL (see PQNode). Since there might be more than those three
	 * possibilities, (e.g. in computing planar subgraphs) this
	 * function may be overloaded by the client.
	 */
	virtual int   status() const {return m_status;};        
    
	//! Sets the variable \a m_status in the derived class PQInternalNode.
	virtual void  status(int s) {m_status = s;};

	//! Returns the variable \a m_type in the derived class PQInternalNode.
	/**
	 * The type of a PQInternalNode is either \b P_NODE or \b Q_NODE (see
	 * PQNode).
	 */
    virtual int   type() const {return m_type;};          
    
	//! Sets the variable \a m_type in the derived class PQInternalNode.
	virtual void  type(int t) {m_type = t;};

private:

	/**
	 * \a m_mark is a variable, storing if a PQInternalNode is 
	 * \b QUEUEUD, \b BLOCKED or \b UNBLOCKED (see PQNode)
	 * during the first phase of the procedure Bubble().
	 */
	int              m_mark;  


	/**
	 * \a m_pointerToInternal stores the adress of the corresponding
	 * internal information. That is information not supposed to be
	 * available for leaves of the PQ-tree. The internal information must
	 * be of type PQInternalKey.
	 * The PQInternalKey information can be overloaded by the
	 * client in order to present different information classes, needed in
	 * the different applications of PQ-trees.
	 */
    PQInternalKey<T,X,Y>* m_pointerToInternal;

	/**
	 * \a m_status is a variable storing the status of a PQInternalNode.
	 * A P- or Q-node can be either \b FULL, \b PARTIAL or \b EMPTY
	 * (see PQNode).
	 */
	int              m_status;

    /**
	 * \a m_status is a variable storing the status of a PQInternalNode.
	 * A P- or Q-node can be either \b FULL, \b PARTIAL or \b EMPTY
	 * (see PQNode).	
	 */
	int              m_type;
};

}

#endif


