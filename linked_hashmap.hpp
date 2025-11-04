/**
 * implement a container like std::linked_hashmap
 */
#ifndef SJTU_LINKEDHASHMAP_HPP
#define SJTU_LINKEDHASHMAP_HPP

// only for std::equal_to<T> and std::hash<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {
    /**
     * In linked_hashmap, iteration ordering is differ from map,
     * which is the order in which keys were inserted into the map.
     * You should maintain a doubly-linked list running through all
     * of its entries to keep the correct iteration order.
     *
     * Note that insertion order is not affected if a key is re-inserted
     * into the map.
     */

template<
	class Key,
	class T,
	class Hash = std::hash<Key>,
	class Equal = std::equal_to<Key>
> class linked_hashmap {
public:
	/**
	 * the internal type of data.
	 * it should have a default constructor, a copy constructor.
	 * You can use sjtu::linked_hashmap as value_type by typedef.
	 */
	typedef pair<const Key, T> value_type;

private:
	// Node structure for doubly-linked list
	struct Node {
		value_type *data;
		Node *prev;
		Node *next;

		Node() : data(nullptr), prev(nullptr), next(nullptr) {}
		Node(const value_type &val) : data(new value_type(val)), prev(nullptr), next(nullptr) {}
		~Node() {
			if (data) delete data;
		}
	};

	// Hash table bucket structure
	struct Bucket {
		Node *node;
		Bucket *next;

		Bucket() : node(nullptr), next(nullptr) {}
		Bucket(Node *n) : node(n), next(nullptr) {}
	};

	// Hash table
	Bucket **table;
	size_t table_size;
	size_t element_count;

	// Doubly-linked list for insertion order
	Node *head;  // dummy head
	Node *tail;  // dummy tail

	Hash hasher;
	Equal equal;

	static const size_t INITIAL_CAPACITY = 16;
	static constexpr double LOAD_FACTOR = 0.75;

	// Helper functions
	size_t get_bucket_index(const Key &key) const {
		return hasher(key) % table_size;
	}

	void init_table(size_t size) {
		table_size = size;
		table = new Bucket*[table_size];
		for (size_t i = 0; i < table_size; ++i) {
			table[i] = nullptr;
		}
	}

	void clear_table() {
		if (table) {
			for (size_t i = 0; i < table_size; ++i) {
				Bucket *bucket = table[i];
				while (bucket) {
					Bucket *next = bucket->next;
					delete bucket;
					bucket = next;
				}
			}
			delete[] table;
			table = nullptr;
		}
	}

	void clear_list() {
		Node *current = head->next;
		while (current != tail) {
			Node *next = current->next;
			delete current;
			current = next;
		}
		head->next = tail;
		tail->prev = head;
	}

	void rehash() {
		size_t new_size = table_size * 2;
		Bucket **new_table = new Bucket*[new_size];
		for (size_t i = 0; i < new_size; ++i) {
			new_table[i] = nullptr;
		}

		// Rehash all elements
		Node *current = head->next;
		while (current != tail) {
			size_t index = hasher(current->data->first) % new_size;
			Bucket *new_bucket = new Bucket(current);
			new_bucket->next = new_table[index];
			new_table[index] = new_bucket;
			current = current->next;
		}

		// Clean up old table
		clear_table();
		table = new_table;
		table_size = new_size;
	}

	Node* find_node(const Key &key) const {
		size_t index = get_bucket_index(key);
		Bucket *bucket = table[index];
		while (bucket) {
			if (equal(bucket->node->data->first, key)) {
				return bucket->node;
			}
			bucket = bucket->next;
		}
		return nullptr;
	}

	void insert_to_list(Node *node) {
		node->prev = tail->prev;
		node->next = tail;
		tail->prev->next = node;
		tail->prev = node;
	}

	void remove_from_list(Node *node) {
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}

	void insert_to_table(Node *node) {
		size_t index = get_bucket_index(node->data->first);
		Bucket *new_bucket = new Bucket(node);
		new_bucket->next = table[index];
		table[index] = new_bucket;
	}

	void remove_from_table(const Key &key) {
		size_t index = get_bucket_index(key);
		Bucket *bucket = table[index];
		Bucket *prev = nullptr;

		while (bucket) {
			if (equal(bucket->node->data->first, key)) {
				if (prev) {
					prev->next = bucket->next;
				} else {
					table[index] = bucket->next;
				}
				delete bucket;
				return;
			}
			prev = bucket;
			bucket = bucket->next;
		}
	}

public:

	/**
	 * see BidirectionalIterator at CppReference for help.
	 *
	 * if there is anything wrong throw invalid_iterator.
	 *     like it = linked_hashmap.begin(); --it;
	 *       or it = linked_hashmap.end(); ++end();
	 */
	class const_iterator;
	class iterator {
	private:
		Node *node;
		const linked_hashmap *map;

		friend class linked_hashmap;
		friend class const_iterator;

	public:
		// The following code is written for the C++ type_traits library.
		// Type traits is a C++ feature for describing certain properties of a type.
		// For instance, for an iterator, iterator::value_type is the type that the
		// iterator points to.
		// STL algorithms and containers may use these type_traits (e.g. the following
		// typedef) to work properly.
		// See these websites for more information:
		// https://en.cppreference.com/w/cpp/header/type_traits
		// About value_type: https://blog.csdn.net/u014299153/article/details/72419713
		// About iterator_category: https://en.cppreference.com/w/cpp/iterator
		using difference_type = std::ptrdiff_t;
		using value_type = typename linked_hashmap::value_type;
		using pointer = value_type*;
		using reference = value_type&;
		using iterator_category = std::output_iterator_tag;


		iterator() : node(nullptr), map(nullptr) {}

		iterator(Node *n, const linked_hashmap *m) : node(n), map(m) {}

		iterator(const iterator &other) : node(other.node), map(other.map) {}

		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
			if (!node || node == map->tail) {
				throw invalid_iterator();
			}
			iterator temp = *this;
			node = node->next;
			return temp;
		}
		/**
		 * TODO ++iter
		 */
		iterator & operator++() {
			if (!node || node == map->tail) {
				throw invalid_iterator();
			}
			node = node->next;
			return *this;
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {
			if (!node || node->prev == map->head) {
				throw invalid_iterator();
			}
			iterator temp = *this;
			node = node->prev;
			return temp;
		}
		/**
		 * TODO --iter
		 */
		iterator & operator--() {
			if (!node || node->prev == map->head) {
				throw invalid_iterator();
			}
			node = node->prev;
			return *this;
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		value_type & operator*() const {
			return *(node->data);
		}
		bool operator==(const iterator &rhs) const {
			return node == rhs.node && map == rhs.map;
		}
		bool operator==(const const_iterator &rhs) const {
			return node == rhs.node && map == rhs.map;
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
			return !(*this == rhs);
		}
		bool operator!=(const const_iterator &rhs) const {
			return !(*this == rhs);
		}

		/**
		 * for the support of it->first.
		 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
		 */
		value_type* operator->() const noexcept {
			return node->data;
		}
	};

	class const_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
		private:
			Node *node;
			const linked_hashmap *map;

			friend class linked_hashmap;
			friend class iterator;

		public:
			using difference_type = std::ptrdiff_t;
			using value_type = typename linked_hashmap::value_type;
			using pointer = const value_type*;
			using reference = const value_type&;
			using iterator_category = std::output_iterator_tag;

			const_iterator() : node(nullptr), map(nullptr) {}

			const_iterator(Node *n, const linked_hashmap *m) : node(n), map(m) {}

			const_iterator(const const_iterator &other) : node(other.node), map(other.map) {}

			const_iterator(const iterator &other) : node(other.node), map(other.map) {}

			const_iterator operator++(int) {
				if (!node || node == map->tail) {
					throw invalid_iterator();
				}
				const_iterator temp = *this;
				node = node->next;
				return temp;
			}

			const_iterator & operator++() {
				if (!node || node == map->tail) {
					throw invalid_iterator();
				}
				node = node->next;
				return *this;
			}

			const_iterator operator--(int) {
				if (!node || node->prev == map->head) {
					throw invalid_iterator();
				}
				const_iterator temp = *this;
				node = node->prev;
				return temp;
			}

			const_iterator & operator--() {
				if (!node || node->prev == map->head) {
					throw invalid_iterator();
				}
				node = node->prev;
				return *this;
			}

			const value_type & operator*() const {
				return *(node->data);
			}

			bool operator==(const iterator &rhs) const {
				return node == rhs.node && map == rhs.map;
			}

			bool operator==(const const_iterator &rhs) const {
				return node == rhs.node && map == rhs.map;
			}

			bool operator!=(const iterator &rhs) const {
				return !(*this == rhs);
			}

			bool operator!=(const const_iterator &rhs) const {
				return !(*this == rhs);
			}

			const value_type* operator->() const noexcept {
				return node->data;
			}
	};

	/**
	 * TODO two constructors
	 */
	linked_hashmap() : table(nullptr), table_size(0), element_count(0) {
		head = new Node();
		tail = new Node();
		head->next = tail;
		tail->prev = head;
		init_table(INITIAL_CAPACITY);
	}

	linked_hashmap(const linked_hashmap &other) : table(nullptr), table_size(0), element_count(0) {
		head = new Node();
		tail = new Node();
		head->next = tail;
		tail->prev = head;
		init_table(other.table_size);

		// Copy all elements in insertion order
		Node *current = other.head->next;
		while (current != other.tail) {
			insert(*(current->data));
			current = current->next;
		}
	}

	/**
	 * TODO assignment operator
	 */
	linked_hashmap & operator=(const linked_hashmap &other) {
		if (this == &other) return *this;

		// Clear current content
		clear();
		clear_table();

		// Reinitialize with other's size
		init_table(other.table_size);

		// Copy all elements
		Node *current = other.head->next;
		while (current != other.tail) {
			insert(*(current->data));
			current = current->next;
		}

		return *this;
	}

	/**
	 * TODO Destructors
	 */
	~linked_hashmap() {
		clear_list();
		clear_table();
		delete head;
		delete tail;
	}

	/**
	 * TODO
	 * access specified element with bounds checking
	 * Returns a reference to the mapped value of the element with key equivalent to key.
	 * If no such element exists, an exception of type `index_out_of_bound'
	 */
	T & at(const Key &key) {
		Node *node = find_node(key);
		if (!node) {
			throw index_out_of_bound();
		}
		return node->data->second;
	}

	const T & at(const Key &key) const {
		Node *node = find_node(key);
		if (!node) {
			throw index_out_of_bound();
		}
		return node->data->second;
	}

	/**
	 * TODO
	 * access specified element
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 *   performing an insertion if such key does not already exist.
	 */
	T & operator[](const Key &key) {
		Node *node = find_node(key);
		if (!node) {
			// Insert with default value
			value_type new_pair(key, T());
			auto result = insert(new_pair);
			return result.first->second;
		}
		return node->data->second;
	}

	/**
	 * behave like at() throw index_out_of_bound if such key does not exist.
	 */
	const T & operator[](const Key &key) const {
		Node *node = find_node(key);
		if (!node) {
			throw index_out_of_bound();
		}
		return node->data->second;
	}

	/**
	 * return a iterator to the beginning
	 */
	iterator begin() {
		return iterator(head->next, this);
	}

	const_iterator cbegin() const {
		return const_iterator(head->next, this);
	}

	/**
	 * return a iterator to the end
	 * in fact, it returns past-the-end.
	 */
	iterator end() {
		return iterator(tail, this);
	}

	const_iterator cend() const {
		return const_iterator(tail, this);
	}

	/**
	 * checks whether the container is empty
	 * return true if empty, otherwise false.
	 */
	bool empty() const {
		return element_count == 0;
	}

	/**
	 * returns the number of elements.
	 */
	size_t size() const {
		return element_count;
	}

	/**
	 * clears the contents
	 */
	void clear() {
		clear_list();
		// Also clear the hash table buckets
		for (size_t i = 0; i < table_size; ++i) {
			Bucket *bucket = table[i];
			while (bucket) {
				Bucket *next = bucket->next;
				delete bucket;
				bucket = next;
			}
			table[i] = nullptr;
		}
		element_count = 0;
	}

	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion),
	 *   the second one is true if insert successfully, or false.
	 */
	pair<iterator, bool> insert(const value_type &value) {
		// Check if key already exists
		Node *existing = find_node(value.first);
		if (existing) {
			return pair<iterator, bool>(iterator(existing, this), false);
		}

		// Check if rehash is needed
		if (element_count >= table_size * LOAD_FACTOR) {
			rehash();
		}

		// Create new node
		Node *new_node = new Node(value);

		// Insert into linked list
		insert_to_list(new_node);

		// Insert into hash table
		insert_to_table(new_node);

		element_count++;

		return pair<iterator, bool>(iterator(new_node, this), true);
	}

	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
	void erase(iterator pos) {
		if (pos.map != this || pos.node == tail || pos.node == head || !pos.node) {
			throw invalid_iterator();
		}

		// Remove from hash table
		remove_from_table(pos.node->data->first);

		// Remove from linked list
		remove_from_list(pos.node);

		// Delete node
		delete pos.node;

		element_count--;
	}

	/**
	 * Returns the number of elements with key
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0
	 *     since this container does not allow duplicates.
	 */
	size_t count(const Key &key) const {
		return find_node(key) ? 1 : 0;
	}

	/**
	 * Finds an element with key equivalent to key.
	 * key value of the element to search for.
	 * Iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */
	iterator find(const Key &key) {
		Node *node = find_node(key);
		if (node) {
			return iterator(node, this);
		}
		return end();
	}

	const_iterator find(const Key &key) const {
		Node *node = find_node(key);
		if (node) {
			return const_iterator(node, this);
		}
		return cend();
	}
};

}

#endif
