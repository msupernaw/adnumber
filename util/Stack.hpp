/* 
 * File:   Stack.hpp
 * Author: matthewsupernaw
 *
 * Created on March 21, 2014, 9:52 AM
 */

#ifndef STACK_HPP
#define	STACK_HPP
#include <stdlib.h>
#include "clfmalloc.h"

namespace ad{
    
    
    
    
        template <class T>
        class Stack {
        public:
            T* st;
            int allocationSize;
            int lastIndex;
    
        public:
            Stack(int stackSize = 1000000);
 
            ~Stack();
    
            inline void resize(int newSize);
            inline void push(const T &x);
            inline void pop();
            inline T& getAndRemove();
            inline T& top();
            inline void clear();
    
            inline const int empty() {
                return this->lastIndex == -1;
            }
        };
    
        //template <class T>
        //FastStack<T>::FastStack() {
        //    lastIndex = -1;
        //    st = NULL;
        //}
    
        template <class T>
        Stack<T>::Stack(int stackSize) {
            st = NULL;
            this->allocationSize = stackSize;
            st = (T*) malloc(sizeof (T) * stackSize);
            lastIndex = -1;
        }
    
        template <class T>
        Stack<T>::~Stack() {
            free(st);
        }
    
        template <class T>
        void Stack<T>::clear() {
            lastIndex = -1;
        }
    
        template <class T>
        T& Stack<T>::top() {
    
            return st[lastIndex];
        }
    
        template <class T>
        T& Stack<T>::getAndRemove() {
            return st[lastIndex--];
        }
    
        template <class T>
        void Stack<T>::pop() {
            --lastIndex;
        }
    
        template <class T>
        void Stack<T>::push(const T &x) {
            if (++lastIndex == (this->allocationSize)) {
    
                this->allocationSize += this->allocationSize;
                st = (T *) realloc(st, this->allocationSize * sizeof (T));
                //        free(st);
                //        this->st = temp;
            }
    
            st[lastIndex] = x;
        }
    
        template <class T>
        void Stack<T>::resize(int newSize) {
            if (st != NULL)
                delete [] st;
            st = new T[newSize];
        }
    
    
    
}


#endif	/* STACK_HPP */

