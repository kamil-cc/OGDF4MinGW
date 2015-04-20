/*
 * $Revision: 1.6 $
 * 
 * last checkin:
 *   $Author: gutwenger $ 
 *   $Date: 2007-11-08 16:10:37 +0100 (Do, 08 Nov 2007) $ 
 ***************************************************************/
 
/** \file
 * \brief Implementation of memory manager for more efficiently
//   allocating small pieces of memory
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

#include <ogdf/basic/basic.h>


namespace ogdf {

// size of (with malloc) allocated blocks
const int nBlockSize = (1 << 13) - sizeof(MemElemPtr);

// the one and only memory manager
#ifdef OGDF_USE_CPP_MEMORY_MANAGER
SimpleMemoryManager g_memory;
#else
MemoryManager g_memory;
#endif


//
// initialization
void MemoryManager::init()
{
	m_freeList = (MemElemPtr*)calloc(OGDF_MAXMEM_SIZE, sizeof(MemElemPtr));
	if (m_freeList == 0)
		THROW(InsufficientMemoryException);

#if defined(OGDF_DEBUG) && !defined(OGDF_AVOID_MEMORY_CHECKING)
	// allocate a vector for each mini-block size maintained by the memory
	// manager representing an empty list (no memory block of this size yet
	// allocated). Required for maintaining memory info structures.
	m_infoList = (DbMemoryInfoListElement**)
		calloc(OGDF_MAXMEM_SIZE, sizeof(DbMemoryInfoListElement));
	if (m_infoList == 0)
		THROW(InsufficientMemoryException);
#endif

	m_blockList = 0;
}


//
// freeing allocated memory blocks
void MemoryManager::cleanup() 
{
	MemElemPtr p, q;
	for (p = m_blockList; p != 0; p = q) {
		q = p->m_next;
		free(p);
	}

	free(m_freeList);

#if defined(OGDF_DEBUG) && !defined(OGDF_AVOID_MEMORY_CHECKING)
	ofstream *osLeaks = 0;

	int nLeaks = 0, nTotalBytes = 0;
	for (int i = 0; i < OGDF_MAXMEM_SIZE; ++i)
	{
		bool bytesWritten = false;

		DbMemoryInfoListElement *pInfoListElement = m_infoList[i];
		while(pInfoListElement != 0) {
			DbMemoryInfoListElement *pNextInfoListElement =
				pInfoListElement->m_next;

			int nSlices =
				nBlockSize / (pInfoListElement->m_nWords*sizeof(void*));
			for (int j = 1; j < nSlices; ++j) {
				const DbMemoryCheckInfo &pInfo = pInfoListElement->m_info[j];

				if (pInfo.m_line != -1) {
					// leak detected

					// output file not yet open?
					if (osLeaks == 0) {
						osLeaks = new ofstream("memory-leaks.txt");
						(*osLeaks) << "Summary of memory leaks:\n";
					}

					// bytes not yet written?
					if (bytesWritten == false) {
						bytesWritten = true;
						(*osLeaks) << "\n" << i << " bytes:\n";
					}

					if (pInfo.m_line == 0)
						(*osLeaks) << "  " << pInfo.m_pBlock << " [new allocation]\n";
					else
						(*osLeaks) << "  " << pInfo.m_pBlock <<
							" line " << pInfo.m_line <<
							" of \"" << pInfo.m_file << "\"\n";

					++nLeaks;
					nTotalBytes += i;
				}
			}

			free(pInfoListElement->m_info);

			free(pInfoListElement);
			pInfoListElement = pNextInfoListElement;
		}
	}
	
	if (osLeaks != 0) {
		(*osLeaks) << "\n" << nTotalBytes << " bytes in " << nLeaks << " block(s)!";
		delete osLeaks;
	}

	free(m_infoList);

	// MEMORY LEAK(S) DETECTED ERROR!
	//   assert that no memory leaks have occured
	//   If this assertion fails, all memory leaks have been written to file
	//   "memory-leaks.txt".
	OGDF_ASSERT(nLeaks == 0);
#endif
}


//
// allocate a new block and slice it into pieces of size nSliceSize
void MemoryManager::allocateBlock(size_t nSizeSlice)
{
	// we assume that m_freeList[nSliceSize] is empty!

	MemElemPtr pBlock = (MemElemPtr)malloc(nBlockSize);
	if (pBlock == 0) THROW(InsufficientMemoryException);

	pBlock->m_next = m_blockList; m_blockList = pBlock;

	// one slice requires nWords words
	size_t nWords = (nSizeSlice+sizeof(void*)-1)/sizeof(void*);

	// we altogether create nSlices slices, including the first one
	// which is reserved for chaining the memory blocks
	// Important: The first slice uses also nWords words in order to
	// avoid any alignment problems though 1 word would be enough to
	// store the next-pointer
	size_t nSlices = nBlockSize / (nWords*sizeof(void*));

#if defined(OGDF_DEBUG) && !defined(OGDF_AVOID_MEMORY_CHECKING)
	// allocate a new memory info list structure for block pBlock
	DbMemoryInfoListElement *pInfoListElement =
		(DbMemoryInfoListElement *) malloc(sizeof(DbMemoryInfoListElement));
	pInfoListElement->m_pBase = pBlock;
	pInfoListElement->m_pStop = pBlock + nSlices * nWords;
	pInfoListElement->m_nWords = nWords;

	// add it to the beginning of list of all those blocks with size nSizeSlice
	pInfoListElement->m_next = m_infoList[nSizeSlice];
	m_infoList[nSizeSlice] = pInfoListElement;

	// allocate array of info structures
	pInfoListElement->m_info =
		(DbMemoryCheckInfo*) malloc(nSlices * sizeof(DbMemoryCheckInfo));

	// initialize each info structure
	// (m_line = -1 means currently not allocated)
	for(int i = 0; i < nSlices; ++i) {
		pInfoListElement->m_info[i].m_line = -1;
		pInfoListElement->m_info[i].m_file = 0;
	}
#endif

	// pStop is the last slice
	MemElemPtr pStop = pBlock+(nSlices-1)*nWords;
	// the first usable slice is at pBlcok+nWords
	MemElemPtr p = *(m_freeList+nSizeSlice) = pBlock+nWords;

	while(p < pStop)
	{
		p = p->m_next = p+nWords;
	}
	p->m_next = 0;
}


//
// allocate nBytes bytes of memory
#ifdef OGDF_DEBUG
void *MemoryManager::allocate(size_t nBytes, const char *file, int line)
#else
void *MemoryManager::allocate(size_t nBytes)
#endif
{
	if (nBytes < OGDF_MAXMEM_SIZE) {
		MemElemPtr *pFreeBytes = m_freeList+nBytes;
		if (*pFreeBytes == 0) allocateBlock(nBytes);
		MemElemPtr p = *pFreeBytes;
		*pFreeBytes = p->m_next;

#if defined(OGDF_DEBUG) && !defined(OGDF_AVOID_MEMORY_CHECKING)
		// p is now in use
		DbMemoryCheckInfo *pInfo = getMemoryCheckInfo(p,nBytes);
		// if this assertion fails, we have a bug in the memory checking
		// mechanism of the memory-manager!
		OGDF_ASSERT(pInfo != 0);
		pInfo->m_pBlock = p;
		pInfo->m_line   = line;
		pInfo->m_file   = file;
#endif

		return p;

	} else {
		void *p = malloc(nBytes);
		if (p == 0) THROW(InsufficientMemoryException);
		return p;
	}
}


//
// deallocate nBytes bytes of memory at address p
void MemoryManager::deallocate(void *p, size_t nBytes)
{
	if (nBytes < OGDF_MAXMEM_SIZE)
	{
#if defined(OGDF_DEBUG) && !defined(OGDF_AVOID_MEMORY_CHECKING)
		DbMemoryCheckInfo *pInfo = getMemoryCheckInfo(p,nBytes);

		// INVALID POINTER ERROR!
		//   assert that p is a valid mini-block
		OGDF_ASSERT(pInfo != 0);

		// MULTIPLE DELETE ERROR!
		//   assert that block is currently allocated
		OGDF_ASSERT(pInfo->m_line != -1);
		pInfo->m_line = -1;
#endif

		MemElemPtr *pFreeBytes = m_freeList+nBytes;

		MemElemPtr(p)->m_next = *pFreeBytes;
		*pFreeBytes = MemElemPtr(p);

	} else free(p);
}


//
// faster version for deallocating chained pieces of equal-sized
// memory chunks (such as element structures of lists)
void MemoryManager::deallocateList(void *pHead, void *pTail, int nBytes) 
{
	if (nBytes < OGDF_MAXMEM_SIZE) {

#if defined(OGDF_DEBUG) && !defined(OGDF_AVOID_MEMORY_CHECKING)
		MemElemPtr q, pStop = MemElemPtr(pTail)->m_next;
		for (q = MemElemPtr(pHead); q != pStop; q = q->m_next) {
			getMemoryCheckInfo(q,nBytes)->m_line = -1;
		}
#endif

		MemElemPtr &freeBytes = *(m_freeList+nBytes);
		MemElemPtr(pTail)->m_next = freeBytes;
		freeBytes = MemElemPtr(pHead);

	} else {
		MemElemPtr q, pStop = MemElemPtr(pTail)->m_next;
		while (pHead != pStop) {
			q = MemElemPtr(pHead)->m_next;
			free(pHead);
			pHead = q;
		}
	}
}


#if defined(OGDF_DEBUG) && !defined(OGDF_AVOID_MEMORY_CHECKING)
DbMemoryCheckInfo *MemoryManager::getMemoryCheckInfo(void *p, size_t nBytes)
{
	DbMemoryInfoListElement *pInfoListElement = m_infoList[nBytes];

	while(pInfoListElement != 0)
	{
		if (pInfoListElement->m_pBase <= p && p < pInfoListElement->m_pStop)
		{
			int diff = (MemElemPtr)p - (MemElemPtr)pInfoListElement->m_pBase;
			int index = diff / pInfoListElement->m_nWords;

			// first entry is reserved for chaining blocks
			// diff must be a multiple of m_nWords
			if (index == 0 || index * pInfoListElement->m_nWords != diff)
				return 0;

			return pInfoListElement->m_info + index;
		}

		pInfoListElement = pInfoListElement->m_next;
	}

	// not found!
	return 0;
}
#endif


} // namespace ogdf
