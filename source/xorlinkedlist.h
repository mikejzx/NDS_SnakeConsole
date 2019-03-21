template<class T> struct Node {
	T data;
	Node<T>* both = NULL; // Xor of previous and next node

	//Node (T d) : data(d) {}
	virtual void SetData (T newData) {
		data = newData;
	}


	Node<T>() {}
	virtual ~Node<T> () {}
};

// XOR linked list implementation.
template<class T> class NodeCollection {
	public:
		Node<T>* head, *tail;
		int length = 0;

		static Node<T>* Xor (Node<T>* a, Node<T>* b) {
			return (Node<T>*)((uintptr_t)a ^ (uintptr_t)b);
		}

		// Re-initialises(clears) the list.
		void Reinitialise () {
			if (length > 0) {
				Node<T>* cur = head, *prev = NULL, *next;
				while (1) {
					next = Xor(prev, cur->both);
					if (prev != NULL) { delete prev; }
					prev = cur;
					cur = next;
					if (cur == NULL) { break; }
				}
				if (prev != NULL) { delete prev; }
			}
			length = 0; head = NULL; tail = NULL;
		}

		// Appends to end of list
		void Append(T data) {
			Node<T>* temp = new Node<T>();
			temp->SetData(data);
			if (length > 0) { 
				temp->both = Xor(tail, NULL); 
				tail->both = Xor(temp, Xor(tail->both, NULL)); 
			}
			else { head =  temp; }
			tail = temp;
			++length;
		}

		// Prepends to beginning of list
		void Prepend(T data) {
			Node<T>* temp = new Node<T>();
			temp->SetData(data);
			if (length > 0) { 
				temp->both = Xor(head, NULL);
				head->both = Xor(temp, Xor(head->both, NULL)); 
			}
			else { tail = temp; }
			head = temp;
			++length;
		}

		Node<T>* Get(int idx) {
			Node<T>* cur = head, *prev = NULL, *next;
			int i = 0;
			while (i < length) {
				if (i == idx) { 
					return cur;
				}
				next = Xor(prev, cur->both);
				prev = cur; cur = next;
				if (cur == NULL) { break; } ++i;
			}
			return 0;
		}

		Node<T>* operator[] (int idx) { return Get(idx); }

		int IndexOf(T* node) {
			Node<T>* cur = head, *prev = NULL, *next;
			int i = 0;
			while (i < length) {
				if (node == &(cur->data)) {
					return i;
				}
				next = Xor(prev, cur->both);
				prev = cur; cur = next;
				if (cur == NULL) { break; } ++i;
			}
			return 0;
		}

		// Using a lambda with this function is usually pretty useless.
		// Instead POINT to a method. Can't believe took me so long to realise this...
		void TraverseForward (void(*function)(T, int idx)) {
			Node<T>* cur = head, *prev = NULL, *next;
			int i = 0;
			while (i < length) {
				function(cur->data, i);
				// Next node address is XOR of prev & cur
				next = Xor(prev, cur->both);
				prev = cur; cur = next;
				if (cur == NULL) { break; } ++i;
			}
		}

		// This algorithm took a lot of effort to come up with!
		// Version 3.2 (14.03.2019)
		void RemoveAt(int idx) {
			Node<T>* cur = head, *prev = NULL, *next = Xor(NULL, head->both);
			Node<T>* target = NULL; // Node to be removed.
			int i = 0;

			while (1) {
				// Node to be removed
				if (i == idx) {
					target = cur;
					next = Xor(prev, cur->both);
					if (prev != NULL) { prev->both = Xor(Xor(prev->both, target), next); }
					if (next != NULL) { next->both = Xor(prev, Xor(next->both, target)); }

					// Re-assign tail if necessary
					if (cur == head) { head = next; }
					else if (cur == tail) { tail = prev; }

					--length;
					delete cur; // Not sure if this is correct xD
					break;
				}

				next = Xor(prev, cur->both);
				prev = cur; 
				cur = next;
				if (cur == NULL) { break; } ++i;
			}
		}
};