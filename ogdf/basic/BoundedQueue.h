/*
 * $Revision: 1.3 $
 * 
 * last checkin:
 *   $Author: gutwenger $ 
 *   $Date: 2007-11-08 16:10:37 +0100 (Do, 08 Nov 2007) $ 
 ***************************************************************/
 
/** \file
 * \brief Declaration and implementation of bounded queue class.
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

#ifndef OGDF_B_QUEUE_H
#define OGDF_B_QUEUE_H


#include <ogdf/basic/basic.h>


namespace ogdf {

template<class E, class INDEX> class BoundedQueue;

// output
template<class E, class INDEX>
void print(ostream &os, const BoundedQueue<E,INDEX> &S, char delim = ' ');


//! The parameterized class \a BoundedQueue<E> implements queues with bounded size.
template<class E, class INDEX = int> class BoundedQueue {

	friend void print OGDF_FT_ENDING (ostream &, const BoundedQueue<E,INDEX> &, char);

	E *m_pStart; //! Pointer to first element of current sequence.
	E *m_pEnd;   //! Pointer to one past last element of current sequence.
	E *m_pStop;  //! Pointer to one past last element of total array.
	E *m_pFirst; //! Pointer to  first element of total array.

public:
	//! Constructs an empty bounded queue for at most \a n elements.
	explicit BoundedQueue(INDEX n) {
		OGDF_ASSERT(n >= 1)
		m_pStart = m_pEnd = m_pFirst = new E[n+1];
		if (m_pFirst == 0) THROW(InsufficientMemoryException);

		m_pStop = m_pFirst+n+1;
	}

	//! Constructs a bounded queue that is a copy of \a Q.
	BoundedQueue(const BoundedQueue<E> &Q) {
		copy(Q);
	}

	// destruction
	~BoundedQueue() { delete [] m_pFirst; }

	//! Returns front element.
	const E &top() const {
		OGDF_ASSERT(m_pStart != m_pEnd)
		return *m_pStart;
	}

	//! Returns front element.
	E &top() {
		OGDF_ASSERT(m_pStart != m_pEnd)
		return *m_pStart;
	}

	//! Returns back element.
	const E &bottom() const {
		OGDF_ASSERT(m_pStart != m_pEnd)
		if (m_pEnd == m_pFirst) return *(m_pStop-1);
		else return *(m_pEnd-1);
	}

	//! Returns back element.
	E &bottom() {
		OGDF_ASSERT(m_pStart != m_pEnd)
		if (m_pEnd == m_pFirst) return *(m_pStop-1);
		else return *(m_pEnd-1);
	}

	//! Returns current size of the queue.
	INDEX size() const {
		return (m_pEnd >= m_pStart) ?
			(m_pEnd - m_pStart) :
			(m_pEnd-m_pFirst)+(m_pStop-m_pStart);
	}

	//! Returns the capacity of the bounded queue.
	INDEX capacity() const { return (m_pStop - m_pFirst)-1; }

	//! Returns true iff the queue is empty.
	bool empty() { return m_pStart == m_pEnd; }

	//! Returns true iff the queue is full.
	bool full() {
		INDEX h = m_pEnd-m_pStart
		return ( h>=0 ) ? 
			(h == m_pStop-m_pFirst-1) :
			(h == -1);
	}

	//! Assignment operator.
	BoundedQueue<E> &operator=(const BoundedQueue<E> &Q) {
		delete [] m_pFirst;
		copy(Q);
		return *this;
	}

	//! Adds \a x at the end of queue.
	void append(const E &x) {
		*m_pEnd++ = x;
		if (m_pEnd == m_pStop) m_pEnd = m_pFirst;
		OGDF_ASSERT(m_pStart != m_pEnd)
	}

	//! Removes front element and returns it.
	E pop() {
		OGDF_ASSERT(m_pStart != m_pEnd)
		E x = *m_pStart++;
		if (m_pStart == m_pStop) m_pStart = m_pFirst;
		return x;
	}

	//! Makes the queue empty.
	void clear() { m_pStart = m_pEnd = m_pFirst; }

private:
	void copy(const BoundedQueue<E> &Q) {
		int n = Q.size()+1;
		m_pEnd = m_pStart = m_pFirst = new E[n];
		if (m_pFirst == 0) THROW(InsufficientMemoryException);

		m_pStop = m_pStart + n;
		for (E *pX = Q.m_pStart; pX != Q.m_pEnd; ) {
			*m_pEnd++ = *pX++;
			if (pX == Q.m_pStop) pX = Q.m_pFirst;
		}
	}
}; // class BoundedQueue


// output
template<class E, class INDEX>
void print(ostream &os, const BoundedQueue<E,INDEX> &Q, char delim = ' ')
{
	for (const E *pX = Q.m_pStart; pX != Q.m_pEnd; ) {
		if (pX != Q.m_pStart) os << delim;
		os << *pX;
		if (++pX == Q.m_pStop) pX = Q.m_pFirst;
	}
}

/*template<class E>
void print(ostream &os, const BoundedQueue<E> &Q)
{ print(os,Q,','); }*/


// output operator
template<class E, class INDEX>
ostream &operator<<(ostream &os, const BoundedQueue<E,INDEX> &Q)
{
	print(os,Q);
	return os;
}

} // end namespace ogdf


#endif
