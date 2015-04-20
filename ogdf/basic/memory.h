/*
 * $Revision: 1.8 $
 * 
 * last checkin:
 *   $Author: gutwenger $ 
 *   $Date: 2007-11-08 16:10:37 +0100 (Do, 08 Nov 2007) $ 
 ***************************************************************/
 
/** \file
 * \brief Declaration of memory manager for allocating small
 *        pieces of memory
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

#ifndef OGDF_MEMORY_H
#define OGDF_MEMORY_H


#include <stdlib.h>
#include <new>


namespace ogdf {


//! The following define switches the memory checking mechanism off.
/**
 *  If \c OGDF_AVOID_MEMORY_CHECKING is not defined, memory checking
 * (checking for invalid pointers with delete, deleting not allocated
 * memory (multiple delete's), and memory leaks) is performed in
 * debug version.
 */
#define OGDF_AVOID_MEMORY_CHECKING
//#warning "ATTN: MEMORY CHECK DISABLED!"


// The ogdf memory management can also be replaced by the usual
// C++ memory management using malloc() and free().
// This is less efficient, but has the advantage that tools like Purify
// can be applied.
//#define OGDF_USE_CPP_MEMORY_MANAGER


// allocating memory slices from 0,...,OGDF_MAXMEM_SIZE-1
#define OGDF_MAXMEM_SIZE 256


struct MemElem { MemElem *m_next; };
typedef MemElem *MemElemPtr;


#ifdef OGDF_DEBUG
/**
 *  The \a DbMemoryCheckInfo structure stores information about the allocation of
 *  a mini-block. It is used for testing the integrety of delete's and
 *  for finding memory leaks. (Debug only)
 */
struct DbMemoryCheckInfo {
	void       *m_pBlock;
	int         m_line;
	const char *m_file;
};

/**
 * The \a DbMemoryInfoListElement structure represents an element in a m_infoList[nBytes]
 * list of MemoryManager. (Debug only)
 */
struct DbMemoryInfoListElement
{
	MemElemPtr m_pBase;
	MemElemPtr m_pStop;
	int        m_nWords;
	DbMemoryInfoListElement *m_next;
	DbMemoryCheckInfo *m_info;
};
#endif


//! The class \a MemoryManager represents the ogdf internal memory manager.
/**
 * <H3>Usage:</H3>
 *
 * Adding the macro \c #OGDF_NEW_DELETE in a class declaration overloads
 * new and delete operators of that class such that they use the OGDF
 * memory manager. This is useful if the size of a class is less than
 * \c #OGDF_MAXMEM_SIZE bytes.
 *
 * The memory-manager performs several checks in a Debug build.
 * <ul>
 *  <li>A \c delete tests if the memory block to be freed was actually
 *      allocated by the OGDF memory-manager, and if this block is
 *      currently allocated.
 *  <li>At program termination, the memory-manager checks for memory
 *      leaks. If leaks occur, all leaks are written to the file
 *      <tt>memory-leaks.txt</tt> and an assertion fails.
 *      If the macro \c #OGDF_NEW is used instead of \c new for allocation,
 *      (e.g., <CODE>MyClass *p = OGDF_NEW MyClass</CODE>)
 *      the output shows the file name and line number of the allocation
 *      that was not freed, otherwise the output is <CODE>"[new allocation]"</CODE>.
 * </ul>
 *
 * The overhead caused by the checking procedures is usally small
 * but can get too big if very many pieces of memory of the same
 * size are allocated. In this case, define <tt>#OGDF_AVOID_MEMORY_CHECKING</tt>
 * in <CODE>basic/basic/memory.h</CODE> (uncomment the <tt>#define</tt>). This prevents the
 * memory manager from performing checking.
 *
 * Another benefit from the OGDF memory-manager is that it throws an
 * InsufficientMemoryException if no more memory is available. Hence
 * it is legal to omit checking if new returned 0 as long as stack-
 * unwinding frees all memory allocated so far.
 * It is also possible to make the usual \c new operator behave the same
 * way (throwing an InsufficientMemoryException) by defining the
 * macro \c #OGDF_MALLOC_NEW_DELETE in a class declaration.
 * 
 * The OGDF memory management can also be replaced by the usual
 * C++ memory management using \c malloc() and \c free(). To do this, 
 * uncomment the define \c #OGDF_USE_CPP_MEMORY_MANAGER at the begin of
 * the header file memory.h.
 * This is less efficient, but has the advantage that tools like Purify
 * can be applied. 
 * 
 * \bug Flag #OGDF_AVOID_MEMORY_CHECKING has to be set in order to work with
 * gcc (3.3.5). If memory checking is enabled, we get a MULTIPLE DELETE ERROR
 * with VariableEmbeddingInserter.
 * 
 */

class MemoryManager {
public:
	MemElemPtr *m_freeList; //!< \a m_freeList[sz] is the free list for memory chunks of size \a sz.

	MemoryManager() { }
	~MemoryManager() { }

	//! Initializes the memory manager.
	void init();
	
	//! Frees all memory blocks allocated by the memory manager.
	void cleanup();

	//! Allocates a new memory block and prepares slices of size \a nSliceSize for memory allocation.
	void allocateBlock(size_t nSizeSlice);

	//! Allocates memory of size \a nBytes.
#ifdef OGDF_DEBUG
	void *allocate(size_t nBytes, const char *file, int line);
#else
	void *allocate(size_t nBytes);
#endif

	//! Deallocates memory at address \a p which is of size \a nBytes.
	void deallocate(void *p, size_t nBytes);

	//! Deallocate a complete list starting at \a pHead and ending at \a pTail.
	/**
	 * The elements are assumed to be chained using the first word of each element and
	 * elements are of size \a nBytes. This is much more efficient the deallocating
	 * each element separately, since the whole chain can be concatenated with the
	 * free list (constant effort).
	 */
	void deallocateList(void *pHead, void *pTail, int nBytes);


private:

	MemElemPtr m_blockList; //!< The list of allocated memory blocks.

#ifdef OGDF_DEBUG
	/**
	 * Returns the check-info structure belonging to mini-block p with
	 * \a nBytes size. Returns 0 if no such block was created by the memory-
	 * manager (i.e., the allocated block had a different size!).
	 */
	DbMemoryCheckInfo *getMemoryCheckInfo(void *p, size_t nBytes);

	DbMemoryInfoListElement **m_infoList;
#endif
};


//! Implements a simple memory manager using \c malloc() and \c free().
class SimpleMemoryManager {
public:
	SimpleMemoryManager() { }
	~SimpleMemoryManager() { }


	void init() { }
	void cleanup() { }


	//! Allocates memory of size \a nBytes.
	void *allocate(size_t nBytes)
	{
		void *p = malloc(nBytes);
		if (p == 0) THROW(ogdf::InsufficientMemoryException);
		return p;
	}


	//! Deallocates memory at address \a p which is of size \a nBytes.
	void deallocate(void *p, size_t /* nBytes */) {
		free(p);
	}

	//! Deallocate a complete list starting at \a pHead and ending at \a pTail.
	/**
	 * The elements are assumed to be chained using the first word of each element and
	 * elements are of size \a nBytes.
	 */
	void deallocateList(void *pHead, void *pTail, int /* nBytes */)
	{
		MemElemPtr q, pStop = MemElemPtr(pTail)->m_next;
		while (pHead != pStop) {
			q = MemElemPtr(pHead)->m_next;
			free(pHead);
			pHead = q;
		}
	}
};


#ifdef OGDF_USE_CPP_MEMORY_MANAGER
extern SimpleMemoryManager g_memory;
#else
extern MemoryManager g_memory; //!< The one and only memory manager instance.
#endif



// this macro defines new and delete operators such that they use
// the global memory manager g_memory
// usage: simply add OGDF_NEW_DELETE within a class declaration

#ifdef OGDF_USE_CPP_MEMORY_MANAGER

#define OGDF_NEW new

#define OGDF_NEW_DELETE \
public: \
void *operator new(size_t nBytes) { \
	return ogdf::g_memory.allocate(nBytes); \
} \
void *operator new(size_t, void *p) {\
	return p;\
}\
\
void operator delete(void *p, size_t nBytes) { \
	if(p) ogdf::g_memory.deallocate(p, nBytes); \
}\

#else

#ifdef OGDF_DEBUG

#define OGDF_NEW new(__FILE__,__LINE__)

// debug version with memory leak detection
#define OGDF_NEW_DELETE \
public: \
void *operator new(size_t nBytes, const char *file, int line) { \
	return ogdf::g_memory.allocate(nBytes,file,line); \
} \
void *operator new(size_t nBytes) { \
	return ogdf::g_memory.allocate(nBytes,0,0); \
} \
void *operator new(size_t, void *p) {\
	return p;\
}\
\
void operator delete(void *p, size_t nBytes) { \
	if(p) ogdf::g_memory.deallocate(p, nBytes); \
}\


#else

#define OGDF_NEW new

// non-debug version with efficient inline code
#define OGDF_NEW_DELETE \
public: \
void *operator new(size_t nBytes) { \
	if (nBytes < OGDF_MAXMEM_SIZE) { \
		ogdf::MemElemPtr *pFreeBytes = ogdf::g_memory.m_freeList+nBytes; \
		if (*pFreeBytes == 0) ogdf::g_memory.allocateBlock(nBytes); \
		ogdf::MemElemPtr p = *pFreeBytes; *pFreeBytes = p->m_next; \
		return p; \
	} else { \
		void *p = malloc(nBytes); \
		if (p == 0) THROW(ogdf::InsufficientMemoryException); \
		return p; \
	} \
} \
void *operator new(size_t, void *p) {\
	return p;\
}\
\
void operator delete(void *p, size_t nBytes) { \
	if (p && nBytes < OGDF_MAXMEM_SIZE) { \
		ogdf::MemElemPtr *pFreeBytes = ogdf::g_memory.m_freeList+nBytes; \
		ogdf::MemElemPtr(p)->m_next = *pFreeBytes; *pFreeBytes = ogdf::MemElemPtr(p); \
	} else free(p); \
}\

#endif
#endif


// The following version performs memory allocation by calling malloc()
// and free(). It can be useful in order to check if there are problems
// with the ogdf memory manager.
// Note: Memory deallocated with the deallocateList() function of the
//   ogdf memory manager will not be freed and causes memory leaks
//   (e.g., List, SList, GraphList)
/*#define OGDF_NEW_DELETE \
public: \
void *operator new(size_t nBytes) { \
	return malloc(nBytes); \
} \
void *operator new(size_t, void *p) {\
	return p;\
}\
\
void operator delete(void *p, size_t nBytes) { \
	free(p); \
}\
void operator delete(void *, void *) \
	{return; }
*/


#define OGDF_MALLOC_NEW_DELETE \
public: \
void *operator new(size_t nBytes) { \
	void *p = malloc(nBytes); \
	if (p == 0) THROW(ogdf::InsufficientMemoryException); \
	return p; \
} \
void *operator new(size_t, void *p) {\
	return p;\
}\
\
void operator delete(void *p) { \
	free(p); \
}\


} // namespace ogdf


#endif
