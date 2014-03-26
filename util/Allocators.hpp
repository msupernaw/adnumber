/* 
 * File:   Allocators.hpp
 * Author: matthewsupernaw
 *
 * Created on March 21, 2014, 3:08 PM
 */

#ifndef ALLOCATORS_HPP
#define	ALLOCATORS_HPP


namespace ad {

    template <typename T>
    class base_allocator {
    public:
        typedef size_t size_type;
        typedef ptrdiff_t difference_type;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T& reference;
        typedef const T& const_reference;
        typedef T value_type;

        base_allocator() {
        }

        base_allocator(const base_allocator<T>&) {
        }

        ~base_allocator() {
        }

        template <class U> base_allocator(const base_allocator<U>&) {
        }

        template <class U> struct rebind {
            typedef base_allocator<U> other;
        };

        T* address(T&) const {
        }

        const T* address(const T&) const {
        }

        T* allocate(size_type, const void* = NULL) {
        }

        void deallocate(T*, size_type) {
        }

        void construct(T*, const T&) {
        }

        void destroy(T*) {
        }

        size_type max_size() const {
        }
    };

    template<class T>
    bool operator ==(const base_allocator<T>&, const base_allocator<T>&);

    template<class T>
    bool operator !=(const base_allocator<T>&, const base_allocator<T>&);


}

#endif	/* ALLOCATORS_HPP */

