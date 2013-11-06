/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_UPDATABLEHEAP_H_
#define UTIL_UPDATABLEHEAP_H_

#include <cstddef>
#include <limits>
#include <list>
#include <utility>

namespace Util {

template<typename Key_t, typename Data_t>
class UpdatableHeap {
	public:
		class UpdatableHeapElement{
			private:
				UpdatableHeapElement *parent;     // needed for heap
				UpdatableHeapElement *h1;
				UpdatableHeapElement *h2;

				Key_t cost;
				bool dummy;              // marks element as dummy

				UpdatableHeapElement(const UpdatableHeapElement &) = delete;
				UpdatableHeapElement(UpdatableHeapElement &&) = delete;
				UpdatableHeapElement & operator=(const UpdatableHeapElement &) = delete;
				UpdatableHeapElement & operator=(UpdatableHeapElement &&) = delete;
			public:
				UpdatableHeapElement(UpdatableHeapElement * parentElement) :
					parent(parentElement), h1(nullptr), h2(nullptr), cost(std::numeric_limits<Key_t>::max()), dummy(true), data() {
				}
				Data_t data;

				Key_t getCost() const {
					return cost;
				}

				friend class UpdatableHeap;
		};

	private:
		size_t elementCount;
		std::list<UpdatableHeapElement *> heapTail;
		UpdatableHeapElement * root;

		//! Move @p h downward in the heap. Call after increasing cost.
		inline void moveDownHeap(UpdatableHeapElement * h);

		//! Move @p h upward in the heap. Call after decreasing cost.
		inline void moveUpHeap(UpdatableHeapElement * h);

		//! Swap heap elements @p hp and @p hc where @p hc is a child of @p hp
		inline static void swapHeapElements(UpdatableHeapElement * hp, UpdatableHeapElement * hc);

		UpdatableHeap(const UpdatableHeap &) = delete;
		UpdatableHeap(UpdatableHeap &&) = delete;
		UpdatableHeap& operator=(const UpdatableHeap &) = delete;
		UpdatableHeap& operator=(UpdatableHeap &&) = delete;

	public:
		UpdatableHeap() : elementCount(0), heapTail(), root(new UpdatableHeapElement(nullptr)) {
			heapTail.push_back(root);
		}

		~UpdatableHeap() {
			clear();
			delete root;
		}

		/**
		 * Add a new element to the heap keyed on cost
		 * @return UpdatableHeapElement representing these data in the heap, needed for update etc.
		 */
		inline UpdatableHeapElement * insert(Key_t cost, const Data_t & data);

		//! Delete @p h from heap. Make sure @p h is from this heap!
		inline void erase(UpdatableHeapElement * h);

		//! Delete top element from heap
		void pop() {
			erase(root);
		}

		//! Delete all elements from heap
		void clear() {
			// TODO can be made more efficient
			while(size() != 0) {
				pop();
			}
		}

		//! Update cost of @p h to newCost keeping heap in order
		void update(UpdatableHeapElement * h, Key_t newCost) {
			bool moveUp = newCost < h->cost;
			h->cost = newCost;
			if(moveUp) {
				moveUpHeap(h);
			} else {
				moveDownHeap(h);
			}
		}

		//! Return pointer to top element (root)
		UpdatableHeapElement * top() const {
			return root;
		}

		//! Return number of elements in heap
		size_t size() const {
			return elementCount;
		}
};

template<typename Key_t, typename Data_t>
inline typename UpdatableHeap<Key_t, Data_t>::UpdatableHeapElement *
UpdatableHeap<Key_t, Data_t>::insert(Key_t cost, const Data_t & data){
	++elementCount;

	UpdatableHeapElement *h = heapTail.front();
	heapTail.pop_front();

	// store data
	h->dummy = false;
	h->cost = cost;
	h->data = data;

	// create two new dummy children
	h->h1 = new UpdatableHeapElement(h);
	heapTail.push_back(h->h1);
	h->h2 = new UpdatableHeapElement(h);
	heapTail.push_back(h->h2);

	moveUpHeap(h);
	return h;
}

template<typename Key_t, typename Data_t>
inline void UpdatableHeap<Key_t, Data_t>::erase(UpdatableHeapElement* h){
	--elementCount;

	UpdatableHeapElement *tmp, *last;
	tmp = heapTail.back();
	heapTail.pop_back();
	delete tmp;
	tmp = heapTail.back();
	heapTail.pop_back();
	heapTail.push_front(tmp);

	// swap last and h
	const Key_t oldKey = h->cost;
	last = tmp->parent;
	if(last!=h){
		// use dummy to replace "last"
		if(last->parent->h1 == last){
			last->parent->h1 = tmp;
		}else{
			last->parent->h2 = tmp;
		}
		tmp->parent = last->parent;

		last->h1 = h->h1;
		last->h1->parent = last;

		last->h2 = h->h2;
		last->h2->parent = last;

		last->parent = h->parent;
		if(last->parent!=nullptr){
			if(last->parent->h1 == h){
				last->parent->h1 = last;
			}else{
				last->parent->h2 = last;
			}
		}
	}else{
		if(h->parent!=nullptr){
			if(h->parent->h1 == h){
				h->parent->h1 = tmp;
			}else{
				h->parent->h2 = tmp;
			}
		}
		tmp->parent = h->parent;
		last = tmp;
	}

	if(root==h)
		root = last;

	delete h;

	if(last->getCost() < oldKey) {
		moveUpHeap(last);
	} else {
		moveDownHeap(last);
	}
}

template<typename Key_t, typename Data_t>
inline void UpdatableHeap<Key_t, Data_t>::moveDownHeap(UpdatableHeapElement* h){
	if(h->dummy)
		return;
	if(h==root){
		if(!h->h1->dummy){
			if(!h->h2->dummy){
				// this has two children
				if(h->cost > h->h1->cost && h->h2->cost >= h->h1->cost){
					// swap this and h1
					root = h->h1;
					swapHeapElements(h, h->h1);
				}else{
					if(h->cost > h->h2->cost){
						// swap this and h2
						root = h->h2;
						swapHeapElements(h, h->h2);
					}
				}
			}else{
				// this has one child
				if(h->cost > h->h1->cost){
					// swap this and h1
					root = h->h1;
					swapHeapElements(h, h->h1);
				}
			}
		}
	}
	bool moved;
	do{
		moved = false;
		if(!h->h1->dummy){
			if(!h->h2->dummy){
				// this has two children
				if(h->cost > h->h1->cost && h->h2->cost >= h->h1->cost){
					// swap this and h1
					moved = true;
					swapHeapElements(h, h->h1);
				}else{
					if(h->cost > h->h2->cost){
						// swap this and h2
						moved = true;
						swapHeapElements(h, h->h2);
					}
				}
			}else{
				// this has one child
				if(h->cost > h->h1->cost){
					// swap this and h1
					moved = true;
					swapHeapElements(h, h->h1);
				}
			}
		}
	}while(moved);
}

template<typename Key_t, typename Data_t>
inline void UpdatableHeap<Key_t, Data_t>::moveUpHeap(UpdatableHeapElement* h){
	if(h->dummy)
		return;
	bool moved;
	do{
		moved = false;
		if(h->parent!=nullptr && h->parent->cost > h->cost){
			// parent has higher cost -> swap
			moved = true;
			swapHeapElements(h->parent, h);
		}
	}while(moved);

	if(h->parent == nullptr)
		root = h;
}

template<typename Key_t, typename Data_t>
inline void UpdatableHeap<Key_t, Data_t>::swapHeapElements(UpdatableHeapElement *hp, UpdatableHeapElement *hc){
	if(hp->h1 == hc) {
		/*      (hp)
		 *   h1 /  \ h2
		 *     /   \
		 *   (hc) (  )
		 */
		// swap child h1
		hc->h1->parent = hp;
		hp->h1 = hc->h1;
		hc->h1 = hp;

		// swap child h2
		hp->h2->parent = hc;
		hc->h2->parent = hp;
		using std::swap;
		swap(hp->h2, hc->h2);
	} else {
		/*      (hp)
		 *   h1 /  \ h2
		 *     /   \
		 *   (  ) (hc)
		 */
		// swap child h2
		hc->h2->parent = hp;
		hp->h2 = hc->h2;
		hc->h2 = hp;

		// swap child h1
		hp->h1->parent = hc;
		hc->h1->parent = hp;
		using std::swap;
		swap(hp->h1, hc->h1);
	}

	// set hc as new child of hp's parent
	if(hp->parent != nullptr) {
		if(hp->parent->h1 == hp){
			hp->parent->h1 = hc;
		} else {
			hp->parent->h2 = hc;
		}
	}

	// set hp's and hc's parents
	hc->parent = hp->parent;
	hp->parent = hc;
}

}

#endif /* UTIL_UPDATABLEHEAP_H_ */
