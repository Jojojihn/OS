#ifndef List_h
#define List_h

///An element to be used in a list
template <typename T>
class ListElem {
    
    
    private:
        T* value;

        ListElem<T> *next;


    protected:
        ListElem() : value(nullptr), next(nullptr) {}

    public:
        explicit ListElem(T *value) : value(value), next(nullptr) {}
        
        ~ListElem() {}

        T *getValue() {
            return value;
        }

        ListElem<T> *getNext() {
            return next;
        }

        void setNext(ListElem<T> *newNext) {
            next = newNext;
        }


};


/**
 * A single linked list.
 * 
 * @tparam T The type of object to store.
 */
template <typename T>
class List {
    private: 
        ///A Pointer used to iterate through the list
        ListElem<T> *iterator;

        ///The head of the list (first element)
        ListElem<T> *head;

        ///Length of the list, one element means a length of one
        unsigned int length;

        ///Gets the ListElement at the specified index
        ListElem<T> *getElem(int index) {
            ListElem<T> *temp = nullptr;
            index = max(0, index);
            if((unsigned int) index < length) {
                temp = head;
   
                for(int i = 0; i < index; i++) {
                    temp = temp->getNext();
                }
            }

            return temp;
        }
    
    public:
        List() : iterator(nullptr), head(nullptr), length(0) {};

        ~List() {
            clear(false);
        }

        /**
         * Gets a pointer to the object at the specified index.
         * 
         * @param index The index of the object
         */
        T* get(int index) {
            return getElem(index)->getValue();
        } 


        /**
         * Starts an iteration. Call iterate() to iterate.
         * 
         * @warning Only one iterator can be used on a list object at a time.
         * 
         * @param startIndex The index of the object to start the iteration on (Clamped to the size of the list)
         */
        void startIteration(unsigned int startIndex) {
            startIndex = max(0, min(length - 1, startIndex));

            if(startIndex == 0) {
                iterator = nullptr;
            } else {
                iterator = getElem(startIndex - 1);
            }
            
        }
        /**
         * Starts an iteration. Call iterate() to iterate.
         * 
         * @warning Only one iterator can be used on a list object at a time.
         * 
         */
        void startIteration() {
            startIteration(0);
        }

        /**
         * This will return a Pointer to the first element of the List when called after startIteration, then the second, etc..
         * 
         * @return The next element, or nullptr if the end has been reached
         */
        T* iterate() {
            if(iterator == nullptr) {
                iterator = head;
                if(iterator == nullptr) {
                    return nullptr;
                } else {
                    return iterator->getValue();
                }
            } else {
                iterator = iterator->getNext();
                return iterator->getValue();
            }    
        }


        /**
         * Adds an object to the List.
         * 
         * @param object Pointer to an object in the heap (Initialize with new! Otherwise the object might get removed once it leaves the current scope)
         */
        void add(T *object) {
            ListElem<T> *newElem = new ListElem<T>(object);
            if(length <= 0) {
                head = newElem;
            } else {
                ListElem<T>* lastElem = getElem(length - 1);
                lastElem->setNext(newElem);
            }
        
            length++;
        }

        /**
         * Removes an object from the List, if found.
         * 
         * @param object A Pointer to the object to remove
         * 
         * @param deleteObject Whether to delete this object from memory. Removing without deleting will result in a memory leak
         *                     if you do not have another pointer to it. Deleting while still having another pointer will make that
         *                     pointer point at garbage.
         */
        void remove(T *object, bool deleteObject) {
            ListElem<T> *temp;
            temp = head;
            if(temp->getValue() == object) {
                head = temp->getNext();
                length--;
                if(deleteObject) {
                    delete temp->getValue();
                } 
                delete temp;

            } else {
                for(unsigned int i = 0; i < length; i++) {
                
                    if(temp->getNext()->getValue() == object) {
                        ListElem<T> *pendingDelete = temp->getNext();            
                        temp->setNext(pendingDelete->getNext());
                        length--;
                        if(deleteObject) {
                            delete pendingDelete->getValue();
                        }
                        delete pendingDelete;
                        break;
                    }
                    temp = temp->getNext();
                }
            }
        }

        /**
         * Clears the list
         * 
         * @param deleteObjects Whether to delete the objects contained in this list from memory. Removing without deleting will result in a memory leak
         *                      if you do not have another pointer to it. Deleting while still having another pointer will make that
         *                      pointer point at garbage.
         */
        void clear(bool deleteObjects) {
            ListElem<T> *tempElem = head;
            
            for(unsigned int i = 0; i < length; i++) {
                ListElem<T> *nextElem = tempElem->getNext();
                if(deleteObjects) {
                    delete tempElem->getValue();
                }
                delete tempElem;
                tempElem = nextElem;
            }
            length = 0;
        }

        /**
         * Checks if the specified object is contained in this list.
         * 
         * @param object A Pointer to the object to check for
         */
        bool contains(T *object) {
            for(unsigned int i = 0; i < length; i++) {
                if(getElem(i)->getValue() == object) {
                    return true;
                }
            }

            return false;
        }

        /**
         * A size of one means there is one element in the list.
         * 
         * @return The size of the list
         */
        unsigned int size() {
            return length;
        }
};

#endif