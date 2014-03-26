/* 
 * File:   Pool.hpp
 * Author: matthewsupernaw
 *
 * Created on March 20, 2014, 11:54 AM
 */

#ifndef POOL_HPP
#define	POOL_HPP

#include <stdlib.h>
#include <vector>
#include <iostream>


namespace ad {

    template<class T>
    class Pool {
        std::vector<T> pool; //actual heap of objects
        std::vector<T* > free_list; //available objects
        long long index;
        size_t size_m, next_size_m;
        Pool<T >* next_m;

        inline void* extend() {
            if (next_m == NULL) {
                //             std::cout<<"extending...               \r";
                this->next_m = new Pool<T> ( this->next_size_m );
            }

            return next_m->malloc();
        }

    public:

        Pool(uint32_t size) : next_size_m(size), size_m(size),pool(size), free_list(size), index(size - 1), next_m(NULL) {
//            std::cout<<__func__<<std::endl;
//            pool = new T[size];
            for (int i = 0; i < size; i++) {
                free_list[i] = (&pool[i]);
            }
        }

        ~Pool() {
            //        free_list.clear();

            if (next_m != NULL) {
                delete next_m;
            }
//            delete[] pool;
        }
        
        void SetResize(uint32_t resize){
            this->next_size_m = resize;
        }
        
        
        inline void* malloc() {
            if (index <= 0) {

                return this->extend();
            }
            return free_list[index--];
        }

        inline void free(void* ptr) {

            if (ptr < &pool[0] || ptr > &pool[size_m - 1]) {
                if(next_m != NULL){
                    next_m->free(ptr);
                }else{
                    std::cout<<"pointer not freed...\n";
                }
             
            } else {
                if(ptr)
                free_list[++index] = (T*) ptr;
            }

        }

    };
}
#endif	/* POOL_HPP */

