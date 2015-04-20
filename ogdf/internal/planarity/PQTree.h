/*
 * $Revision: 1.5 $
 * 
 * last checkin:
 *   $Author: gutwenger $ 
 *   $Date: 2007-11-17 23:06:08 +0100 (Sa, 17 Nov 2007) $ 
 ***************************************************************/
 
/** \file
 * \brief Declaration and implementation of the class PQTree.
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


#ifndef OGDF_PQTREE_H
#define OGDF_PQTREE_H


#include <string.h>

#include <ogdf/basic/Stack.h>
#include <ogdf/basic/Queue.h>
#include <ogdf/basic/Array.h>

#include <ogdf/internal/planarity/PQTreeRoot.h>
#include <ogdf/internal/planarity/PQNode.h>
#include <ogdf/internal/planarity/PQInternalNode.h>
#include <ogdf/internal/planarity/PQLeaf.h>
#include <ogdf/internal/planarity/PQLeafKey.h>
#include <ogdf/internal/planarity/PQInternalKey.h>
#include <ogdf/internal/planarity/PQNodeKey.h>
 

namespace ogdf {


#define INFOSIZE       1024
#define P_NODE_CAT     4
#define Q_NODE_CAT     6
#define LEAF_CAT       8
#define LEAF_FULL_CAT  9


template<class T,class X,class Y>

class PQTree: public PQTreeRoot {

public:

	PQTree();

	virtual ~PQTree();

	bool    addNewLeavesToTree(PQInternalNode<T,X,Y>        *father,
                               SListPure<PQLeafKey<T,X,Y>*> &leafKeys);

	bool    addNewLeavesToTree(PQInternalNode<T,X,Y>        *father,
                               SListPure<PQLeafKey<T,X,Y>*> &leafKeys,
                               PQNode<T,X,Y>                *leftSibling,
                               PQNode<T,X,Y>                *rightSibling);

	void    emptyNode(PQNode<T,X,Y>* nodePtr);

	virtual void front(PQNode<T,X,Y>* nodePtr,
	 				   SListPure<PQLeafKey<T,X,Y>*> &leafKeys);

	virtual void printOutCurrentTree(const char *filename,const char* ending);

    virtual void CleanNode(PQNode<T,X,Y>* nodePtr);

    virtual void Cleanup();

    virtual void clientDefinedEmptyNode(PQNode<T,X,Y>* nodePtr);

    virtual void emptyAllPertinentNodes();

    virtual int  Initialize(SListPure<PQLeafKey<T,X,Y>*> &leafKeys);

    virtual bool Reduction(SListPure<PQLeafKey<T,X,Y>*> &leafKeys,
                          int redNumber = 0);

	PQNode<T,X,Y>* root() const;

	void writeGML(const char *fileName);
	void writeGML(ostream &os);


protected:


	//! is a pointer to the root of the $PQ$-tree.
	PQNode<T,X,Y>*   m_root;                     

    //! is a pointer to the root of the pertinent subtree.
	PQNode<T,X,Y>*   m_pertinentRoot;

	//! is a pointer to the virtual root of the pertinent subtree, in case that the pertinent root cannot be detected.
    PQNode<T,X,Y>*   m_pseudoRoot;   

	//! Stores the total number of nodes that have been allocated. 
	/**
	 * Gives every node that has been used once in the
	 * PQ-tree a unique identification number. 
	*/
    int            m_identificationNumber;      

	//! Stores the number of leaves.
    int            m_numberOfLeaves;

	/**
	 * Stores all nodes that have been marked \b FULL or
	 * \b PARTIAL during a reduction. After the reduction has been
	 * finished succesfully, all pertinent nodes are reinitialized and
	 * prepared for the next reduction. This list also contains pertinent
	 * nodes that have been removed during a reduction. When detected in
	 * the stack, their memory is freed.
	 */
    List<PQNode<T,X,Y>*>  *m_pertinentNodes; 



   

   #ifdef DEBUG
	virtual   bool  Bubble(SListPure<PQLeafKey<T,X,Y>*> &leafKeys,
                           int redNumber);
   #else
	virtual   bool  Bubble(SListPure<PQLeafKey<T,X,Y>*> &leafKeys);
   #endif

	virtual   bool  Reduce(SListPure<PQLeafKey<T,X,Y>*> &leafKeys,
						   int printYes = 0);

	virtual   bool  templateL1(PQNode<T,X,Y> *nodePtr,
                               bool isRoot);

	virtual   bool  templateP1(PQNode<T,X,Y> *nodePtr,
                               bool isRoot);

	virtual   bool  templateP2(PQNode<T,X,Y> **nodePtr);

	virtual   bool  templateP3(PQNode<T,X,Y> *nodePtr);

	virtual   bool  templateP4(PQNode<T,X,Y> **nodePtr);
      
	virtual   bool  templateP5(PQNode<T,X,Y> *nodePtr);

	virtual   bool  templateP6(PQNode<T,X,Y> **nodePtr);

	virtual   bool  templateQ1(PQNode<T,X,Y> *nodePtr,
                               bool isRoot); 
      
	virtual   bool  templateQ2(PQNode<T,X,Y> *nodePtr,
                               bool isRoot);

	virtual   bool  templateQ3(PQNode<T,X,Y> *nodePtr);
      

   
	virtual   bool   addNodeToNewParent(PQNode<T,X,Y>* parent,
                                        PQNode<T,X,Y>* child);

	virtual   bool   addNodeToNewParent(PQNode<T,X,Y>* parent,
                                        PQNode<T,X,Y>* child,
                                        PQNode<T,X,Y>* leftBrother,
                                        PQNode<T,X,Y>* rightBrother);

	virtual   bool   checkIfOnlyChild(PQNode<T,X,Y> *child,
					                  PQNode<T,X,Y> *parent);

	virtual void   destroyNode(PQNode<T,X,Y> *nodePtr);

	virtual void   exchangeNodes(PQNode<T,X,Y> *oldNode,
                                 PQNode<T,X,Y> *newNode);

	virtual void   linkChildrenOfQnode(PQNode<T,X,Y> *installed,
                                       PQNode<T,X,Y> *newChild);

	virtual void   removeChildFromSiblings(PQNode<T,X,Y>* nodePtr);

	virtual int    removeNodeFromTree(PQNode<T,X,Y>* parent,
	   								  PQNode<T,X,Y>* child);

   
    List<PQNode<T,X,Y>*>*  fullChildren(PQNode<T,X,Y>* nodePtr){
		return nodePtr->fullChildren;
	};


    List<PQNode<T,X,Y>*>*  partialChildren(PQNode<T,X,Y>* nodePtr){
		return nodePtr->partialChildren;
	}; 

    virtual PQNode<T,X,Y>* clientLeftEndmost(PQNode<T,X,Y>* nodePtr) const{
		return nodePtr->m_leftEndmost;
	};

    virtual PQNode<T,X,Y>* clientRightEndmost(PQNode<T,X,Y>* nodePtr) const{
		return nodePtr->m_rightEndmost;
	};

    virtual PQNode<T,X,Y>* clientNextSib(PQNode<T,X,Y>* nodePtr,
                                         PQNode<T,X,Y>* other) const{
		return nodePtr->getNextSib(other);
	};

    virtual PQNode<T,X,Y>* clientSibLeft(PQNode<T,X,Y>* nodePtr) const{
		return nodePtr->m_sibLeft;
	};

    virtual PQNode<T,X,Y>* clientSibRight(PQNode<T,X,Y>* nodePtr) const{
		return nodePtr->m_sibRight;
	};

    virtual int   clientPrintNodeCategorie(PQNode<T,X,Y>* nodePtr);

    virtual const char* clientPrintStatus(PQNode<T,X,Y>* nodePtr);

    virtual const char* clientPrintType(PQNode<T,X,Y>* nodePtr);

private:
      
	void addLeavesRunThrough(PQInternalNode<T,X,Y>        *parent,
                             SListPure<PQLeafKey<T,X,Y>*> &leafKeys,
                             PQLeaf<T,X,Y>                *actualSon);
                              
	bool checkChain(PQNode<T,X,Y> *nodePtr,
                    PQNode<T,X,Y> *firstFull,
                    PQNode<T,X,Y> **seqStart,
                    PQNode<T,X,Y> **seqEnd);

	void copyFullChildrenToPartial(PQNode<T,X,Y> *nodePtr,
                                   PQNode<T,X,Y> *partialChild);

	PQNode<T,X,Y>*  createNodeAndCopyFullChildren(
										List<PQNode<T,X,Y>*> *fullNodes);
/*
	int  inExceptions(int searchKey,
                      int Exceptions[],
                      int arraySize);
*/
	void printNode(char *filename,
                   int number,
                   PQNode<T,X,Y>* father,
                   PQNode<T,X,Y>* son);  

	void removeBlock(PQNode<T,X,Y> *nodePtr,
                     bool isRoot);

	void sortExceptions(int Exceptions[],
                        int arraySize);

};


/************************************************************************
                      addLeavesRunThrough
************************************************************************/ 

/**
 * The function addLeavesRunThrough() is used only by the function
 * addNewLeavesToTree().
 * The function creates a sequence of new leaves out of a set of new
 * elements of type PQLeafKey stored in the array of leafKeys. 
 * It uses the following parameters.
 * @param parent is a pointer to the node that will be the parent of
 *     the new leaves.
 * @param leafKeys contains the set of elements of type
 *     PQLeafKey. There must be a proper PQLeaf allocated with
 *     leafKeys[0]. All other elements stored in
 *     leafKeys are not allowed to have a PQLeaf allocated. The
 *     function addLeavesRunTrough() allocates for each one of them a PQLeaf and sets the sibling
 *     pointers.
 * @param numberOfElements depicts the number of elements of type
 *     PQLeafKey stored in leafKeys
 * @param actualSon is a return value. It depicts the PQLeaf that has
 *     been allocated last. This will be needed by the calling function in
 *     order to introduce the sequence of leaves correctly into the
 *     children of parent.
 */

template<class T,class X,class Y>
void PQTree<T,X,Y>::addLeavesRunThrough(PQInternalNode<T,X,Y> *parent,
                                   SListPure<PQLeafKey<T,X,Y>*> &leafKeys,
                                   PQLeaf<T,X,Y> *actualSon)
{
	PQLeafKey<T,X,Y>*   newKey = 0;
	int       i      = 0;

	SListIterator<PQLeafKey<T,X,Y>*> it = leafKeys.begin();
	if (it.valid())
		++it;
	for (; it.valid(); ++it)
	{
		newKey = *it; //leafKeys[i];
		actualSon = OGDF_NEW PQLeaf<T,X,Y>(m_identificationNumber++,EMPTY,newKey);
		actualSon->m_parent = parent;
		actualSon->m_parentType = parent->type();
		parent->m_childCount++;
		if (leafKeys[i-1]->nodePointer()->putSibling(actualSon) 
			== LEFT)
			actualSon->m_sibRight = leafKeys[i-1]->nodePointer();
		else
			actualSon->m_sibLeft = leafKeys[i-1]->nodePointer();
	}
}
   

/************************************************************************
                      addNewLeavesToTree
************************************************************************/ 

/**
 * The function addNewLeavesToTree() adds a set of elements to the already
 * existing set of elements of a PQ-tree.
 * These elements have to be of type PQLeafKey
 * and are handed to the function in an array leafKeys.
 * The father of the new elements that has to be an existing P- or Q-node,
 * has to be specified and is not allowed to have children.
 *
 * The above mentioned facts
 * are checked by the function addNodeToNewParent() and the process
 * of adding a child to parent is interrupted with an error message
 * returning 0 as soon none of the facts is fullfilled.
 * The function addNewLeavesToTree() returns 1 if it
 * succeeded in adding the leaves to parent.
 */

template<class T,class X,class Y>
bool PQTree<T,X,Y>::addNewLeavesToTree(PQInternalNode<T,X,Y> *father,
                                  SListPure<PQLeafKey<T,X,Y>*> &leafKeys)
{
	PQLeafKey<T,X,Y>*    newKey = 0;
	PQLeaf<T,X,Y>*       aktualSon = 0;
	PQNode<T,X,Y>*       oldSon = 0;
	PQNode<T,X,Y>*       firstSon = 0;

	if (!leafKeys.empty())
	{
		OGDF_ASSERT(!father->m_childCount) 
		// Father has children. Brothers expected

		/// Enter the first element as PQLeaf to the [[parent]].
		SListIterator<PQLeafKey<T,X,Y>*> it = leafKeys.begin();
		newKey = *it; //leafKeys[0];
		aktualSon = OGDF_NEW PQLeaf<T,X,Y>(m_identificationNumber++,
									 EMPTY,newKey);
		firstSon = aktualSon;
		firstSon->m_parent = father;
		firstSon->m_parentType = father->type();
		father->m_childCount++;
		oldSon = firstSon;

		/// Enter all other elements as leaves to [[parent]].
		for (++it; it.valid(); ++it)
		{
			newKey = *it; //leafKeys[i];
			aktualSon = OGDF_NEW PQLeaf<T,X,Y>(m_identificationNumber++,
										 EMPTY,newKey);
			aktualSon->m_parent = father;
			aktualSon->m_parentType = father->type();
			father->m_childCount++;
			oldSon->m_sibRight = aktualSon;
			aktualSon->m_sibLeft = oldSon;
			oldSon = aktualSon;
		}
		if (father->type() == P_NODE)
		/// Set the reference pointers if [[parent]] is a $P$-node.			
		{
			firstSon->m_sibLeft = oldSon;
			oldSon->m_sibRight = firstSon;
			father->m_referenceChild = firstSon;
			firstSon->m_referenceParent = father;
		}
		else if (father->type() == Q_NODE)
		/// Set the endmost children if [[parent is a $Q$-node.
		{
			father->m_leftEndmost = firstSon;
			father->m_rightEndmost = oldSon;
		}
		return true; 
	}
	else 
		return false;
}


/************************************************************************
                      addNewLeavesToTree
************************************************************************/ 

/**
 * The function addNewLeavesToTree() adds a set of elements to the already
 * existing set of elements of a PQ-tree.
 * These elements have to be of type PQLeafKey
 * and are handed to the function in an array leafKeys.
 * The father of the new elements that has to be an existing P- or Q-node,
 * has to be specified and is allowed to have children. In this case
 * the siblings of the new leaves that are instantiated, have to be specified
 * as well. If no siblings are specified, the function addNewLeavesToTree1()
 * is called by default.
 *
 * The client of this function should observe the following fact:
 *   - If \a parent is a P-node than only one sibling is needed in order
 *     to enter the new leaves. If the client specifies two siblings in
 *     \a leftBrother
 *     and \a rightBrother, then an arbitrary one is choosen to be a sibling.
 *   - If \a parent is a Q-node, two siblings <b>must be specified</b> if
 *     the new leaves have to become interior children of the Q-node. If just
 *     one sibling is specified, then this implies that the new leaves are
 *     about to become endmost children of \a parent. So either \a leftBrother or
 *     \a rightBrother must store an existing endmost child of parent.
 *
 * The above mentioned facts
 * are checked by the function addNodeToNewParent(() and the process
 * of adding a child to \a parent is interrupted with an error message
 * returning 0 as soon
 * none of the facts is fullfilled. 
 * The function addNewLeavesToTree() returns 1 if it
 * succeeded in adding the leaves to \ parent.
 */

template<class T,class X,class Y>
bool PQTree<T,X,Y>::addNewLeavesToTree(PQInternalNode<T,X,Y> *parent,
                                  SListPure<PQLeafKey<T,X,Y>*> &leafKeys,
                                  PQNode<T,X,Y> *leftBrother,
                                  PQNode<T,X,Y> *rightBrother)
{
	PQLeafKey<T,X,Y>*      newKey = 0;
	PQLeaf<T,X,Y>*     actualSon = 0;
   
	if ((leftBrother == 0) && (rightBrother == 0))
		return addNewLeavesToTree(parent,leafKeys);
	else if (!leafKeys.empty())
	{
		SListIterator<PQLeafKey<T,X,Y>*> it = leafKeys.begin();
		newKey = *it; //leafKeys[0];
		actualSon = OGDF_NEW PQLeaf<T,X,Y>(m_identificationNumber++,EMPTY,newKey);
		actualSon->m_parent = parent;
		actualSon->m_parentType = parent->type();
		parent->m_childCount++;

		if (parent->type() == P_NODE)
		{
			/*
			[[parent]] is a $P$-node with children. 
			In this case either the [[leftBrother]] or the [[rightBrother]] 
			stores a pointer to an  existing child of [[parent]] and 
			[[parent]] is a $P$-node. In case that two brothers are stored, 
			an arbitrary one is choosen to be the next sibling of the sequence
			of children that has to be introduced as children of [[parent]]. 
			This brother is stored in [[brother]]. This chunk lists the code 
			for constructing a sequence of new leaves and introducing the 
			sequence next to [[brother]]. The pointer [[sister]] denotes a 
			pointer to an arbitrary sibling of [[brother]].
			*/
			PQNode<T,X,Y>* brother = 0;
			PQNode<T,X,Y>* sister  = 0;
			if (leftBrother != 0)
				brother = leftBrother;
			else
				brother = rightBrother;
			sister = brother->m_sibRight;

			actualSon->m_sibLeft = brother;
			brother->m_sibRight = actualSon;

			addLeavesRunThrough(parent,leafKeys,actualSon);

			actualSon->m_sibRight = sister;
			sister->m_sibLeft = actualSon;
			return true;
		}

		else if (leftBrother == 0)
		{
			/*
			[[parent]] is a  node with children.
			The [[leftBrother]] is a [[0]]-pointer while the 
			[[rightBrother]] denotes an existing child of [[parent]]. The 
			node [[rightBrother]] {\bf must be} one of the two endmost 
			children of [[parent]]. If this is not the case, the chunk detects 
			this, halts the procedure [[addNewLeavesToTree]] printing an error 
			message and returning [[0]].
			If [[rightBrother]] is endmost child of [[parent]], then this 
			chunk constructs a sequence of leaves and adds it at the one end 
			where [[rightBrother]] hides. The last leave of the sequence is 
			made the new endmost child of [[parent]] on the corresponding side.
			*/
			if (rightBrother == parent->m_leftEndmost)
			{
				actualSon->m_sibRight = rightBrother;
				if (!rightBrother->putSibling(actualSon))
					return false;
				else
				{
					addLeavesRunThrough(parent,leafKeys,actualSon);
					parent->m_leftEndmost = actualSon;
					return true;
				}
			}
			else if (rightBrother == parent->m_rightEndmost)
			{
				actualSon->m_sibLeft = rightBrother;
				if (!rightBrother->putSibling(actualSon))
					return false;
				else
				{
					addLeavesRunThrough(parent,leafKeys,actualSon);
					parent->m_rightEndmost = actualSon;
					return true;
				}
			}
			else
			{
				OGDF_ASSERT(false) //missing second brother.
				return false;
			}
		}

		else if (rightBrother == 0)
		{
			/*
			[[parent]] is a $Q$-node with children. 
			The [[rightBrother]] is a [[0]]-pointer while the 
			[[leftBrother]] denotes an existing child of [[parent]]. The node 
			[[leftBrother]] {\bf must be} one of the two endmost children of 
			[[parent]]. If this is not the case, the chunk detects this, 
			halts the procedure [[addNewLeavesToTree]] printing an error 
			message and returning [[0]].
			If [[leftBrother]] is endmost child of [[parent]], then this chunk 
			constructs a sequence of leaves and adds it at the one end where 
			[[leftBrother]] hides. The last leave of the sequence is made the 
			new endmost child of [[parent]] on the corresponding side.
			*/
			if (leftBrother == parent->m_rightEndmost)
			{
				actualSon->m_sibLeft = leftBrother;
				if (!leftBrother->putSibling(actualSon))
					return false;
				else
				{
					addLeavesRunThrough(parent,leafKeys,actualSon);
					parent->m_rightEndmost = actualSon;
					return true;
				}
			}
			else if (leftBrother == parent->m_leftEndmost)
			{
				actualSon->m_sibRight = leftBrother;
				if (!leftBrother->putSibling(actualSon))
					return false;
				else
				{
					addLeavesRunThrough(parent,leafKeys,actualSon);
					parent->m_leftEndmost = actualSon;
					return true;
				}
			}
			else
			{
				OGDF_ASSERT(false) //missing second brother.
				return false;
			}
		}

		else
		{
			/*
			[[parent]] is a node with children. 
			both the [[rightBrother]] and the [[leftBrother]] denote 
			existing children of [[parent]]. In this case, [[leftBrother]] 
			and [[rightBrother]] must be immideate siblings. If this is not 
			the case, this will be detected during the function call 
			[[changeSiblings]] of the class [[PQNode.h]] (see 
			\ref{PQNode.changeSiblings}) in the first line of this chunk. 
			If the chunk recognizes the failure of [[changeSiblings]] it halts 
			the procedure [[addNewLeavesToTree]], printing an error message 
			and returning [[0]].
			If the two brothers are immediate siblings, this chunk constructs a
			sequence of leaves and and adds it between the two brothers as 
			interior	children of the $Q$-node [[parent]].
			*/
			if (!leftBrother->changeSiblings(rightBrother,actualSon))
			{
				OGDF_ASSERT(false) //brothers are not immediate siblings.
				return false;
			}
			else
			{
				if (leftBrother->m_sibRight == actualSon)
					actualSon->m_sibLeft = leftBrother;
				else
					actualSon->m_sibRight = leftBrother;
				addLeavesRunThrough(parent,leafKeys,actualSon);
				actualSon->putSibling(rightBrother);
				return rightBrother->changeSiblings(leftBrother,actualSon);
			}
		}
	}

	else
		return true;
}


/************************************************************************
                      addNodeToNewParent
************************************************************************/ 

/**
 * The function addNodeToNewParent() adds a node \a child as a child 
 * to another node specified in \a parent. 
 * The \a parent of the new node has to be an existing P- or Q-node and
 * is \b not allowed to have children.
 * In the case, that \a parent has children, addNewNodeToParent()
 * returns 0 printing an error-message.
 * In this case, use the function addNodeToParent() while specifying the future
 * siblings of \a child. See addNodeToNewParent2() for more
 * details.
 *
 * After successfully inserting \a child to \a parent the function
 * addNewNodeToParent() returns 1. Otherwise it returns
 * 0.
 */

template<class T,class X,class Y>
bool PQTree<T,X,Y>::addNodeToNewParent(PQNode<T,X,Y>* parent,
                                      PQNode<T,X,Y>* child)
{
	OGDF_ASSERT(parent->type() == P_NODE && parent->type() == Q_NODE)
		//parent type not valid.

	if (child != 0)
	{
		OGDF_ASSERT(parent->m_childCount == 0)   
			//when adding new nodes: Brothers expected.
		child->m_parent = parent;
		child->m_parentType = parent->type();
		parent->m_childCount++;

		/*
		Set the reference pointers in case that [[parent]] is a $P$-node.
		If [[parent]] is a $Q$-node, this chunk sets the endmost children 
		of [[parent]]. Since [[child]] is the only child of [[parent]] 
		both endmost pointers are set to [[child]].
		*/
		if (parent->type() == P_NODE)
		{
			child->m_sibLeft = child;
			child->m_sibRight = child;
			parent->m_referenceChild = child;
			child->m_referenceParent = parent;
		}
		else if (parent->type() == Q_NODE)
		{
			parent->m_leftEndmost = child;
			parent->m_rightEndmost = child;
		}
        
		return true;
	}

	else 
		return false;
}


/************************************************************************
                      addNodeToNewParent
************************************************************************/ 

/**
 * The function addNodeToNewParent() adds a node \a child to the children
 * of another node specified in \a parent. 
 * The \a parent of the new node has to be an existing P- or Q-node and
 * is allowed to have children. In case that \a parent has children,
 * the siblings of the new introduced child <b> must be specified</b>.
 * If no siblings are specified, the function addNodeToNewParent(PQNode<T,X,Y>*,PQNode<T,X,Y>*)
 * is called by default.
 * If the \a parent is not specified, the function assumes that
 * \a child is added as interior child to a Q-node.
 *
 * The client of this function should observe the following facts:
 *   - If \a parent is a P-node, than only one sibling is needed in order
 *     to enter the \a child. If the client specifies two siblings in \a leftBrother
 *     and \a rightBrother, then an arbitrary one is choosen to be a sibling.
 *   - If \a parent is a Q-node, two siblings <b>must be specified</b> if
 *     \a child has to become an interior child of the Q-node. If just
 *     one sibling is specified, this implies that \a child is about to become
 *     a new endmost child of \a parent. So either \a leftBrother or
 *     \a rightBrother must store an existing endmost child of \a parent.
 *   - If \a parent is a zero pointer, addNodeToNewParents() assumes
 *     that \a child is added as interior child to a Q-node. In this
 *     case \b both siblings of \a child <b>have to be specified</b>. Observe
 *     however, that it is also legal to specify the parent in this case.
 *
 * The above mentioned facts
 * are checked by the function addNodeToNewParent() and the process
 * of adding a child to \a parent is interrupted with an error message
 * returning 0 as soon
 * none of the facts is fullfilled.
 * The function addNodeToNewParent() returns 1 if it
 * succeeded in adding the \a child to \a parent.
*/

template<class T,class X,class Y>
bool PQTree<T,X,Y>::addNodeToNewParent(PQNode<T,X,Y>* parent,
                                      PQNode<T,X,Y>* child,
                                      PQNode<T,X,Y>* leftBrother,
                                      PQNode<T,X,Y>* rightBrother)
{
   
	if (parent != 0)
	{
		OGDF_ASSERT(parent->type() == P_NODE || parent->type() == Q_NODE)
			//parent type not valid
		if ((leftBrother == 0) && (rightBrother == 0))
			return addNodeToNewParent(parent,child);
		else if (child != 0)
		{
			child->m_parent = parent;
			child->m_parentType = parent->type();
			parent->m_childCount++;

			if (parent->type() == P_NODE)
			{
				/*
				The parent is a $P$-node with children.
				Either [[leftBrother]] or [[rightBrother]] stores 
				a pointer to an existing child of [[parent]] and [[parent]] 
				is a $P$-node. In case that two brothers are stored, an 
				arbitrary one is choosen to be the next sibling of [[child]].
				This brother is stored in [[brother]]. The pointer [[sister]]
				denotes a pointer to an arbitrary sibling of [[brother]].
				*/
				PQNode<T,X,Y>* brother = 0;
				PQNode<T,X,Y>* sister  = 0;
				if (leftBrother != 0)
					brother = leftBrother;
				else
					brother = rightBrother;
				sister = brother->m_sibRight;
				child->m_sibLeft = brother;
				child->m_sibRight = sister;
				brother->m_sibRight = child;
				sister->m_sibLeft = child;
				return true;
			}
      
			else if (leftBrother == 0)
			{
				/*
				The parent is a $Q$-node with children.
				The [[leftBrother]] is a [[0]]-pointer while the 
				[[rightBrother]] denotes an existing child of [[parent]].
				The node [[rightBrother]] {\bf must be} one of the two endmost 
				children of [[parent]]. If this is not the case, the chunk 
				detects this, halts the procedure [[addNewLeavesToTree]] 
				printing an error message and returning [[0]].
				If [[rightBrother]] is endmost child of [[parent]], then 
				this chunk adds [[child]] at the one end where
				[[rightBrother]] hides. The node [[child]] is then made the
				new endmost child of [[parent]] on the corresponding side.
				*/
				if (rightBrother == parent->m_leftEndmost)
				{
					parent->m_leftEndmost = child;
					child->m_sibRight = rightBrother;
					rightBrother->putSibling(child,LEFT);
					return true;
				}
				else if (rightBrother == parent->m_rightEndmost)
				{
					parent->m_rightEndmost = child;
					child->m_sibLeft = rightBrother;
					rightBrother->putSibling(child,LEFT);
					return true;
				}
				//else
				OGDF_ASSERT(false);//missing second brother
				return false;
			}

			else if (rightBrother == 0)
			{
				/*
				The parent is a $Q$-node with children. 
				The [[rightBrother]] is a [[0]]-pointer while the 
				[[leftBrother]] denotes an existing child of [[parent]]. The 
				node [[leftBrother]] {\bf must be} one of the two endmost 
				children of [[parent]]. If this is not the case, the chunk 
				detects this, halts the procedure [[addNodeToNewParent]] 
				printing an error message and returning [[0]].
				If [[leftBrother]] is endmost child of [[parent]], then this 
				chunk adds [[child]] at the one end where [[leftBrother]] 
				hides. The node [[child]] is then made new endmost child of 
				[[parent]] on the corresponding side.
				*/
				if (leftBrother == parent->m_rightEndmost)
				{
					parent->m_rightEndmost = child;
					child->m_sibLeft = leftBrother;
					leftBrother->putSibling(child,RIGHT);
					return true;
				}
				else if (leftBrother == parent->m_leftEndmost)
				{
					parent->m_leftEndmost = child;
					child->m_sibRight = leftBrother;
					leftBrother->putSibling(child,RIGHT);
					return true;
				}
				//else
				OGDF_ASSERT(false); //missing second brother
				return false;
			}

			else
			{
				/*
				The parent is a $Q$-node with children. 
				Both the [[rightBrother]] and the [[leftBrother]] denote 
				existing children of [[parent]]. In this case, [[leftBrother]] 
				and [[rightBrother]] must be immideate siblings. If this is 
				not the case, this will be detected during the function call
				[[changeSiblings]] of the class [[PQNode.h]] (see 
				\ref{PQNode.changeSiblings}) in the first two lines of this 
				chunk. If the chunk recognizes the failure of 
				[[changeSiblings]] it halts the procedure 
				[[addNewLeavesToTree]], printing an error message and 
				returning [[0]].
				If the two brothers are immediate siblings, this chunk 
				adds [[child]] between the two brothers as interior child of 
				the $Q$-node [[parent]].
				*/
				if (rightBrother->changeSiblings(leftBrother,child) &&
					leftBrother->changeSiblings(rightBrother,child))
				{
					if (leftBrother->m_sibRight == child)
					{
						child->m_sibLeft = leftBrother;
						child->m_sibRight = rightBrother;
					}
					else 
					{
						child->m_sibLeft = rightBrother;
						child->m_sibRight = leftBrother;
					}
					return true;
				}
				// else
				OGDF_ASSERT(false); //brothers are not siblings.
				return false;
			}
		}
		else
			return false;
	}
	else if (leftBrother != 0 && rightBrother != 0)
	{
		/*
		The parent is a $Q$-node with children. 
		Both the [[rightBrother]] and the [[leftBrother]] denote 
		existing children of [[parent]]. In this case, [[leftBrother]] 
		and [[rightBrother]] must be immideate siblings. If this is 
		not the case, this will be detected during the function call
		[[changeSiblings]] of the class [[PQNode.h]] (see 
		\ref{PQNode.changeSiblings}) in the first two lines of this 
		chunk. If the chunk recognizes the failure of 
		[[changeSiblings]] it halts the procedure 
		[[addNewLeavesToTree]], printing an error message and 
		returning [[0]].
		If the two brothers are immediate siblings, this chunk 
		adds [[child]] between the two brothers as interior child of 
		the $Q$-node [[parent]].
		*/
		if (rightBrother->changeSiblings(leftBrother,child) &&
			leftBrother->changeSiblings(rightBrother,child))
		{
			if (leftBrother->m_sibRight == child)
			{
				child->m_sibLeft = leftBrother;
				child->m_sibRight = rightBrother;
			}
			else 
			{
				child->m_sibLeft = rightBrother;
				child->m_sibRight = leftBrother;
			}
			return true;
		}
		//else
		OGDF_ASSERT(false); //brothers are not siblings.
		return false;
	}       
	else
		return true;

}



/************************************************************************
                           Bubble
************************************************************************/ 

/**
 * The function Bubble() realizes a function described in [Booth].
 * It <em>bubbles</em> up from the pertinent leaves to the pertinent root
 * in order to make sure that every pertinent node in the pertinent subtree
 * has a valid pointer to its parent. If Bubble() does not succed in doing so,
 * then the set of elements, stored in the \a leafKeys cannot form
 * a consecutive sequence.
 *
 * The function Bubble() uses a wide variaty of variables, explained
 * in detail below.
 *   - \a blockcount is the number of blocks of blocked nodes during
 *     the bubbling up phase.
 *   - \a numBlocked is the number of blocked nodes during the
 *     bubbling up phase.
 *   - \a blockedSiblings counts the number of blocked siblings that
 *      are adjacent to \a checkNode. A node has 0, 1 or 2 blocked siblings.
 *      A child of a P-node has no blocked siblings. Endmost children of
 *      Q-nodes have at most 1 blocked sibling. The interior children of
 *      a Q-Node have at most 2 blocked siblings.
 *   - \a checkLeaf is a pointer used for finding the pertinent leaves.
 *   - \a checkNode is a pointer to the actual node.
 *   - \a checkSib is a pointer used to examin the siblings of [[checkNode]].
 *   - \a offTheTop is a variable which is either 0 (that is its
 *     initial value) or 1 in case that the root of the tree has been
 *     process during the first phase.
 *   - \a parent is a pointer to the parent of \a checkNode, if \a checkNode
 *     has a valid parent pointer.
 *   - \a processNodes is a first-in first-out list that is used for
 *     sequencing the order in which the nodes are processed.
 *   - \a blockedNodes is a stack storing all nodes that have been
 *     once blocked. In case that the [[m_pseudoRoot]] has to be
 *     introduced, the stack contains the blocked nodes.
 */

#ifdef DEBUG
template<class T,class X,class Y>
bool PQTree<T,X,Y>::Bubble(SListPure<PQLeafKey<T,X,Y>*> &leafKeys,
                          int redNumber)
#else
template<class T,class X,class Y>
bool PQTree<T,X,Y>::Bubble(SListPure<PQLeafKey<T,X,Y>*> &leafKeys)
#endif
{
	Queue<PQNode<T,X,Y>*> processNodes;
	Stack<PQNode<T,X,Y>*> blockedNodes;

#ifdef DEBUG
	List<PQNode<T,X,Y>*> *processLeaves = OGDF_NEW List<PQNode<T,X,Y>*>;
#endif


	int         blockCount       = 0;
	int         numBlocked       = 0;
	int         offTheTop        = 0;

	int         blockedSiblings  = 0;
	PQNode<T,X,Y>*   checkLeaf   = 0;
	PQNode<T,X,Y>*   checkNode   = 0;
	PQNode<T,X,Y>*   checkSib    = 0;
	PQNode<T,X,Y>*   holdSib     = 0;
	PQNode<T,X,Y>*   oldSib      = 0;
	PQNode<T,X,Y>*   parent      = 0;
	PQNode<T,X,Y>*   lastBlocked = 0;

	/*
	Enter the [[Full]] leaves into the queue [[processNodes]].
	In a first step the pertinent leaves have to be identified in the tree
	and entered on to the queue [[processNodes]]. The identification of
	the leaves can be done with the help of a pointer stored in every 
	[[PQLeafKey]] (see \ref{PQLeafKey}) in constant time for every element.	
	*/
	//for (i = 0; i < leafKeys.size(); i++)
	SListIterator<PQLeafKey<T,X,Y>*> it;
	for (it  = leafKeys.begin(); it.valid(); ++it) 
	{
		checkLeaf = (*it)->nodePointer(); //leafKeys[i]->nodePointer();
		checkLeaf->mark(QUEUED);
		processNodes.append(checkLeaf);
		m_pertinentNodes->pushFront(checkLeaf);
		#ifdef DEBUG
			checkLeaf->status(FULL);
			processLeaves->pushFront(checkLeaf);
		#endif

	}

#ifdef DEBUG
	char nb[INFOSIZE];
	ogdf::sprintf(nb,"%d",redNumber);
	printOutCurrentTree("PQBPrint",nb);
	while (!processLeaves->empty())
	{
		checkLeaf = processLeaves->popFrontRet();
		checkLeaf->status(EMPTY);
	}
#endif

	while ((processNodes.size() + blockCount + offTheTop) > 1)
	{
		if (processNodes.size() == 0)
			/* 
			No consecutive sequence possible.
			The queue [[processNodes]] does not contain any nodes for
			processing and the sum of [[blockCount]] and [[offTheTop]] is
			greater than 1. If the queue is empty, the root of the pertinent 
			subtree was already processed. Nevertheless, there are blocked 
			nodes since [[offTheTop]] is either be [[0]] or [[1]], hence 
			[[blockCount]] must be at least [[1]]. Such blocked nodes cannot 
			form a consecutive sequence with all nodes in the set 
			[[leafKeys]]. Observe that this chunk finishes the function 
			[[Bubble]]. Hence every memory allocated by the function [[Bubble]] 
			has to be deleted here as well.        
			*/
			return false;
		/*
		If there are still nodes to be processed in which case the queue
		[[processNodes]] is not empty, we get the next node from the queue.
		By default this node has to be marked as blocked.
		*/
		checkNode = processNodes.pop();
		blockedNodes.push(checkNode);
		checkNode->mark(BLOCKED);
		blockedSiblings = 0;

		/*
		Check if node is adjacent to an unblocked node.
		After getting the node [[checkNode]] from the queue, its siblings are 
		checked, whether they are unblocked. If they are, then they have a 
		valid pointer to their parent and the parent pointer of [[checkNode]] 
		is updated.
		*/
		if ((checkNode->m_parentType != P_NODE) && (checkNode != m_root))
                                            // checkNode is son of a Q_NODE.
                                            // Check if it is blocked.
		{
			if (clientSibLeft(checkNode) == 0)
                                            // checkNode is endmost child of
                                            // a Q_NODE. It has a valid pointer
                                            // to its parent.
			{
				checkNode->mark(UNBLOCKED);
				if (clientSibRight(checkNode) && 
					clientSibRight(checkNode)->mark() == BLOCKED)
					blockedSiblings++;
			}

			else if (clientSibRight(checkNode) == 0)
                                            // checkNode is endmost child of
                                            // a Q_NODE. It has a valid pointer
                                            // to its parent.
			{
				checkNode->mark(UNBLOCKED);
				if (clientSibLeft(checkNode) && 
					clientSibLeft(checkNode)->mark() == BLOCKED)
					blockedSiblings++;
			}
             

			else
                                            // checkNode is not endmost child of
                                            // a Q_NODE. It has not a valid 
											// pointer to its parent.
			{
				if (clientSibLeft(checkNode)->mark() == UNBLOCKED)
                                            // checkNode is adjacent to an
                                            // unblocked node. Take its parent.
				{
					checkNode->mark(UNBLOCKED);
					checkNode->m_parent = clientSibLeft(checkNode)->m_parent;
				}
				else if (clientSibLeft(checkNode)->mark() == BLOCKED)
					blockedSiblings++;

				if (clientSibRight(checkNode)->mark() == UNBLOCKED)
                                            // checkNode is adjacent to an 
                                            // unblocked node. Take its parent.
				{
					checkNode->mark(UNBLOCKED);
					checkNode->m_parent = clientSibRight(checkNode)->m_parent;
				}
				else if (clientSibRight(checkNode)->mark() == BLOCKED)
					blockedSiblings++;
			}
		}
         
		else                                // checkNode is son of a P_NODE
                                            // and children of P_NODEs
                                            // cannot be blocked.
			checkNode->mark(UNBLOCKED);

		if (checkNode->mark() == UNBLOCKED)
		{
			parent = checkNode->m_parent;

			/*
			Get maximal consecutive set of blocked siblings.
			This chunk belongs to the procedure [[bubble]].
			The node [[checkNode]] is [[UNBLOCKED]].
			If the parent of [[checkNode]] is a $Q$-Node, then we check the
			siblings [[checkSib]] of [[checkNode]] whether they are 
			[[BLOCKED]]. If they are blocked,  they have to be marked 
			[[UNBLOCKED]] since they are adjacent to the [[UNBLOCKED]] node 
			[[checkNode]]. We then have to proceed with the siblings of 
			[[checkSib]] in order to find [[BLOCKED]] nodes
			adjacent to [[checkSib]]. This is repeated until no [[BLOCKED]]
			nodes are found any more.

			Observe that while running through the children of the $Q$-Node
			(referred by the pointer [[parent]]), their parent pointers, 
			as well as the [[pertChildCount]] of [[parent]] are updated. 
			Furthermore we reduce simultaneously the count [[numBlocked]].	
			*/
			if (blockedSiblings > 0)
			{
				if (clientSibLeft(checkNode) != 0)
				{
					checkSib = clientSibLeft(checkNode);
					oldSib = checkNode;
					holdSib = 0;
					while (checkSib->mark() == BLOCKED)
					{
						checkSib->mark(UNBLOCKED);
						checkSib->m_parent = parent;
						numBlocked--;
						parent->m_pertChildCount++;
						holdSib = clientNextSib(checkSib,oldSib);
						oldSib = checkSib;
						checkSib = holdSib;
						OGDF_ASSERT(checkSib != 0)
							//Blocked node as endmost child of a Q_NODE.
					}
				}

				if (clientSibRight(checkNode) != 0)
				{
					checkSib = clientSibRight(checkNode);
					oldSib = checkNode;
					holdSib = 0;
					while (checkSib->mark() == BLOCKED)
					{
						checkSib->mark(UNBLOCKED);
						checkSib->m_parent = parent;
						numBlocked--;
						parent->m_pertChildCount++;
						holdSib = clientNextSib(checkSib,oldSib);
						oldSib = checkSib;
						checkSib = holdSib;
						OGDF_ASSERT(checkSib != 0)
							//Blocked node as endmost child of a Q_NODE.
					}
				}
			}// if (blockedSiblings > 0)


			/*
			Process parent of [[checkNode]]
			After processing the siblings of the [[UNBLOCKED]] [[checkNode]]
			the parent has to be processed. If [[checkNode]] is the root
			of the tree we do nothing except setting the flag [[offTheTop]].
			If it is not the root and [[parent]] has not been placed onto the
			queue [[processNodes]], the [[parent]] is placed on to 
			[[processNodes]].

			Observe that the number [[blockCount]] is updated. Since 
			[[checkNode]] was [[UNBLOCKED]] all perinent nodes adjacent 
			to that node became [[UNBLOCKED]] as well. Therefore the number 
			of blocks is reduced by the number of [[BLOCKED]] siblings of 
			[[checkNode]].
			*/
			if (parent == 0)
	                                       // checkNode is root of the tree.
				offTheTop = 1;
			else
	                                       // checkNode is not the root.
			{
				parent->m_pertChildCount++;
				if (parent->mark() == UNMARKED)
				{
					processNodes.append(parent);
					m_pertinentNodes->pushFront(parent);
					parent->mark(QUEUED);
				}
			}

			blockCount = blockCount - blockedSiblings;
			blockedSiblings = 0;

		}//if (checkNode->mark() == UNBLOCKED)

		else
		{
			/*
			Process blocked [[checkNode]]
			Since [[checkNode]] is [[BLOCKED]], we cannot continue 
			processing at this point in the Tree. We have to wait until 
			this node becomes unblocked. So only the variables 
			[[blockCount]] and [[numBlocked]] are updated. 
			*/
			blockCount = blockCount + 1 - blockedSiblings;
			numBlocked++;
			lastBlocked = checkNode;
		}

	}//while ((processNodes.size() + blockCount + offTheTop) > 1)

	if (blockCount == 1)
	{
		/*
		If [[blockCount]] $= 1$ enter [[m_pseudoRoot]] to the tree
		In case that the root of the pertinent subtree is a $Q$-node 
		with empty children on both sides and the pertinent children 
		in the middle, it is possible that the $PQ$-tree is reducible. 
		But since the sequence of pertinent children of the $Q$-node is 
		blocked, the procedure is not able to find the parent of its 
		pertinent children. This is due to the fact that the interior
		children of a $Q$-node do not have a valid parent pointer.

		So the root of the pertinent subtree is not known, hence cannot be 
		entered into the processing queue used in the function call [[Reduce]]
		(see \ref{Reduce}). To solve this problem a special node only designed 
		for this cases is used: [[m_pseudoRoot]]. It simulates the root of the 
		pertinent subtree. This works out well, since for this node the only 
		possible template maching is [[templateQ3]] (see \ref{templateQ3}), 
		where no pointers to the endmost children of a $Q$-node are used. 
		*/
		while (!blockedNodes.empty())
		{
			checkNode = blockedNodes.pop();
			if (checkNode->mark() == BLOCKED)
			{
				checkNode->mark(UNBLOCKED);
				checkNode->m_parent = m_pseudoRoot;
				m_pseudoRoot->m_pertChildCount++;              
				OGDF_ASSERT(!checkNode->endmostChild())
					//Blocked node as endmost child of a Q_NODE.
			}
		}
	}

	return true;
}


/************************************************************************
                          checkChain
************************************************************************/ 

/**
 * The function checkChain() is used by the function templateQ2()
 * and templateQ3().
 * It checks whether all full children of a Q-node
 * \a nodePtr form a consecutive sequence. If the full nodes do so,
 * the procedure returns 1 as a result, otherwise 0.
 *
 * The pointer \a firstFull denotes just an arbirtary full child. Starting
 * from this position, checkChain sweeps through the consecutive
 * sequence, halting as soon as a nonfull child is detected.
 * The two pointers \a seqStart and \a seqEnd are set within the
 * function \a checkChain. They denote the first and last node of the consecutive
 * sequence.
 *
 * The client should observe that it is not possible to avoid the use
 * of such a function. According to the procedure Bubble() children of
 * Q-nodes get unblocked as soon as they are adjacent to any pertinent
 * sibling. This includes that chains of more than two partial children
 * are regarded as unblocked as well.
 * Such chains are of course not reducible and therefore
 * have to be detected by the function checkChain().
 *
 * Following we give an overview of the variables used in
 * checkChain().
 *   - \a fullCount counts the number of children that are
 *     discovered by the function checkChain(). This is necessary, since
 *     checkChain() is used by two template matching functions
 *     templateQ2() and templateQ3() where in the latter case the
 *     pointer \a firstFull may point to any full child in the front of the Q-node
 *     \a nodePtr.
 *   - \a notFull is set 1 when an empty child is encountered.
 *   - \a checkNode is the actual node that is examined.
 *   - \a leftNext is the next node that has to be examined on the
 *     left side of \a firstFull.
 *   - \a leftOld is the node that has been examined right before
 *     \a checkNode on the left side of \a firstFull.
 *   - \a rightNext is the next node that has to be examined on the
 *     right side of \a firstFull. Not needed when checkChain() was
 *     called by templateQ2().
 *   - \a rightOld is the node that has been examined right before
 *     \a checkNode on the right side of \afirstFull.
 *     Not needed when checkChain() was called by templateQ2().
*/

template<class T,class X,class Y>
bool PQTree<T,X,Y>::checkChain(PQNode<T,X,Y> *nodePtr,
							  PQNode<T,X,Y> *firstFull,
							  PQNode<T,X,Y> **seqStart, 
							  PQNode<T,X,Y> **seqEnd)

{
	int           fullCount   = 0;
	int           notFull     = 0;      
	PQNode<T,X,Y>  *checkNode;       
	PQNode<T,X,Y>  *leftNext    = 0;
	PQNode<T,X,Y>  *leftOld     = 0;
	PQNode<T,X,Y>  *rightNext   = 0;
	PQNode<T,X,Y>  *rightOld    = 0;
   
	fullCount = nodePtr->fullChildren->size();
	fullCount--;                    // firstFull does have a FULL label.

	/*
	Start at the [[firstFull]] child sweeping through the full children on 
	the left side of [[firstfull]]. It stops as soon as a nonfull child is 
	detected. The last found full child is stored in [[seqEnd]].
	*/
	leftNext = clientSibLeft(firstFull);
	(*seqEnd) = firstFull;
	if (leftNext != 0)
	{
		if (leftNext->status() == FULL)
			fullCount--;
		else
			notFull = 1;
		if (notFull)
			(*seqEnd) = firstFull;
		else
		{
			leftOld = firstFull;
			checkNode = leftNext;
    
			while (fullCount > 0 && !notFull)
                                       // There are still full children to be
                                       // counted, and no empty child has been
                                       // encountered on this side.
			{
				leftNext = clientNextSib(checkNode,leftOld);
				if (leftNext != 0 && leftNext->status() == FULL)
					fullCount--;
				else
					notFull = 1;
				leftOld = checkNode;
				checkNode = leftNext;
			}
			if (checkNode != 0 && checkNode->status() == FULL)
				(*seqEnd)  = checkNode;
			else if (leftOld != 0 && leftOld->status() == FULL)
				(*seqEnd) = leftOld;
			else
			{
				OGDF_ASSERT(false)
					 //searching consecutive sequence in Q2 or Q3.
			}
		}
	}

	/*
	Start at the [[firstFull]] child sweeping through the full children on 
	the right side of [[firstfull]]. It stops as soon as a nonfull child is 
	detected.
	*/
	notFull = 0;
	rightNext = clientSibRight(firstFull);
	(*seqStart) = firstFull;
	if(rightNext != 0)
	{
		if (rightNext->status() == FULL)
			fullCount--;
		else
			notFull = 1;
		if (notFull)
			(*seqStart) = firstFull;
		else
		{
			rightOld = firstFull;
			checkNode = rightNext;

			while (fullCount > 0 && !notFull)
                                       // There are still full children to be
                                       // counted, and no empty child has been
                                       // encountered on this side.
			{
				rightNext = clientNextSib(checkNode,rightOld);
				if (rightNext != 0 && rightNext->status() == FULL)
					fullCount--;
				else
					notFull = 1;
				rightOld = checkNode;
				checkNode = rightNext;
			}
			if (checkNode != 0 && checkNode->status() == FULL)
				(*seqStart) = checkNode;
			else if (rightOld != 0 && rightOld->status() == FULL)
				(*seqStart) = rightOld;
			else
			{
				OGDF_ASSERT(false)
                     //searching consecutive seqeuence in Q2 or Q3.
			}
		}
	}



	if (firstFull == (*seqEnd))
	{
		checkNode = (*seqEnd);
		(*seqEnd) = (*seqStart);
		(*seqStart) = checkNode;
	}
      
	if (fullCount == 0)
                                    // All full children occupy a consecutive
                                    // sequence.
		return true;
	else
		return false;
}


/************************************************************************
                          checkIfOnlyChild
************************************************************************/ 

/**
 * The function checkIfOnlyChild() checks if \a child is the only
 * child of \a parent.  If so, \a child is connected to its
 * grandparent, as long as parent is not the root of the tree. In case
 * that \a parent is the root of the tree and \a child is its only
 * child, the node \a child becomes the new root of the tree. The parent then
 * is completely removed from the tree and destroyd. The return value of
 * the method checkIfOnlyChild() is 1, if \a child was the only
 * child of parent. Otherwise the return value is 0.
 * Before applying the function exchangeNodes(), the function removeChildFromSiblings()
 * is applied. This is usefull in case
 * the node \a parent has some ignored children and has to be reused
 * within some extra algorithmic context.
 */

template<class T,class X,class Y>
bool PQTree<T,X,Y>::checkIfOnlyChild(PQNode<T,X,Y> *child,
									PQNode<T,X,Y> *parent)

{
	if ((parent->type() == P_NODE && parent->m_childCount == 1)
		|| (parent->type() == Q_NODE && parent->m_leftEndmost == child
			&& parent->m_rightEndmost == child))
	{
		removeChildFromSiblings(child);
		child->m_parent = parent->m_parent;
		if (parent->m_parent != 0)         // parent is not the root.
			exchangeNodes(parent,child);
		else
		{
			exchangeNodes(parent,child);
			m_root = child;
		}
		destroyNode(parent);
		return true;
	}
	else
		return false;
}


/************************************************************************
                         CleanNode
************************************************************************/ 

/**
 * The function CleanNode() is a virtual function called for every
 * node by the functions Cleanup() and 
 * emptyAllPertinentNodes()
 * of the PQ-tree before deleting it.
 * This allows the user to access necessary informations that are worth
 * storing before all nodes of the tree are deleted. This also allows the
 * user to delete any information class such as PQLeafKey,
 * PQNodeKey and PQInternalKey if
 * this is useful. 
 */

template<class T,class X,class Y>
void PQTree<T,X,Y>::CleanNode(PQNode<T,X,Y>* nodePtr)
{
	nodePtr = nodePtr;
}


/************************************************************************
                         Cleanup
************************************************************************/ 

/**
 * The function Cleanup() removes the entire PQ-tree, stored in the
 * class template PQTree. The function Cleanup() is called by the
 * destructor of the class template PQTree. 
 * It scans all nodes of the tree and frees the
 * memory used by the tree. Cleanup() includes the removal of the memory
 * allocated by the following datastructures:
 *   - \a m_root,
 *   - \a m_pseudoRoot,
 *   - \a m_pertinentNodes.
 * The function Cleanup() enables the client to reuse the function
 * Initialize().
 *
 * In order to free the allocated memory, all nodes of the
 * tree have to be deleted, hence there destructors are called.
 * In order to achieve this, we start at the root of the tree and go down the
 * tree to the leaves for reaching every node. When a node is processed,
 * (besides the \a m_root, this will always be the node \a checkNode)
 * the pointers of all its children are stored in a queue \a helpqueue and
 * then the processed node is deleted.
 * 
 * The use of a queue \a helpqueue is a must, since the nodes do not
 * have pointers to all of their children, as the children mostly do
 * not have a pointer to their parent.
 *
 * It might look weird at the first glance that the function Cleanup()
 * calls the function emptyAllPertinentNodes(), but if some nodes were removed
 * during a reduction, they were stored in the stack \a m_pertinentNodes.
 * These nodes have to be deleted as well
 * which is provided by the function emptyAllPertinentNodes().
 */

template<class T,class X,class Y>
void PQTree<T,X,Y>::Cleanup()
{

	PQNode<T,X,Y>*  checkNode = 0;
	PQNode<T,X,Y>*  firstSon  = 0;
	PQNode<T,X,Y>*  nextSon   = 0;
	PQNode<T,X,Y>*  lastSon   = 0;
	PQNode<T,X,Y>*  oldSib    = 0;
	PQNode<T,X,Y>*  holdSib   = 0;

  
	Queue<PQNode<T,X,Y>*> *helpqueue = OGDF_NEW Queue<PQNode<T,X,Y>*>;


	if (m_root != 0)
	{
		emptyAllPertinentNodes();
		
		/*
		Process the [[m_root]] of the [[PQTree]]. Before deleting [[m_root]],
		pointers to all its children are stored in the queue [[helpqueue]].
		*/
		if (m_root->type() == P_NODE)
		{
			if (m_root->m_referenceChild != 0) 
			{
				firstSon = m_root->m_referenceChild;
				helpqueue->append(firstSon);

				if (firstSon->m_sibRight != 0)
					nextSon = firstSon->m_sibRight;
				while (nextSon != firstSon)
				{
					helpqueue->append(nextSon);
					nextSon = nextSon->m_sibRight;
				}
			}
		}
		else if (m_root->type() == Q_NODE)
		{  
			firstSon = m_root->m_leftEndmost;
			helpqueue->append(firstSon);
            
			lastSon = m_root->m_rightEndmost;
			helpqueue->append(lastSon);

			nextSon = lastSon->getNextSib(oldSib);
			oldSib = lastSon;
			while (nextSon != firstSon)
			{
				helpqueue->append(nextSon);
				holdSib = nextSon->getNextSib(oldSib);
				oldSib = nextSon;
				nextSon = holdSib;
			}
		}


		CleanNode(m_root);
		if (m_root)
			delete m_root;

		while (!helpqueue->empty())
		{
			checkNode = helpqueue->pop();

			/*
			Process an arbitrary node [[checkNode]] of the [[PQTree]].
			Before deleting [[checkNode]],
			pointers to all its children are stored in the queue [[helpqueue]].
			*/
			if (checkNode->type() == P_NODE)
			{  
				if (checkNode->m_referenceChild != 0) 
				{
					firstSon = checkNode->m_referenceChild;
					helpqueue->append(firstSon);

					if (firstSon->m_sibRight != 0)
						nextSon = firstSon->m_sibRight;
					while (nextSon != firstSon)
					{
						helpqueue->append(nextSon);
						nextSon = nextSon->m_sibRight;
					}
				}
			}
			else if (checkNode->type() == Q_NODE)
			{
				oldSib = 0;
				holdSib = 0;

				firstSon = checkNode->m_leftEndmost;
				helpqueue->append(firstSon);

				lastSon = checkNode->m_rightEndmost;
				helpqueue->append(lastSon);

				nextSon = lastSon->getNextSib(oldSib);
				oldSib = lastSon;
				while (nextSon != firstSon)
				{
					helpqueue->append(nextSon);
					holdSib = nextSon->getNextSib(oldSib);
					oldSib = nextSon;
					nextSon = holdSib;
				}
			}

			CleanNode(checkNode);
			if (checkNode)
				delete checkNode;
		}
	}
	delete helpqueue;

	CleanNode(m_pseudoRoot);
	if (m_pseudoRoot)
		delete m_pseudoRoot;

	if (m_pertinentNodes)
		delete m_pertinentNodes;

	m_root = 0;
	m_pertinentRoot = 0;
	m_pseudoRoot = 0;
	m_pertinentNodes = 0;

	m_numberOfLeaves = 0;
	m_identificationNumber = 0;
}


/************************************************************************
                 clientDefinedEmptyNode
************************************************************************/ 

/**
 * If the user wishes to use different flags in an derived class of PQTree
 * that are not available in this implementation, he can overload the function
 * clientDefinedEmptyNode() in order to make a valid cleanup of the nodes.
 * It will be called per default by the function emptyAllPertinentNodes().
 */

template<class T,class X,class Y>
void PQTree<T,X,Y>::clientDefinedEmptyNode(PQNode<T,X,Y>* nodePtr)
{
	emptyNode(nodePtr);
}

/*
\subsection{\tt clientLeftEndmost}\label{clientLeftEndmost}
The function [[clientLeftEndmost]] is a virtual function that can be
overloaded in a derived class. The objective of the function is mainly
to return the endmost child on the left side of a $Q$-node [[nodePtr]]. 
By overloading this function, a special child on the left side of
[[nodePtr]] can be choosen.  

The function was develloped in order to ignore certain nodes
during the reduction of a subset of elements $S \subseteq U$, hence it
is used in the functions of the class template [[PQTree]] that
correspond to these reductions.

The user should be aware of the following fact: {\bf This function is
not used at every point of the source code where the endmost child on
the left side of a $Q$-node is needed.}
The reason for this is hidden in the fact that we always have to differ
between 
\begin{itemize}
\item changing pointers between nodes of the tree
\end{itemize}
and
\begin{itemize}
\item deciding the next action according to the information stored at
a certain node.
\end{itemize}

The function [[clientLeftEndmost]] is only used in the latter case within
the following functions:
\begin{description}
\item[[Bubble]] see \ref{Bubble},
\item[[templateQ1]] see \ref{templateQ1},
\item[[templateQ2]] see \ref{templateQ2},
\item[[templateQ3]] see \ref{templateQ3},
\item[[checkChain]] called by one of the above mentioned template functions, 
see \ref{checkChain},
\item[[removeBlock]] called by one of the above mentioned template functions, 
except [[templateQ1]] (see \ref{templateQ1}), see \ref{removeBlock}.
\end{description}
*/

/************************************************************************
                        clientLeftEndmost
************************************************************************/ 
/*
template<class T,class X,class Y>
PQNode<T,X,Y>* PQTree<T,X,Y>::clientLeftEndmost(PQNode<T,X,Y>* nodePtr) const 

{
	return nodePtr->m_leftEndmost;
}
*/
/*
\subsection{\tt clientNextSib}\label{clientNextSib}
The function [[clientNextSib]] is a virtual function that can be
overloaded in a derived class. The objective of the function is mainly
to return the next sibling of [[nodePtr]] that is unequal to [[other]],
while [[other]] denotes a sibling of [[nodePtr]] that has not to be choosen.
By overloading this function, a special sibling on the side of
[[nodePtr]] where [[other]] is not contained can be choosen.  

The function was develloped in order to ignore certain nodes
during the reduction of a subset of elements $S \subseteq U$, hence it
is used in the functions of the class template [[PQTree]] that
correspond to these reductions.

The user should be aware of the following fact: {\bf This function is
not used at every point of the source code where a sibling different from
[[other]] has to be choosen.}
The reason for this is hidden in the fact that we always have to differ
between 
\begin{itemize}
\item changing pointers between nodes of the tree
\end{itemize}
and
\begin{itemize}
\item deciding the next action according to the information stored at
a certain node.
\end{itemize}

The function [[clientNextSib]] is only used in the latter case within
the following functions:
\begin{description}
\item[[Bubble]] see \ref{Bubble}
\item[[checkChain]] see \ref{checkChain}, which called by one of the
template  functions [[templateQ1]] 
(see \ref{templateQ1}), [[templateQ2]] (see \ref{templateQ2}) and  [[templateQ3]] 
(see \ref{templateQ3}).
\end{description}
*/

/************************************************************************
                           clientNextSib
************************************************************************/ 
/*
template<class T,class X,class Y>
PQNode<T,X,Y>* PQTree<T,X,Y>::clientNextSib(PQNode<T,X,Y>* nodePtr,
											PQNode<T,X,Y>* other) const
{
	return nodePtr->getNextSib(other);
}
*/


/************************************************************************
                        clientPrintNodeCategorie
************************************************************************/ 

/**
 * If the user wishes to use different flags in a derived class of PQTree
 * that are not available in this implementation, he can overload the function
 * clientPrintNodeCategorie(). This function is called per default by the functions
 * printOutCurrentTree() and printNode().
 * With the help of this function it is possible to influence the layout of the nodes
 * by using new, different lables depicting node categories
 * in the <em>Tree Interface</em>.
 */

template<class T,class X,class Y>
int PQTree<T,X,Y>::clientPrintNodeCategorie(PQNode<T,X,Y>* nodePtr) 

{
	if (nodePtr != 0)
		return 1;                     // 1 is the standard node categrie
			                          // in the Tree Interface.
	else
		return 0;
}


/************************************************************************
                        clientPrintStatus
************************************************************************/ 

/**
 * If the user wishes to use different status in a derived class of PQTree
 * that are not available in this implementation, he can overload the function
 * clientPrintStatus(). This function is called per default by the functions
 * printOutCurrentTree() and printNode().
 * With the help of this function it is possible to influence the information stored
 * at nodes in the <em>Tree Interface</em> that concern the
 * status of a node.
 */

template<class T,class X,class Y>
const char*  PQTree<T,X,Y>::clientPrintStatus(PQNode<T,X,Y>* nodePtr) 

{
	if (nodePtr != 0)
		return "ERROR";
	else
		return "ERROR: clientPrintStatus: NO NODE ACCESSED";            
}


/************************************************************************
                        clientPrintType
************************************************************************/ 

/**
 * If the user wishes to use different types in a derived class of PQTree
 * that are not available in this implementation, he can overload the function
 * clientPrintType(). This function is called per default by the functions
 * printOutCurrentTree() and printNode().
 * With the help of this function it is possible to influence the information stored
 * at nodes in the <em>Tree Interface</em> that concern the
 * type of a node.
 */

template<class T,class X,class Y>
const char*  PQTree<T,X,Y>::clientPrintType(PQNode<T,X,Y>* nodePtr) 

{
	if (nodePtr != 0)
		return "ERROR";
	else
		return "ERROR: clientPrintType: NO NODE ACCESSED";
}

/*
\subsection{\tt clientRightEndmost}\label{clientRightEndmost}
The function [[clientRightEndmost]] is a virtual function that can be
overloaded in a derived class. The objective of the function is mainly
to return the endmost child on the right side of [[nodePtr]]. 
By overloading this function, a special child on the right side of
[[nodePtr]] can be choosen.  

The function was develloped in order to ignore certain nodes
during the reduction of a subset of elements $S \subseteq U$, hence it
is used in the functions of the class template [[PQTree]] that
correspond to these reductions.

The user should be aware of the following fact: {\bf This function is
not used at every point of the source code where the endmost child on
the right side of a $Q$-node is needed.}
The reason for this is hidden in the fact that we always have to differ
between 
\begin{itemize}
\item changing pointers between nodes of the tree
\end{itemize}
and
\begin{itemize}
\item deciding the next action according to the information stored at
a certain node.
\end{itemize}

The function [[clientRightEndmost]] is only used in the latter case within
the following functions:
\begin{description}
\item[[Bubble]] see \ref{Bubble},
\item[[templateQ1]] see \ref{templateQ1},
\item[[templateQ2]] see \ref{templateQ2},
\item[[templateQ3]] see \ref{templateQ3},
\item[[checkChain]] called by one of the above mentioned template functions, 
see \ref{checkChain},
\item[[removeBlock]] called by one of the above mentioned template functions 
except [[templateQ1]], see \ref{removeBlock}.
\end{description}
*/

/************************************************************************
                        clientRightEndmost
************************************************************************/ 
/*
template<class T,class X,class Y>
PQNode<T,X,Y>* PQTree<T,X,Y>::clientRightEndmost(PQNode<T,X,Y>* nodePtr) const 

{
	return nodePtr->m_rightEndmost;
}
*/

/*
\subsection{\tt clientSibLeft}\label{clientSibLeft}
The function [[clientSibLeft]] is a virtual function that can be
overloaded in a derived class. The objective of the function is mainly
to return the sibling on the left side of [[nodePtr]]. 
By overloading this function, a special sibling on the left side of
[[nodePtr]] can be choosen.  

The function was develloped in order to ignore certain nodes
during the reduction of a subset of elements $S \subseteq U$, hence it
is used in the functions of the class template [[PQTree]] that
correspond to these reductions.

The user should be aware of the following fact: {\bf This function is
not used at every point of the source code where the sibling
on the left side of a node is needed.}
The reason for this is hidden in the fact that we always have to differ
between 
\begin{itemize}
\item changing pointers between nodes of the tree
\end{itemize}
and
\begin{itemize}
\item deciding the next action according to the information stored at
a certain node.
\end{itemize}

The function [[clientSibLeft]] is only used in the latter case within
the following functions:
\begin{description}
\item[[Bubble]] see \ref{Bubble},
\item[[templateQ2]] see \ref{templateQ2},
\item[[templateQ3]] see \ref{templateQ3},
\item[[checkChain]] called by one of the above mentioned template functions plus the
function [[templateQ1]] (see \ref{templateQ1}),  see \ref{checkChain},
\item[[removeBlock]] called by one of the above mentioned template functions.
see \ref{removeBlock}
\end{description}
*/

/************************************************************************
                            clientSibLeft
************************************************************************/ 

/*
template<class T,class X,class Y>
PQNode<T,X,Y>* PQTree<T,X,Y>::clientSibLeft(PQNode<T,X,Y>* nodePtr) const 

{
  return nodePtr->m_sibLeft;
}
*/

/*
\subsection{\tt clientSibRight}\label{clientSibRight}
The function [[clientSibRight]] is a virtual function that can be
overloaded in a derived class. The objective of the function is mainly
to return the sibling on the right side of [[nodePtr]]. 
By overloading this function, a special sibling on the right side of
[[nodePtr]] can be choosen.  

The function was develloped in order to ignore certain nodes
during the reduction of a subset of elements $S \subseteq U$, hence it
is used in the functions of the class template [[PQTree]] that
correspond to these reductions.

The user should be aware of the following fact: {\bf This function is
not used at every point of the source code where the sibling
on the right side of a node is needed.}
The reason for this is hidden in the fact that we always have to differ
between 
\begin{itemize}
\item changing pointers between nodes of the tree
\end{itemize}
and
\begin{itemize}
\item deciding the next action according to the information stored at
a certain node.
\end{itemize}

The function [[clientSibRight]] is only used in the latter case within
the following functions:
\begin{description}
\item[[Bubble]] see \ref{Bubble},
\item[[templateQ2]] see \ref{templateQ2},
\item[[templateQ3]] see \ref{templateQ3},
\item[[checkChain]] called by one of the above mentioned template functions plus the
function [[templateQ1]] (see \ref{templateQ1}),  see \ref{checkChain},
\item[[removeBlock]] called by one of the above mentioned template functions, 
see \ref{removeBlock}.
\end{description}
*/

/************************************************************************
                           clientSibRight
************************************************************************/ 

/*
template<class T,class X,class Y>
PQNode<T,X,Y>* PQTree<T,X,Y>::clientSibRight(PQNode<T,X,Y>* nodePtr) const 

{
	return nodePtr->m_sibRight;
}
*/

/************************************************************************
                         Constructor
************************************************************************/ 


template<class T,class X,class Y> PQTree<T,X,Y>::PQTree()
{
	m_root = 0;
	m_pertinentRoot = 0;
	m_pseudoRoot = 0;
   
	m_numberOfLeaves = 0;
	m_identificationNumber = 0;

   m_pertinentNodes = 0;
}



/************************************************************************
                     copyFullChildrenToPartial
************************************************************************/ 

/**
 * The function copyFullChildrenToPartial() 
 * copies all full children of \a nodePtr to a new P-node 
 * The node \a nodePtr has to be a P-node. The new P-node
 * is added to \a partialChild as an endmost child of \a partialChild.
 * The node \a partialChild has to be a Q-node and the new P-node is added
 * to the side of \a partialChild where the pertinent children are.
 *
 * The new P-node is allocated by this function and referenced by the
 * variable \a newNode.
 *
 * The function copyFullChildrenToPartial() is used by the functions
 * templateP4(), templateP5(), and templateP6().
 */

template<class T,class X,class Y>
void PQTree<T,X,Y>::copyFullChildrenToPartial(PQNode<T,X,Y> *nodePtr,
											  PQNode<T,X,Y> *partialChild)
{
	PQNode<T,X,Y>  *newNode = 0;
	PQNode<T,X,Y>  *checkNode = 0;

	if (nodePtr->fullChildren->size() > 0)
                                        // There are some full children to
                                        // be copied.
	{
		nodePtr->m_childCount = nodePtr->m_childCount -
		                        nodePtr->fullChildren->size();

		newNode = createNodeAndCopyFullChildren(nodePtr->fullChildren);

                                         // Introduce newNode as endmost
                                         // child of the partial Q-node.
		partialChild->m_childCount++;
		partialChild->fullChildren->pushFront(newNode);

		if (clientLeftEndmost(partialChild)->status() == FULL)
		{
			checkNode = partialChild->m_leftEndmost;
			partialChild->m_leftEndmost = newNode;
			linkChildrenOfQnode(checkNode,newNode);
         
		}
		else if (clientRightEndmost(partialChild)->status() == FULL)
		{
			checkNode = partialChild->m_rightEndmost;
			partialChild->m_rightEndmost = newNode;	
			linkChildrenOfQnode(checkNode,newNode);
		}
		else 
		{
			//ERROR: Endmostchild not found.
			OGDF_ASSERT(false)
		}
			
		newNode->m_parent = partialChild;
		newNode->m_parentType = Q_NODE;
	}
}


/************************************************************************
                     createNodeAndCopyFullChildren
************************************************************************/ 

/**
 * The function createNodeAndCopyFullChildren() copies the full children of a
 * P-node that are stored in the stack \a fullNodes to a new P-node.
 * This new P-node
 * is created by the function and stored in \a newNode if there is more than one full child.
 * If there is just one full child, it is not necessary to construct a new
 * P-node and the full child is stored in \a newNode. 
 * The \a newNode is the return value of the procedure.
 *
 * The function createNodeAndCopyFullChildren() is used by
 * templateP2() templateP3() and the function copyFullChildrenToPartial().
 * The function createNodeAndCopyFullChildren() uses the following
 * variables.
 *   - \a newNode stores the adress of the new allocated P-node or
 *     the adress of the only full child.
 *   - \a oldSon is a variable used for adding the full nodes as
 *     children to the new P-node.
 *   - \a firstSon stores the adress of the first detected full
 *     child. It is needed  for adding the full nodes as
 *     children to the new P-node.
 *   - \a checkSon is a variable used for adding the full nodes as
 *     children to the new P-node.
 *   - \a newPQnode is used for proper allocation of the new P-node.
 */

template<class T,class X,class Y>
PQNode<T,X,Y>* PQTree<T,X,Y>::createNodeAndCopyFullChildren(
								List<PQNode<T,X,Y>*> *fullNodes)
{
	PQNode<T,X,Y>  *newNode    = 0;
	PQNode<T,X,Y>  *oldSon     = 0;
	PQNode<T,X,Y>  *firstSon   = 0;
	PQNode<T,X,Y>  *checkSon   = 0;

	PQInternalNode<T,X,Y> *newPQnode = 0;

	if (fullNodes->size() == 1)
	{
		/*
		There is just one full child. So no new $P$-node is created. The
		full child is copied as child to the [[partialChild]].
		*/
		newNode = fullNodes->popFrontRet();
		removeChildFromSiblings(newNode);
	}
   
	else
	{
		/*
		This chunk belongs to the function [[createNodeAndCopyFullChildren]].
		There is more than one full child, so a new $P$-node is created.
		This chunk first allocates the memory for the new $P$-node that will
		be stored in [[newNode]]. Then it pops the nodes out of the stack 
		[[fullNodes]] and introduces them as sons of [[newNode]].
		Popping the nodes out of the stack implies at the same time
		that they are removed from the [[fullChildren]] stack of the
		$P$-node of their parent.
		*/
		newPQnode = OGDF_NEW PQInternalNode<T,X,Y>(m_identificationNumber++,P_NODE,FULL);
		newNode = newPQnode;
		m_pertinentNodes->pushFront(newNode);
		newNode->m_pertChildCount = fullNodes->size();
		newNode->m_childCount = fullNodes->size();

		/*
		The first node is copied separately, since we need the pointer to it 
		for setting the pointers to the siblings of the next full nodes.
		*/
		firstSon = fullNodes->popFrontRet();
		removeChildFromSiblings(firstSon);
		newNode->fullChildren->pushFront(firstSon);
		firstSon->m_parent = newNode;
		firstSon->m_parentType = newNode->type();
		oldSon = firstSon;


		/*
		All remaining nodes that are stored in the stack [[fullNodes]] are
		introduced as children of the new $P$-node [[newNode]]. Observe
		that the children of a $P$-node must form a doubly linked list.
		Hence the last node and the [[firstSon]] must be linked via their 
		siblings pointers.
		*/
		while (!fullNodes->empty())
		{
			checkSon = fullNodes->popFrontRet();
			removeChildFromSiblings(checkSon);
			newNode->fullChildren->pushFront(checkSon);
			oldSon->m_sibRight = checkSon;
			checkSon->m_sibLeft = oldSon;
			checkSon->m_parent = newNode;
			checkSon->m_parentType = newNode->type();
			oldSon = checkSon;
		}
		firstSon->m_sibLeft = oldSon;
		oldSon->m_sibRight = firstSon;
		newNode->m_referenceChild = firstSon;
		firstSon->m_referenceParent = newNode;

	}
   
	return newNode;
}


/************************************************************************
                          destroyNode
************************************************************************/ 

/**
 * The function destroyNode() marks a node as TO_BE_DELETED. This
 * enables the function emptyAllPertinentNodes()
 * to remove the node and free its memory.
 */

template<class T,class X,class Y>
void PQTree<T,X,Y>::destroyNode(PQNode<T,X,Y> *nodePtr)
{

	nodePtr->status(TO_BE_DELETED);
}


/************************************************************************
                         Destructor
************************************************************************/ 

/**
 * The function shown here is the destructor of the class template PQTree.
 * In order to free allocated memory, all nodes of the
 * tree have to be deleted, hence their destructors have to be called.
 * This is done in the function Cleanup().
 * Furthermore all other initialized memory has to be freed which is
 * done as well in the function Cleanup().
 */

template<class T,class X,class Y> PQTree<T,X,Y>::~PQTree()
{
	Cleanup();
}


/************************************************************************
                 emptyAllPertinetNodes
************************************************************************/ 

/**
 * The function emptyAllPertinentNodes() has to be called after a reduction
 * has been processed. In cleans up all flags that have been set in the
 * pertinent nodes during the reduction process. All pertinent nodes have been
 * stored in the private member stack \a m_pertinentNodes of the class template
 * PQTree during the Bubble()-phase
 * or when processing one of the templates (see templateL1() to templateQ3()).
 */

template<class T,class X,class Y>
void PQTree<T,X,Y>::emptyAllPertinentNodes()
{
	PQNode<T,X,Y>   *nodePtr;


	while(!m_pertinentNodes->empty())
	{
		nodePtr = m_pertinentNodes->popFrontRet();
		switch (nodePtr->status())
		{
			case TO_BE_DELETED:
				if (nodePtr == m_root)
					m_root = 0;
                CleanNode(nodePtr);
                if (nodePtr)
					delete nodePtr;
                break;

			case FULL:
				emptyNode(nodePtr);
                break;

			case PARTIAL:
				emptyNode(nodePtr);
                break;

			default:
				clientDefinedEmptyNode(nodePtr);
                break;
		}
	}
	m_pseudoRoot->m_pertChildCount = 0;
	m_pseudoRoot->m_pertLeafCount = 0;
	m_pseudoRoot->fullChildren->clear();
	m_pseudoRoot->partialChildren->clear();
	m_pseudoRoot->status(EMPTY);
	m_pseudoRoot->mark(UNMARKED);
}


/************************************************************************
                        emptyNode
************************************************************************/ 

/**
 * The funtion emptyNode() cleans up all stacks, flags and pointers of a
 * pertinent node that has been visited during the reduction process.
 */

template<class T,class X,class Y>
void PQTree<T,X,Y>::emptyNode(PQNode<T,X,Y> *nodePtr)
{
	nodePtr->status(EMPTY);
	nodePtr->m_pertChildCount = 0;
	nodePtr->m_pertLeafCount = 0;
	nodePtr->fullChildren->clear();
	nodePtr->partialChildren->clear();
	nodePtr->mark(UNMARKED);
}


/************************************************************************
                     exchangeNodes
************************************************************************/ 

/**
 * The function exchangeNodes() replaces the \a oldNode by the \a newNode
 * in the tree. This is a function used very often in the template matchings,
 * normally in combination with the construction of a new node which has to
 * conquer the place of an existing node in the tree.
 *
 * This function can be used in all cases, so the parent of \a oldNode
 * is allowed to be either a Q-node or a P-node and \a oldNode may be
 * any child of its parent.
 *
 * The client should observe, that this function does \b not reset
 * the pointer \a m_root. If necessary, this has to be done explicitly by
 * the client himself.
 */

template<class T,class X,class Y>
void PQTree<T,X,Y>::exchangeNodes(PQNode<T,X,Y> *oldNode,
                              PQNode<T,X,Y> *newNode)
{
	if (oldNode->m_referenceParent != 0)
	{
		/*
		The node [[oldNode]] is connected to its parent
		via the reference pointer of the doubly linked list. The [[newNode]]
		will be the new reference child and is linked via the reference
		pointers to the $P$-node.
		*/
		oldNode->m_referenceParent->m_referenceChild = newNode;
		newNode->m_referenceParent = oldNode->m_referenceParent;
		oldNode->m_referenceParent = 0;
	}

	else if (oldNode->endmostChild())
	{
		/*
		The [[oldNode]] is endmost child of a Q-node. So its parent 
		contains an extra pointer to [[oldNode]]. Link the parent of 
		[[oldNode]] to [[newNode]] via this pointer and make [[newNode]] 
		endmost child of its new parent.
		*/
		if (oldNode->m_parent->m_leftEndmost == oldNode)
			oldNode->m_parent->m_leftEndmost = newNode;
		else if (oldNode->m_parent->m_rightEndmost == oldNode)
			oldNode->m_parent->m_rightEndmost = newNode;
	}

	if ((oldNode->m_sibLeft == oldNode) && (oldNode->m_sibRight == oldNode))
	{
		/*
		Two possible cases are occured.
		\begin{enumerate}
		\item [[oldNode]] is the only child of a $P$-node. In order to
		implement the doubly linked list of children of the $P$-node, the sibling
		pointers of [[newNode]] are set to [[newNode]].
		\item [[oldNode]] is the [[m_root]] of the $PQ$-tree. Since
		by our definition of the $PQ$-tree the sibling pointers of the
		[[m_root]] point to the root itself, (i.e. to make sure that 
		checking for the endmost child property is also valid for the root)
		the sibling pointers of [[newNode]] are set to [[newNode]] as well.
		\end{enumerate}
		*/
		oldNode->m_sibLeft = 0;
		oldNode->m_sibRight = 0;
		if (oldNode->m_parent == 0)
		{
			newNode->m_sibLeft = newNode;
			newNode->m_sibRight = newNode;
		}
		else
		{
			newNode->m_sibLeft = newNode;
			newNode->m_sibRight = newNode;
		}
	}
	else 
	{
		OGDF_ASSERT(!(oldNode->m_sibLeft == oldNode))
		//sibling pointers of old node are not compatible
	    OGDF_ASSERT(!(oldNode->m_sibRight == oldNode))
		//sibling pointers of old node are not compatible.
	}
	/*
	Manage the exchange of [[oldNode]] and [[newNode]] according to
	[[oldNode]]'s siblings. The chunk checks both siblings of
	[[oldNode]] and resets the sibling pointers of [[oldNode]]'s siblings
	as well as the sibling pointers of [[newNode]].
	*/
	if (oldNode->m_sibLeft != 0)
	{
		if (oldNode->m_sibLeft->m_sibRight == oldNode)
			oldNode->m_sibLeft->m_sibRight = newNode;
		else if (oldNode->m_sibLeft->m_sibLeft == oldNode)
			oldNode->m_sibLeft->m_sibLeft = newNode;
		else
		{
			//Sibling was not connected to child.
			OGDF_ASSERT(false)
		}
		newNode->m_sibLeft = oldNode->m_sibLeft;
		oldNode->m_sibLeft = 0;
	}

	if (oldNode->m_sibRight != 0)
	{
		if (oldNode->m_sibRight->m_sibLeft == oldNode)
			oldNode->m_sibRight->m_sibLeft = newNode;
		else if (oldNode->m_sibRight->m_sibRight == oldNode)
			oldNode->m_sibRight->m_sibRight = newNode;
		else
		{
			//Sibling was not connected to child.
			OGDF_ASSERT(false)
		}
		newNode->m_sibRight = oldNode->m_sibRight;
		oldNode->m_sibRight = 0;
	}
      
	newNode->m_parentType = oldNode->m_parentType;
	newNode->m_parent = oldNode->m_parent;
}


/************************************************************************
                        front
************************************************************************/ 

/**
 * The function front()
 * returns the keys stored in the leaves of the front of
 * \a nodePtr. A specified node \a nodePtr of the PQ-tree is
 * handed to the function and front() detects the leaves in the front
 * of this node returning the elements represented by the leaves. These
 * elements are 
 * stored in an array of keys named \a leafKeys. 
 * The return value is the numbers of leaves that have been detected.
 * Observe that front() uses \a leafKeys[0] to store the
 * first key.
 */

template<class T,class X,class Y>
void PQTree<T,X,Y>::front(PQNode<T,X,Y>* nodePtr,
						 SListPure<PQLeafKey<T,X,Y>*> &leafKeys)
{   
	PQNode<T,X,Y>*  checkNode = 0;

	Queue<PQNode<T,X,Y>*> helpqueue;
	helpqueue.append(nodePtr);
  
	while (!helpqueue.empty())
	{
		checkNode = helpqueue.pop();

		if (checkNode->type() == LEAF)
			leafKeys.pushBack(checkNode->getKey());      
		else
		{
			PQNode<T,X,Y>*  firstSon  = 0;
			PQNode<T,X,Y>*  nextSon   = 0;
			PQNode<T,X,Y>*  oldSib    = 0;
			PQNode<T,X,Y>*  holdSib   = 0;

			if (checkNode->type() == P_NODE)
			{  
				OGDF_ASSERT(checkNode->m_referenceChild) 
				firstSon = checkNode->m_referenceChild;
			}
 			else if (checkNode->type() == Q_NODE)
			{
				OGDF_ASSERT(checkNode->m_leftEndmost)
				firstSon = checkNode->m_leftEndmost;
			}				
			helpqueue.append(firstSon);
			nextSon = firstSon->getNextSib(oldSib);
			oldSib = firstSon;
			while (nextSon && nextSon != firstSon)
			{
				helpqueue.append(nextSon);
				holdSib = nextSon->getNextSib(oldSib);
				oldSib = nextSon;
				nextSon = holdSib;
			}
		}
	}
}




/*
\subsection{\tt fullChildrenList}\label{fullChildrenList}
The function [[fullChildrenList]] returns a pointer to the list of full 
children of a node. This allows all manipulations on the stack and
should be handled very carefully during the reduction of subsets $S
\subseteq U$.
*/

/************************************************************************
                       fullChildrenList
************************************************************************/ 
/*

template<class T,class X,class Y>
List<PQNode<T,X,Y>*>*  PQTree<T,X,Y>::fullChildrenList(
											PQNode<T,X,Y>* nodePtr)
{
	return nodePtr->fullChildren;
}

*/


/************************************************************************
                         Initialize
************************************************************************/ 

/**
 * The function Initialize() initializes the PQ-tree with a set of elements.
 * These elements have to be template classes of the class template PQLeafKey
 * and are handed to the function in an array \a leafKeys.
 * The function constructs the universal PQ-tree. If the
 * \a numberOfElements > 1, the universal PQ-tree consists of one P-node
 * as root (stored in \a m_root) and all leaves gathered underneath the P-node,
 * symbolizing all kinds of permutations. If \a numberOfElements = 1,
 * the universal PQ-tree consists of a single PQLeaf, being the root of
 * the tree. 
 *
 * Observe that the first element has to be stored in
 * \a leafKeys[0] and the last one in
 * \a leafKeys[\a numberOfElements-1].
 * The function Initialize() returns 1, if the initialization of
 * the PQ-tree was successful.
 */

template<class T,class X,class Y> 
int PQTree<T,X,Y>::Initialize(SListPure<PQLeafKey<T,X,Y>*> &leafKeys)
{
	PQInternalNode<T,X,Y>   *newNode   = 0;
	PQInternalNode<T,X,Y>   *newNode2  = 0;
	PQLeaf<T,X,Y>     *newLeaf   = 0;

	m_pertinentNodes = OGDF_NEW List<PQNode<T,X,Y>*>;


	if (!leafKeys.empty())
	{
		newNode2 = OGDF_NEW PQInternalNode<T,X,Y>(-1,Q_NODE,PARTIAL);
		m_pseudoRoot = newNode2;

		if ( leafKeys.begin() != leafKeys.end()) // at least two elements
		{
			newNode = OGDF_NEW PQInternalNode<T,X,Y>(m_identificationNumber++,P_NODE,EMPTY);
			m_root = newNode;
			m_root->m_sibLeft = m_root;
			m_root->m_sibRight = m_root;
			return addNewLeavesToTree(newNode,leafKeys);
		}
		else
		{
			newLeaf = OGDF_NEW PQLeaf<T,X,Y>(m_identificationNumber++,EMPTY,*leafKeys.begin());
			m_root = newLeaf;
			m_root->m_sibLeft = m_root;
			m_root->m_sibRight = m_root;
			return 1;
		}
	}    
	else
		return 0;
}


/************************************************************************
                       linkChildrenOfQnode
************************************************************************/ 

/**
 * The function linkChildrenOfQnode() links the two endmost children
 * of two \b different Q-nodes via their sibling pointers together.
 * The purpose of doing this is to combine the children of two Q-nodes
 * as children of only one Q-node. This function does not reset the
 * pointers to the endmost children of the Q-node. This has to be done
 * by the client of the function.
 */

template<class T,class X,class Y>
void PQTree<T,X,Y>::linkChildrenOfQnode(PQNode<T,X,Y> *installed, 
                                        PQNode<T,X,Y> *newChild)

{
	if ((installed != 0) && (newChild != 0))
	{
		if (installed->m_sibLeft == 0)
		{
			installed->m_sibLeft = newChild;
			if (newChild->m_sibRight == 0)
				newChild->m_sibRight = installed;
			else
				newChild->m_sibLeft = installed;
		}
		else if (installed->m_sibRight == 0)
		{
			installed->m_sibRight = newChild;
			if (newChild->m_sibLeft == 0)
				newChild->m_sibLeft = installed;
			else
				newChild->m_sibRight = installed;
		}
		else
		{
			//endmost child with 2 siblings encountered.
			OGDF_ASSERT(false)
		}
	}
}


/*
\subsection{\tt partialChildrenList}\label{partialChildrenList}
The function [[partialChildrenList]] returns a pointer to the list of 
partial children of a node. This allows all manipulations on the stack and
should be handled very carefully during the reduction of subsets $S
\subseteq U$.
*/
/************************************************************************
                      partialChildrenList
************************************************************************/ 
/*
template<class T,class X,class Y> 
List<PQNode<T,X,Y>*>*  PQTree<T,X,Y>::partialChildrenList(
											PQNode<T,X,Y>* nodePtr)

{
	return nodePtr->partialChildren;
}
*/


/************************************************************************
                           printNode
************************************************************************/ 

/**
 * The function printNode() prints out a node of the PQ-node in a special
 * format that can be read by the <em>Tree Interface</em>.
 * The function will only be called when an outline of
 * the tree has to be given for debuging purposes.
 */

template<class T,class X,class Y> 
void PQTree<T,X,Y>::printNode(char *filename, int number,
                              PQNode<T,X,Y>* parent, PQNode<T,X,Y>* son)
{
	ofstream output(filename,ios::app);


	output << "e " << parent->m_debugTreeNumber
           << ' ' <<number << endl;

	output << "n " << number << ' ' << "\\i"
           << (son->type() == Q_NODE ? " Q-NODE" :
              (son->type() == P_NODE ? " P-NODE" : 
              (son->type() == LEAF   ? " LEAF"   :
              (clientPrintType(son) ) )))
           << " IdentificationNumber: " << son->m_identificationNumber
           << ", Status: "
           << (son->status() == FULL ? " FULL"           :
              (son->status() == EMPTY ? " EMPTY"         :
              (son->status() == PARTIAL ? " PARTIAL"     :
              (son->status() == PERTINENT ? " PERTINENT" : 
              (clientPrintStatus(son) ) ))))
           << (son->getNodeInfo() != 0 ?
               son->getNodeInfo()->print()   : " "  )
           << (son->getInternal() != 0 ?
               son->getInternal()->print()   : " " )
           << (son->getKey() != 0 ?
               son->getKey()->print()   : " " )
           << " Child Count: " << son->m_childCount
           << "\\i" << endl;

	 output << "c " << number << ' '
  	        << (son->type() == Q_NODE  ? Q_NODE_CAT     :
               (son->type() == P_NODE  ? P_NODE_CAT     :
               (son->status() == FULL  ? LEAF_FULL_CAT  : 
               (son->status() == EMPTY ? LEAF_CAT       :
               (clientPrintNodeCategorie(son) ) ))))
            << endl;
  
	son->m_debugTreeNumber = number;
}


/************************************************************************
                 printOutCurrentTree
************************************************************************/ 

/**
 * The function printOutCurrentTree() prints the current PQ-tree in a
 * special fileformat. The name of the file consists of prefix \a name and
 * a postfix \a ending. The fileformat can be read by the <em>Tree Interface</em>.
 */

template<class T,class X,class Y>
void PQTree<T,X,Y>::printOutCurrentTree(const char *name,const char *ending)
{
	char filename[INFOSIZE];

	ogdf::strcpy(filename,INFOSIZE,name);
	ogdf::strcat(filename,INFOSIZE,ending);

	ofstream output(filename,ios::out);
	output << "#TYPE: COMPLETE TREE" << endl
		   << "#TIME: NOT" << endl
           << "#BOUNDS: NONE" << endl
           << "#INFORMATION: STANDARD" << endl
           << "#NODE_NUMBER: NONE" << endl;

	PQNode<T,X,Y>*  checkNode = 0;
	PQNode<T,X,Y>*  firstSon  = 0;
	PQNode<T,X,Y>*  nextSon   = 0;
	PQNode<T,X,Y>*  lastSon   = 0;
	PQNode<T,X,Y>*  oldSib    = 0;
	PQNode<T,X,Y>*  holdSib   = 0;
   
	int   count = 1;
  
	Queue<PQNode<T,X,Y>*> *helpqueue = OGDF_NEW Queue<PQNode<T,X,Y>*>;
	m_root->m_debugTreeNumber = count;

	output << "n " << count << ' ' << "\\iROOT: "
           << (m_root->type() == Q_NODE ? " Q-NODE" :
              (m_root->type() == P_NODE ? " P-NODE" : 
              (m_root->type() == LEAF   ? " LEAF"   :
              (clientPrintType(m_root) ) )))
           << ", IdentificationNumber: " << m_root->m_identificationNumber
           << ", Status: "
           << (m_root->status() == FULL ? " FULL"           :
              (m_root->status() == EMPTY ? " EMPTY"         :
              (m_root->status() == PARTIAL ? " PARTIAL"     :
              (m_root->status() == PERTINENT ? " PERTINENT" : 
              (clientPrintStatus(m_root) ) ))))
           << (m_root->getNodeInfo() != 0 ?
               m_root->getNodeInfo()->print()   : " "  )
           << (m_root->type() == LEAF ? " "                   :
              (m_root->getInternal() != 0 ?
               m_root->getInternal()->print()   : " " ))
           << (m_root->type() != LEAF ? " "                   :
              (m_root->getKey() != 0 ?
               m_root->getKey()->print()   : " " ))
           << " Child Count: " << m_root->m_childCount
           << "\\i" << endl;

	output << "c " << count++ << ' '
           << (m_root->type() == Q_NODE  ? Q_NODE_CAT    : 
              (m_root->type() == P_NODE  ? P_NODE_CAT    :
              (m_root->status() == FULL  ? LEAF_FULL_CAT :
              (m_root->status() == EMPTY ? LEAF_CAT      :
              (clientPrintNodeCategorie(m_root) ) ))))
           << endl;
  
	helpqueue->append(m_root);
   
    while (!helpqueue->empty())
    {
		checkNode = helpqueue->pop();

		if (checkNode->type() == P_NODE)
		{  
			if (checkNode->m_referenceChild != 0) 
			{
				firstSon = checkNode->m_referenceChild;
				helpqueue->append(firstSon);
				printNode(filename,count++,checkNode,firstSon);
 
				if (firstSon->m_sibRight != 0)
				nextSon = firstSon->m_sibRight;
				while (nextSon != firstSon)
				{
					helpqueue->append(nextSon);
					printNode(filename,count++,checkNode,nextSon);
					nextSon = nextSon->m_sibRight;
				}
			}
		}
		else if (checkNode->type() == Q_NODE)
		{
			oldSib = 0;
			holdSib = 0;

			firstSon = checkNode->m_leftEndmost;
			helpqueue->append(firstSon);
			printNode(filename,count++,checkNode,firstSon);
      
			lastSon = checkNode->m_rightEndmost;
			if ( firstSon != lastSon)
			{
				helpqueue->append(lastSon);
				printNode(filename,count++,checkNode,lastSon);

				nextSon = lastSon->getNextSib(oldSib);
				oldSib = lastSon;
				while (nextSon != firstSon)
				{
					helpqueue->append(nextSon);
					printNode(filename,count++,checkNode,nextSon);
					holdSib = nextSon->getNextSib(oldSib);
					oldSib = nextSon;
					nextSon = holdSib;
				}
			}
		}
	}
	delete helpqueue;
}


/************************************************************************
                 writeGML
************************************************************************/ 

/**
 * The function writeGML() prints the PQ-tree in the GML
 * fileformat. The filename is ended by a ".gml" and can be read 
 * eg. by the <em>AGD</em>.
 */

template<class T,class X,class Y>
void PQTree<T,X,Y>::writeGML(const char *fileName)
{
	ofstream os(fileName);
	writeGML(os);
}

template<class T,class X,class Y>
void PQTree<T,X,Y>::writeGML(ostream &os)
{
	Array<int> id(0,m_identificationNumber,0);
	int nextId = 0;

	SListPure<PQNode<T,X,Y>*> helpQueue;
	SListPure<PQNode<T,X,Y>*> secondTrace;


	os.setf(ios::showpoint);
	os.precision(10);

	os << "Creator \"ogdf::PQTree::writeGML\"\n";
	os << "directed 1\n";

	os << "graph [\n";


	PQNode<T,X,Y>*  checkNode = m_root;
	PQNode<T,X,Y>*  firstSon  = 0;
	PQNode<T,X,Y>*  nextSon   = 0;
	PQNode<T,X,Y>*  lastSon   = 0;
	PQNode<T,X,Y>*  oldSib    = 0;
	PQNode<T,X,Y>*  holdSib   = 0;	


	if (checkNode->type() != LEAF)
		secondTrace.pushBack(checkNode);


	while (checkNode)
	{

		os << "node [\n";

		os << "id " << (id[checkNode->m_identificationNumber] = nextId++) << "\n";

		os << "label \"" << checkNode->m_identificationNumber;
		if (checkNode->getKey() != 0)
			os << checkNode->getKey()->print();
		os << "\"\n";

//			os << "graphics [\n";
//			os << "w " << m_width[v] << "\n";
//			os << "h " << m_height[v] << "\n";
//			os << "type \"rectangle\"\n";
//			os << "width 1.0\n";
//			os << "]\n"; // graphics

			os << "graphics [\n";
			if (m_root->status() == FULL)
			{
				if (checkNode->type() == P_NODE)
					os << "fill \"#FF0000\"\n";
				else if (checkNode->type() == Q_NODE)
					os << "fill \"#0000A0\"\n";
				else if (checkNode->type() == LEAF)
					os << "fill \"#FFFFE6\"\n";
			}
			else if (m_root->status() == EMPTY)
			{
				if (checkNode->type() == P_NODE)
					os << "fill \"#FF0000\"\n";
				else if (checkNode->type() == Q_NODE)
					os << "fill \"#0000A0\"\n";
				else if (checkNode->type() == LEAF)
					os << "fill \"#00FF00\"\n";
			}
			else if (m_root->status() == PARTIAL)
			{
				if (checkNode->type() == P_NODE)
					os << "fill \"#FF0000\"\n";
				else if (checkNode->type() == Q_NODE)
					os << "fill \"#0000A0\"\n";
				else if (checkNode->type() == LEAF)
					os << "fill \"#FFFFE6\"\n";
			}
			else if (m_root->status() == PERTINENT)
			{
				if (checkNode->type() == P_NODE)
					os << "fill \"#FF0000\"\n";
				else if (checkNode->type() == Q_NODE)
					os << "fill \"#0000A0\"\n";
				else if (checkNode->type() == LEAF)
					os << "fill \"#FFFFE6\"\n";
			}


			os << "]\n"; // graphics
		os << "]\n"; // node



		if (checkNode->type() == P_NODE)
		{  
			if (checkNode->m_referenceChild != 0) 
			{
				firstSon = checkNode->m_referenceChild;
				helpQueue.pushBack(firstSon);
	
				if (firstSon->m_sibRight != 0)
				nextSon = firstSon->m_sibRight;
				while (nextSon != firstSon)
				{
					helpQueue.pushBack(nextSon);
					nextSon = nextSon->m_sibRight;
				}
			}
		}
		else if (checkNode->type() == Q_NODE)
		{
			oldSib = 0;
			holdSib = 0;

			firstSon = checkNode->m_leftEndmost;
			helpQueue.pushBack(firstSon);
      
			lastSon = checkNode->m_rightEndmost;
			if ( firstSon != lastSon)
			{
				helpQueue.pushBack(lastSon);
				nextSon = lastSon->getNextSib(oldSib);
				oldSib = lastSon;
				while (nextSon != firstSon)
				{
					helpQueue.pushBack(nextSon);
					holdSib = nextSon->getNextSib(oldSib);
					oldSib = nextSon;
					nextSon = holdSib;
				}
			}
		}
		if (!helpQueue.empty())
		{
			checkNode = helpQueue.popFrontRet();
			if (checkNode->type() != LEAF)
				secondTrace.pushBack(checkNode);
		}
		else
			checkNode = 0;
	}


	SListIterator<PQNode<T,X,Y>*> it;

	for (it = secondTrace.begin(); it.valid(); it++)
	{
		checkNode = *it;
		if (checkNode->type() == P_NODE)
		{  
			if (checkNode->m_referenceChild != 0) 
			{
				firstSon = checkNode->m_referenceChild;
				os << "edge [\n";
				os << "source " << id[checkNode->m_identificationNumber] << "\n";
				os << "target " << id[firstSon->m_identificationNumber] << "\n";
				os << "]\n"; // edge
	
				if (firstSon->m_sibRight != 0)
				nextSon = firstSon->m_sibRight;
				while (nextSon != firstSon)
				{
					os << "edge [\n";
					os << "source " << id[checkNode->m_identificationNumber] << "\n";
					os << "target " << id[nextSon->m_identificationNumber] << "\n";
					os << "]\n"; // edge
					nextSon = nextSon->m_sibRight;
				}
			}
		}
		else if (checkNode->type() == Q_NODE)
		{
			oldSib = 0;
			holdSib = 0;

			firstSon = checkNode->m_leftEndmost;      
			lastSon = checkNode->m_rightEndmost;

			os << "edge [\n";
			os << "source " << id[checkNode->m_identificationNumber] << "\n";
			os << "target " << id[lastSon->m_identificationNumber] << "\n";
			os << "]\n"; // edge
			if ( firstSon != lastSon)
			{
				nextSon = lastSon->getNextSib(oldSib);
				os << "edge [\n";
				os << "source " << id[checkNode->m_identificationNumber] << "\n";
				os << "target " << id[nextSon->m_identificationNumber] << "\n";
				os << "]\n"; // edge

				oldSib = lastSon;
				while (nextSon != firstSon)
				{
					holdSib = nextSon->getNextSib(oldSib);
					oldSib = nextSon;
					nextSon = holdSib;
					os << "edge [\n";
					os << "source " << id[checkNode->m_identificationNumber] << "\n";
					os << "target " << id[nextSon->m_identificationNumber] << "\n";
					os << "]\n"; // edge

				}
			}
		}
	}
	os << "]\n"; // graph
}



/************************************************************************
                            Reduce
************************************************************************/ 

/**
 * The function Reduce() does the reduction of the pertinent leaves
 * with the help of the template matchings, designed by Booth and Lueker.
 * The reader should observe that this function can only be called after
 * every pertinent node in the pertinent subtree has gotten a valid parent
 * pointer. If this is not the case, the programm will be interrupted
 * by run-time errors such as seqmentation faults. The pertinent nodes
 * can get valid parent pointers by using the function Bubble().
 * If the function Bubble() returns 1, then it was succesful in
 * giving each pertinent node in the pertinent subtree a valid parent pointer.
 * If the function returns 0, then some nodes do not have a valid
 * parent pointer and the pertinent leaves are not reducable.
 *
 * The function Reduce() starts with the pertinent leaves and stores
 * them in
 * a queue \a processNodes. Every time a node is processed, its parent is
 * checked whether all its pertinent children are already processed.
 * If this is the case, the parent is allowed to be processed as well
 * and stored in the queue.
 *
 * Processing a node means that the function Reduce() tries to apply
 * one of the template matchings. In case that one template matching was
 * successful, the node was reduced and Reduce() tries to reduce the
 * next node.
 * In case that no template matching was successfully applied, the tree is
 * is irreducible. This causes the reduction process to be halted
 * returning 0.
 *
 * The folllowing variables are used by the function Reduce().
 *   - \a checkLeaf is a pointer to a various PQLeaf of the set of
 *     elements that has to be reduced.
 *   - \a checkNode is a pointer to a various node of the pertinent
 *     subtree.
 *   - \a pertLeafCount counts the number of pertinent leaves in the
 *     PQ-tree. Since Reduce() takes care that every node knows the
 *     number of pertinent leaves in its frontier, the root of the
 *     pertinent subtree can be identified with the help of \a pertLeafCount.
 *   - \a processNodes is a queue storing nodes of the pertinent
 *     subtree that are considered to be reduced next. A node may be
 *     reduced (and therefore is pushed on to \a processNodes) as soon as
 *     all its pertinent children have been reduced.
 */

template<class T,class X,class Y>
bool PQTree<T,X,Y>::Reduce(SListPure<PQLeafKey<T,X,Y>*> &leafKeys,int printYes)
{
	PQNode<T,X,Y>*          checkLeaf     = 0;
	PQNode<T,X,Y>*          checkNode     = 0;
	int                   pertLeafCount = 0;
	Queue<PQNode<T,X,Y>*>  processNodes;

	// Needed for Printing Service
	int                   count = 1;
	char                  cNum[8];

	/*
	In a first step the pertinent leaves have to be identified in the tree
	and are entered on to the queue [[processNodes]]. The identification of
	the leaves can be done with the help of a pointer stored in every 
	[[PQLeafKey]] in constant time for every element.   
	*/
	SListIterator<PQLeafKey<T,X,Y>*> it;
	for (it = leafKeys.begin(); it.valid(); ++it)
	{
		checkLeaf = (*it)->nodePointer(); //leafKeys[i]->nodePointer();
		checkLeaf->status(FULL);
		checkLeaf->m_pertLeafCount = 1;
		processNodes.append(checkLeaf);
		pertLeafCount++;
	}

	checkNode = processNodes.top();
	while ((checkNode != 0) && (processNodes.size() > 0))
	{
		checkNode = processNodes.pop();

		if (checkNode->m_pertLeafCount < pertLeafCount)
		{ 

			/*
			Application of the template matchings to a pointer [[checkNode]] 
			storing the adress of a node that is {\bf not the root} of the 
			pertinent subtree. Before applying the template matchings to 
			[[checkNode]], some values of the parent of [[checkNode]] are 
			updated. The number of the parents pertinent children stored in 
			[[pertChildCount]] is count down by one. In case that 
			[[checkNode->m_parent->m_pertChildCount == 0]], we know that all 
			pertinent children of the parent have been processed. Since the 
			parent then is allowed to be processed as well, 
			[[checkNode->m_parent]] is stored in the queue [[processNodes]].
			*/
			checkNode->m_parent->m_pertLeafCount = 
										checkNode->m_parent->m_pertLeafCount
	                                    + checkNode->m_pertLeafCount;

			checkNode->m_parent->m_pertChildCount--;
			if (!checkNode->m_parent->m_pertChildCount)
				processNodes.append(checkNode->m_parent);
			if (!templateL1(checkNode,0))
			if (!templateP1(checkNode,0))
			if (!templateP3(checkNode))
			if (!templateP5(checkNode))
			if (!templateQ1(checkNode,0))
			if (!templateQ2(checkNode,0))
				checkNode= 0;

			if (printYes)
			{
				ogdf::sprintf(cNum,8,"%d",count++);
				printOutCurrentTree("RED",cNum);
			}	
		}
		else
		{
			/*
			application of the template matchings to a pointer [[checkNode]] 
			that stores the adress of a node that {\bf is the root} of the 
			pertinent subtree. In a case that a template matching was 
			successfully applied, the pointer [[checkNode]] stores after the 
			application the adress of the root of pertinent subtree. This 
			includes nodes that have been newly introduced as root of the 
			perinent subtree during the application. If no template matching 
			was successfully applied [[checkNode]] is a [[0]] pointer.
			*/
			if (!templateL1(checkNode,1))
			if (!templateP1(checkNode,1))
			if (!templateP2(&checkNode))
			if (!templateP4(&checkNode))
			if (!templateP6(&checkNode))
			if (!templateQ1(checkNode,1))
			if (!templateQ2(checkNode,1))
			if (!templateQ3(checkNode))
				checkNode = 0;

			if (printYes)
			{
				ogdf::sprintf(cNum,8,"%d",count++);
				printOutCurrentTree("RED",cNum);
			}
		}
	}
	m_pertinentRoot = checkNode;

	if (m_pertinentRoot == 0)
		return false;
	else
		return true;
}



/************************************************************************
                         Reduction
************************************************************************/ 

/**
 * The function Reduction() tests whether permissible permutations of the
 * elements of U exist such that the elements of a subset S of U,
 * stored in \a leafKeys, form a consecutive sequence. If there exists
 * such a permutation, the PQ-tree is reduced and Reduction()
 * returns 1.
 *
 * The function Reduction() gets a list \aleafKeys
 * of pointers to elements of type PQLeafKey, 
 * representing all elements of S. 
 *
 * Reduction() calls the procedure Bubble() and if Bubble() was
 * successful, Reduction() calls the function Reduce().
 */

template<class T,class X,class Y> 
bool PQTree<T,X,Y>::Reduction(SListPure<PQLeafKey<T,X,Y>*> &leafKeys,
                             int redNumber)
{

#ifdef DEBUG
	bool success = Bubble(leafKeys,redNumber);
#else
	bool success = Bubble(leafKeys);
#endif

	if (!success)
		return success;
	else
		return Reduce(leafKeys,redNumber);

}


/************************************************************************
                          removeBlock
************************************************************************/ 

/**
 * This chunk contains the procedure removeBlock. It is used by the functions
 * templateQ2() and templateQ3(). The node \a nodePtr is expected to be a
 * Q-node with no, one or at most two partial children, such that
 * the partial and full children of \a nodePtr form a legal consecutive
 * sequence, hence can be reduced. 
 *
 * The function removeBlock() does the
 * following: Of every partial node that is found in the sequence of children of
 * \a nodePtr, all children are removed from that partial node and included
 * as children of \a nodePtr, occupying the place of the partial node in
 * the sequence of children of \a nodePtr. Thereby, removeBlock() takes
 * care, that the newly included full children of \a nodePtr form
 * a consecutive sequence with the already existing pertinent children of
 * \a nodePtr. The partial node itself is deleted afterwards.
 */

template<class T,class X,class Y>
void PQTree<T,X,Y>::removeBlock(PQNode<T,X,Y> *nodePtr,bool isRoot)

{
	/*
	For every
	partial child we keep a set of pointers. Since there are at most
	two partial children, we initialize two sets. Every set contains
	besides a pointer to the partial child four pointers to its endmost 
	children, sorted according to their full or empty labels and pointers 
	to the immediate siblings of the partial child, also sorted according
	to their full or empty labels.
	*/
	///Pointer to the first partial child
	PQNode<T,X,Y>  *partial_1        = 0;
	
	/*
	Pointer to the full endmost child (more
	precisely: to the endmost child appearing on the full side) of
	[[partial_1]]. In case that ignored nodes are used, this [[endfull_1]]
	may store the adress of an ignored node.
	*/
	PQNode<T,X,Y>  *endfull_1       = 0;
	
	/*
	Pointer to the empty endmost child  (more
	precisely: to the endmost child appearing on the empty side) of
	[[partial_1]]. In case that ignored nodes are used, this [[endempty_1]]
	may store the adress of an ignored node.
	*/
	PQNode<T,X,Y>  *endempty_1      = 0;
	
	/*
	Pointer to the first {\it non ignored} node 
	with full status. [[realfull_1]] is identical to [[endfull_1]] if no
	ignored nodes appear at the full end of the first partial child. 	
	*/
	PQNode<T,X,Y>  *realfull_1      = 0;
	
	/*
	Pointer to the first {\it non ignored} node 
	with empty status. [[realempty_1]] is identical to [[endempty_1]] if no
	ignored nodes appear at the empty end of the first partial child.  
	*/
	PQNode<T,X,Y>  *realempty_1     = 0;

	// Pointer to the second partial child.
	PQNode<T,X,Y>  *partial_2        = 0;
	
	/*
	Pointer to the full endmost child (more
	precisely: to the endmost child appearing on the full side) of
	[[partial_2]]. In case that ignored nodes are used, this [[endfull_2]]
	may store the adress of an ignored node.
	*/
	PQNode<T,X,Y>  *endfull_2       = 0;

	/*
	Pointer to the empty endmost child  (more
	precisely: to the endmost child appearing on the empty side) of
	[[partial_2]]. In case that ignored nodes are used, this [[endempty_2]]
	may store the adress of an ignored node.
	*/
	PQNode<T,X,Y>  *endempty_2      = 0;
	
	/*
	Pointer to the first {\it non ignored} node 
	with full status. [[realfull_2]] is identical to [[endfull_2]] if no
	ignored nodes appear at the full end of the first partial child. 
	*/
	PQNode<T,X,Y>  *realfull_2      = 0;
	
	/*
	Pointer to the first {\it non ignored} node 
	with empty status. [[realempty_2]] is identical to [[endempty_2]] if no
	ignored nodes appear at the empty end of the first partial child.  
	*/
	PQNode<T,X,Y>  *realempty_2     = 0;
      
	/*
	Pointer to a full sibling of
	[[partial_1]], if it exists. In case that ignored nodes are used 
	this [[sibfull_1]] stores the direct sibling of [[partial_1]]
	on the side where the full siblings are. Hence [[sibfull_1]] may
	store an ignored node.
	*/
	PQNode<T,X,Y>  *sibfull_1       = 0; 
	
	/*
	Pointer to a partial sibling of
	[[partial_1]], if it exists. In case that ignored nodes are used 
	this [[sibpartial_1]] stores the direct sibling of [[partial_1]]
	on the side where a partial sibling is. Hence [[sibpartial_1]] may
	store an ignored node.
	*/
	PQNode<T,X,Y>  *sibpartial_1    = 0;
	
	/*
	Pointer to an empty sibling of
	[[parial_1]], if it exists. In case that ignored nodes are used 
	this [[sibempty_1]] stores the direct sibling of [[partial_1]]
	on the side where the empty siblings are. Hence [[sibempty_1]] may
	store an ignored node.
	*/
	PQNode<T,X,Y>  *sibempty_1      = 0;  
	
	/*
	Pointer only used in case that [[partial_1]] has
	no non-ignored siblings on one side. [[partial_1]] then is endmost child
	of [[nodePtr]], but ignored children may appear between [[partial_1]]
	and the end of sequence of children of [[nodePtr]]. The
	[[nonstatussib_1]] then stores the adress of the endmost ignored child.
	Observe that it is not valid for a $Q$-node to have only one
	non-ignored child and several ignored children. Hence this situation
	is only allowed to appear {\bf once} on one side of [[partial_1]]. 
	Every other situation results in an error message.	
	*/
	PQNode<T,X,Y>  *nonstatussib_1 = 0;

	/*
	Pointer to a full sibling of
	[[partial_2]], if it exists. In case that ignored nodes are used 
	this [[sibfull_2]] stores the direct sibling of [[partial_2]]
	on the side where the full siblings are. Hence [[sibfull_2]] may
	store an ignored node.
	*/
	PQNode<T,X,Y>  *sibfull_2       = 0;   
	
	/*
	Pointer to a partial sibling of
	[[partial_2]], if it exists. In case that ignored nodes are used 
	this [[sibpartial_2]] stores the direct sibling of [[partial_2]]
	on the side where a partial sibling is. Hence [[sibpartial_2]] may
	store an ignored node.
	*/
	PQNode<T,X,Y>  *sibpartial_2    = 0;
	
	/*
	Pointer to an empty sibling of
	[[parial_2]], if it exists. In case that ignored nodes are used 
	this [[sibempty_2]] stores the direct sibling of [[partial_2]]
	on the side where the empty siblings are. Hence [[sibempty_2]] may
	store an ignored node.
	*/
	PQNode<T,X,Y>  *sibempty_2      = 0;  
	
	/*
	Pointer	only used in case that [[partial_2]] has
	no non-ignored siblings on one side. [[partial_2]] then is endmost child
	of [[nodePtr]], but ignored children may appear between [[partial_2]]
	and the end of sequence of children of [[nodePtr]]. The
	[[nonstatussib_2]] then stores the adress of the endmost ignored child.
	Observe that it is not valid for a $Q$-node to have only one
	non-ignored child and several ignored children. Hence this situation
	is only allowed to appear {\bf once} on one side of [[partial_2]]. 
	Every other situation results in an error message.
	*/
	PQNode<T,X,Y>  *nonstatussib_2 = 0;

	PQNode<T,X,Y>  *helpptr          = 0;

	PQNode<T,X,Y>  *checkVarLeft    = 0; 

	PQNode<T,X,Y>  *checkVarRight   = 0;

	/*
	[[endmostcheck]] is [[1]], if [[partial_1]] is the endmost
	child of [[nodePtr]]. Per default, [[endmostcheck]] is [[0]].
	*/
	int          endmostcheck     = 0;      
   
   
	nodePtr->status(PARTIAL);
	if (!isRoot)
		nodePtr->m_parent->partialChildren->pushFront(nodePtr);
      
	if (!nodePtr->partialChildren->empty())
 
												// Get a partial child.

	{
		partial_1 = nodePtr->partialChildren->popFrontRet();

												// Get the full and empty 
												// endmost children of the 
												// partial child [[partial_1]].

		checkVarLeft = clientLeftEndmost(partial_1);
		checkVarRight = clientRightEndmost(partial_1);
		if (checkVarLeft->status() == FULL)
		{
			 endfull_1 = partial_1->m_leftEndmost;
			realfull_1 = checkVarLeft;
		}
		else if (checkVarRight->status() == FULL)
		{
			endfull_1 = partial_1->m_rightEndmost;
			realfull_1 = checkVarRight;
		}
		else
		{
			//partial child with no full endmost child detected.
			OGDF_ASSERT(false)
		}
		if (checkVarLeft->status() == EMPTY)
		{
			endempty_1 = partial_1->m_leftEndmost;
			realempty_1 = checkVarLeft;
		}
		else if (checkVarRight->status() == EMPTY)
		{
			endempty_1 = partial_1->m_rightEndmost;
			realempty_1 = checkVarRight;
		}
		else
		{
			OGDF_ASSERT(false)
			//partial child with no empty endmost child detected.
		}

												// Get the immediate 
												// siblings of the partial 
												// child [[partial_1]].

		if (clientSibLeft(partial_1) != 0)
		{
			if (clientSibLeft(partial_1)->status() == FULL)
				sibfull_1 = partial_1->m_sibLeft;
			else if (clientSibLeft(partial_1)->status() == EMPTY)
				sibempty_1 = partial_1->m_sibLeft;
			else if (clientSibLeft(partial_1)->status() == PARTIAL)
				sibpartial_1 = partial_1->m_sibLeft;
		}
		else 
			nonstatussib_1 = partial_1->m_sibLeft;

		if (clientSibRight(partial_1) != 0)
		{
			if (clientSibRight(partial_1)->status() == FULL)
				sibfull_1 = partial_1->m_sibRight;
			else if (clientSibRight(partial_1)->status() == EMPTY)
				sibempty_1 = partial_1->m_sibRight;
			else if (clientSibRight(partial_1)->status() == PARTIAL)
				sibpartial_1 = partial_1->m_sibRight;
		}
		else if (nonstatussib_1 == 0)
			nonstatussib_1 = partial_1->m_sibRight;
		else 
		{
			//partial child detected with no siblings of valid status.
			OGDF_ASSERT(false)
		}
	}



	if (!nodePtr->partialChildren->empty())
                                       
											// There is a second partial child.

	{
		partial_2 = nodePtr->partialChildren->popFrontRet();

											// Get the full and empty endmost 
											// children of the partial 
											// child [[partial_2]].
		
		checkVarLeft = clientLeftEndmost(partial_2);
		checkVarRight = clientRightEndmost(partial_2);
		if (checkVarLeft->status() == FULL)
		{
			endfull_2 = partial_2->m_leftEndmost;
			realfull_2 = checkVarLeft;
		}
		else if (checkVarRight->status() == FULL)
		{
			endfull_2 = partial_2->m_rightEndmost;
			realfull_2 = checkVarRight;
		}
		else
		{
			//partial child with no full endmost child detected.
			OGDF_ASSERT(false)
		}

		if (checkVarLeft->status() == EMPTY)
		{
			endempty_2 = partial_2->m_leftEndmost;
			realempty_2 = checkVarLeft;
		}
		else if (checkVarRight->status() == EMPTY)
		{
			endempty_2 = partial_2->m_rightEndmost;
			realempty_2 = checkVarRight;
		}
		else
		{
			//partial child with no empty endmost child detected.
			OGDF_ASSERT(false)
		}

												// Get the immediate siblings 
												// of the partial child 
												// [[partial_2]].

		if (clientSibLeft(partial_2) != 0)
		{
			if (clientSibLeft(partial_2)->status() == FULL)
				sibfull_2 = partial_2->m_sibLeft;
			else if (clientSibLeft(partial_2)->status() == EMPTY)
				sibempty_2 = partial_2->m_sibLeft;
			else if (clientSibLeft(partial_2)->status() == PARTIAL)
				sibpartial_2 = partial_2->m_sibLeft;
		}
		else 
			nonstatussib_2 = partial_2->m_sibLeft;


		if (clientSibRight(partial_2) != 0)
		{
			if (clientSibRight(partial_2)->status() == FULL)
				sibfull_2 = partial_2->m_sibRight;
			else if (clientSibRight(partial_2)->status() == EMPTY)
				sibempty_2 = partial_2->m_sibRight;
			else if (clientSibRight(partial_2)->status() == PARTIAL)
				sibpartial_2 = partial_2->m_sibRight;
		}
		else if (nonstatussib_2 == 0)
			nonstatussib_2 = partial_2->m_sibRight;
		else 
		{
			//partial child detected with no siblings of valis status.
			OGDF_ASSERT(false);
		}
	}



	if (partial_1 != 0 && partial_2 != 0)
	
	/*
	Connect the endmost
	children of the partial children [[partial_1]] and [[partial_2]] correctly
	with their new siblings. In doing this, all children of the partial
	children become children of [[nodePtr]]. The reader should observe that
	the parent pointers of the interior children of [[partial_1]] and 
	[[partial_2]] are not updated in order to hit the linear time complexity.

	When including the children of the partial children to the children
	of [[nodePtr]], it is taken care that all full children
	form a consecutive sequence afterwards. If neccessary the pointers to the
	endmost children of [[nodePtr]] are updated.
	*/
	{
		if (sibfull_1 != 0 && sibfull_2 != 0)
                                          // There are full children  between
                                          // the 2 partial nodes.
                                          // Connect the full children
                                          // between the 2 partial children
                                          // with the full endmost children
                                          // of the 2 partial nodes.
		{
			sibfull_1->changeSiblings(partial_1,endfull_1);
			endfull_1->putSibling(sibfull_1);
			sibfull_2->changeSiblings(partial_2,endfull_2);
			endfull_2->putSibling(sibfull_2);
		}

		else if (sibpartial_1 != 0 && sibpartial_2 != 0)
                                          // There are no full children between
                                          // the 2 partial nodes. Connect the
                                          // full endmost children of the
                                          // partial nodes as siblings.
		{
			if (partial_1 == sibpartial_2 && partial_2 == sibpartial_1)
                                          // Regular Case.
			{
				endfull_1->putSibling(endfull_2);
				endfull_2->putSibling(endfull_1);
			}
                                          // Only ignored children between 
                                          // partial_1 and partial_2. 
			else
			{
				endfull_1->putSibling(sibpartial_1);
				sibpartial_1->changeSiblings(partial_1,endfull_1);
				endfull_2->putSibling(sibpartial_2);
				sibpartial_2->changeSiblings(partial_2,endfull_2);
			}
         
		}
                                            // Include the children of the 
                                            // partial children with their
                                            // full nodes inbetween into
                                            // the sequence of the children of
                                            // Q-node nodePtr.
		if (sibempty_1 == 0)
                                            // partial_1 is endmost child of
                                            // nodePtr. Make the empty endmost
                                            // child of partial_1 be the new 
											// endmost child of nodePtr.
                                            
		{
			if (nonstatussib_1 == 0)
                                            // Regular case.
			{
				nodePtr->changeEndmost(partial_1,endempty_1);
			}
			else
                                          // Only ignored children between 
                                          // partial_1 and one end of nodePtr. 
			{
				nonstatussib_1->changeSiblings(partial_1,endempty_1);
				endempty_1->putSibling(nonstatussib_1);
			}
			endempty_1->m_parent = nodePtr;
			realempty_1->m_parent = nodePtr;
		}
                                     
		else
                                            // partial_1 is not endmost child.
		{
			 sibempty_1->changeSiblings(partial_1,endempty_1);
			endempty_1->putSibling(sibempty_1);
		}


		if (sibempty_2 == 0)
                                            // partial_2 is endmost child of
                                            // nodePtr. Make the empty endmost
                                            // child of partial_2 be the new 
                                            // endmost child of nodePtr.
		{
			if (nonstatussib_2 == 0)
                                            // Regular case.
			{
				nodePtr->changeEndmost(partial_2,endempty_2);
			}
			else
                                            // Only ignored children between 
                                            // partial_1 and one end of 
											// nodePtr. 
			{
				nonstatussib_2->changeSiblings(partial_2,endempty_2);
				endempty_2->putSibling(nonstatussib_2);
			}
			endempty_2->m_parent = nodePtr;
			realempty_2->m_parent = nodePtr;
		}
                                     
		else
                                            // partial_2 is not endmost child.
		{
			sibempty_2->changeSiblings(partial_2,endempty_2);
			endempty_2->putSibling(sibempty_2);
		}


                                            // Copy the full children of 
                                            // partial_1 and partial_2 to 
											// nodePtr.
		while (!partial_2->fullChildren->empty())
		{
			helpptr = partial_2->fullChildren->popFrontRet();
			nodePtr->fullChildren->pushFront(helpptr);
		}
		nodePtr->m_childCount = nodePtr->m_childCount +partial_2->m_childCount - 1;
                                    
		destroyNode(partial_2);

		while (!partial_1->fullChildren->empty())
		{
			helpptr = partial_1->fullChildren->popFrontRet();
			nodePtr->fullChildren->pushFront(helpptr);
		}
		nodePtr->m_childCount = nodePtr->m_childCount +partial_1->m_childCount - 1;

		destroyNode(partial_1);
	}

	
	else if (partial_1 != 0)

	/*
	Connect the endmost
	children of the partial child [[partial_1]] correctly
	with their new siblings. In doing this, all children of the partial
	child become children of [[nodePtr]]. The reader should observe that
	the parent pointers of the interior children of [[partial_1]] 
	are not updated in order to hit the linear time complexity.

	When including the children of [[partial_1]] to the children
	of [[nodePtr]], it is taken care that all full children
	form a consecutive sequence afterwards. If necessary the pointers to the
	endmost children of [[nodePtr]] are updated.	
	*/
	
	{
		if ((clientLeftEndmost(nodePtr) == partial_1) ||
			(clientRightEndmost(nodePtr) == partial_1))
                                            // partial_1 is endmost child.
			endmostcheck = 1;
               
		if (sibfull_1 != 0)
                                            // There are full children on one
                                            // side of the partial node.
                                            // Connect the full children with
                                            // the full endmost child of 
											// partial_1.
		{
			sibfull_1->changeSiblings(partial_1,endfull_1);
			endfull_1->putSibling(sibfull_1);
		}

		else if (!endmostcheck)
                                            // There are not any full children
                                            // and partial_1 is not endmost.
                                            // So get the 2nd empty sibling
                                            // of partial_1 and connect it
                                            // to the full endmost child
                                            // of partial_1.
		{
			if (partial_1->m_sibLeft != sibempty_1)
				sibempty_2 = partial_1->m_sibLeft;
			else
				sibempty_2 = partial_1->m_sibRight;

			sibempty_2->changeSiblings(partial_1,endfull_1);
			endfull_1->putSibling(sibempty_2);
		}

		else
                                            // partial_1 is endmost child
                                            // and there are no full children.
                                            // Make the full endmost child of 
                                            // partial_1 be the endmostchild
                                            // of nodePtr.
		{

			if (nonstatussib_1 == 0)
                                            // Regular case.
			{
				nodePtr->changeEndmost(partial_1,endfull_1);
			}
			else
                                            // Only ignored children between 
                                            // partial_1 and one end of 
											// nodePtr. 
			{
				nonstatussib_1->changeSiblings(partial_1,endfull_1);
				endfull_1->putSibling(nonstatussib_1);
			}
			endfull_1->m_parent = nodePtr;
			realfull_1->m_parent = nodePtr;

		}

		if (sibempty_1 == 0)
                                            // There are no empty children.
                                            // partial_1 is endmost child of
                                            // nodePtr. Make the empty endmost
                                            // child of partial_1 be the new 
                                            // endmost child of nodePtr.
		{
			if (nonstatussib_1 == 0)
                                            // Regular case.
			{
				nodePtr->changeEndmost(partial_1,endempty_1);
			}
			else
                                            // Only ignored children between 
                                            // partial_1 and one end of 
											// nodePtr. 
			{
				nonstatussib_1->changeSiblings(partial_1,endempty_1);
				endempty_1->putSibling(nonstatussib_1);
			}
			endempty_1->m_parent = nodePtr;
			realempty_1->m_parent = nodePtr;
		}
                                     
		else
                                            // There are empty children. So 
                                            // connect the empty endmost child
                                            // of partial_1 with sibempty_1.
		{
			sibempty_1->changeSiblings(partial_1,endempty_1);
			endempty_1->putSibling(sibempty_1);
		}

		while (!partial_1->fullChildren->empty())
		{
			helpptr = partial_1->fullChildren->popFrontRet();
			nodePtr->fullChildren->pushFront(helpptr);
		}

		nodePtr->m_childCount = nodePtr->m_childCount + partial_1->m_childCount - 1;
		destroyNode(partial_1);

	}
	// else nodePtr does not have partial children. Then nothing is to do. 
}


/************************************************************************
                     removeChildFromSiblings
************************************************************************/ 

/**
 * The function removeChildFromSiblings() removes the node \a nodePtr from
 * the doubly linked list of its parent. In case that \a nodePtr is endmost
 * child of an Q-node or child of a P-node equiped with a valid
 * reference pointer \a referenceParent to its parent (see PQNode),
 * these pointers are considered as
 * well and the adjacent siblings of \a nodePtr have to cover
 * \a nodePtr's task.
 */

template<class T,class X,class Y>
void PQTree<T,X,Y>::removeChildFromSiblings(PQNode<T,X,Y>* nodePtr)
{
	PQNode<T,X,Y>* sibling = 0;

	if (nodePtr->m_referenceParent != 0)
	{
		/*
		Checksif [[nodePtr]] is connected with its parent via the reference
		pointers (see \ref{PQNode}). If so, the next sibling of [[nodePtr]]
		will be the the new reference child.
		*/
		nodePtr->m_referenceParent->m_referenceChild = nodePtr->m_sibRight;
		nodePtr->m_sibRight->m_referenceParent = nodePtr->m_referenceParent;
		if (nodePtr->m_referenceParent->m_referenceChild == nodePtr)
			nodePtr->m_referenceParent->m_referenceChild = 0;
		nodePtr->m_referenceParent = 0;
	}
	else if (nodePtr->endmostChild())
	{  
		/*
		Check if [[nodePtr]] is the endmost child of a $Q$-node.
		If so, the next sibling of [[nodePtr]] will be the the new endmost
		child of the $Q$-node. Observe that the sibling then gets a valid
		pointer to its parent.
		*/
		sibling = nodePtr->getNextSib(0);
		if (nodePtr->m_parent->m_leftEndmost == nodePtr)
			nodePtr->m_parent->m_leftEndmost = sibling;
		else if (nodePtr->m_parent->m_rightEndmost == nodePtr)
			nodePtr->m_parent->m_rightEndmost = sibling;
		if (sibling != 0)
			sibling->m_parent = nodePtr->m_parent;
	}

	/*
	Remove [[nodePtr]] from its immediate siblings and links the
	siblings via the [[sibRight]] and [[sibLeft]] pointers.
	*/
	if ((nodePtr->m_sibRight != 0) && (nodePtr->m_sibRight != nodePtr))
	{
		if (nodePtr->m_sibRight->m_sibLeft == nodePtr)
			nodePtr->m_sibRight->m_sibLeft = nodePtr->m_sibLeft;
		else if (nodePtr->m_sibRight->m_sibRight == nodePtr)
			nodePtr->m_sibRight->m_sibRight = nodePtr->m_sibLeft;
		else
		{
			//Sibling was not connected to child.
			OGDF_ASSERT(false)
		}
	}
	if ((nodePtr->m_sibLeft != 0) && (nodePtr->m_sibLeft != nodePtr))
	{
		if (nodePtr->m_sibLeft->m_sibRight == nodePtr)
			nodePtr->m_sibLeft->m_sibRight = nodePtr->m_sibRight;
		else if (nodePtr->m_sibLeft->m_sibLeft == nodePtr)
			nodePtr->m_sibLeft->m_sibLeft = nodePtr->m_sibRight;
		else
		{
			//(Sibling was not connected to child.
			OGDF_ASSERT(false);
		}
	}

	nodePtr->m_sibRight = 0;
	nodePtr->m_sibLeft = 0;
}


/************************************************************************
                 removeNodeFromTree
************************************************************************/ 

/**
 * The function removeNodeFromTree() has to be handled with great care by
 * the user. This function is not used in any of the functions of
 * the class template PQTree and can only be accessed by inheritance.
 * 
 * Its objective is to remove a node \a child from the PQ-tree. To do so,
 * the \a parent of the node \a child has to be known by the user.
 * To indicate this, the parent has to be handed over by her.
 *
 * <b>This function does not check if \a parent is the parent node of
 * \a child</b>. This has to be guaranteed by the user. The reason for
 * this riscfull approach lies in the details of the powerful data structure
 * PQ-tree. In order to reach linear runtime, the internal children
 * of a Q-node normally do not have valid parent pointers. So forcing
 * this function to search the parent would cost in worst case
 * linear runtime for one call of the function removeNodeFromTree().
 * Its up to the user to do better.
 * 
 * Calling removeNodeFromTree() with a 0-pointer for \a parent,
 * will always terminate this function with an ERROR-message and returning
 * -1 as value.
 *
 * The return value is an integer value used to indicate how many children
 * the \a parent after the removal of \a child still has. The client should
 * observe that internal nodes in the PQ-tree which have just one or
 * no children at all do not make sense. However, the function
 * removeNodeFromTree() <b>does not check if \a parent has less than
 * two children after the removal of \< child</b>.
 * So in case that \a parent has less than two children, the user has to check
 * this by herself and remove the \a parent, probably using the function
 * checkIfOnlyChild().
 *
 * There are two reasons why the function removeNodeFromTree() does
 * not check if \a parent has  less than two children after the removal
 * of \a child:
 *   -# The user might keep the node in the tree in order to add new nodes
 *      as children to it.
 *   -# Again, the parent of \a parent might not be known to \a parent,
 *      hence removeNodeTree() would have to search, at the cost of linear time
 *      consumption, for the parent of \a parent first before removing
 *      \a parent from the tree.
 *
 * Observe that removeNodeFromTree() does not free the allocated
 * memory of \a child. This has to be done by the user \b after calling
 * removeNodeFromTree(). It also offers the opportunity to reuse
 * deleted nodes. Observe that the identification number of a node
 * \a m_identificationNumber (see PQNode) cannot be changed.
 */

template<class T,class X,class Y>
int PQTree<T,X,Y>::removeNodeFromTree(PQNode<T,X,Y>* parent,PQNode<T,X,Y>* child)
{
	if (parent !=0)
	{
		removeChildFromSiblings(child);
		parent->m_childCount--;
		if ((child->status() == FULL) || (child->status() == PARTIAL))
			parent->m_pertChildCount--;
		return parent->m_childCount;
	}
	else
	{
		//parent is invalid 0-pointer.
		return -1;
	}   
}


/************************************************************************
							root
************************************************************************/      

/**
 * The function root() returns a pointer of the root node of the PQTree.
 */

template<class T,class X,class Y>
PQNode<T,X,Y>* PQTree<T,X,Y>::root() const
{
	return m_root;
}


/************************************************************************
                       sortExceptions
************************************************************************/

/**
 * The function sortExceptions() is only called by the function frontExcept().
 * It sorts the exceptions before frontExcept()
 * scans the frontier.
 */

template<class T,class X,class Y>
void PQTree<T,X,Y>::sortExceptions(int Exceptions[],int arraySize)

{
   int  i = 0;
   int  changed = 1;
   int  item = 0;


   while (changed)
   {
      changed = 0;
      for (i = 0; i < (arraySize-1); i++)
      {
         if (Exceptions[i] > Exceptions[i+1])
         {
            item = Exceptions[i];
            Exceptions[i] = Exceptions[i+1];
            Exceptions[i+1] = item;
            changed = 1;
         }

      }
   }
}


/************************************************************************
                          templateL1
************************************************************************/ 

/**
 * The function templateL1() implements the template matching for
 * leaves.
 * The function requires as input any pointer to a node stored in
 * \a nodePtr. If the node stored in \a nodePtr is a PQLeaf,
 * templateL1() considers itself responsible for the node and will
 * apply the template matching for pertinent leaves to \a nodePtr.
 * If the flag \a isRoot is set to 1, it signalizes
 * templateL1() that \a nodePtr is the root of
 * the pertinent subtree. In any other case the flag has to be 0.
 *
 * If templateL1() was responsible for \a nodePtr and the
 * reduction was successful, the return value is 1. Otherwise
 * the return value is 0.
 *
 * The function updates the \a fullChildren stack of its parent, as long as
 * \a nodePtr  is not the root of the pertinent subtree.
 * Observe that \a nodePtr needs a valid pointer to its parent. This
 * can be achieved by using the function Bubble() or any other
 * appropriate, user defined function.
 */

template<class T,class X,class Y>
bool PQTree<T,X,Y>::templateL1(PQNode<T,X,Y> *nodePtr,
							   bool isRoot)
{
	if ((nodePtr->type() == LEAF) && (nodePtr->status() == FULL))
	{
		if (!isRoot)
			nodePtr->m_parent->fullChildren->pushFront(nodePtr);
		return true;
	}
	else
		return false;
}


/************************************************************************
                          templateP1
************************************************************************/ 

/**
 * The function templateP1() implements the template matching for
 * P-nodes with only full children.
 * The function requires as input any pointer to a node stored in
 * \a nodePtr. If the node stored in \a nodePtr is a P-node with
 * only full children,
 * templateP1() considers itself responsible for the node and will
 * apply the template matching for full P-nodes to \a nodePtr.
 * If the flag \a isRoot is set to 1, it signalizes
 * templateP1() that \a nodePtr is the root of
 * the pertinent subtree. In any other case the flag has to be 0.
 *
 * If templateP1() was responsible for \a nodePtr and the
 * reduction was successful, the return value is 1. Otherwise
 * the return value is 0.
 *
 * If the P-node is not the root of the pertinent subtree,
 * the \a fullChildren stack of the parent of \a nodePtr is updated.
 * If the P-node is the root of the pertinent subtree, nothing has to
 * be done. 
 */

template<class T,class X,class Y>
bool PQTree<T,X,Y>::templateP1(PQNode<T,X,Y> *nodePtr,
							   bool isRoot)
{
	if (nodePtr->type() != P_NODE ||
		nodePtr->fullChildren->size() != nodePtr->m_childCount)
		return false;
	else
	{
		nodePtr->status(FULL);
		if (!isRoot)
			nodePtr->m_parent->fullChildren->pushFront(nodePtr);

		return true;
	}
}


/************************************************************************
                          templateP2
************************************************************************/ 

/**
 * The function templateP2() implements the template matching for a
 * P-node with full \b and empty children that is the root of
 * the pertinent subtree.
 * The function requires as input any pointer to a node stored in
 * \ nodePtr. If the node stored in \a nodePtr is a P-node with
 * no partial children,
 * templateP2() considers itself responsible for the node and will
 * apply the template matching \b P2 to \a nodePtr.
 * Observe that the user calling this function has to make sure that
 * \a nodePtr is partial and is the root of the pertinent subtree.
 *
 * If templateP2() was responsible for \a nodePtr and the
 * reduction was successful, the return value is 1. Otherwise
 * the return value is 0.
 *
 * The function templateP2() creates a new full P-node that
 * will be the new root of the pertinent subtree. It then copies all full
 * children from \a nodePtr to the new root of the pertinent subtree.
 */

template<class T,class X,class Y>
bool PQTree<T,X,Y>::templateP2(PQNode<T,X,Y> **nodePtr)
{
	PQNode<T,X,Y> *newNode = 0;

	if ((*nodePtr)->type() != P_NODE ||
		(*nodePtr)->partialChildren->size() > 0)
		return false;

	else
	{
		(*nodePtr)->m_childCount = (*nodePtr)->m_childCount -
								   (*nodePtr)->fullChildren->size() + 1;
                                       // Gather all full children of nodePtr
                                       // as children of the new P-node.
                                       // Delete them from nodePtr.

		newNode = createNodeAndCopyFullChildren((*nodePtr)->fullChildren);
                                       // Correct parent-pointer and
                                       // sibling-pointers of the new P-node.
                                       
		newNode->m_parent = (*nodePtr);
		newNode->m_sibRight = (*nodePtr)->m_referenceChild->m_sibRight;
		newNode->m_sibLeft = newNode->m_sibRight->m_sibLeft;
		newNode->m_sibLeft->m_sibRight = newNode;
		newNode->m_sibRight->m_sibLeft = newNode;
		newNode->m_parentType = P_NODE;
                                       // The new P-node now is the root of
                                       // the pertinent subtree.
		(*nodePtr) = newNode;

		return true;
	} 
}


/************************************************************************
                          templateP3
************************************************************************/ 

/*
 * The function templateP3() implements the template matching for a
 * P-node with full \b and empty children that is \b not the root of
 * the pertinent subtree.
 * The function requires as input any pointer to a node stored in
 * \ nodePtr. If the node stored in \a nodePtr is a P-node with
 * no partial children,
 * templateP3() considers itself responsible for the node and will
 * apply the template matching \b P3 to \a nodePtr.
 * Observe that the user calling this function has to make sure that
 * \a nodePtr is partial and is not the root of the pertinent subtree.
 *
 * If templateP3() was responsible for \a nodePtr and the
 * reduction was successful, the return value is 1. Otherwise
 * the return value is 0.
 *
 * The function templateP3() creates
 * a new full P-node, stored in \a newPnode and copies the full children
 * of \a nodePtr to \a newPnode. \a nodePtr keeps all empty children
 * and will be labeled empty. A new partial Q-node will be created and stored
 * in \a newQnode. \a newQnode is placed at the position of \a nodePtr
 * in the tree and gets two children: \a nodePtr itself and the newly
 * created \a newPnode.
 *
 * The function templateP3() uses a few variables.
 *   - \a newPnode is the pointer to the new P-node, or, in case
 *     that \a nodePtr has only one full child, is the pointer of this child.
 *   - \a newQnode is the pointer to the new Q-node.
 *   - \a newNode is used for the proper allocation of the new
 *     Q-node.
 *   - \a emptyNode is a pointer to any empty child of nodePtr.
 */

template<class T,class X,class Y>
bool PQTree<T,X,Y>::templateP3(PQNode<T,X,Y> *nodePtr)
{

	PQInternalNode<T,X,Y>   *newNode = 0;
	PQNode<T,X,Y>     *newQnode = 0;
	PQNode<T,X,Y>     *newPnode = 0;
	PQNode<T,X,Y>     *emptyNode = 0;

   
	if (nodePtr->type() != P_NODE || nodePtr->partialChildren->size() > 0)
		return false;

	else
	{
		/*
		Create a new partial $Q$-node stored in [[newQnode]].
		It replaces [[nodePtr]] by the Q-node [[newQnode]] in the $PQ$-tree
		and makes [[nodePtr]] endmost child of [[newQnode]].
		This is done by updating parent-pointers and sibling-pointers. 
		*/
		newNode = OGDF_NEW PQInternalNode<T,X,Y>(m_identificationNumber++,Q_NODE,PARTIAL);
		newQnode = newNode;
		m_pertinentNodes->pushFront(newQnode);

		exchangeNodes(nodePtr,newQnode);
		nodePtr->m_parent = newQnode;
		nodePtr->m_parentType = Q_NODE;

		newQnode->m_leftEndmost = (nodePtr);
		newQnode->m_childCount = 1;
      

		/*
		Create a new full $P$-node stored in [[newPnode]].
		It copies the full children of [[nodePtr]] to [[newPnode]].
		The new $P$-node will then be included into the tree as child of
		the new $Q$-node [[newQnode]].
		*/
		if (nodePtr->fullChildren->size() > 0)
		{
			nodePtr->m_childCount = nodePtr->m_childCount -
                                    nodePtr->fullChildren->size();
                                    
			newPnode = createNodeAndCopyFullChildren(nodePtr->fullChildren);
			newPnode->m_parentType = Q_NODE;

                                           // Update newQnode.
			newQnode->m_childCount++;
			newQnode->fullChildren->pushFront(newPnode);
                                           // Update sibling pointers.
			nodePtr->m_sibRight = newPnode;
			newPnode->m_sibLeft = nodePtr;
			newQnode->m_rightEndmost = newPnode;
			newPnode->m_parent = newQnode;
		}



                                      // Check if nodePtr contains
                                      // only one son. If so, nodePtr
                                      // will be deleted from the tree.
		emptyNode = nodePtr->m_referenceChild;
		checkIfOnlyChild(emptyNode,nodePtr);
                                      // Update partialchildren stack of
                                      // the parent of the new Q-node.
		newQnode->m_parent->partialChildren->pushFront(newQnode);      

		return true;
	}
}


/************************************************************************
                          templateP4
************************************************************************/ 

/**
 * The function templateP4() implements the template matching for a
 * P-node with full, empty and exactly one partial children. The
 * P-node has to be the root of the pertinent subtree.
 * The function requires as input any pointer to a node stored in
 * \a nodePtr. If the node stored in \a nodePtr is a P-node with
 * one partial child,
 * templateP4() considers itself responsible for the node and will
 * apply the template matching \b P4 to \a nodePtr.
 * Observe that the user calling this function has to make sure that
 * \a nodePtr is the root of the pertinent subtree.
 *
 * If templateP4() was responsible for \a nodePtr and the
 * reduction was successful, the return value is 1. Otherwise
 * the return value is 0.
 *
 * The function templateP4() creates a new full P-node, if neccessary,
 * and copies the full children of \a nodePtr to this P-node.
 * The new P-node then is made endmost child of \a partialChild.
 * The node \a partialChild is used to store the adress of the partial
 * child of \a nodePtr.
 * The \a partialChild itself stays child of \a nodePtr.
 * Most of the here described action is done in the function
 * copyFullChildrenToPartial().
 */

template<class T,class X,class Y>
bool PQTree<T,X,Y>::templateP4(PQNode<T,X,Y> **nodePtr)
{
	PQNode<T,X,Y>  *partialChild;           // pointer to the partialChild

   
	if ((*nodePtr)->type() != P_NODE   ||
		(*nodePtr)->partialChildren->size() != 1)
		return false;
      
	else
	{
		partialChild = (*nodePtr)->partialChildren->popFrontRet();
		copyFullChildrenToPartial(*nodePtr,partialChild);
                                      // If nodePtr does not have any
                                      // empty children, then it has to
                                      // be deleted and the partial node
                                      // is occupying its place in the tree.
		checkIfOnlyChild(partialChild,*nodePtr);
                                      // The partial child now is
                                      // root of the pertinent subtree.
		*nodePtr = partialChild;

		return true;
	}
}


/************************************************************************
                          templateP5
************************************************************************/ 

/**
 * The function templateP5() implements the template matching for a
 * P-node with full, empty children and exactly one partial child. The
 * P-node is not allowed to be the root of the pertinent subtree.
 * The function requires as input any pointer to a node stored in
 * \a nodePtr. If the node stored in \a nodePtr is a P-node with
 * one partial child,
 * templateP5() considers itself responsible for the node and will
 * apply the template matching \b P5 to \a nodePtr.
 * Observe that the user calling this function has to make sure that
 * \a nodePtr is not the root of the pertinent subtree.
 *
 * If templateP5() was responsible for \a nodePtr and the
 * reduction was successful, the return value is 1. Otherwise
 * the return value is 0.
 *
 * The function templateP5() uses a few variables.
 *   - \a partialChild is a pointer to the partial child of
 *     \a nodePtr.
 *   - \a checkNode is a pointer to the endmost empty child of
 *     \a partialChild.
 *   - \a emptyNode is a pointer to the empty node that is copied as
 *     endmost child to \a partialChild.
 *   - \a emptyChildCount stores the number of empty children of
 *     \a nodePtr.
 *
 * If neccessary, the function templateP5() creates a new full P-node
 * and copies all full children of \a nodePtr to this new full P-node.
 * All empty children of \a nodePtr stay empty children of \a nodePtr.
 *
 * The new full P-node and \a nodePtr will be the new endmost children of
 * \a partialChild. The \a partialChild then occupies the position
 * of \a nodePtr in the PQ-tree.
 */

template<class T,class X,class Y>
bool PQTree<T,X,Y>::templateP5(PQNode<T,X,Y> *nodePtr)
{
	PQNode<T,X,Y>  *partialChild    = 0;
	PQNode<T,X,Y>  *checkNode       = 0;   
	PQNode<T,X,Y>  *emptyNode       = 0;
	int           emptyChildCount = 0;  
   

	if ((nodePtr->type() != P_NODE) ||
		(nodePtr->partialChildren->size() != 1))
		return false;
      
	else   
	{
		/*
		Remove [[partialChild]] from the children of [[nodePtr]]. The node
		[[partialChild]] then occupies the position of [[nodePtr]] in the 
		$PQ$-tree which is done in the function call [[exchangeNodes]] 
		(\ref{exchangeNodes}). The chunk then removes all full children from 
		[[nodePtr]] and adds them as children of a new $P$-node as endmost
		child of [[partialChild]]. This is done in the function call
		[[copyFullChildrenToPartial]] (\ref{copyFullChildrenToPartial}).
		When this chunk has finished, [[nodePtr]] has only empty children.
		*/
		emptyChildCount = nodePtr->m_childCount - 
							nodePtr->fullChildren->size() - 1;
		partialChild = nodePtr->partialChildren->popFrontRet();
		nodePtr->m_parent->partialChildren->pushFront(partialChild);
		removeChildFromSiblings(partialChild);
		exchangeNodes(nodePtr,partialChild);
		copyFullChildrenToPartial(nodePtr,partialChild);

		if (emptyChildCount > 0)
		{
			/*
			Check if [[nodePtr]] has just one empty child. If so, the child 
			is stored in [[emptyNode]] in order to be added to the empty
			side of the partial $Q$-node [[partialChild]]. If [[nodePtr]] 
			has more than one empty child, [[nodePtr]] is stored in 
			[[emptyNode]] in order to be added to the empty
			side of the partial $Q$-node [[partialChild]].
			*/
			if (emptyChildCount == 1)
			{
				emptyNode = nodePtr->m_referenceChild;
				removeChildFromSiblings(emptyNode);
			}
	              
			else
			{
				emptyNode = nodePtr;
				emptyNode->m_childCount = emptyChildCount;
			}

			/*
			Check at which side of [[partialChild]]
			the empty children hide. [[emptyNode]] stores the empty node
			that is added to the empty side of [[partialChild]].
			*/
			if (clientLeftEndmost(partialChild)->status() == EMPTY)
			{
				checkNode = partialChild->m_leftEndmost;
				partialChild->m_leftEndmost = emptyNode;
			}
			else if (clientRightEndmost(partialChild)->status() == EMPTY)
			{
				checkNode = partialChild->m_rightEndmost;
				partialChild->m_rightEndmost = emptyNode;
			}
			else
			{
				OGDF_ASSERT(false)
					//Endmostchild not found
			}

			linkChildrenOfQnode(checkNode,emptyNode);
			emptyNode->m_parent = partialChild;
			emptyNode->m_parentType = Q_NODE;
			partialChild->m_childCount++;
		}
                                       // If nodePtr did not have any empty
                                       // children it has to be deleted.
		if (emptyChildCount <= 1)
			destroyNode(nodePtr);


		return true;
	}
}


/************************************************************************
                          templateP6
************************************************************************/ 

/**
 * The function templateP6() implements the template matching for a
 * P-node with full, empty and exactly two partial children. The
 * P-node must be the root of the pertinent subtree.
 * The function requires as input any pointer to a node stored in
 * \a nodePtr. If the node stored in \a nodePtr is a P-node with
 * two partial children,
 * templateP6() considers itself responsible for the node and will
 * apply the template matching \b P6 to \a nodePtr.
 * Observe that the user calling this function has to make sure that
 * \a nodePtr is the root of the pertinent subtree.
 *
 * If templateP6() was responsible for \a nodePtr and the
 * reduction was successful, the return value is 1. Otherwise
 * the return value is 0.
 *
 * The function templateP6() creates, if neccessary,
 * a new full P-node and copies all the full children of \a nodePtr
 * to the new full P-node, whereas all empty children stay children of
 * \a nodePtr. The new P-node will be copied to one of the partial children
 * as endmost child of this partial node. The children of the second
 * partial node are copied to the first one, such that the pertinent nodes
 * form a consecutive sequence.
 *
 * The following variables are used in the function templateP6().
 *   - \a partial_1 is a pointer to the first partial child of
 *     \a nodePtr.
 *   - \a partial_2 is a pointer to the second partial child of
 *     \a nodePtr.
 *   - \a fullEnd_1 is a pointer to a full endmost child of \a partial_1.
 *   - \a fullEnd_2 is a pointer to a full endmost child of \a partial_2.
 *   - \a emptyEnd_2 is a pointer to the empty endmost child (more
 *     precisely: to the endmost child appearing on the empty side) of
 *     \a partial_2. In case that <em>ignored nodes</em> are used, this
 *     \a emptyEnd_2 may store the adress of an ignored node.
 *   - \a realEmptyEnd_2 is a pointer to the first <em>non ignored</em>
 *     node with empty status on the empty side of \a partial_2. In case
 *     that no ignored nodes are used, \a realEmpty_2 is identical to
 *     \a endEmpty_2.
*/

template<class T,class X,class Y>
bool PQTree<T,X,Y>::templateP6(PQNode<T,X,Y> **nodePtr)

{
	PQNode<T,X,Y>  *partial_1       = 0;
	PQNode<T,X,Y>  *partial_2       = 0;
	PQNode<T,X,Y>  *fullEnd_1       = 0;
	PQNode<T,X,Y>  *fullEnd_2       = 0;
	PQNode<T,X,Y>  *emptyEnd_2      = 0;
	PQNode<T,X,Y>  *realEmptyEnd_2  = 0;   

	if ((*nodePtr)->type() != P_NODE   ||
		(*nodePtr)->partialChildren->size() != 2)
		return false;
      
	else
	{
		/*
		Get the partial children of [[nodePtr]] and removes the second 
		partial child stored in [[partial_2]] from the children of 
		[[nodePtr]]. If there are any full children of [[nodePtr]], the 
		chunk removes them from the children of [[nodePtr]] and copies them 
		as children to a new $P$-node. This new $P$-node is then made
		endmost child of [[partial_1]].
		*/
		partial_1 = (*nodePtr)->partialChildren->popFrontRet();
		partial_2 = (*nodePtr)->partialChildren->popFrontRet();

		removeChildFromSiblings(partial_2);
		(*nodePtr)->m_childCount--;
		copyFullChildrenToPartial(*nodePtr,partial_1);

		/*
		Check the endmost children of the two partial children of [[nodePtr]]
		and stores them at approriate places, remembering what kind of type
		the endmost children are. 		
		*/
		if (clientLeftEndmost(partial_1)->status() == FULL)
			fullEnd_1 = partial_1->m_leftEndmost;
		else if (clientRightEndmost(partial_1)->status() == FULL)
			fullEnd_1 = partial_1->m_rightEndmost;
		else
		{
			OGDF_ASSERT(false)
				 //partial child with no FULL endmost child detected
		}

		if (clientLeftEndmost(partial_2)->status() == FULL)
			fullEnd_2 = partial_2->m_leftEndmost;
		else if(clientLeftEndmost(partial_2)->status() == EMPTY)
		{
			emptyEnd_2 = partial_2->m_leftEndmost;
			realEmptyEnd_2 = clientLeftEndmost(partial_2);
		}
		else
		{
			OGDF_ASSERT(false)
				//partial child with no FULL or EMPTY endmost child detected
		}

		if (clientRightEndmost(partial_2)->status() == FULL)
			fullEnd_2 = partial_2->m_rightEndmost;
		else if(clientRightEndmost(partial_2)->status() == EMPTY)
		{
			emptyEnd_2 = partial_2->m_rightEndmost;
			realEmptyEnd_2 = clientRightEndmost(partial_2);
		}
		else
		{
			OGDF_ASSERT(false)
				//partial child with no FULL or EMPTY endmost child detected
		}
		OGDF_ASSERT(fullEnd_2 != emptyEnd_2)
			//partial child with same type of endmost child detected

		/*
		The children of [[partial_2]] are removed from their parent and
		added as children to [[partial_1]]. This is done by resetting the
		sibling pointers of the two endmost children of [[partial_1]] and
		[[partial_2]] and the endmost child pointers of [[partial_1]].
		Observe that the parent pointers are not updated. The node
		[[partial_2]] is deleted.
		*/
		while (!partial_2->fullChildren->empty())
			partial_1->fullChildren->pushFront(partial_2->fullChildren->popFrontRet());
		linkChildrenOfQnode(fullEnd_1,fullEnd_2);
		if (partial_1->m_leftEndmost == fullEnd_1)
			partial_1->m_leftEndmost = emptyEnd_2;
		else
			partial_1->m_rightEndmost = emptyEnd_2;

		emptyEnd_2->m_parent = partial_1;
		emptyEnd_2->m_parentType = Q_NODE;

		realEmptyEnd_2->m_parent = partial_1;
		realEmptyEnd_2->m_parentType = Q_NODE;

		partial_1->m_childCount = partial_1->m_childCount +
                                partial_2->m_childCount;
		destroyNode(partial_2);



                                      // If nodePtr does not have any
                                      // empty children, then it has to
                                      // be deleted and the partial node
                                      // is occupying its place in the tree.
		checkIfOnlyChild(partial_1,*nodePtr);
                                    // partial_1 is now root of the
                                    // pertinent subtree.
		*nodePtr = partial_1;

		return true;
	}
}


/************************************************************************
                          templateQ1
************************************************************************/ 

/**
 * The function templateQ1() implements the template matching for
 * Q-nodes with only full children.
 * The function requires as input any pointer to a node stored in
 * \a nodePtr. If the node stored in \a nodePtr is a Q-node with
 * only full children,
 * templateQ1() considers itself responsible for the node and will
 * apply the template matching for full Q-nodes to \a nodePtr.
 * If the flag \a isRoot is set to 1, it signalizes
 * templateQ1() that \a nodePtr is the root of
 * the pertinent subtree. In any other case the flag has to be 0.
 *
 * If templateQ1() was responsible for \a nodePtr and the
 * reduction was successful, the return value is 1. Otherwise
 * the return value is 0.
 *
 * Different to the templateP1() for P-nodes,
 * this function is not able to check if Q-node is full by comparing
 * the number of children with the number of full children. The reason
 * is the application of the \a m_pseudoRoot at certain steps in the
 * matching algorithm. This \a m_pseudoRoot is used instead of the real
 * root of the pertinent subtree in case that no parent pointer was
 * found. But this implies that changing  the number of the children of
 * the pertinent root is not registered by the pertinent root. Hence we
 * are not allowed to use the \a childCount of Q-nodes.
 */

template<class T,class X,class Y>
bool PQTree<T,X,Y>::templateQ1(PQNode<T,X,Y> *nodePtr,
							   bool isRoot)
{

   PQNode<T,X,Y>*   seqStart = 0;
   PQNode<T,X,Y>*   seqEnd = 0;

   if (nodePtr->type() == Q_NODE &&
       nodePtr != m_pseudoRoot &&
       clientLeftEndmost(nodePtr)->status() == FULL &&
       clientRightEndmost(nodePtr)->status() == FULL)
   {

      if (checkChain(nodePtr,clientLeftEndmost(nodePtr),&seqStart,&seqEnd))
      {
         nodePtr->status(FULL);
         if (!isRoot)
            nodePtr->m_parent->fullChildren->pushFront(nodePtr);
         return true;
      }
      else 
         return false;
   }
   else
   {
      return false;
   }
}


/************************************************************************
                          templateQ2
************************************************************************/ 

/**
 * The function templateQ2() implements the template matching for
 * Q-nodes  with a pertinent
 * sequence of children on one side of the Q-node. 
 * The function requires as input any pointer to a node stored in
 * \a nodePtr. If the node stored in \a nodePtr is a Q-node with
 * a pertinent
 * sequence of children on one side of the Q-node,
 * templateQ2() considers itself responsible for the node and will
 * apply the template matching \b Q2 to \a nodePtr.
 * If the flag \a isRoot is set to 1, it signalizes
 * templateQ2() that \a nodePtr is the root of
 * the pertinent subtree. In any other case the flag has to be 0.
 *
 * If templateQ2() was responsible for \a nodePtr and the
 * reduction was successful, the return value is 1. Otherwise
 * the return value is 0.
 *
 * Below a short description is given of all different cases that
 * may occure and that are handled by the function templateQ2(), \b regardless
 * whether the Q-node \a nodePtr is root of the pertinent subtree or not.
 * The description is somewhat trunkated and should be understood as a
 * stenographic description of the labels of the children of \a nodePtr
 * when running through the children from one side to the other. Of course
 * we leave the mirror-images out.
 *   - full, empty
 *   - full, partial, empty
 *   - full, partial
 *   - partial, empty.
 *
 * templateQ2() uses the following variables.
 *   - \a fullNode is a pointer to the full endmost child of \a nodePtr.
 *   - \a sequenceBegin is a pointer to the first node of the
 *     sequence of full children. Identical to the node fullNode and
 *     mainly needed by the function checkChain().
 *   - \a sequenceEnd is a pointer to the last node of the sequence
 *     of full children. Is set by the function checkChain().
 *   - \a partialChild is a pointer to the partial child of \a nodePtr.
 *   - \a sequenceCons is 1 if all full children of
 *     \a nodePtr form a consecutive sequence with one full child beeing
 *     an endmost child of \a nodePtr \b and the partial child is
 *     adjacent to the sequence.
 *
 * templateQ2() first checks if one of the above mentioned cases
 * occures and
 * then applies the necessary template matching.
 * No special action has to be performed for the full nodes. If there exists
 * a partial child that will be stored in \a partialChild, its children
 * are made children of \a nodePtr. So to say, \a partialChild is lifted
 * up to the Q-node \a nodePtr and the occurance of the children
 * of \a partialChild is fixed within the children of \a nodePtr.
 * (Remember that a partial child is also a Q-node).
 */

template<class T,class X,class Y>
bool PQTree<T,X,Y>::templateQ2(PQNode<T,X,Y> *nodePtr,bool isRoot)
{
	PQNode<T,X,Y>   *fullNode      = 0;
	PQNode<T,X,Y>   *sequenceBegin = 0;
	PQNode<T,X,Y>   *sequenceEnd   = 0;
	PQNode<T,X,Y>   *partialChild  = 0;
   
	bool       sequenceCons   = false;
   

	if (nodePtr->type() != Q_NODE ||
		nodePtr->partialChildren->size() > 1)
		return false;
      
	else
	{
		if (nodePtr->fullChildren->size() > 0)
		{
			/*
			Get a full endmost child of
			the $Q$-node [[nodePtr]] if there exists one.
			*/
			if (nodePtr->m_leftEndmost != 0)
			{
				fullNode = clientLeftEndmost(nodePtr);
				if (fullNode->status() != FULL) 
					fullNode = 0;
			}
			if (nodePtr->m_rightEndmost != 0  && fullNode == 0)
			{
				fullNode = clientRightEndmost(nodePtr);
				if (fullNode->status() != FULL)
					fullNode = 0;
			}

			/*
			In case that a full endmost child of [[nodePtr]] exists, this 
			child has been stored in [[fullNode]] and the chunk checks by 
			calling the function [[checkChain]] (\ref{checkChain}), if all
			full children of [[nodePtr]] form a consecutive sequence.
			In case that the full children
			of [[nodePtr]] form a consecutive sequence the
			return value of [[checkChain]] is [[1]]. If a partial child
			stored in [[partialChild]] exists, the chunk checks if
			[[partialChild]] is adjacent to the sequence of full children.
			If the latter case is [[1]], the flag [[sequenceCons]] is 
			set to [[1]] and the function [[templateQ2]] is allowed to 
			reduce the pertient children of [[nodePtr]].
			*/
			if (fullNode != 0)
				sequenceCons = checkChain(nodePtr,fullNode,
										   &sequenceBegin,&sequenceEnd);

			if (sequenceCons && (nodePtr->partialChildren->size() == 1))
			{
				//nodePtr->partialChildren->startAtBottom();
				//partialChild = nodePtr->partialChildren->readNext();
				partialChild = nodePtr->partialChildren->front();
				sequenceCons = false;

				if (clientSibLeft(sequenceEnd) == partialChild ||
					clientSibRight(sequenceEnd) == partialChild)
					sequenceCons = true;
			}
		}
		else
		{
			if (!nodePtr->partialChildren->empty())
			{
				/*
				If the $Q$-node [[nodePtr]] has no full children but one 
				partial child this chunk checks, if the partial child is 
				endmost child of the [[nodePtr]]. If this is not the case, 
				[[nodePtr]] cannot be reduced by the template matching 
				{\bf Q2}.
				*/
				//nodePtr->partialChildren->startAtBottom();
				//partialChild = nodePtr->partialChildren->readNext();
				partialChild = nodePtr->partialChildren->front();
				if ((clientLeftEndmost(nodePtr) == partialChild) ||
					(clientRightEndmost(nodePtr) == partialChild))
					sequenceCons = true; 
			}
		}

		if (sequenceCons)
			removeBlock(nodePtr,isRoot);
		return sequenceCons;
	}
}


/************************************************************************
                          templateQ3
************************************************************************/ 

/*
 * The function templateQ3() implements the template matching for
 * Q-nodes with empty and/or partial children at both ends and a sequence
 * of full and/or partial children in the middle. The Q-node must be the
 * root of the pertinent subtree.
 * The function requires as input any pointer to a node stored in
 * \a nodePtr. If the node stored in \a nodePtr is a Q-node 
 * with empty and/or partial children at both ends and a sequence
 * full or partial children in the middle,
 * templateQ3() considers itself responsible for the node and will
 * apply the template matching \b Q3 to \a nodePtr.
 * Observe that the user calling this function has to make sure that
 * \a nodePtr is the root of the pertinent subtree.
 *
 * If templateQ3() was responsible for \a nodePtr and the
 * reduction was successful, the return value is 1. Otherwise
 * the return value is 0.
 *
 * Below a short description is given of all different cases that
 * may occure and are handled by the function templateQ3(), \b regardless
 * whether the Q-node \a nodePtr is the root of the pertinent subtree or not.
 * The description is somewhat trunkated and should be understood as a
 * stenographic description of the labels of the children of \a nodePtr
 * when running through the children from one side to the other. Of course
 * we leave the mirror-images out.
 *   - empty, full, empty
 *   - empty, partial, full, partial, empty
 *   - empty, partial, full, empty
 *   - empty, partial, full, partial
 *   - partial, full, partial
 *   - empty, partial, partial, empty
 *   - empty, partial, partial
 *   - partial, partial
 *
 * The function templateQ3() uses the following variables.
 *   - \a fullChild is a pointer to an arbitrary full child of \a nodePtr.
 *   - \a fullStart is a pointer to the first full child of a
 *     consecutive sequence of full children.
 *   - \a fullEnd is a pointer to the last full child of a
 *     consecutive sequence of full children.
 *   - \a partial_1 is a pointer to the first partial child of \a nodePtr.
 *   - \a partial_2 is a pointer to the second partial child of \a nodePtr.
 *   - \a conssequence is 1 if the pertinent children of
 *     \a nodePtr form a consecutive sequence with at most one partial
 *     child at every end of the sequence.
 *   - \a found is a help variable.
 *
 * templateQ3() first checks if one of the above mentioned cases
 * occures and then applies the neccessary template matching.
 * No special action has to be performed for the full nodes. If there exist
 * one or two partial children which will be stored in \a partial_1
 * or \a partial_2, their children
 * are made children of \a nodePtr. So to say, \a partial_1 and
 * partial_2 are lifted up to the Q-node \a nodePtr
 * and the occurance of their children
 * is fixed within the children of \a nodePtr.
 */

template<class T,class X,class Y>
bool PQTree<T,X,Y>::templateQ3(PQNode<T,X,Y> *nodePtr)                      

{
	PQNode<T,X,Y>   *fullChild    = 0;
	PQNode<T,X,Y>   *fullStart    = 0;
	PQNode<T,X,Y>   *fullEnd      = 0;  
	PQNode<T,X,Y>   *partial_1    = 0;
	PQNode<T,X,Y>   *partial_2    = 0;
	bool            conssequence  = false; 
	bool            found         = false;
   

	if (nodePtr->type() != Q_NODE || nodePtr->partialChildren->size() >= 3)
		return false;	
      
	else
	{

		/*
		Check ifthe
		pertinent children of [[nodePtr]] form a consecutive sequence. We
		differ between two cases:
		\begin{enumerate}
		\item There exist full children of [[nodePtr]]. First check with
		the function [[checkChain]] (\ref{checkChain}) if the full children
		form a consecutive sequence. In case that the check was
		successful, check if each partial child is adjacent to a full child.
		If both checks were successful, the pertient children form a
		consecutive sequence.
		\item There do not exist full children. Check if the partial
		children (there are at most two of them) form a consecutive sequence.
		If the test was successful, the pertinent children form a
		consecutive sequence.
		*/
		
		if (!nodePtr->fullChildren->empty())
		{
			/*
			A consecutive
			sequence of full children has been detected, containing all full
			children of [[nodePtr]]. The chunk checks if each partial child 
			of [[nodePtr]] is adjacent to a full child. Observe that the 
			function [[templateQ3]] only reaches this chunk when [[nodePtr]] 
			has less than three partial children.
			*/
//			nodePtr->fullChildren->startAtBottom();
//			fullChild = nodePtr->fullChildren->readNext();
			fullChild = nodePtr->fullChildren->front();
			conssequence = checkChain(nodePtr,fullChild,&fullStart,&fullEnd);
			if (conssequence)
			{
				//nodePtr->partialChildren->startAtBottom();
				//while (!nodePtr->partialChildren->readLast())
				ListIterator<PQNode<T,X,Y>*> it;
				for (it = nodePtr->partialChildren->begin(); it.valid(); ++it)
				{
					//partial_1 = nodePtr->partialChildren->readNext();
					partial_1 = *it;
					found = false;
					if ((clientSibLeft(fullStart) == partial_1)  ||
						(clientSibRight(fullStart) == partial_1) ||
						(clientSibLeft(fullEnd) == partial_1)    ||
						(clientSibRight(fullEnd) == partial_1)     )
						found = true;
					if (!found)
						conssequence = found;
	          
				}


  
			}
		}

		else if (nodePtr->partialChildren->size() == 2)
		{
			/*
			In case that the node [[nodePtr]] does not have any full children,
			this chunk checks if the partial children are adjacent.
			*/
			partial_1 = nodePtr->partialChildren->
				front();
			partial_2 = nodePtr->partialChildren->back();
			if ((clientSibLeft(partial_1) == partial_2)  ||
				(clientSibRight(partial_1) == partial_2)   )
				found = true;
			conssequence = found;

		}

 

		bool isRoot = true;
		if (conssequence)
			removeBlock(nodePtr,isRoot);
		return conssequence;
	}
}


}

#endif


