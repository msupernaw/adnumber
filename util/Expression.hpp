/* 
 * File:   Expression.hpp
 * Author: matthewsupernaw
 *
 * Created on March 20, 2014, 11:54 AM
 */

#ifndef EXPRESSION_HPP
#define	EXPRESSION_HPP

#include <stdlib.h>
#include <assert.h>
#include <stack>
#include <string>
#include <sstream>
#include <cmath>
#include <vector>
#include <iostream>



#include "Stack.hpp"


#define USE_POOL

#ifdef USE_POOL

#ifndef DEFAULT_POOL_SIZE
#define DEFAULT_POOL_SIZE 10000000
#endif

#include "Pool.hpp"






#endif


#define ExpressionPtr Expression<T>*
#define NEW_EXPRESSION(T) new ad::Expression<T>
#define ExpressionStack ad::Stack<ad::Expression<T>* >
#define TypeStack ad::Stack<T>
#define PairStack ad::Stack<std::pair<T, T> > stack;

namespace ad {

    enum Operation {
        MINUS = 0,
        PLUS,
        MULTIPLY,
        DIVIDE,
        SIN,
        COS,
        TAN,
        ASIN,
        ACOS,
        ATAN,
        ATAN2, //atan(adnumber,adnumber)
        ATAN3, //atan(T,adnumber)
        ATAN4, //atan(adnumber,T)
        SQRT,
        POW, //pow(adnumber,adnumber)
        POW1, //pow(T,adnumber)
        POW2, //pow(adnumber,T)
        LOG,
        LOG10,
        EXP,
        SINH,
        COSH,
        TANH,
        ABS,
        FABS,
        FLOOR,
        CONSTANT,
        VARIABLE,
        NONE
    };


    template<class T> class ADNumber;

    template<class T>
    class Expression {
#ifdef USE_POOL
        static Pool<Expression<T> > pool_m;
#endif
        std::string name_m;
        T value_m;
        ExpressionPtr left_m;
        ExpressionPtr right_m;
        unsigned long id_m;
        Operation op_m;
        mutable int count_m;
        unsigned int index;
        static bool use_recusion_m;
        template<class TT> friend class ADNumber;

        bool IsUsingRecursion() {
            return Expression<T>::use_recusion_m;
        }

        void SetUseRecurion(bool use_recursion) {
            Expression<T>::use_recusion_m = use_recursion;
        }

    public:

        Expression()
        : right_m(NULL),
        left_m(NULL),
        op_m(CONSTANT),
        //        name_m(std::string("na")),
        id_m(0),
        value_m(T(0.0)),
        count_m(0) {


        }

        Expression(const T &value, const unsigned long &id, const std::string &name, const Operation &op, ExpressionPtr left, ExpressionPtr right)
        : right_m(right),
        left_m(left),
        op_m(op),
        //        name_m(name),
        id_m(id),
        value_m(value),
        count_m(0) {

            if (left != NULL) {
                left->take();
            }

            if (right != NULL) {
                right->take();
            }
        }

        Expression(const T &value, const unsigned long &id, const Operation &op, ExpressionPtr left, ExpressionPtr right)
        : op_m(op),
        id_m(id),
        right_m(right),
        left_m(left),
        value_m(value),
        //        name_m(std::string("na")),
        count_m(0) {
            //            std::cout << __func__ << ":" << __LINE__ << "\n";
            if (left != NULL) {
                left->take();
            }

            if (right != NULL) {
                right->take();
            }
        }

        virtual ~Expression() {


        }

        static void SetPoolResizePolicy(uint32_t size) {
#ifdef USE_POOL
            Expression<T>::pool_m.SetResize(size);
#endif

        }

        inline void take() const {
            ++count_m;
        }

        inline void release(bool ignore_delete = false) {


            //std::cout<<count_m<<std::endl;
            assert(count_m > 0);
            --count_m;

            if (Expression<T>::IsUsingRecursion()) {
                if (count_m == 0) {
                    if (this->GetLeft()) {
                        this->GetLeft()->release();
                    }

                    if (this->GetRight()) {
                        this->GetRight()->release();
                    }

                    delete this;
                }
            } else {

                if (count_m == 0 && !ignore_delete) {

                    //                                std::stack<Expression<T>*, std::vector<Expression<T>* > > stack;
                    ad::Stack<Expression<T>* > stack;

                    ExpressionPtr n = this;
                    bool do_delete = false;
                    //ExpressionPtr exp;

                    while (!stack.empty() || n != NULL) {

                        do_delete = false;

                        if (n != NULL) {

                            //visit
                            if (n->count_m == 0) {
                                do_delete = true;

                                if (n->GetLeft() != NULL) {
                                    n->GetLeft()->release(true);
                                }

                                if (n->GetRight() != NULL) {
                                    n->GetRight()->release(true);
                                }

                            }


                            if (n->GetRight() != NULL) {
                                stack.push(n->GetRight());
                            }

                            if (do_delete) {
                                ExpressionPtr exp = n;
                                //                             exp = n;
                                n = n->GetLeft();

                                if (exp != this) {
#ifdef USE_POOL
                                    Expression<T>::pool_m.free(exp);
#else

                                    delete exp;
#endif
                                }
                            } else {
                                n = n->GetLeft();
                            }



                        } else {
                            n = stack.top();
                            stack.pop();
                        }


                    }

#ifdef USE_POOL
                    Expression<T>::pool_m.free(this);
#else

                    delete this;
#endif
                    //                 free(this);
                }
            }
        }

        inline int References() const {
            return count_m;
        }
#ifdef USE_POOL

        void* operator new (size_t size) throw () {

            //            void* ptr = (void*) Expression<T>::pool_m.malloc();

            return Expression<T>::pool_m.malloc();
        }

        void operator delete (void* ptr) throw () {

            Expression<T>::pool_m.free((Expression<T>*)ptr);

        }
#endif

#ifdef USE_CLFMALLOC

        inline void* operator new (size_t size) throw (std::bad_alloc) {
            return malloc(size);
        }

        inline void operator delete (void* ptr)throw () {
            free(ptr);
        }
#endif

        bool HasID(const uint32_t &id) {
            //     std::cout << this->id_ << " ?= " << id << "\n";
            if (this->id_m == id) {
                return true;
            }
            if (this->left_m) {
                if (this->left_m->HasID(id)) {
                    return true;
                }
            }

            if (this->right_m) {
                if (this->right_m->HasID(id)) {
                    return true;
                }
            }

            return false;
        }

        /*!
         * Builds a expression tree representing the derivative with respect to 
         * some ADNumber via its id.(reverse mode) 
         * 
         * @return Expression<T>
         */
        Expression<T>* Differentiate(const uint32_t &id) {
            //#warning need to check partial derivatives....

            Expression<T>* ret = new Expression<T > ();


            switch (op_m) {

                case CONSTANT:
                    //f(x) = C
                    //f'(x) = 0

                    ret->op_m = CONSTANT;
                    ret->value_m = T(0); //this->value_;


                    return ret;

                case VARIABLE:
                    if (this->id_m == id) {
                        //f(x) = x
                        //f'(x) = 1

                        ret->op_m = CONSTANT;
                        ret->value_m = T(1.0);


                        return ret;
                    } else {//constant
                        //f(x) = C
                        //f'(x) = 0
                        ret->op_m = CONSTANT;
                        ret->value_m = T(0.0);
                        return ret;
                    }
                case MINUS:

                    //f(x) = g(x) - h(x)
                    //f'(x) = g'(x) - h'(x)

                    ret->op_m = MINUS;
                    if (this->left_m) {
                        ret->left_m = this->left_m->Differentiate(id);

                    }

                    if (this->right_m) {
                        ret->right_m = this->right_m->Differentiate(id);
                    }

                    return ret;

                case PLUS:

                    //f(x) = g(x) + h(x)
                    //f'(x) = g'(x) + h'(x)

                    ret->op_m = PLUS;
                    if (this->left_m) {
                        ret->left_m = this->left_m->Differentiate(id);
                    }

                    if (this->right_m) {
                        ret->right_m = this->right_m->Differentiate(id);
                    }


                    return ret;

                case DIVIDE:

                    //f(x) = g(x)/h(x);
                    //f'(x) = (g'(x)h(x) - g(x)h'(x))/h(x)^2


                    ret->op_m = DIVIDE;

                    ret->left_m = new Expression<T > (); //g'(x)h(x) - g(x)h'(x)
                    ret->left_m->op_m = MINUS;


                    ret->left_m->left_m = new Expression<T > (); //g'(x)h(x)
                    ret->left_m->left_m->op_m = MULTIPLY;
                    if (this->left_m) {
                        ret->left_m->left_m->left_m = this->left_m->Differentiate(id);
                    }
                    ret->left_m->left_m->right_m = this->right_m; //->Clone();

                    ret->left_m->right_m = new Expression<T > (); //g(x)h'(x)
                    ret->left_m->right_m->op_m = MULTIPLY;
                    ret->left_m->right_m->left_m = this->left_m; //->Clone();
                    if (this->right_m) {
                        ret->left_m->right_m->right_m = this->right_m->Differentiate(id);
                    }


                    ret->right_m = new Expression<T > ();
                    ret->right_m->op_m = MULTIPLY;
                    ret->right_m->left_m = this->right_m; //->Clone();
                    ret->right_m->right_m = this->right_m; //->Clone();


                    return ret;

                case MULTIPLY:
                    //f(x) = g(x)h(x);
                    //f'(x) = g'(x)h(x) + g(x)h'(x)

                    if (this->left_m->op_m == CONSTANT
                            && this->right_m->op_m != CONSTANT) {
                        ret->op_m = MULTIPLY;
                        if (this->left_m) {
                            ret->left_m = this->left_m; //->Clone();
                        }
                        if (this->right_m) {
                            ret->right_m = this->right_m->Differentiate(id);
                        }


                    } else if (this->right_m->op_m == CONSTANT
                            && this->left_m->op_m != CONSTANT) {
                        ret->op_m = MULTIPLY;
                        if (this->left_m) {
                            ret->left_m = this->left_m->Differentiate(id);
                        }
                        if (this->right_m) {
                            ret->right_m = this->right_m; //->Clone();
                        }
                    } else {



                        ret->op_m = PLUS;

                        ret->left_m = new Expression<T > ();
                        ret->left_m->op_m = MULTIPLY;

                        ret->left_m->right_m = this->right_m; //->Clone();

                        if (this->right_m != NULL) {
                            ret->left_m->left_m = this->left_m->Differentiate(id);
                        }
                        ret->right_m = new Expression<T > ();
                        ret->right_m->op_m = MULTIPLY;

                        ret->right_m->left_m = this->left_m; //->Clone();
                        if (this->left_m != NULL) {
                            ret->right_m->right_m = this->right_m->Differentiate(id);
                        }



                    }
                    return ret;

                case SIN:

                    if (this->left_m->HasID(id)) {
                        //f'(x) = cos(x)

                        ret->op_m = MULTIPLY;
                        ret->left_m = this->left_m->Differentiate(id);
                        ret->right_m = new Expression<T > ();
                        ret->right_m->op_m = COS;
                        ret->right_m->left_m = this->left_m; //->Clone();

                        return ret;
                    } else {
                        ret->op_m = CONSTANT;
                        ret->value_m = T(0.0);


                        return ret;
                    }

                case COS:
                    if (this->left_m->HasID(id)) {
                        //f'(x) = -sin(x)

                        ret->op_m = MULTIPLY;


                        ret->left_m = this->left_m->Differentiate(id);
                        ret->right_m = new Expression<T > ();

                        ret->right_m->op_m = MULTIPLY;
                        ret->right_m->left_m = new Expression<T > ();
                        ret->right_m->left_m->op_m = CONSTANT;
                        ret->right_m->left_m->value_m = T(-1.0);

                        ret->right_m->right_m = new Expression<T > ();
                        ret->right_m->right_m->op_m = SIN;
                        ret->right_m->right_m->left_m = this->left_m; //->Clone();


                        return ret;

                    } else {
                        ret->op_m = CONSTANT;
                        ret->value_m = T(0.0);


                        return ret;
                    }
                case TAN:
                    if (this->left_m->HasID(id)) {
                        //f'(x) = 1/cos(x)

                        ret->op_m = MULTIPLY;
                        ret->left_m = this->left_m->Differentiate(id);


                        ret->right_m = new Expression<T > ();
                        ret->right_m->op_m = MULTIPLY;

                        ret->right_m->left_m = new Expression<T > ();
                        ret->right_m->left_m->op_m = DIVIDE;


                        ret->right_m->left_m->left_m = new Expression<T > ();
                        ret->right_m->left_m->left_m->op_m = CONSTANT;
                        ret->right_m->left_m->left_m->value_m = T(1.0);


                        ret->right_m->left_m->right_m = new Expression<T > ();
                        ret->right_m->left_m->right_m->op_m = COS;
                        ret->right_m->left_m->right_m->left_m = this->left_m; //->Clone();


                        ret->right_m->right_m = new Expression<T > ();
                        ret->right_m->right_m->op_m = DIVIDE;


                        ret->right_m->right_m->left_m = new Expression<T > ();
                        ret->right_m->right_m->left_m->op_m = CONSTANT;
                        ret->right_m->right_m->left_m->value_m = T(1.0);


                        ret->right_m->right_m->right_m = new Expression<T > ();
                        ret->right_m->right_m->right_m->op_m = COS;
                        ret->right_m->right_m->right_m->left_m = this->left_m; //->Clone();


                        return ret;
                    } else {
                        ret->op_m = CONSTANT;
                        ret->value_m = T(0.0);


                        return ret;
                    }
                case ASIN:

                    if (this->left_m->HasID(id)) {
                        //f(x) = asin(x)
                        //f'(x) = 1/(2 sqrt(1-x^2)= 1/(pow((1-pow(x,2)),0.5)

                        ret->op_m = MULTIPLY;
                        ret->left_m = this->left_m->Differentiate(id);


                        ret->right_m = new Expression<T > ();
                        ret->right_m->op_m = DIVIDE;

                        ret->right_m->left_m = new Expression<T > ();
                        ret->right_m->left_m->op_m = CONSTANT;
                        ret->right_m->left_m ->value_m = T(1.0);

                        ret->right_m->right_m = new Expression<T > ();
                        ret->right_m->right_m->op_m = POW;

                        ret->right_m->right_m->left_m = new Expression<T > ();
                        ret->right_m->right_m->left_m->op_m = MINUS;

                        ret->right_m->right_m->left_m->left_m = new Expression<T > ();
                        ret->right_m->right_m->left_m->left_m->op_m = CONSTANT;
                        ret->right_m->right_m->left_m->left_m->value_m = T(1.0);

                        ret->right_m->right_m->left_m->right_m = new Expression<T > ();
                        ret->right_m->right_m->left_m->right_m->op_m = POW;
                        ret->right_m->right_m->left_m->right_m->left_m = this->left_m; //->Clone();

                        ret->right_m->right_m->left_m->right_m->right_m = new Expression<T > ();
                        ret->right_m->right_m->left_m->right_m->right_m->op_m = CONSTANT;
                        ret->right_m->right_m->left_m->right_m->right_m->value_m = T(2.0);

                        ret->right_m->right_m->right_m = new Expression<T > ();
                        ret->right_m->right_m->right_m->op_m = CONSTANT;
                        ret->right_m->right_m->right_m->value_m = T(0.5);

                        return ret;
                    } else {
                        ret->op_m = CONSTANT;
                        ret->value_m = T(0.0);


                        return ret;
                    }
                case ACOS:

                    if (this->left_m->HasID(id)) {
                        //f(x) = acos(x)
                        //f'(x) = -1/(sqrt(1-x^2) = -1/(pow((1-pow(x,2)),0.5)
                        //-1/sqrt(1-x^2)
                        ret->op_m = MULTIPLY;
                        ret->left_m = new Expression<T > ();
                        ret->left_m->op_m = MULTIPLY;
                        ret->left_m->left_m = new Expression<T > ();

                        ret->left_m->left_m->op_m = CONSTANT;
                        ret->left_m->left_m->value_m = T(-1.0);


                        ret->left_m->right_m = this->left_m->Differentiate(id);

                        ret->right_m = new Expression<T > ();
                        ret->right_m->op_m = DIVIDE;

                        ret->right_m->left_m = new Expression<T > ();
                        ret->right_m->left_m->op_m = CONSTANT;
                        ret->right_m->left_m ->value_m = T(1.0);

                        ret->right_m->right_m = new Expression<T > ();
                        ret->right_m->right_m->op_m = POW;

                        ret->right_m->right_m->left_m = new Expression<T > ();
                        ret->right_m->right_m->left_m->op_m = MINUS;

                        ret->right_m->right_m->left_m->left_m = new Expression<T > ();
                        ret->right_m->right_m->left_m->left_m->op_m = CONSTANT;
                        ret->right_m->right_m->left_m->left_m->value_m = T(1.0);

                        ret->right_m->right_m->left_m->right_m = new Expression<T > ();
                        ret->right_m->right_m->left_m->right_m->op_m = POW;
                        ret->right_m->right_m->left_m->right_m->left_m = this->left_m; //->Clone();

                        ret->right_m->right_m->left_m->right_m->right_m = new Expression<T > ();
                        ret->right_m->right_m->left_m->right_m->right_m->op_m = CONSTANT;
                        ret->right_m->right_m->left_m->right_m->right_m->value_m = T(2.0);

                        ret->right_m->right_m->right_m = new Expression<T > ();
                        ret->right_m->right_m->right_m->op_m = CONSTANT;
                        ret->right_m->right_m->right_m->value_m = T(0.5);

                        return ret;
                    } else {
                        ret->op_m = CONSTANT;
                        ret->value_m = T(0.0);


                        return ret;
                    }
                case ATAN:
                    if (this->left_m->HasID(id)) {
                        //f(x) = atan(x)
                        //f'(x) 1/(x^2+1)

                        ret->op_m = DIVIDE;
                        ret->left_m = new Expression<T > ();
                        ret->left_m->op_m = MULTIPLY;
                        ret->left_m->right_m = new Expression<T > ();

                        ret->left_m->right_m->op_m = CONSTANT;
                        ret->left_m->right_m->value_m = T(1.0);


                        ret->left_m->left_m = this->left_m->Differentiate(id);

                        ret->right_m = new Expression<T > ();
                        ret->right_m->op_m = PLUS;

                        ret->right_m->left_m = new Expression<T > ();
                        ret->right_m->left_m->op_m = MULTIPLY;
                        ret->right_m->left_m->left_m = this->left_m; //->Clone();
                        ret->right_m->left_m->right_m = this->left_m; //->Clone();


                        ret->right_m->right_m = new Expression<T > ();
                        ret->right_m->right_m->op_m = CONSTANT;
                        ret->right_m->right_m->value_m = T(1.0);


                        return ret;
                    } else {
                        ret->op_m = CONSTANT;
                        ret->value_m = T(0.0);


                        return ret;

                    }
                case ATAN2:
                    //if w.r.t. check both expressions for id
                    if (this->left_m->HasID(id)) {
                        //f(x) = atan2(x,y)
                        //f'(x) y/(x^2+y^2)

                        ret->op_m = DIVIDE;
                        ret->left_m = new Expression<T > ();
                        ret->left_m->op_m = MULTIPLY;
                        ret->left_m->left_m = this->right_m; //->Clone(); //y
                        ret->left_m->right_m = left_m->Differentiate(id);


                        ret->right_m = new Expression<T > ();
                        ret->right_m->op_m = PLUS;

                        ret->right_m->left_m = new Expression<T > ();
                        ret->right_m->left_m->op_m = MULTIPLY;
                        ret->right_m->left_m->left_m = this->left_m; //->Clone();
                        ret->right_m->left_m->right_m = this->left_m; //->Clone();


                        ret->right_m->right_m = new Expression<T > ();
                        ret->right_m->right_m->op_m = MULTIPLY;
                        ret->right_m->right_m->left_m = this->right_m; //->Clone();
                        ret->right_m->right_m->right_m = this->right_m; //->Clone();


                        return ret;
                    } else {
                        ret->op_m = CONSTANT;
                        ret->value_m = T(0.0);


                        return ret;
                    }
                case ATAN3:

                    //can be removed.
                    break;

                case ATAN4:
                    break;
                case SQRT:
                    if (this->left_m->HasID(id)) {
                        //f(x) = sqrt(x)
                        //f'(x) = .5/sqrt(x)

                        ret->op_m = DIVIDE;
                        ret->left_m = new Expression<T > ();
                        ret->left_m->op_m = MULTIPLY;

                        ret->left_m->right_m = new Expression<T > ();
                        ret->left_m->right_m->value_m = T(0.5);

                        ret->left_m->left_m = this->left_m->Differentiate(id);


                        ret->right_m = new Expression<T > ();
                        ret->right_m->op_m = SQRT;
                        ret->right_m->left_m = this->left_m; //->Clone();

                        //std::cout<<ret->ToString();

                        return ret;
                    } else {
                        ret->op_m = CONSTANT;
                        ret->value_m = T(0.0);


                        return ret;
                    }
                case POW:

                    if (this->left_m->HasID(id)) {
                        //f(x) =  x^y
                        //f'(x) = yx^y-1

                        ret->op_m = MULTIPLY;

                        ret->left_m = new Expression<T > ();
                        ret->left_m->op_m = MULTIPLY;
                        ret->left_m->left_m = this->left_m->Differentiate(id);
                        ret->left_m->right_m = this->right_m; //->Clone();



                        ret->right_m = new Expression<T > ();
                        ret->right_m->op_m = POW;


                        ret->right_m->left_m = this->left_m; //->Clone();


                        ret->right_m->right_m = new Expression<T > ();
                        ret->right_m->right_m->op_m = MINUS;
                        ret->right_m->right_m->left_m = this->right_m; //->Clone();

                        ret->right_m->right_m->right_m = new Expression<T > ();
                        ret->right_m->right_m->right_m->op_m = CONSTANT;
                        ret->right_m->right_m->right_m->value_m = T(1.0);



                        return ret;
                    } else {
                        ret->op_m = CONSTANT;
                        ret->value_m = T(0.0);


                        return ret;
                    }
                    //                case POW1:
                    //
                    //                    break;
                    //                    //                return pow(this->left_value_, this->right_value_ - T(1.0));
                    //                case POW2:
                    //                    break;
                    //                    //                return pow(this->left_value_, this->right_value_ - T(1.0));
                case LOG:
                    if (this->left_m->HasID(id)) {
                        //f(x) = log(x)
                        //f'(x) = 1/x

                        ret->op_m = DIVIDE;
                        ret->left_m = new Expression<T > ();
                        ret->left_m->op_m = MULTIPLY;
                        ret->left_m->left_m = new Expression<T > ();
                        ret->left_m->left_m->op_m = CONSTANT;
                        ret->left_m->left_m->value_m = T(1.0);
                        ret->left_m->right_m = this->left_m->Differentiate(id);

                        ret->right_m = this->left_m; //->Clone();



                        return ret;
                    } else {
                        ret->op_m = CONSTANT;
                        ret->value_m = T(0.0);


                        return ret;
                    }
                case LOG10:
                    //f(x) = log10(x)
                    //f'(x) = 1/(xlog(10))

                    if (this->left_m->HasID(id)) {



                        ret->op_m = DIVIDE;

                        ret->left_m = new Expression<T > ();
                        ret->left_m->op_m = MULTIPLY;

                        ret->left_m->left_m = new Expression<T > ();
                        ret->left_m->left_m->op_m = CONSTANT;
                        ret->left_m->left_m->value_m = T(1.0);

                        ret->left_m->right_m = this->left_m->Differentiate(id);

                        ret->right_m = new Expression<T > ();
                        ret->right_m->op_m = MULTIPLY;

                        ret->right_m->left_m = this->left_m; //->Clone();

                        ret->right_m->right_m = new Expression<T > ();
                        ret->right_m->right_m->op_m = CONSTANT;
                        ret->right_m->right_m->value_m = log(T(10.0));


                        return ret;
                    } else {
                        ret->op_m = LOG;
                        ret->left_m = this; //->Clone();


                        return ret;
                    }
                case EXP:
                    //f(x) = e^x
                    //f'(x) =e^x

                    if (this->left_m->HasID(id)) {

                        ret->op_m = MULTIPLY;
                        ret->left_m = this->left_m->Differentiate(id);


                        ret->right_m = new Expression<T > ();
                        ret->right_m->op_m = EXP;
                        ret->right_m->left_m = this->left_m; //->Clone();



                        return ret;
                    } else {
                        ret->op_m = CONSTANT;
                        ret->value_m = T(0.0);


                        return ret;
                    }
                case SINH:
                    if (this->left_m->HasID(id)) {
                        //f(x) = sinh(x)
                        //f'(x) = cosh(x)

                        ret->op_m = MULTIPLY;
                        ret->left_m = this->left_m->Differentiate(id);

                        ret->right_m = new Expression<T > ();
                        ret->right_m->op_m = COSH;
                        ret->right_m->left_m = this->left_m; //->Clone();


                        return ret;
                    } else {
                        ret->op_m = CONSTANT;
                        ret->value_m = T(0.0);


                        return ret;
                    }
                case COSH:
                    if (this->left_m->HasID(id)) {

                        ret->op_m = MULTIPLY;
                        ret->left_m = this->left_m->Differentiate(id);

                        ret->right_m = new Expression<T > ();
                        ret->right_m->op_m = SINH;
                        ret->right_m->left_m = this->left_m; //->Clone();


                        return ret;
                    } else {
                        ret->op_m = CONSTANT;
                        ret->value_m = T(0.0);


                        return ret;
                    }
                case TANH:
                    //f(x) = tanh(x)
                    //f'(x) =1- tanh(x)*tanh(x)


                    if (this->left_m->HasID(id)) {

                        ret->op_m = MULTIPLY;

                        ret->left_m = this->left_m->Differentiate(id);

                        ret->right_m = new Expression<T > ();
                        ret->right_m->op_m = MULTIPLY;
                        ret->right_m->left_m = new Expression<T > ();


                        ret->right_m->left_m->op_m = DIVIDE;
                        ret->right_m->left_m->left_m = new Expression<T > ();
                        ret->right_m->left_m->left_m->op_m = CONSTANT;
                        ret->right_m->left_m->left_m->value_m = T(1.0);


                        ret->right_m->left_m->right_m = new Expression<T > ();
                        ret->right_m->left_m->right_m->op_m = COSH;
                        ret->right_m->left_m->right_m->left_m = this->left_m; //->Clone();


                        ret->right_m->right_m = ret->right_m->left_m; //->Clone();


                        return ret;
                    } else {
                        ret->op_m = CONSTANT;
                        ret->value_m = T(0.0);


                        return ret;
                    }

                case FABS:

                    if (this->left_m->HasID(id)) {

                        ret->op_m = DIVIDE;
                        ret->left_m = new Expression<T > ();
                        ret->left_m->op_m = MULTIPLY;

                        ret->left_m->left_m = this->left_m->Differentiate(id);
                        ret->left_m->right_m = this->left_m; //->Clone();;


                        ret->right_m = new Expression<T > ();
                        ret->right_m->op_m = FABS;
                        ret->right_m->left_m = this->left_m; //->Clone();


                        return ret;
                    } else {
                        ret->op_m = CONSTANT;
                        ret->value_m = T(0.0);


                        return ret;
                    }
                case FLOOR:
                    if (this->left_m->id_m == id) {



                        ret->op_m = MULTIPLY;

                        ret->left_m = this->left_m->Differentiate(id);

                        ret->right_m = new Expression<T > ();
                        ret->right_m->op_m = FLOOR;
                        ret->right_m->left_m = this->left_m; //->Clone();


                        return ret;
                    } else {
                        ret->op_m = CONSTANT;
                        ret->value_m = T(0.0);


                        return ret;
                    }
                case NONE://shouldn't happen.
                    return this; //->Clone();

                default:
                    return NULL;
            }
            return NULL;
        }

        /*!
         * Evaluate this expression. 
         * @return 
         */
        const T Evaluate() const {

            T l = T(0);
            T r = T(0);

            if (this->right_m != NULL) {
                r = this->right_m->Evaluate();
            }

            if (this->left_m != NULL) {
                l = this->left_m->Evaluate();
            }




            switch (op_m) {
                case CONSTANT:

                    return this->value_m;
                case VARIABLE:

                    return this->value_m;
                case MINUS:

                    return (l - r);
                case PLUS:

                    return (l + r);
                case DIVIDE:

                    return (l / r);
                case MULTIPLY:

                    return (l * r);
                case SIN:

                    return sin(l);
                case COS:

                    return cos(l);
                case TAN:

                    return tan(l);
                case ASIN:

                    return asin(l);
                case ACOS:

                    return acos(l);
                case ATAN:

                    return atan(l);
                case ATAN2:

                    return atan2(l, r);
                    //                case ATAN3:
                    //                    break;
                    //                case ATAN4:
                    //                    break;
                case SQRT:

                    return sqrt(l);
                case POW:

                    return pow(l, r);
                    //                case POW1:
                    //                    break;
                    //                case POW2:
                    //                    break;
                case LOG:

                    return log(l);
                case LOG10:

                    return log10(l);
                case EXP:

                    return exp(l);
                case SINH:

                    return sinh(l);
                case COSH:

                    return cosh(l);
                case TANH:

                    return tanh(l);
                case FABS:

                    return fabs(l);
                case ABS:

                    return abs(l);
                case FLOOR:

                    return floor(l);
                case NONE:

                    return this->value_m;
                default:
                    return T(0);
            }
            return T(0);
        }

        /**
         * Returns the evaluated derivative of this expression tree. While the
         * derivative is computed, no expression tree manipulations are made.
         * @param id
         * @return 
         */
        T EvaluateDerivative(const uint32_t &id, bool &has_id) {
            //#warning need to check partial derivatives....

            T ret, g, h = T(-999.0);

            T left_derivative = T(0);
            T right_derivative = T(0);

            if (this->GetLeft()) {
                left_derivative = this->GetLeft()->EvaluateDerivative(id, has_id);
            }

            if (this->GetRight()) {
                right_derivative = this->GetLeft()->EvaluateDerivative(id, has_id);
            }



            switch (op_m) {

                case CONSTANT:
                    //f(x) = C
                    //f'(x) = 0

                    return T(0);

                case VARIABLE:
                    if (this->id_m == id) {
                        //f(x) = x
                        //f'(x) = 1
                        has_id = true;

                        return T(1.0);
                    } else {//constant
                        //f(x) = C
                        //f'(x) = 0

                        return T(0.0);
                    }
                case MINUS:

                    //f(x) = g(x) - h(x)
                    //f'(x) = g'(x) - h'(x)


                    return left_derivative - right_derivative;

                case PLUS:

                    //f(x) = g(x) + h(x)
                    //f'(x) = g'(x) + h'(x)


                    return left_derivative + right_derivative;


                case DIVIDE:

                    //f(x) = g(x)/h(x);
                    //f'(x) = (g'(x)h(x) - g(x)h'(x))/h(x)^2


                    ret = (left_derivative * this->right_m->Evaluate() -
                            this->left_m->Evaluate() * right_derivative) /
                            (this->right_m->Evaluate() * this->right_m->Evaluate());


                    return ret;

                case MULTIPLY:
                    //f(x) = g(x)h(x);
                    //f'(x) = g'(x)h(x) + g(x)h'(x)

                    if (this->left_m->op_m == CONSTANT
                            && this->right_m->op_m != CONSTANT) {

                        ret = this->left_m->Evaluate() * right_derivative;


                    } else if (this->right_m->op_m == CONSTANT
                            && this->left_m->op_m != CONSTANT) {

                        ret = left_derivative * this->right_m->Evaluate();
                    } else {

                        //g'(x)h(x) + g(x)h'(x)

                        ret = left_derivative * this->right_m->Evaluate() +
                                this->left_m->Evaluate() * right_derivative;


                    }
                    return ret;

                case SIN:

                    if (has_id) {
                        //f'(x) = cos(x)
                        ret = left_derivative *
                                std::cos(this->left_m->Evaluate());

                        return ret;
                    } else {
                        return T(0.0);
                    }

                case COS:
                    if (has_id) {
                        //f'(x) = -sin(x)


                        g = left_derivative;

                        ret = g * T(-1.0) * std::sin(this->left_m->Evaluate());

                        return ret;

                    } else {

                        return T(0.0);
                    }
                case TAN:
                    if (has_id) {
                        //f'(x) = 1/cos(x)


                        g = left_derivative;

                        ret = g * ((T(1.0) / std::cos(this->left_m->Evaluate()))*(T(1.0) / std::cos(this->left_m->Evaluate())));


                        return ret;
                    } else {

                        return T(0.0);
                    }
                case ASIN:

                    if (has_id) {


                        //f(x) = asin(x)
                        //f'(x) = 1/(2 sqrt(1-x^2)= 1/(pow((1-pow(x,2)),0.5)


                        g = left_derivative;

                        ret = (g * T(1.0) / std::pow((T(1.0) - std::pow(this->left_m->Evaluate(), T(2.0))), T(0.5)));

                        return ret;
                    } else {
                        return T(0.0);
                    }
                case ACOS:

                    if (has_id) {
                        g = left_derivative;

                        ret = (g * T(-1.0) / std::pow((T(1.0) - std::pow(this->left_m->Evaluate(), T(2.0))), T(0.5)));

                        return ret;
                    } else {

                        return T(0.0);
                    }
                case ATAN:
                    if (has_id) {
                        g = left_derivative;
                        ret = (g * T(1.0) / (this->left_m->Evaluate() * this->left_m->Evaluate() + T(1.0)));

                        return ret;
                    } else {
                        //                        ret->op_m = CONSTANT;
                        //                        ret->value_m = T(0.0);
                        return T(0.0);

                    }
                case ATAN2:
                    //if w.r.t. check both expressions for id
                    if (has_id) {
                        //f(x) = atan2(x,y)
                        //f'(x) y/(x^2+y^2)

                        g = left_derivative;
                        ret = (this->right_m->Evaluate() * g / (this->left_m->Evaluate() * this->left_m->Evaluate()+(this->right_m->Evaluate() * this->right_m->Evaluate())));

                        return ret;
                    } else {

                        return T(0.0);
                    }
                case ATAN3:

                    //can be removed.
                    break;

                case ATAN4:
                    break;
                case SQRT:
                    if (has_id) {
                        //f(x) = sqrt(x)
                        //f'(x) = .5/sqrt(x)
                        g = left_derivative;
                        ret = g * T(.5) / std::sqrt(this->left_m->Evaluate());


                        return ret;
                    } else {
                        return T(0.0);
                    }
                case POW:

                    if (has_id) {
                        //f(x) =  x^y
                        //f'(x) = yx^y-1
                        ret = (left_derivative * this->right_m->Evaluate()) *
                                std::pow(this->left_m->Evaluate(), (this->right_m->Evaluate() - T(1.0)));

                        return ret;
                    } else {

                        return T(0.0);
                    }

                case LOG:
                    if (has_id) {
                        //f(x) = log(x)
                        //f'(x) = 1/x
                        ret = (left_derivative * T(1.0)) / this->left_m->Evaluate();

                        return ret;
                    } else {

                        return T(0.0);
                    }
                case LOG10:
                    //f(x) = log10(x)
                    //f'(x) = 1/(xlog(10))

                    if (has_id) {

                        ret = (left_derivative * T(1.0)) / (this->left_m->Evaluate() * std::log(T(10.0)));

                        return ret;
                    } else {
                        return T(0.0);
                    }
                case EXP:
                    //f(x) = e^x
                    //f'(x) =e^x

                    if (has_id) {
                        ret = left_derivative * std::exp(this->left_m->Evaluate());

                        return ret;
                    } else {

                        return T(0.0);
                    }
                case SINH:
                    if (has_id) {
                        //f(x) = sinh(x)
                        //f'(x) = cosh(x)
                        return left_derivative * std::cosh(this->left_m->Evaluate());

                        return ret;
                    } else {

                        return T(0.0);
                    }
                case COSH:
                    if (has_id) {
                        return left_derivative * std::sinh(this->left_m->Evaluate());

                        return ret;
                    } else {

                        return ret;
                    }
                case TANH:
                    //f(x) = tanh(x)
                    //f'(x) =1- tanh(x)*tanh(x)


                    if (has_id) {

                        ret = left_derivative * (T(1.0) / std::cosh(this->left_m->Evaluate()))*(T(1.0) / std::cosh(this->left_m->Evaluate()));


                        return ret;
                    } else {

                        return T(0.0);
                    }

                case FABS:

                    if (has_id) {

                        ret = (left_derivative * this->left_m->Evaluate()) /
                                std::fabs(this->left_m->Evaluate());

                        return ret;
                    } else {

                        return T(0.0);
                    }
                case FLOOR:
                    if (has_id) {

                        ret = left_derivative * std::floor(this->left_m->Evaluate());

                        return ret;
                    } else {

                        return ret;
                    }
                case NONE://shouldn't happen.
                    return ret;

                default:
                    return ret;
            }
            return NULL;
        }

        const unsigned long GetId() const {
            return id_m;
        }

        void SetId(const unsigned long &id) {
            id_m = id;
        }

        ExpressionPtr GetLeft() const {
            return left_m;
        }

        void SetLeft(ExpressionPtr left) {
            if (GetLeft() != NULL) {
                GetLeft()->release();
            }

            if (left != NULL) {
                left->take();
            }
            left_m = left;

        }

        virtual const std::string GetName() const {

            if (name_m == "") {
                std::stringstream ss;
                ss << "x" << GetId();
                return ss.str();
            }
            return name_m;
        }

        virtual void SetName(const std::string &name) {
            //            name_m = name;
        }

        const Operation GetOp() const {
            return op_m;
        }

        void SetOp(const Operation &op) {
            op_m = op;
        }

        ExpressionPtr GetRight() const {
            return right_m;
        }

        void SetRight(ExpressionPtr right) {
            if (GetRight() != NULL) {
                GetRight()->release();
            }

            if (right != NULL) {
                right->take();
            }

            right_m = right;

        }

        const T GetValue() const {
            return value_m;
        }

        void SetValue(T value) {
            value_m = value;
        }
        //

        /*!
         * Represent this expression as a string. ADNumbers are represented
         * in wkt format by default unless latex flag is set to true.
         * Constants are represented by value.
         *
         */
        std::string ToString(bool latex = false) {

            //            if (Size() > 1000) {
            //                return std::string("Error: ToString...expression too long.");
            //            }
            //
            std::stringstream ss;
            std::stringstream temp;

            std::string l, r;



            if (GetRight() != NULL) {
                r = GetRight()->ToString(latex);
            }

            if (GetLeft() != NULL) {
                l = GetLeft()->ToString(latex);
            }


            temp.str("");



            switch (GetOp()) {
                case CONSTANT:
                    if (latex) {
                        ss << GetValue();
                    } else {
                        ss << "CONST[" << GetValue() << "]";
                    }
                    break;
                case VARIABLE:
                    if (latex) {
                        if (GetName() == "") {
                            ss << "x" << GetId();
                        } else {
                            ss << GetName();
                        }
                    } else {
                        ss << "VAR[" << GetValue() << ",ID[" << GetId() << "]" << "]";
                    }
                    break;
                case MINUS:
                    ss << "(" << l << " - " << r << ")";
                    break;
                case PLUS:
                    ss << "(" << l << " + " << r << ")";
                    break;
                case DIVIDE:
                    if (latex) {
                        ss << "\\frac{" << l << "}{" << r << "}";
                    } else {
                        ss << l << " / " << r;
                    }
                    break;
                case MULTIPLY:

                    if (latex && GetRight()->GetOp() == POW
                            && GetRight()->GetRight()->GetOp() == CONSTANT
                            && GetRight()->GetRight()->GetValue() == T(-1)) {
                        ss << "\\frac{" << l << "}{" << GetRight()->GetLeft()->ToString(latex) << "}";
                    } else {

                        ss << "(" << l << " * " << r << ")";
                    }
                    break;
                case SIN:
                    ss << "sin(" << l << ")";
                    break;
                case COS:
                    ss << "cos(" << l << ")";
                    break;
                case TAN:
                    ss << "tan(" << l << ")";
                    break;
                case ASIN:
                    ss << "asin(" << l << ")";
                    break;
                case ACOS:
                    ss << "acos(" << l << ")";
                    break;
                case ATAN:
                    ss << "atan(" << l << ")";
                    break;
                case ATAN2:
                    ss << "atan(" << l << "," << r << ")";
                    break;
                case ATAN3:
                    ss << "atan(" << l << "," << r << ")";
                    break;
                case ATAN4:
                    ss << "atan(" << l << "," << r << ")";
                    break;
                case SQRT:
                    ss << "sqrt(" << l << ")";
                    break;
                case POW:


                case POW1:

                case POW2:
                    if (latex) {
                        ss << l << "^{" << r << "}";
                    } else {
                        ss << "pow(" << l << "," << r << ")";
                    }
                    break;
                case LOG:
                    if (latex) {
                        ss << "ln(" << l << ")";
                    } else {
                        ss << "log(" << l << ")";
                    }
                    break;
                case LOG10:
                    ss << "log10(" << l << ")";
                    break;
                case EXP:
                    ss << "exp(" << l << ")";
                    break;
                case SINH:
                    ss << "sinh(" << l << ")";
                    break;
                case COSH:
                    ss << "cosh(" << l << ")";
                    break;
                case TANH:
                    ss << "tanh(" << l << ")";
                    break;
                case FABS:
                    ss << "fabs(" << l << ")";
                case FLOOR:
                    ss << "floor(" << l << ")";
                case NONE:
                    break;
                default:
                    break;
            }

            //ss << ")";

            return ss.str();

        }
        //
        //        bool HasId(const unsigned long &id) {
        //
        //            //            InOrderIterator<T> it(this);
        //            //            Expression<T>* exp;
        //            //            while (it) {
        //            //                exp = it;
        //            //                if (exp->GetId() == id) {
        //            //                    return true;
        //            //                }
        //            //                it++;
        //            //            }
        //
        //            ExpressionPtr n = this;
        //            // std::vector<Expression<T>* > vect(1000);
        //            ExpressionStack stack;
        //            //Stack<ExpressionPtr > stack(100);
        //            // begin infix traversal
        //            // First iteration will never have a NULL root node
        //            // So it will be impossible to ever pop from an empty stack
        //
        //            do {
        //                if (n == NULL) {
        //                    // No right sub tree from previous iteration
        //                    // Continue processing the stack
        //                    n = stack.top();
        //                    stack.pop();
        //                } else {
        //                    // There exists a right sub tree from previous iteration
        //                    while (n->GetLeft() != NULL) {
        //                        stack.push(n);
        //                        n = n->GetLeft();
        //                    }
        //                }
        //
        //                if (n->GetId() == id) {
        //                    return true;
        //                }
        //
        //                // Check for a right sub tree
        //                n = n->GetRight();
        //
        //            } while (!stack.empty() || n != NULL);
        //
        //            return false;
        //        }
        //

        void Update(const unsigned long &id, const T &value) {

            //            InOrderIterator<T> it(this);
            //            Expression<T>* exp;
            //            while (it) {
            //                exp = it;
            //                if (exp->GetId() == id) {
            //                    return true;
            //                }
            //                it++;
            //            }

            ExpressionPtr n = this;
            // std::vector<Expression<T>* > vect(1000);
            ExpressionStack stack;
            //Stack<ExpressionPtr > stack(100);
            // begin infix traversal
            // First iteration will never have a NULL root node
            // So it will be impossible to ever pop from an empty stack

            do {
                if (n == NULL) {
                    // No right sub tree from previous iteration
                    // Continue processing the stack
                    n = stack.top();
                    stack.pop();
                } else {
                    // There exists a right sub tree from previous iteration
                    while (n->GetLeft() != NULL) {
                        stack.push(n);
                        n = n->GetLeft();
                    }
                }

                if (n->GetId() == id) {
                    n->SetValue(value);
                }

                // Check for a right sub tree
                n = n->GetRight();

            } while (!stack.empty() || n != NULL);


        }


    private:




    };



#ifdef USE_POOL
    template<class T>
    Pool<Expression<T> > Expression<T>::pool_m(DEFAULT_POOL_SIZE);
#endif
    template<class T>
    bool Expression<T>::use_recusion_m = false;

    template<class T>
    static ExpressionPtr Clone(ExpressionPtr exp) {
        //  Lock l(mutex);


        std::vector<Expression<T>* > vect;
        ExpressionStack queue;
        queue.push(exp);
        Expression<T>* n;

        std::stack<Expression<T>* > q2;
        Expression<T>* fresh;
        //(T value, unsigned long id, std::string name, Operation op, ExpressionPtr left, ExpressionPtr right)
        Expression<T>* root2 = NEW_EXPRESSION(T) (exp->GetValue(), exp->GetId(), exp->GetName(), exp->GetOp(), NULL, NULL);

        q2.push(root2);

        while (!queue.empty()) {
            n = queue.top();
            queue.pop();

            fresh = q2.top();
            q2.pop();
            if (n->GetLeft() != NULL) {
                queue.push(n->GetLeft());
                Expression<T>* exp = NEW_EXPRESSION(T) ();

                exp->SetId(n->GetLeft()->GetId());
                exp->SetName(n->GetLeft()->GetName());
                exp->SetOp(n->GetLeft()->GetOp());
                exp->SetValue(n->GetLeft()->GetValue());
                fresh->SetLeft(exp);
                q2.push(fresh->GetLeft());
            }
            if (n->GetRight() != NULL) {
                queue.push(n->GetRight());
                Expression<T>* exp = NEW_EXPRESSION(T) ();

                exp->SetId(n->GetRight()->GetId());
                exp->SetName(n->GetRight()->GetName());
                exp->SetOp(n->GetRight()->GetOp());
                exp->SetValue(n->GetRight()->GetValue());
                fresh->SetRight(exp);
                q2.push(fresh->GetRight());
            }
        }

        return root2;
    }

    template<class T>
    class ExpresionIterator {
    protected:


        //typename std::stack<Expression<T>*, std::vector<ad::Expression<T>* > > stack_m;

        ExpressionStack stack_m;
        Expression<T>* root;
        virtual int Intitialize() = 0;
    public:

        ExpresionIterator() : root(NULL) {

        }

        ExpresionIterator(Expression<T>* exp) : root(exp) {

        }

        ExpresionIterator(const ExpresionIterator& it) {
        }



        virtual int operator !() = 0;

        operator int() {
            return !stack_m.empty();
        }

        //            operator ExpressionPtr() {
        //                return stack_m.top();
        //            }


        virtual inline const int operator ++(int) = 0;

    };

    template<class T>
    class InOrderIterator : protected ExpresionIterator<T> {
    public:

        operator int() {
            return !this->stack_m.empty();
        }

        operator Expression<T>*() {
            return this->stack_m.top();
        }

        InOrderIterator(Expression<T>* exp) : ExpresionIterator<T>(exp) {
            Intitialize();
        }

        InOrderIterator(const ExpresionIterator<T>& it) {
        }

        virtual inline const int operator++(int) {

            if (!this->stack_m.empty()) {
                Expression<T> * n = this->stack_m.top();
                this->stack_m.pop();
                Expression<T> * next = n->GetRight();

                if (next)
                    LeftSide(next);
            }

            return !this->stack_m.empty();
        }

        virtual inline int operator !() {
            return !this->stack_m.empty();
        }




    protected:

        virtual int Intitialize() {
            while (!this->stack_m.empty()) {
                this->stack_m.pop();
            }
            LeftSide(this->root);
            return !this->stack_m.empty();
        }


    private:

        void LeftSide(Expression<T>* exp) {
            while (exp) {
                this->stack_m.push(exp);
                exp = exp->GetLeft();
            }
        }

    };

    template<class T>
    class PreOrderIterator : protected ExpresionIterator<T> {

        PreOrderIterator(Expression<T>* exp) : ExpresionIterator<T>(exp) {
            this->stack_m.push(exp);
        }

        PreOrderIterator(const ExpresionIterator<T>& it) {
        }

        operator int() {
            return !this->stack_m.empty();
        }

        operator ad::Expression<T>*() {
            return currNode;
        }

        virtual inline int operator !() {
            return !this->stack_m.empty();
        }

        virtual inline const int operator++(int) {
            Next();
            return !this->stack_m.empty();
        }

    private:
        Expression<T>* n;
        Expression<T>* prevNode;
        Expression<T>* currNode;

        inline void Next() {
            if (this->stack_m.empty()) {
                return;
            }

            currNode = this->stack_m.top();
            this->stack_m.pop();

            if (currNode->GetRight() != NULL) {
                this->stack_m.push(currNode->GetRight());
            }

            if (currNode->GetLeft() != NULL) {
                this->stack_m.push(currNode->GetLeft());
            }
        }

    };

    template<class T>
    class PostOrderIterator : protected ExpresionIterator <T> {
    public:

        PostOrderIterator(Expression<T>* exp) : ExpresionIterator<T>(exp) {
            Intitialize();
        }

        PostOrderIterator(const ExpresionIterator<T>& it) {
        }

        operator int() {
            return has_more;
        }

        operator ad::Expression<T>*() {
            return currNode;
        }

        virtual inline int operator !() {
            return !this->stack_m.empty();
        }

        virtual inline const int operator++(int) {
            Next();
            return !this->stack_m.empty();
        }
    protected:

        virtual int Intitialize() {


            this->stack_m.push(this->root);
            currNode = NULL;
            prevNode = NULL;
            has_more = 1;
            Next();




            return !this->stack_m.empty();
        }

    private:

        Expression<T>* n;
        Expression<T>* prevNode;
        Expression<T>* currNode;
        int has_more;

        inline void Next() {
            if (this->stack_m.empty()) {
                has_more = 0;
                return;
            }

            bool breakout = false;

            while (!this->stack_m.empty()) {
                currNode = this->stack_m.top();


                if (prevNode == NULL || prevNode->GetLeft() == currNode || prevNode->GetRight() == currNode) {
                    if (currNode->GetLeft() != NULL) {
                        this->stack_m.push(currNode->GetLeft());
                    } else if (currNode->GetRight() != NULL) {
                        this->stack_m.push(currNode->GetRight());
                    }
                } else if (currNode->GetLeft() == prevNode) {
                    if (currNode->GetRight() != NULL) {
                        this->stack_m.push(currNode->GetRight());
                    }
                } else {
                    n = currNode;
                    this->stack_m.pop();
                    breakout = true;
                    //  
                }

                prevNode = currNode;

                if (breakout) {
                    break;
                }

            }

        }

    };

    template<class T>
    static T Evaluate(ExpressionPtr exp) {

        PostOrderIterator<T> it(exp);
        TypeStack stack;

        while (it) {
            T lhs = T(0);
            T rhs = T(0);
            Expression<T>* currNode = it;

            switch (currNode->GetOp()) {

                case CONSTANT:
                    stack.push(currNode->GetValue());
                    break;
                case VARIABLE:
                    stack.push(currNode->GetValue());
                    break;
                case PLUS:
                    rhs = stack.top();
                    stack.pop();
                    lhs = stack.top();
                    stack.pop();
                    stack.push(lhs + rhs);
                    break;
                case MINUS:
                    rhs = stack.top();
                    stack.pop();
                    lhs = stack.top();
                    stack.pop();
                    stack.push(lhs - rhs);
                    break;
                case MULTIPLY:
                    rhs = stack.top();
                    stack.pop();
                    lhs = stack.top();
                    stack.pop();
                    stack.push(lhs * rhs);
                    break;
                case DIVIDE:
                    rhs = stack.top();
                    stack.pop();
                    lhs = stack.top();
                    stack.pop();
                    stack.push(lhs / rhs);
                    break;

                case SIN:
                    lhs = stack.top();
                    stack.pop();
                    stack.push(std::sin(lhs));
                    break;
                case COS:
                    lhs = stack.top();
                    stack.pop();
                    stack.push(std::cos(lhs));
                    break;
                case TAN:
                    lhs = stack.top();
                    stack.pop();
                    stack.push(std::tan(lhs));
                    break;
                case ASIN:
                    lhs = stack.top();
                    stack.pop();
                    stack.push(std::asin(lhs));
                    break;
                case ACOS:
                    lhs = stack.top();
                    stack.pop();
                    stack.push(std::acos(lhs));
                    break;
                case ATAN:
                    lhs = stack.top();
                    stack.pop();
                    stack.push(std::atan(lhs));
                    break;
                case ATAN2:
                    rhs = stack.top();
                    stack.pop();
                    lhs = stack.top();
                    stack.pop();
                    stack.push(std::atan2(lhs, rhs));
                    break;
                case SQRT:
                    lhs = stack.top();
                    stack.pop();
                    stack.push(std::sqrt(lhs));
                    break;
                case POW:
                    rhs = stack.top();
                    stack.pop();
                    lhs = stack.top();
                    stack.pop();
                    stack.push(std::pow(lhs, rhs));
                    break;
                case LOG:
                    lhs = stack.top();
                    stack.pop();
                    stack.push(std::log(lhs));
                    break;
                case LOG10:
                    lhs = stack.top();
                    stack.pop();
                    stack.push(std::log10(lhs));
                    break;
                case EXP:
                    lhs = stack.top();
                    stack.pop();
                    stack.push(std::exp(lhs));
                    break;
                case SINH:
                    lhs = stack.top();
                    stack.pop();
                    stack.push(std::sinh(lhs));
                    break;
                case COSH:
                    lhs = stack.top();
                    stack.pop();
                    stack.push(std::cosh(lhs));
                    break;
                case TANH:
                    lhs = stack.top();
                    stack.pop();
                    stack.push(std::tanh(lhs));
                    break;
                case FABS:
                    lhs = stack.top();
                    stack.pop();
                    stack.push(std::fabs(lhs));
                    break;
                case ABS:
                    lhs = stack.top();
                    stack.pop();
                    stack.push(std::fabs(lhs));
                    break;
                case FLOOR:
                    lhs = stack.top();
                    stack.pop();
                    stack.push(std::floor(lhs));
                    break;
                case NONE:

                    break;
                default:
                    break;

            }
            it++;
        }


        return stack.top();

    }

    template<class T>
    static T EvaluateDerivative(ExpressionPtr exp, const unsigned long &id) {

        PostOrderIterator<T> it(exp);

        PairStack stack;

        bool found = false;
        while (it) {
            std::pair<T, T> lhs = std::pair<T, T > (0, 0);
            std::pair<T, T> rhs = std::pair<T, T > (0, 0);

            T temp = 0;

            Expression<T>* currNode = it;

            switch (currNode->GetOp()) {

                case CONSTANT:
                    stack.push(std::pair<T, T > (currNode->GetValue(), T(0)));
                    break;
                case VARIABLE:
                    if (currNode->GetId() == id) {
                        found = true;
                        //f(x) = x
                        //f'(x) = 1
                        stack.push(std::pair<T, T > (currNode->GetValue(), T(1)));
                    } else {//constant
                        //f(x) = C
                        //f'(x) = 0
                        stack.push(std::pair<T, T > (currNode->GetValue(), T(0)));
                    }
                    break;
                case PLUS:
                    rhs = stack.top();
                    stack.pop();
                    lhs = stack.top();
                    stack.pop();
                    stack.push(std::pair<T, T > (lhs.first + rhs.first, lhs.second + rhs.second));
                    // ret = lhs.second + rhs.second;
                    break;
                case MINUS:
                    rhs = stack.top();
                    stack.pop();
                    lhs = stack.top();
                    stack.pop();
                    stack.push(std::pair<T, T > (lhs.first - rhs.first, lhs.second - rhs.second));
                    // ret = lhs.second-rhs.second;
                    break;
                case MULTIPLY:
                    rhs = stack.top();
                    stack.pop();
                    lhs = stack.top();
                    stack.pop();
                    temp = lhs.second * rhs.first + lhs.first * rhs.second;
                    stack.push(std::pair<T, T > (lhs.first * rhs.first, temp));
                    // ret =temp;

                    break;
                case DIVIDE:
                    rhs = stack.top();
                    stack.pop();
                    lhs = stack.top();
                    stack.pop();
                    temp = (lhs.second * rhs.first + lhs.first * rhs.second) / (rhs.first * rhs.first);
                    stack.push(std::pair<T, T > (lhs.first / rhs.first, temp));
                    // ret = temp;
                    break;

                case SIN:
                    lhs = stack.top();
                    stack.pop();
                    if (/*currNode->HasId(id)*/found) {
                        stack.push(std::pair<T, T > (std::sin(lhs.first), lhs.second * std::cos(lhs.first)));
                    } else {
                        stack.push(std::pair<T, T > (std::sin(lhs.first), T(0)));
                    }
                    //ret = stack.top().second;
                    break;
                case COS:
                    lhs = stack.top();
                    stack.pop();
                    if (/*currNode->HasId(id)*/found) {
                        stack.push(std::pair<T, T > (std::cos(lhs.first), lhs.second * (-1.0) * std::sin(lhs.first)));
                    } else {
                        stack.push(std::pair<T, T > (std::cos(lhs.first), T(0)));
                    }
                    //ret = stack.top().second;
                    break;
                case TAN:
                    lhs = stack.top();
                    stack.pop();
                    if (/*currNode->HasId(id)*/found) {
                        temp = lhs.second * ((T(1.0) / std::cos(lhs.first))*(T(1.0) / std::cos(lhs.first)));
                        stack.push(std::pair<T, T > (std::tan(lhs.first), temp));
                    } else {
                        stack.push(std::pair<T, T > (std::tan(lhs.first), T(0)));
                    }
                    //ret = stack.top().second;
                    break;
                case ASIN:
                    lhs = stack.top();
                    stack.pop();
                    if (/*currNode->HasId(id)*/found) {
                        temp = (lhs.second * T(1.0) / std::pow((T(1.0) - std::pow(lhs.first, T(2.0))), T(0.5)));
                        stack.push(std::pair<T, T > (std::asin(lhs.first), temp));
                    } else {
                        stack.push(std::pair<T, T > (std::asin(lhs.first), T(0)));
                    }
                    //ret = stack.top().second;
                    break;
                case ACOS:
                    lhs = stack.top();
                    stack.pop();
                    if (/*currNode->HasId(id)*/found) {
                        temp = (lhs.second * T(-1.0) / std::pow((T(1.0) - std::pow(lhs.first, T(2.0))), T(0.5)));
                        stack.push(std::pair<T, T > (std::acos(lhs.first), temp));
                    } else {
                        stack.push(std::pair<T, T > (std::acos(lhs.first), T(0)));
                    }
                    //ret = stack.top().second;
                    break;
                case ATAN:
                    lhs = stack.top();
                    stack.pop();
                    if (/*currNode->HasId(id)*/found) {
                        temp = (lhs.second * T(1.0) / (lhs.first * lhs.first + T(1.0)));
                        stack.push(std::pair<T, T > (std::atan(lhs.first), temp));
                    } else {
                        stack.push(std::pair<T, T > (std::atan(lhs.first), T(0)));
                    }
                    //ret = stack.top().second;
                    break;
                case ATAN2:
                    rhs = stack.top();
                    stack.pop();
                    lhs = stack.top();
                    stack.pop();
                    if (/*currNode->HasId(id)*/found) {
                        temp = (rhs.first * lhs.second / (lhs.first * lhs.first + (rhs.first * rhs.first)));
                        stack.push(std::pair<T, T > (std::atan2(lhs.first, rhs.first), temp));
                    } else {
                        stack.push(std::pair<T, T > (std::atan2(lhs.first, rhs.first), T(0)));
                    }
                    //ret = stack.top().second;
                    break;
                case SQRT:
                    lhs = stack.top();
                    stack.pop();
                    if (/*currNode->HasId(id)*/found) {
                        temp = lhs.second * T(.5) / std::sqrt(lhs.first);
                        stack.push(std::pair<T, T > (std::sqrt(lhs.first), temp));
                    } else {
                        stack.push(std::pair<T, T > (std::sqrt(lhs.first), T(0)));
                    }
                    //ret = stack.top().second;
                    break;
                case POW:
                    rhs = stack.top();
                    stack.pop();
                    lhs = stack.top();
                    stack.pop();
                    if (/*currNode->HasId(id)*/found) {
                        temp = (lhs.second * rhs.first) *
                                std::pow(lhs.first, (rhs.first - T(1.0)));
                        stack.push(std::pair<T, T > (std::pow(lhs.first, rhs.first), temp));
                    } else {
                        stack.push(std::pair<T, T > (std::pow(lhs.first, rhs.first), T(0)));
                    }
                    //ret = stack.top().second;
                    break;
                case LOG:
                    lhs = stack.top();
                    stack.pop();
                    if (/*currNode->HasId(id)*/found) {
                        temp = (lhs.second * T(1.0)) / lhs.first;
                        stack.push(std::pair<T, T > (std::log(lhs.first), temp));
                    } else {
                        stack.push(std::pair<T, T > (std::log(lhs.first), T(0)));
                    }
                    //ret = stack.top().second;
                    break;
                case LOG10:
                    lhs = stack.top();
                    stack.pop();
                    if (/*currNode->HasId(id)*/found) {
                        temp = (lhs.second * T(1.0)) / (lhs.first * std::log(T(10.0)));
                        stack.push(std::pair<T, T > (std::log10(lhs.first), temp));
                    } else {
                        stack.push(std::pair<T, T > (std::log10(lhs.first), T(0)));
                    }
                    //ret = stack.top().second;
                    break;
                case EXP:
                    lhs = stack.top();
                    stack.pop();
                    if (/*currNode->HasId(id)*/found) {
                        temp = lhs.second * std::exp(lhs.first);
                        stack.push(std::pair<T, T > (std::exp(lhs.first), temp));
                    } else {
                        stack.push(std::pair<T, T > (std::exp(lhs.first), T(0)));
                    }
                    //ret = stack.top().second;
                    break;
                case SINH:
                    lhs = stack.top();
                    stack.pop();
                    if (/*currNode->HasId(id)*/found) {
                        temp = lhs.second * std::cosh(lhs.first);
                        stack.push(std::pair<T, T > (std::sinh(lhs.first), temp));
                    } else {
                        stack.push(std::pair<T, T > (std::sinh(lhs.first), T(0)));
                    }
                    //ret = stack.top().second;
                    break;
                case COSH:
                    lhs = stack.top();
                    stack.pop();
                    if (/*currNode->HasId(id)*/found) {
                        temp = lhs.second * std::sinh(lhs.first);
                        stack.push(std::pair<T, T > (std::cosh(lhs.first), temp));
                    } else {
                        stack.push(std::pair<T, T > (std::cosh(lhs.first), T(0)));
                    }
                    //ret = stack.top().second;
                    break;
                case TANH:
                    lhs = stack.top();
                    stack.pop();
                    if (/*currNode->HasId(id)*/found) {
                        temp = lhs.second * (T(1.0) / std::cosh(lhs.first))*(T(1.0) / std::cosh(lhs.first));
                        stack.push(std::pair<T, T > (std::tanh(lhs.first), temp));
                    } else {
                        stack.push(std::pair<T, T > (std::tanh(lhs.first), T(0)));
                    }
                    //ret = stack.top().second;
                    break;
                case FABS:
                    lhs = stack.top();
                    stack.pop();
                    if (/*currNode->HasId(id)*/found) {
                        temp = (lhs.second * lhs.first) /
                                std::fabs(lhs.first);
                        stack.push(std::pair<T, T > (std::fabs(lhs.first), temp));
                    } else {
                        stack.push(std::pair<T, T > (std::fabs(lhs.first), T(0)));
                    }
                    //ret = stack.top().second;
                    break;
                case ABS:
                    lhs = stack.top();
                    stack.pop();
                    if (/*currNode->HasId(id)*/found) {
                        temp = (lhs.second * lhs.first) /
                                std::fabs(lhs.first);
                        stack.push(std::pair<T, T > (std::fabs(lhs.first), temp));
                    } else {
                        stack.push(std::pair<T, T > (std::fabs(lhs.first), T(0)));
                    }
                    //ret = stack.top().second;
                    break;
                case FLOOR:
                    lhs = stack.top();
                    stack.pop();
                    if (/*currNode->HasId(id)*/found) {

                        temp = T(0); //lhs.second * T(std::floor(lhs.first));
                        stack.push(std::pair<T, T > (std::floor(lhs.first), temp));
                    } else {
                        stack.push(std::pair<T, T > (std::floor(lhs.first), T(0)));
                    }
                    //ret = stack.top().second;
                    break;
                case NONE:
                    std::cout << "nothing to do here.\n";
                    break;
                default:
                    break;

            }

            it++;
        }


        return stack.top().second;

    }

    template<class T>
    static ad::Expression<T>* Differentiate(ad::Expression<T>* exp, unsigned long id = 0) {
        std::deque<std::pair<ad::Expression<T>*, ad::Expression<T>*> > stack;

        ad::PostOrderIterator<T> it(exp);

        bool found = false;


        while (it) {


            std::pair<ad::Expression<T>*, ad::Expression<T>*> lhs =
                    std::pair<ad::Expression<T>*, ad::Expression<T>*> (NULL, NULL);
            std::pair<ad::Expression<T>*, ad::Expression<T>*> rhs =
                    std::pair<ad::Expression<T>*, ad::Expression<T>*> (NULL, NULL);

            ad::Expression<T>* currNode = it;
            ad::Expression<T>* temp;
            switch (currNode->GetOp()) {

                case ad::CONSTANT:
                    temp = new ad::Expression<T > ();
                    temp->SetValue(0.0);
                    temp->SetOp(ad::VARIABLE);
                    //temp->take();
                    stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    break;
                case ad::VARIABLE:
                    if (currNode->GetId() == id) {
                        found = true;
                        //f(x) = x
                        //f'(x) = 1
                        temp = new ad::Expression<T > ();
                        temp->SetValue(1.0);
                        temp->SetOp(ad::VARIABLE);
                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    } else {//constant
                        //f(x) = C
                        //f'(x) = 0
                        temp = new ad::Expression<T > ();
                        temp->SetValue(0.0);
                        temp->SetOp(ad::VARIABLE);
                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    }
                    break;
                case ad::PLUS:
                    rhs = stack.front();
                    stack.pop_front();
                    lhs = stack.front();
                    stack.pop_front();

                    temp = new ad::Expression<T > ();
                    temp->SetOp(ad::PLUS);
                    temp->SetLeft(ad::Clone(lhs.second));
                    temp->SetRight(ad::Clone(rhs.second));
                    //temp->take();
                    stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    break;
                case ad::MINUS:
                    rhs = stack.front();
                    stack.pop_front();
                    lhs = stack.front();
                    stack.pop_front();

                    temp = new ad::Expression<T > ();
                    temp->SetOp(ad::MINUS);
                    temp->SetLeft(ad::Clone(lhs.second));
                    temp->SetRight(ad::Clone(rhs.second));
                    //temp->take();
                    stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    break;
                case ad::MULTIPLY:
                    rhs = stack.front();
                    stack.pop_front();
                    lhs = stack.front();
                    stack.pop_front();
                    temp = new ad::Expression<T > ();
                    temp->SetOp(ad::PLUS);
                    temp->SetLeft(new ad::Expression<T > ());
                    temp->GetLeft()->SetOp(ad::MULTIPLY);
                    temp->GetLeft()->SetLeft(ad::Clone(lhs.second));
                    temp->GetLeft()->SetRight(rhs.first);
                    temp->SetRight(new ad::Expression<T > ());

                    temp->GetRight()->SetOp(ad::MULTIPLY);
                    temp->GetRight()->SetLeft(lhs.first);
                    temp->GetRight()->SetRight(ad::Clone(rhs.second));

                    //temp->take();
                    stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));

                    break;
                case ad::DIVIDE:
                    rhs = stack.front();
                    stack.pop_front();
                    lhs = stack.front();
                    stack.pop_front();
                    temp = new ad::Expression<T > ();
                    temp->SetOp(ad::DIVIDE);
                    temp->SetLeft(new ad::Expression<T > ());
                    temp->GetLeft()->SetOp(ad::PLUS);
                    temp->GetLeft()->SetLeft(new ad::Expression<T > ());
                    temp->GetLeft()->GetLeft()->SetOp(ad::MULTIPLY);
                    temp->GetLeft()->GetLeft()->SetLeft(ad::Clone(lhs.second));
                    temp->GetLeft()->GetLeft()->SetRight(rhs.first);
                    temp->GetLeft()->SetRight(new ad::Expression<T > ());

                    temp->GetLeft()->GetRight()->SetOp(ad::MULTIPLY);
                    temp->GetLeft()->GetRight()->SetLeft(lhs.first);
                    temp->GetLeft()->GetRight()->SetRight(ad::Clone(rhs.second));

                    temp->SetRight(new ad::Expression<T > ());
                    temp->GetRight()->SetOp(ad::MULTIPLY);
                    temp->GetRight()->SetLeft(rhs.first);
                    temp->GetRight()->SetRight(rhs.first);

                    //temp->take();
                    stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));

                    break;
                case ad::SIN:
                    lhs = stack.front();
                    stack.pop_front();

                    if (found) {
                        temp = new ad::Expression<T > ();
                        temp->SetOp(ad::MULTIPLY);
                        temp->SetLeft(ad::Clone(lhs.second));
                        temp->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->SetOp(ad::COS);
                        temp->GetRight()->SetLeft(lhs.first);
                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    } else {
                        temp = new ad::Expression<T > ();
                        temp->SetValue(0.0);
                        temp->SetOp(ad::VARIABLE);
                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    }

                    break;
                case ad::COS:
                    lhs = stack.front();
                    stack.pop_front();

                    if (found) {
                        temp = new ad::Expression<T > ();
                        temp->SetOp(ad::MULTIPLY);

                        temp->SetLeft(new ad::Expression<T>);
                        temp->GetLeft()->SetOp(ad::MULTIPLY);
                        temp->GetLeft()->SetLeft(new ad::Expression<T > ());
                        temp->GetLeft()->GetLeft()->SetOp(ad::CONSTANT);
                        temp->GetLeft()->GetLeft()->SetValue(-1.0);
                        temp->GetLeft()->SetRight(ad::Clone(lhs.second));



                        temp->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->SetOp(ad::SIN);
                        temp->GetRight()->SetLeft(lhs.first);
                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    } else {
                        temp = new ad::Expression<T > ();
                        temp->SetValue(0.0);
                        temp->SetOp(ad::VARIABLE);
                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    }

                    break;

                case ad::TAN:
                    lhs = stack.front();
                    stack.pop_front();

                    if (found) {
                        temp = new ad::Expression<T > ();
                        temp->SetOp(ad::MULTIPLY);

                        temp->SetLeft(ad::Clone(lhs.second));

                        temp->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->SetOp(ad::MULTIPLY);
                        temp->GetRight()->SetLeft(new ad::Expression<T > ());
                        temp->GetRight()->GetLeft()->SetOp(ad::DIVIDE);
                        temp->GetRight()->GetLeft()->SetLeft(new ad::Expression<T > ());
                        temp->GetRight()->GetLeft()->GetLeft()->SetOp(ad::CONSTANT);
                        temp->GetRight()->GetLeft()->GetLeft()->SetValue(1.0);
                        temp->GetRight()->GetLeft()->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->GetLeft()->GetRight()->SetOp(ad::COS);
                        temp->GetRight()->GetLeft()->GetRight()->SetLeft(lhs.first);

                        temp->GetRight()->SetRight(ad::Clone(temp->GetRight()->GetLeft()));

                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    } else {
                        temp = new ad::Expression<T > ();
                        temp->SetValue(0.0);
                        temp->SetOp(ad::VARIABLE);
                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    }

                    break;

                case ad::ASIN:
                    lhs = stack.front();
                    stack.pop_front();

                    if (found) {
                        temp = new ad::Expression<T > ();
                        temp->SetOp(ad::MULTIPLY);

                        temp->SetLeft(ad::Clone(lhs.second));
                        temp->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->SetOp(ad::DIVIDE);
                        temp->GetRight()->SetLeft(new ad::Expression<T > ());
                        temp->GetRight()->GetLeft()->SetOp(ad::CONSTANT);
                        temp->GetRight()->GetLeft()->SetValue(1.0);

                        temp->GetRight()->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->GetRight()->SetOp(ad::POW);
                        temp->GetRight()->GetRight()->SetLeft(new ad::Expression<T > ());
                        temp->GetRight()->GetRight()->GetLeft()->SetOp(ad::MINUS);
                        temp->GetRight()->GetRight()->GetLeft()->SetLeft(new ad::Expression<T > ());
                        temp->GetRight()->GetRight()->GetLeft()->GetLeft()->SetOp(ad::CONSTANT);
                        temp->GetRight()->GetRight()->GetLeft()->GetLeft()->SetValue(1.0);

                        temp->GetRight()->GetRight()->GetLeft()->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->GetRight()->GetLeft()->GetRight()->SetOp(ad::POW);
                        temp->GetRight()->GetRight()->GetLeft()->GetRight()->SetLeft(lhs.first);
                        temp->GetRight()->GetRight()->GetLeft()->GetRight()->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->GetRight()->GetLeft()->GetRight()->GetRight()->SetOp(ad::CONSTANT);
                        temp->GetRight()->GetRight()->GetLeft()->GetRight()->GetRight()->SetValue(2.0);

                        temp->GetRight()->GetRight()->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->GetRight()->GetRight()->SetOp(ad::CONSTANT);
                        temp->GetRight()->GetRight()->GetRight()->SetValue(0.5);

                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    } else {
                        temp = new ad::Expression<T > ();
                        temp->SetValue(0.0);
                        temp->SetOp(ad::VARIABLE);
                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    }

                    break;
                case ad::ACOS:
                    lhs = stack.front();
                    stack.pop_front();

                    if (found) {
                        temp = new ad::Expression<T > ();
                        temp->SetOp(ad::MULTIPLY);

                        temp->SetLeft(ad::Clone(lhs.second));
                        temp->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->SetOp(ad::DIVIDE);
                        temp->GetRight()->SetLeft(new ad::Expression<T > ());
                        temp->GetRight()->GetLeft()->SetOp(ad::CONSTANT);
                        temp->GetRight()->GetLeft()->SetValue(-1.0);

                        temp->GetRight()->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->GetRight()->SetOp(ad::POW);
                        temp->GetRight()->GetRight()->SetLeft(new ad::Expression<T > ());
                        temp->GetRight()->GetRight()->GetLeft()->SetOp(ad::MINUS);
                        temp->GetRight()->GetRight()->GetLeft()->SetLeft(new ad::Expression<T > ());
                        temp->GetRight()->GetRight()->GetLeft()->GetLeft()->SetOp(ad::CONSTANT);
                        temp->GetRight()->GetRight()->GetLeft()->GetLeft()->SetValue(1.0);

                        temp->GetRight()->GetRight()->GetLeft()->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->GetRight()->GetLeft()->GetRight()->SetOp(ad::POW);
                        temp->GetRight()->GetRight()->GetLeft()->GetRight()->SetLeft(lhs.first);
                        temp->GetRight()->GetRight()->GetLeft()->GetRight()->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->GetRight()->GetLeft()->GetRight()->GetRight()->SetOp(ad::CONSTANT);
                        temp->GetRight()->GetRight()->GetLeft()->GetRight()->GetRight()->SetValue(2.0);

                        temp->GetRight()->GetRight()->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->GetRight()->GetRight()->SetOp(ad::CONSTANT);
                        temp->GetRight()->GetRight()->GetRight()->SetValue(0.5);

                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    } else {
                        temp = new ad::Expression<T > ();
                        temp->SetValue(0.0);
                        temp->SetOp(ad::VARIABLE);
                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    }

                    break;
                case ad::ATAN:
                    lhs = stack.front();
                    stack.pop_front();

                    if (found) {
                        temp = new ad::Expression<T > ();
                        temp->SetOp(ad::MULTIPLY);

                        temp->SetLeft(ad::Clone(lhs.second));
                        temp->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->SetOp(ad::DIVIDE);
                        temp->GetRight()->SetLeft(new ad::Expression<T > ());
                        temp->GetRight()->GetLeft()->SetOp(ad::CONSTANT);
                        temp->GetRight()->GetLeft()->SetValue(1.0);

                        temp->GetRight()->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->GetRight()->SetOp(ad::PLUS);
                        temp->GetRight()->GetRight()->SetLeft(new ad::Expression<T > ());
                        temp->GetRight()->GetRight()->GetLeft()->SetOp(ad::MULTIPLY);
                        temp->GetRight()->GetRight()->GetLeft()->SetLeft(lhs.first);
                        temp->GetRight()->GetRight()->GetLeft()->SetRight(lhs.first);
                        temp->GetRight()->GetRight()->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->GetRight()->GetRight()->SetOp(ad::CONSTANT);
                        temp->GetRight()->GetRight()->GetRight()->SetValue(1.0);

                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    } else {
                        temp = new ad::Expression<T > ();
                        temp->SetValue(0.0);
                        temp->SetOp(ad::VARIABLE);
                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    }

                    break;
                case ad::ATAN2:

                    rhs = stack.front();
                    stack.pop_front();
                    lhs = stack.front();
                    stack.pop_front();



                    if (found) {
                        temp = new ad::Expression<T > ();
                        temp->SetOp(ad::MULTIPLY);

                        temp->SetLeft(ad::Clone(ad::Clone(rhs.second)));
                        temp->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->SetOp(ad::DIVIDE);
                        temp->GetRight()->SetLeft(ad::Clone(ad::Clone(lhs.second)));


                        temp->GetRight()->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->GetRight()->SetOp(ad::PLUS);
                        temp->GetRight()->GetRight()->SetLeft(new ad::Expression<T > ());
                        temp->GetRight()->GetRight()->GetLeft()->SetOp(ad::MULTIPLY);
                        temp->GetRight()->GetRight()->GetLeft()->SetLeft(ad::Clone(lhs.first));
                        temp->GetRight()->GetRight()->GetLeft()->SetRight(ad::Clone(lhs.first));
                        temp->GetRight()->GetRight()->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->GetRight()->GetRight()->SetOp(ad::MULTIPLY);
                        temp->GetRight()->GetRight()->GetRight()->SetLeft(ad::Clone(rhs.first));
                        temp->GetRight()->GetRight()->GetRight()->SetRight(ad::Clone(rhs.first));

                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));

                    } else {
                        temp = new ad::Expression<T > ();
                        temp->SetValue(0.0);
                        temp->SetOp(ad::VARIABLE);
                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    }

                    break;
                case ad::SQRT:
                    lhs = stack.front();
                    stack.pop_front();

                    if (found) {
                        temp = new ad::Expression<T > ();
                        temp->SetOp(ad::MULTIPLY);

                        temp->SetLeft(ad::Clone(lhs.second));
                        temp->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->SetOp(ad::DIVIDE);
                        temp->GetRight()->SetLeft(new ad::Expression<T > ());
                        temp->GetRight()->GetLeft()->SetOp(ad::CONSTANT);
                        temp->GetRight()->GetLeft()->SetValue(.5);

                        temp->GetRight()->SetRight(lhs.first);

                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    } else {
                        temp = new ad::Expression<T > ();
                        temp->SetValue(0.0);
                        temp->SetOp(ad::VARIABLE);
                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    }

                    break;
                case ad::POW:

                    rhs = stack.front();
                    stack.pop_front();
                    lhs = stack.front();
                    stack.pop_front();



                    if (found) {
                        temp = new ad::Expression<T > ();
                        temp->SetOp(ad::MULTIPLY);
                        temp->SetLeft(new ad::Expression<T > ());
                        temp->GetLeft()->SetOp(ad::MULTIPLY);
                        temp->GetLeft()->SetLeft(ad::Clone(lhs.second));
                        temp->GetLeft()->SetRight(rhs.first);

                        temp->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->SetOp(ad::POW);
                        temp->GetRight()->SetLeft(lhs.first);

                        temp->GetRight()->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->GetRight()->SetOp(ad::MINUS);
                        temp->GetRight()->GetRight()->SetLeft(rhs.first);
                        temp->GetRight()->GetRight()->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->GetRight()->GetRight()->SetOp(ad::CONSTANT);
                        temp->GetRight()->GetRight()->GetRight()->SetValue(1.0);

                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));

                    } else {
                        temp = new ad::Expression<T > ();
                        temp->SetValue(0.0);
                        temp->SetOp(ad::VARIABLE);
                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    }

                    break;
                case ad::LOG:

                    lhs = stack.front();
                    stack.pop_front();



                    if (found) {
                        temp = new ad::Expression<T > ();
                        temp->SetOp(ad::DIVIDE);
                        temp->SetLeft(ad::Clone(lhs.second));
                        temp->SetRight(lhs.first);
                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));

                    } else {
                        temp = new ad::Expression<T > ();
                        temp->SetValue(0.0);
                        temp->SetOp(ad::VARIABLE);
                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    }

                    break;
                case ad::LOG10:

                    lhs = stack.front();
                    stack.pop_front();



                    if (found) {
                        temp = new ad::Expression<T > ();
                        temp->SetOp(ad::DIVIDE);
                        temp->SetLeft(ad::Clone(lhs.second));
                        temp->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->SetOp(ad::MULTIPLY);
                        temp->GetRight()->SetLeft(lhs.first);
                        temp->GetRight()->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->GetRight()->SetOp(ad::LOG);
                        temp->GetRight()->GetRight()->SetLeft(new ad::Expression<T > ());
                        temp->GetRight()->GetRight()->GetLeft()->SetOp(ad::CONSTANT);
                        temp->GetRight()->GetRight()->GetLeft()->SetValue(10.0);


                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));

                    } else {
                        temp = new ad::Expression<T > ();
                        temp->SetValue(0.0);
                        temp->SetOp(ad::VARIABLE);
                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    }

                    break;
                case ad::EXP:

                    lhs = stack.front();
                    stack.pop_front();



                    if (found) {
                        temp = new ad::Expression<T > ();
                        temp->SetOp(ad::MULTIPLY);
                        temp->SetLeft(ad::Clone(lhs.second));
                        temp->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->SetOp(ad::EXP);
                        temp->GetRight()->SetLeft(lhs.first);


                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));

                    } else {
                        temp = new ad::Expression<T > ();
                        temp->SetValue(0.0);
                        temp->SetOp(ad::VARIABLE);
                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    }

                    break;
                case ad::SINH:

                    lhs = stack.front();
                    stack.pop_front();



                    if (found) {
                        temp = new ad::Expression<T > ();
                        temp->SetOp(ad::MULTIPLY);
                        temp->SetLeft(ad::Clone(lhs.second));
                        temp->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->SetOp(ad::COSH);
                        temp->GetRight()->SetLeft(lhs.first);


                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));

                    } else {
                        temp = new ad::Expression<T > ();
                        temp->SetValue(0.0);
                        temp->SetOp(ad::VARIABLE);
                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    }

                    break;
                case ad::COSH:

                    lhs = stack.front();
                    stack.pop_front();



                    if (found) {
                        temp = new ad::Expression<T > ();
                        temp->SetOp(ad::MULTIPLY);
                        temp->SetLeft(ad::Clone(lhs.second));
                        temp->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->SetOp(ad::SINH);
                        temp->GetRight()->SetLeft(lhs.first);


                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));

                    } else {
                        temp = new ad::Expression<T > ();
                        temp->SetValue(0.0);
                        temp->SetOp(ad::VARIABLE);
                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    }

                    break;
                case ad::TANH:
                    lhs = stack.front();
                    stack.pop_front();

                    if (found) {
                        temp = new ad::Expression<T > ();
                        temp->SetOp(ad::MULTIPLY);

                        temp->SetLeft(ad::Clone(lhs.second));

                        temp->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->SetOp(ad::MULTIPLY);
                        temp->GetRight()->SetLeft(new ad::Expression<T > ());
                        temp->GetRight()->GetLeft()->SetOp(ad::DIVIDE);
                        temp->GetRight()->GetLeft()->SetLeft(new ad::Expression<T > ());
                        temp->GetRight()->GetLeft()->GetLeft()->SetOp(ad::CONSTANT);
                        temp->GetRight()->GetLeft()->GetLeft()->SetValue(1.0);
                        temp->GetRight()->GetLeft()->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->GetLeft()->GetRight()->SetOp(ad::COSH);
                        temp->GetRight()->GetLeft()->GetRight()->SetLeft(lhs.first);

                        temp->GetRight()->SetRight(ad::Clone(temp->GetRight()->GetLeft()));

                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    } else {
                        temp = new ad::Expression<T > ();
                        temp->SetValue(0.0);
                        temp->SetOp(ad::VARIABLE);
                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    }

                    break;
                case ad::FABS:
                    lhs = stack.front();
                    stack.pop_front();

                    if (found) {
                        temp = new ad::Expression<T > ();
                        temp->SetOp(ad::DIVIDE);
                        temp->SetLeft(new ad::Expression<T > ());
                        temp->GetLeft()->SetOp(ad::MULTIPLY);
                        temp->GetLeft()->SetLeft(ad::Clone(lhs.second));
                        temp->GetLeft()->SetRight(lhs.first);
                        temp->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->SetOp(ad::FABS);
                        temp->GetRight()->SetLeft(lhs.first);


                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    } else {
                        temp = new ad::Expression<T > ();
                        temp->SetValue(0.0);
                        temp->SetOp(ad::VARIABLE);
                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    }

                    break;
                case ad::ABS:
                    lhs = stack.front();
                    stack.pop_front();

                    if (found) {
                        temp = new ad::Expression<T > ();
                        temp->SetOp(ad::DIVIDE);
                        temp->SetLeft(new ad::Expression<T > ());
                        temp->GetLeft()->SetOp(ad::MULTIPLY);
                        temp->GetLeft()->SetLeft(ad::Clone(lhs.second));
                        temp->GetLeft()->SetRight(lhs.first);
                        temp->SetRight(new ad::Expression<T > ());
                        temp->GetRight()->SetOp(ad::FABS);
                        temp->GetRight()->SetLeft(lhs.first);


                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    } else {
                        temp = new ad::Expression<T > ();
                        temp->SetValue(0.0);
                        temp->SetOp(ad::VARIABLE);
                        //temp->take();
                        stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));
                    }

                    break;
                case ad::FLOOR:
                    lhs = stack.front();
                    stack.pop_front();


                    temp = new ad::Expression<T > ();
                    temp->SetValue(0.0);
                    temp->SetOp(ad::VARIABLE);
                    //temp->take();
                    stack.push_front(std::pair<ad::Expression<T>*, ad::Expression<T>*> (currNode, temp));


                    break;



            }



            it++;
        }

        for (int i = 0; i < stack.size(); i++) {
            //            if(stack[i].second->References() <1){
            std::cout << stack[i].second->References() << "references...\n";
            //                std::cout<<stack[i].second->ToString(true)<<"\n\n\n";
            //            }
        }


        return stack.front().second;


    }


}
#endif	/* EXPRESSION_HPP */

