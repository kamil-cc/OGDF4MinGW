/*
 * $Revision: 1.7 $
 * 
 * last checkin:
 *   $Author: klein $ 
 *   $Date: 2007-11-12 15:53:31 +0100 (Mo, 12 Nov 2007) $ 
 ***************************************************************/
 
/** \file
 * \brief Declares & Implements Binary Heap, and Top10Heap
 * 
 * \author Markus Chimani
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

#ifndef OGDF_MIN_HEAP_H
#define OGDF_MIN_HEAP_H

#include<ogdf/basic/Array.h>

namespace ogdf {

//! Augments any data elements of type \a X with keys of type \a Score.
/**
 * Also defines comparator function using the keys.
 * This class is intended as a helpful convenience class for using with MinHeap, Top10Heap,.. 
 */
template<class Score, class X> class Valued {
	Score v;
	X x;
public:
	//! Constructor of empty element. Be careful!
	Valued() : v(0), x(0) {};
	//! Constructor using a key/value pair
	Valued(Score vt, X xt) : v(vt), x(xt) {}
	//! Copy-constructor
	Valued(const Valued& V) : v(V.v), x(V.x) {}
	//! Returns the key of the element
	Score value() const { return v; }
	//! Returns the data of the element
	X item() const { return x;}
	//! Comparison oprator based on the compare-operator for the key type (\a Score)
	bool operator<(const Valued<Score,X>& V) const { return v<V.v; }
	//! Comparison oprator based on the compare-operator for the key type (\a Score)
	bool operator<=(const Valued<Score,X>& V) const { return v<=V.v; }
	//! Comparison oprator based on the compare-operator for the key type (\a Score)
	bool operator>(const Valued<Score,X>& V) const { return v>V.v; }
	//! Comparison oprator based on the compare-operator for the key type (\a Score)
	bool operator>=(const Valued<Score,X>& V) const { return v>=V.v; }
	//! Comparison oprator based on the compare-operator for the key type (\a Score)
	bool operator==(const Valued<Score,X>& V) const { return v==V.v; }
	//! Comparison oprator based on the compare-operator for the key type (\a Score)
	bool operator!=(const Valued<Score,X>& V) const { return v!=V.v; }
};


//! Dynamically growing binary heap tuned for efficiency on a small interface (compared to BinaryHeap).
/**
 * It assumes that the data-elements are themselves comparable, i.e., the compare-function
 * of the items implicitly defines the keys. Hence this datastructure allows no key-changing 
 * operations (decreaseKey, etc.).
 * 
 * The heap grows (using doubling) dynamically, if there are more elements added. Furthermore,
 * MinHeap allows to be directly indexed using traditional array-syntax, e.g., for iterating over
 * all its elements.  
 * 
 * If your intended datastructure do not dorectly offer a compare function, but you have
 * certain key-values (scores, etc.), you may want to use the convenience-class 
 * Valued < Score,X > to bind both together and use within MinHeap.  
 */
template<class X, class INDEX = int>
class MinHeap {	
private:
	Array<X,INDEX> data; // array starts at index 1
	INDEX num;
public:
	//! Construtor, giving initial array size
	MinHeap(INDEX size) : data(1, size), num(0) {}

	//! Returns true if the heap is empty
	bool empty() const { return num == 0; }
	//! Returns the number of elements in the heap
	INDEX size() const { return num; }
	
	//! empties the heap
	void clear() { num = 0; }

	//! Returns a reference to the top (i.e., smallest) element of the heap. It does not remove it. [O(1)]
	const X& top() const {
		return data[1];
	}
	
	//! Adds an element to the heap [O(log n)]
	void push(X& x) {
		X y;
		if(num == capacity())
			data.grow(capacity(),y); // double the size & init with nulls
		data[++num] = x;
		heapup(num);
	}
	
	//! Returns the top (i.e., smallest) element and removed it from the heap [O(log n)]
	X pop() {
		data.swap(1, num--);
		heapdown();
		return data[num+1];
	}
	
	//! obtain const references to the element at index \a idx (the smallest array index is 0, as for traditional C-arrays)
	const X& operator[](INDEX idx) const {
		return data[idx+1];
	}
	

protected:
	//! Returns the current array-size of the heap, i.e., the number of elements which can be added before the next resize occurs.
	INDEX capacity() const { return data.size(); }

	void heapup(INDEX idx) {
		INDEX papa;
		while( (papa = idx/2) > 0) {
			if( data[papa] > data[idx] ) {
				data.swap(papa, idx);
				idx = papa;
			} else return; //done
		}
	}
	
	void heapdown() {
		INDEX papa = 1;
		INDEX son;
		while(true) {
			if( (son = 2*papa) < num && data[son+1] < data[son] )
				son++;
			if( son <= num && data[son] < data[papa]) {
				data.swap(papa, son);
				papa = son;
			} else return;
		}		
	}	
};

//! A variant of MinHeap which always holds only the X (e.g. X=10) elements with the highest keys.
/**
 * It assumes that the data-elements are themselves comparable, i.e., the compare-function
 * of the items implicitly defines the keys. 
 * 
 * If your intended datastructure do not dorectly offer a compare function, but you have
 * certain key-values (scores, etc.), you may want to use the convenience-class 
 * Valued < Score,X > to bind both together and use within MinHeap.  
 */
template<class X, class INDEX = int>
class Top10Heap : protected MinHeap<X,INDEX> { // favors the 10 highest values...
public:
	//! The type for results of a Top10Heap::push operation
	enum PushResult { Accepted, Rejected, Swapped };
	
	//! Convenience function: Returns true if the PushResults states that the newly pushed element is new in the heap
	static bool successful(PushResult r) { return r != Rejected; }
	//! Convenience function: Returns true if the PushResults states that push caused an element to be not/no-longer in the heap
	static bool returnedSomething(PushResult r) { return r != Accepted; }

	//! Constructor generating a heap which holds the 10 elements with highest value ever added to the heap
	Top10Heap() : MinHeap<X,INDEX>(10) {}
	//! Constructor generating a heap which holds the \a size elements with highest value ever added to the heap
	Top10Heap(INDEX size) : MinHeap<X,INDEX>(size) {}

	//! Returns true if the heap contains no elements
	bool empty() const { return MinHeap<X,INDEX>::empty(); }
	//! Returns true if the heap is completely filled (i.e. the next push operation will return something)
	bool full() const { return size() == capacity(); }
	//! Returns the number of elements in the heap
	INDEX size() const { return MinHeap<X,INDEX>::size(); }
	//! Returns the size of the heap specified when constructing: this is the number of top elements stored.
	INDEX capacity() const { return MinHeap<X,INDEX>::capacity(); }

	//! empties the heap
	void clear() { MinHeap<X,INDEX>::clear(); }

	//! Tries to push the element \a x onto the heap (and may return a removed element as \a out).
	/**
	 * If the heap is not yet completely filled, the pushed element is accepted and added to the heap.
	 * The function returns \a Accepted, and the \a out parameter is not touched.
	 * 
	 * If the heap is filled and the key of the pushed element is too small to be accepted 
	 * (i.e. the heap is filled with all larger elements), then the element if rejected: The funtion 
	 * returns \a Rejected, and the \a out parameter is set to \x.
	 * 
	 * If the heap is filled and the key of the pushed element is large enough to belong to the top 
	 * elements, the element is accepted and the currently smallest element in the heap is removed 
	 * from the heap. The function returns \a Swapped and sets the \out parameter to the element
	 * removed from the heap.
	 * 
	 * You may want to use the convenience funtions \a successful and \a returnedSomething on the 
	 * return-value if you are only interested certain aspects of the push.
	 */
	PushResult push(X& x, X& out) { // returns element that got kicked out - out is uninitialized if heap wasn't full (i.e. PushResult equals Accepted)
		PushResult ret = Accepted;
		if(capacity() == size()) {
			if(MinHeap<X,INDEX>::top() >= x) {// reject new item since it's too bad
				out = x;
				return Rejected;
			}
			out = MinHeap<X,INDEX>::pop(); // remove worst first
			ret = Swapped;
		}
		MinHeap<X,INDEX>::push(x);
		return ret;
	}
	
	//! Simple (and slightly faster) variant of Top10Heap::push.
	/**
	 * The behavior is the identical to Top10Heap::push, but there is nothing reported to the outside
	 */
	void pushBlind(X& x) {
		if(capacity() == size()) {
			if(MinHeap<X,INDEX>::top() >= x) // reject new item since it's too bad
				return;
			MinHeap<X,INDEX>::pop(); // remove worst first
		}
		MinHeap<X,INDEX>::push(x);
	}
	
	//! obtain const references to the element at index \a idx
	/**
	 * The smallest array index is 0, as for traditional C-arrays.
	 * Useful, e.g., when iterating through the final heap elements.
	 */
	const X& operator[](INDEX idx) const { // ATTN: simulate index starting at 0, to be "traditional" to the outside!!!
		return MinHeap<X,INDEX>::operator[](idx);
	}
};

} // end namespace ogdf


#endif
