/*!
 *  Software to compute derivatives. Support for nth partial and mixed 
 *  derivatives, and mixed derivatives.
 */

/*!
 *   This library is dual-licensed: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License version <N> as 
 *   published by the Free Software Foundation. For the terms of this 
 *   license, see licenses/gpl_v<N>.txt or <http://www.gnu.org/licenses/>.
 *
 *   You are free to use this library under the terms of the GNU General
 *   Public License, but WITHOUT ANY WARRANTY; without even the implied 
 *   warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *   See the GNU General Public License for more details.
 *
 *  Alternatively, you can license this library under a commercial
 *  license.
 *
 *               ADNumber Commercial License(ACL)
 *               ================================
 * ---------------------------------------------------------------------
 * 
 *  The license is non-exclusively granted to a single person or company,
 *  per payment of the license fee, for the lifetime of that person or
 *  company. The license is non-transferrable.
 * 
 *  The ACL grants the licensee the right to use ADNumber in commercial
 *  closed-source projects. Modifications may be made to ADNumber with no
 *  obligation to release the modified source code. ADNumber (or pieces
 *  thereof) may be included in any number of projects authored (in whole
 *  or in part) by the licensee.
 * 
 *  The licensee may use any version of ADNumber, past, present or future,
 *  as is most convenient. This license does not entitle the licensee to
 *  receive any technical support, updates or bug fixes, except as such are
 *  made publicly available to all ADNumber users.
 * 
 *  The licensee may not sub-license ADNumber itself, meaning that any
 *  commercially released product containing all or parts of ADNumber must
 *  have added functionality beyond what is available in ADNumber;
 *  ADNumber itself may not be re-licensed by the licensee.
 * 
 *  To obtain a commercial license agreement, contact:
 * 
 *  Matthew Supernaw
 *  msupernaw@gmail.com
 * 
 */

/*!
 *
 * File:   ADNumber.hpp
 * Author: Matthew R. Supernaw
 *
 * Created on January 4, 2012, 7:08 PM
 */


#ifndef AD_ADNUMBER_HPP
#define	AD_ADNUMBER_HPP

#include <stdlib.h> 
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <vector>
#include <list>
#include <stack>
#include <map>
#include <cmath>
#include <assert.h>
#include <queue>
#include <utility>
#include <string>
#include <limits>
#include <stdint.h>



#define USE_CLFMALLOC

#ifdef USE_CLFMALLOC

#include "clfmalloc.h"

#endif

#define USE_FAST_STACK

#ifdef USE_FAST_STACK

#ifdef ADNUMBER_MPI_SUPPORT
#include <mpi.h>
#endif


//std::stack<Expression<T>*, std::vector<Expression<T>* > >
#define ExpressionStack FastStack<Expression<T>* >
//std::stack<T, std::vector<T> >
#define TypeStack FastStack<T>
#define PairStack FastStack<std::pair<T, T> > stack;
#else


//std::stack<Expression<T>*, std::vector<Expression<T>* > >
#define ExpressionStack std::stack<Expression<T>*, std::vector<Expression<T>* > >
//std::stack<T, std::vector<T> >
#define TypeStack std::stack<T, std::vector<T> >
#define PairStack std::stack<std::pair<T, T>, std::vector<std::pair<T, T> > > stack(vect);

#endif



#define ExpressionPtr Expression<T>*



#define NEW_EXPRESSION(T)  new ad::Expression<T>

#define DELETE_EXPRESSION(x) delete x

namespace ad {

    template<class T> class ADNumber;

    template<class T>
    static T DerivativeValue(const ADNumber<T> &x, const ADNumber<T> &wrt, unsigned int order = 1);

    template<class T>
    static const ADNumber<T> Derivative(const ADNumber<T> &x, const ADNumber<T> &wrt, unsigned int order = 1);

    template<class T>
    const std::string ToString(const ADNumber<T> &x, bool latex = true);

#ifdef COLLECTION_TRACE
    std::vector<std::pair<void*, std::string> > collected_garbage;
#endif

    /*!
     * Creates a unique identifier.
     * @return 
     */
    class IDGenerator {
    public:
        static IDGenerator * instance();

        const unsigned long next() {
            return _id++;
        }
    private:

        IDGenerator() : _id(1) {
        }

        unsigned long _id;
    };

    static IDGenerator* only_copy;

    inline IDGenerator *
    IDGenerator::instance() {

        if (!only_copy) {
            only_copy = new IDGenerator();
        }

        return only_copy;
    }

    template <class T>
    class FastStack {
    public:
        T* st;
        int allocationSize;
        int lastIndex;

    public:
        FastStack(int stackSize = 100);
        // FastStack();
        ~FastStack();

        inline void resize(int newSize);
        inline void push(T x);
        inline void pop();
        inline T getAndRemove();
        inline T top();
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
    FastStack<T>::FastStack(int stackSize) {
        st = NULL;
        this->allocationSize = stackSize;
        st = (T*) malloc(sizeof (T) * stackSize);
        lastIndex = -1;
    }

    template <class T>
    FastStack<T>::~FastStack() {
        free(st);
    }

    template <class T>
    void FastStack<T>::clear() {
        lastIndex = -1;
    }

    template <class T>
    T FastStack<T>::top() {

        return st[lastIndex];
    }

    template <class T>
    T FastStack<T>::getAndRemove() {
        return st[lastIndex--];
    }

    template <class T>
    void FastStack<T>::pop() {
        --lastIndex;
    }

    template <class T>
    void FastStack<T>::push(T x) {
        if (++lastIndex == (this->allocationSize)) {

            this->allocationSize += this->allocationSize;
            st = (T *) realloc(st, this->allocationSize * sizeof (T));
            //        free(st);
            //        this->st = temp;
        }

        st[lastIndex] = x;
    }

    template <class T>
    void FastStack<T>::resize(int newSize) {
        if (st != NULL)
            delete [] st;
        st = new T[newSize];
    }

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

    template<class T>
    class Expression {
        T value_m;
        ExpressionPtr left_m;
        ExpressionPtr right_m;
        unsigned long id_m;
        Operation op_m;
        mutable int count_m;
        std::string name_m;
#ifdef AD_THREAD_SAFE
        mutable Mutex mutex;
#endif

    public:

        Expression()
        : right_m(NULL),
        left_m(NULL),
        op_m(CONSTANT),
        //name("na"),
        id_m(0),
        value_m(T(0.0)),
        count_m(0) {


        }

        Expression(const T &value, const unsigned long &id, const std::string &name, const Operation &op, ExpressionPtr left, ExpressionPtr right)
        : right_m(right),
        left_m(left),
        op_m(op),
        name_m(name),
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
        : right_m(right),
        left_m(left),
        op_m(op),
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

        ~Expression() {


        }

        inline void take() const {
            ++count_m;
        }

        inline void release(bool ignore_delete = false) {
            //            if (count <= 0) {
            //                std::cout << "assert" << ToString() << "\n" << std::flush;
            //            }


            assert(count_m > 0);
            --count_m;

            if (count_m == 0 && !ignore_delete) {

                ExpressionStack stack;

                ExpressionPtr n = this;
                bool do_delete = false;

                do {
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
                            n = n->GetLeft();
                            if (exp != this) {
                                free(exp);
                            }
                        } else {
                            n = n->GetLeft();
                        }



                    } else {
                        n = stack.top();
                        stack.pop();
                    }


                } while (!stack.empty() || n != NULL);

                free(this);
            }
        }

        inline int References() const {
            return count_m;
        }



        //   
        //        const std::vector<Expression<T>* > List() {
        //
        //            std::stack <Expression<T> * > S;
        //            std::vector<Expression<T>* > list;
        //            Expression<T>* p = this;
        //
        //            do {
        //                while (p != NULL) {
        //                    // store a node in the stack and visit it's left child
        //                    S.push(p);
        //                    p = p->GetLeft();
        //                }
        //
        //                // If there are nodes in the stack to which we can move up
        //                // then pop it
        //                if (!S.empty()) {
        //                    p = S.top();
        //                    S.pop();
        //
        //                    // print the nodes value
        //                    //  cout << " -" << p->GetId() << "- ";
        //                    list.push_back(p);
        //
        //                    // vistit the right child now
        //                    p = p->GetRight();
        //                }
        //
        //                // while the stack is not empty or there is a valid node
        //            } while (!S.empty() || p != NULL);
        //            return list;
        //        }

#ifdef USE_CLFMALLOC

        void* operator new (size_t size) throw (std::bad_alloc) {
            assert(size == sizeof (Expression));
            void* ptr = malloc(size);
            return ptr;
        }

        void operator delete (void* ptr)throw () {
            free(ptr);
            ptr = NULL;
        }
#endif

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

        const std::string GetName() const {
            if (name_m == "") {
                std::stringstream ss;
                ss << "x" << GetId();
                return ss.str();
            }
            return name_m;
        }

        void SetName(const std::string &name) {
            name_m = name;
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

        bool HasId(const unsigned long &id) {

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
                    return true;
                }

                // Check for a right sub tree
                n = n->GetRight();

            } while (!stack.empty() || n != NULL);

            return false;
        }

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


        virtual const int operator ++(int) = 0;

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

        virtual const int operator++(int) {

            if (!this->stack_m.empty()) {
                Expression<T> * n = this->stack_m.top();
                this->stack_m.pop();
                Expression<T> * next = n->GetRight();

                if (next)
                    LeftSide(next);
            }

            return !this->stack_m.empty();
        }

        virtual int operator !() {
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

        virtual int operator !() {
            return !this->stack_m.empty();
        }

        virtual const int operator++(int) {
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

        virtual int operator !() {
            return !this->stack_m.empty();
        }

        virtual const int operator++(int) {
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
            T lhs = 0;
            T rhs = 0;
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
                    if (currNode->HasId(id)) {
                        stack.push(std::pair<T, T > (std::sin(lhs.first), lhs.second * std::cos(lhs.first)));
                    } else {
                        stack.push(std::pair<T, T > (std::sin(lhs.first), T(0)));
                    }
                    //ret = stack.top().second;
                    break;
                case COS:
                    lhs = stack.top();
                    stack.pop();
                    if (currNode->HasId(id)) {
                        stack.push(std::pair<T, T > (std::cos(lhs.first), lhs.second * (-1.0) * std::sin(lhs.first)));
                    } else {
                        stack.push(std::pair<T, T > (std::cos(lhs.first), T(0)));
                    }
                    //ret = stack.top().second;
                    break;
                case TAN:
                    lhs = stack.top();
                    stack.pop();
                    if (currNode->HasId(id)) {
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
                    if (currNode->HasId(id)) {
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
                    if (currNode->HasId(id)) {
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
                    if (currNode->HasId(id)) {
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
                    if (currNode->HasId(id)) {
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
                    if (currNode->HasId(id)) {
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
                    if (currNode->HasId(id)) {
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
                    if (currNode->HasId(id)) {
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
                    if (currNode->HasId(id)) {
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
                    if (currNode->HasId(id)) {
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
                    if (currNode->HasId(id)) {
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
                    if (currNode->HasId(id)) {
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
                    if (currNode->HasId(id)) {
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
                    if (currNode->HasId(id)) {
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
                    if (currNode->HasId(id)) {
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
                    if (currNode->HasId(id)) {

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

    /**
     * Builds the derivative expression with respect to id. 
     * 
     * @param id
     * @return 
     */
    template<class T>
    static ExpressionPtr Differentiate(ExpressionPtr exp, unsigned long id = 0) {


        std::map<ExpressionPtr, ExpressionPtr > derivatives;
        typename std::map<ExpressionPtr, ExpressionPtr >::iterator df_it;

        PostOrderIterator<T> it(exp);


        while (it) {
            Expression<T>* f = it;

            //check if it was created.. if not,create it.
            df_it = derivatives.find(f);

            if (df_it == derivatives.end()) {

                derivatives[(ExpressionPtr) f] = NEW_EXPRESSION(T) ();
            }


            if (f->GetLeft() != NULL) {

                df_it = derivatives.find(f->GetLeft());
                if (df_it == derivatives.end()) {

                    derivatives[(ExpressionPtr) f->GetLeft()] = NEW_EXPRESSION(T) ();

                }
            }

            if (f->GetRight() != NULL) {

                df_it = derivatives.find(f->GetRight());

                if (df_it == derivatives.end()) {

                    derivatives[(ExpressionPtr) f->GetRight()] = NEW_EXPRESSION(T) ();

                }
            }


            ExpressionPtr df = derivatives[f];

            switch (f->GetOp()) {

                case CONSTANT:
                    df->SetOp(CONSTANT);
                    df->SetValue(T(0.0));
                    break;
                case VARIABLE:
                    if (f->GetId() == id) {
                        df->SetOp(CONSTANT);
                        df->SetValue(T(1.0));
                    } else {
                        df->SetOp(CONSTANT);
                        df->SetValue(T(0.0));
                    }
                    break;
                case MINUS:
                    df->SetOp(MINUS);
                    df->SetLeft(derivatives[f->GetLeft()]);
                    df->SetRight(derivatives[f->GetRight()]);
                    break;
                case PLUS:
                    df->SetOp(PLUS);
                    df->SetLeft(derivatives[f->GetLeft()]);
                    df->SetRight(derivatives[f->GetRight()]);
                    break;
                case DIVIDE:


                    df->SetOp(DIVIDE);

                    df->SetLeft(NEW_EXPRESSION(T) ()); //g'(x)h(x) - g(x)h'(x)
                    df->GetLeft()->SetOp(MINUS);



                    df->GetLeft()->SetLeft(NEW_EXPRESSION(T) ()); //g'(x)h(x)
                    df->GetLeft()->GetLeft()->SetOp(MULTIPLY);
                    if (f->GetLeft()) {
                        df->GetLeft()->GetLeft()->SetLeft(derivatives[f->GetLeft()]);
                    }
                    df->GetLeft()->GetLeft()->SetRight(f->GetRight());


                    df->GetLeft()->SetRight(NEW_EXPRESSION(T) ()); //g(x)h'(x)
                    df->GetLeft()->GetRight()->SetOp(MULTIPLY);
                    df->GetLeft()->GetRight()->SetLeft(f->GetLeft());

                    if (f->GetRight()) {
                        df->GetLeft()->GetRight()->SetRight(derivatives[f->GetRight()]);
                    }



                    df->SetRight(NEW_EXPRESSION(T) ());
                    df->GetRight()->SetOp(POW);
                    df->GetRight()->SetLeft(f->GetRight());
                    //NEW_EXPRESSION(T)(2.0, 0, "", CONSTANT, NULL, NULL)
                    df->GetRight()->SetRight(NEW_EXPRESSION(T)(2.0, 0, "", CONSTANT, NULL, NULL));
                    // df->GetRight()->SetRight(f->GetRight()->Clone());

                    break;
                case MULTIPLY:
                    //                    if (f->GetLeft()->GetOp() == CONSTANT
                    //                            && f->GetRight()->GetOp() != CONSTANT) {
                    //                        df->SetOp(MULTIPLY);
                    //                        if (f->GetLeft() != NULL) {
                    //                            df->SetLeft(f->GetLeft());
                    //                        }
                    //                        if (f->GetRight() != NULL) {
                    //                            df->SetRight(derivatives[f->GetRight()]);
                    //                        }
                    //
                    //
                    //                    } else if (f->GetRight()->GetOp() == CONSTANT
                    //                            && f->GetLeft()->GetOp() != CONSTANT) {
                    //                        df->SetOp(MULTIPLY);
                    //                        if (f->GetLeft()) {
                    //                            df->SetLeft(derivatives[f->GetLeft()]);
                    //                        }
                    //                        if (f->GetRight()) {
                    //                            df->SetRight(f->GetRight());
                    //                        }
                    //                    } else {



                    df->SetOp(PLUS);

                    df->SetLeft(NEW_EXPRESSION(T) ());
                    df->GetLeft()->SetOp(MULTIPLY);

                    df->GetLeft()->SetRight(f->GetRight());

                    if (f->GetRight() != NULL) {
                        df->GetLeft()->SetLeft(derivatives[f->GetLeft()]);
                    }
                    df->SetRight(NEW_EXPRESSION(T) ());
                    df->GetRight()->SetOp(MULTIPLY);

                    df->GetRight()->SetLeft(f->GetLeft());
                    if (f->GetLeft() != NULL) {
                        df->GetRight()->SetRight(derivatives[f->GetRight()]);
                    }
                    //  }
                    break;
                case SIN:
                    if (f->GetLeft()->HasId(id)) {
                        //f'(x) = cos(x)

                        df->SetOp(MULTIPLY);
                        df->SetLeft(derivatives[f->GetLeft()]); // = left->Differentiate(id);

                        df->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->SetOp(COS);
                        df->GetRight()->SetLeft(f->GetLeft()); // = left->DeepCopy();

                    } else {
                        df->SetOp(CONSTANT);
                        df->SetValue(T(0));
                    }

                    break;
                case COS:

                    if (f->GetLeft()->HasId(id)) {
                        //f'(x) = -sin(x)
                        df->SetOp(MULTIPLY);

                        df->SetLeft(derivatives[f->GetLeft()]);

                        df->SetRight(NEW_EXPRESSION(T) ());

                        df->GetRight()->SetOp(MULTIPLY);

                        df->GetRight()->SetLeft(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetLeft()->SetOp(CONSTANT);
                        df->GetRight()->GetLeft()->SetValue(T(-1.0));


                        df->GetRight()->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetRight()->SetOp(SIN);
                        df->GetRight()->GetRight()->SetLeft(f->GetLeft());

                    } else {
                        df->SetOp(CONSTANT);
                        df->SetValue(T(0.0));
                    }
                    break;
                case TAN:

                    if (f->GetLeft()->HasId(id)) {
                        //f'(x) = 1/cos(x)

                        df->SetOp(MULTIPLY);
                        df->SetLeft(derivatives[f->GetLeft()]); //f->GetLeft()->Differentiate(id);



                        df->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->SetOp(MULTIPLY);


                        df->GetRight()->SetLeft(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetLeft()->SetOp(DIVIDE);



                        df->GetRight()->GetLeft()->SetLeft(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetLeft()->GetLeft()->SetOp(CONSTANT);
                        df->GetRight()->GetLeft()->GetLeft()->SetValue(T(1.0));



                        df->GetRight()->GetLeft()->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetLeft()->GetRight()->SetOp(COS);
                        df->GetRight()->GetLeft()->GetRight()->SetLeft(f->GetLeft());



                        df->GetRight()->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetRight()->SetOp(DIVIDE);



                        df->GetRight()->GetRight()->SetLeft(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetRight()->GetLeft()->SetOp(CONSTANT);
                        df->GetRight()->GetRight()->GetLeft()->SetValue(T(1.0));



                        df->GetRight()->GetRight()->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetRight()->GetRight()->SetOp(COS);
                        df->GetRight()->GetRight()->GetRight()->SetLeft(f->GetLeft());

                    } else {
                        df->SetOp(CONSTANT);
                        df->SetValue(T(0.0));
                    }
                    break;
                case ASIN:

                    if (f->GetLeft()->HasId(id)) {
                        //f(x) = asin(x)
                        //f'(x) = 1/(2 sqrt(1-x^2)= 1/(pow((1-pow(x,2)),0.5)

                        df->SetOp(MULTIPLY);
                        df->SetLeft(derivatives[f->GetLeft()]); //f->GetLeft()->Differentiate(id);



                        df->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->SetOp(DIVIDE);


                        df->GetRight()->SetLeft(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetLeft()->SetOp(CONSTANT);
                        df->GetRight()->GetLeft()->SetValue(T(1.0));


                        df->GetRight()->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetRight()->SetOp(POW);


                        df->GetRight()->GetRight()->SetLeft(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetRight()->GetLeft()->SetOp(MINUS);


                        df->GetRight()->GetRight()->GetLeft()->SetLeft(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetRight()->GetLeft()->GetLeft()->SetOp(CONSTANT);
                        df->GetRight()->GetRight()->GetLeft()->GetLeft()->SetValue(T(1.0));


                        df->GetRight()->GetRight()->GetLeft()->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetRight()->GetLeft()->GetRight()->SetOp(POW);
                        df->GetRight()->GetRight()->GetLeft()->GetRight()->SetLeft(f->GetLeft());


                        df->GetRight()->GetRight()->GetLeft()->GetRight()->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetRight()->GetLeft()->GetRight()->GetRight()->SetOp(CONSTANT);
                        df->GetRight()->GetRight()->GetLeft()->GetRight()->GetRight()->SetValue(T(2.0));


                        df->GetRight()->GetRight()->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetRight()->GetRight()->SetOp(CONSTANT);
                        df->GetRight()->GetRight()->GetRight()->SetValue(T(0.5));

                    } else {
                        df->SetOp(CONSTANT);
                        df->SetValue(T(0.0));

                    }
                    break;
                case ACOS:
                    if (f->GetLeft()->HasId(id)) {
                        //f(x) = acos(x)
                        //f'(x) = -1/(sqrt(1-x^2) = -1/(pow((1-pow(x,2)),0.5)
                        //-1/sqrt(1-x^2)
                        df->SetOp(MULTIPLY);

                        df->SetLeft(NEW_EXPRESSION(T) ());
                        df->GetLeft()->SetOp(MULTIPLY);

                        df->GetLeft()->SetLeft(NEW_EXPRESSION(T) ());

                        df->GetLeft()->GetLeft()->SetOp(CONSTANT);
                        df->GetLeft()->GetLeft()->SetValue(T(-1.0));


                        df->GetLeft()->SetRight(derivatives[f->GetLeft()]);


                        df->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->SetOp(DIVIDE);


                        df->GetRight()->SetLeft(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetLeft()->SetOp(CONSTANT);
                        df->GetRight()->GetLeft()->SetValue(T(1.0));


                        df->GetRight()->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetRight()->SetOp(POW);


                        df->GetRight()->GetRight()->SetLeft(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetRight()->GetLeft()->SetOp(MINUS);


                        df->GetRight()->GetRight()->GetLeft()->SetLeft(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetRight()->GetLeft()->GetLeft()->SetOp(CONSTANT);
                        df->GetRight()->GetRight()->GetLeft()->GetLeft()->SetValue(T(1.0));


                        df->GetRight()->GetRight()->GetLeft()->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetRight()->GetLeft()->GetRight()->SetOp(POW);
                        df->GetRight()->GetRight()->GetLeft()->GetRight()->SetLeft(f->GetLeft());


                        df->GetRight()->GetRight()->GetLeft()->GetRight()->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetRight()->GetLeft()->GetRight()->GetRight()->SetOp(CONSTANT);
                        df->GetRight()->GetRight()->GetLeft()->GetRight()->GetRight()->SetValue(T(2.0));


                        df->GetRight()->GetRight()->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetRight()->GetRight()->SetOp(CONSTANT);
                        df->GetRight()->GetRight()->GetRight()->SetValue(T(0.5));

                    } else {
                        df->SetOp(CONSTANT);
                        df->SetValue(T(0.0));

                    }
                    break;
                case ATAN:
                    if (f->GetLeft()->HasId(id)) {
                        //f(x) = atan(x)
                        //f'(x) 1/(x^2+1)

                        df->SetOp(DIVIDE);

                        df->SetLeft(NEW_EXPRESSION(T) ());
                        df->GetLeft()->SetOp(MULTIPLY);

                        df->GetLeft()->SetRight(NEW_EXPRESSION(T) ());

                        df->GetLeft()->GetRight()->SetOp(CONSTANT);
                        df->GetLeft()->GetRight()->SetValue(T(1.0));


                        df->GetLeft()->SetLeft(derivatives[f->GetLeft()]);


                        df->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->SetOp(PLUS);


                        df->GetRight()->SetLeft(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetLeft()->SetOp(MULTIPLY);
                        df->GetRight()->GetLeft()->SetLeft(f->GetLeft());
                        df->GetRight()->GetLeft()->SetRight(f->GetLeft());



                        df->GetRight()->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetRight()->SetOp(CONSTANT);
                        df->GetRight()->GetRight()->SetValue(T(1.0));


                    } else {
                        df->SetOp(CONSTANT);
                        df->SetValue(T(0.0));


                    }
                    break;

                case ATAN2:
                    if (f->GetLeft()->HasId(id)) {
                        //f(x) = atan2(x,y)
                        //f'(x) y/(x^2+y^2)

                        df->SetOp(DIVIDE);

                        df->SetLeft(NEW_EXPRESSION(T) ());
                        df->GetLeft()->SetOp(MULTIPLY);
                        df->GetLeft()->SetLeft(f->GetRight()); //y
                        df->GetLeft()->SetRight(derivatives[f->GetLeft()]);



                        df->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->SetOp(PLUS);


                        df->GetRight()->SetLeft(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetLeft()->SetOp(MULTIPLY);
                        df->GetRight()->GetLeft()->SetLeft(f->GetLeft());
                        df->GetRight()->GetLeft()->SetRight(f->GetLeft());



                        df->GetRight()->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetRight()->SetOp(MULTIPLY);
                        df->GetRight()->GetRight()->SetLeft(f->GetRight());
                        df->GetRight()->GetRight()->SetRight(Clone(f->GetRight()));

                    } else {
                        df->SetOp(CONSTANT);
                        df->SetValue(T(0.0));

                    }
                    break;
                case SQRT:
                    if (f->GetLeft()->HasId(id)) {
                        //f(x) = sqrt(x)
                        //f'(x) = .5/sqrt(x)

                        df->SetOp(DIVIDE);

                        df->SetLeft(NEW_EXPRESSION(T) ());
                        df->GetLeft()->SetOp(MULTIPLY);


                        df->GetLeft()->SetRight(NEW_EXPRESSION(T) ());
                        df->GetLeft()->GetRight()->SetValue(T(0.5));

                        df->GetLeft()->SetLeft(derivatives[f->GetLeft()]);



                        df->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->SetOp(SQRT);
                        df->GetRight()->SetLeft(f->GetLeft());
                    } else {
                        df->SetOp(CONSTANT);
                        df->SetValue(T(0.0));
                    }
                    break;
                case POW:

                    if (f->GetLeft()->HasId(id)) {
                        //f(x) =  x^y
                        //f'(x) = yx^y-1

                        df->SetOp(MULTIPLY);
                        df->SetLeft(NEW_EXPRESSION(T) ());
                        df->GetLeft()->SetOp(MULTIPLY);
                        df->GetLeft()->SetLeft(derivatives[f->GetLeft()]);
                        df->GetLeft()->SetRight(f->GetRight());


                        df->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->SetOp(POW);


                        df->GetRight()->SetLeft(f->GetLeft());



                        df->GetRight()->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetRight()->SetOp(MINUS);
                        df->GetRight()->GetRight()->SetLeft(f->GetRight());


                        df->GetRight()->GetRight()->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetRight()->GetRight()->SetOp(CONSTANT);
                        df->GetRight()->GetRight()->GetRight()->SetValue(T(1.0));

                    } else {
                        df->SetOp(CONSTANT);
                        df->SetValue(T(0.0));
                    }
                    break;
                case LOG:
                    if (f->GetLeft()->HasId(id)) {
                        //f(x) = log(x)
                        //f'(x) = 1/x

                        df->SetOp(DIVIDE);

                        df->SetLeft(NEW_EXPRESSION(T) ());
                        df->GetLeft()->SetOp(MULTIPLY);

                        df->GetLeft()->SetLeft(NEW_EXPRESSION(T) ());
                        df->GetLeft()->GetLeft()->SetOp(CONSTANT);
                        df->GetLeft()->GetLeft()->SetValue(T(1.0));
                        df->GetLeft()->SetRight(derivatives[f->GetLeft()]);

                        df->SetRight(f->GetLeft());
                    } else {
                        df->SetOp(CONSTANT);
                        df->SetValue(T(0.0));
                    }
                    break;
                case LOG10:
                    //f(x) = log10(x)
                    //f'(x) = 1/(xlog(10))

                    if (f->GetLeft()->HasId(id)) {



                        df->SetOp(DIVIDE);

                        df->SetLeft(NEW_EXPRESSION(T) ());
                        df->GetLeft()->SetOp(MULTIPLY);


                        df->GetLeft()->SetLeft(NEW_EXPRESSION(T) ());
                        df->GetLeft()->GetLeft()->SetOp(CONSTANT);
                        df->GetLeft()->GetLeft()->SetValue(T(1.0));

                        df->GetLeft()->SetRight(derivatives[f->GetLeft()]);

                        df->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->SetOp(MULTIPLY);

                        df->GetRight()->SetLeft(f->GetLeft());


                        df->GetRight()->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetRight()->SetOp(CONSTANT);
                        df->GetRight()->GetRight()->SetValue(std::log(T(10.0)));
                    } else {
                        df->SetOp(LOG);
                        df->SetLeft(f);
                    }
                    break;
                case EXP:
                    //f(x) = e^x
                    //f'(x) =e^x

                    if (f->GetLeft()->HasId(id)) {

                        df->SetOp(MULTIPLY);
                        df->SetLeft(derivatives[f->GetLeft()]);



                        df->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->SetOp(EXP);
                        df->GetRight()->SetLeft(f->GetLeft());
                    } else {
                        df->SetOp(CONSTANT);
                        df->SetValue(T(0.0));
                    }
                    break;
                case SINH:
                    if (f->GetLeft()->HasId(id)) {
                        //f(x) = sinh(x)
                        //f'(x) = cosh(x)

                        df->SetOp(MULTIPLY);
                        df->SetLeft(derivatives[f->GetLeft()]);


                        df->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->SetOp(COSH);
                        df->GetRight()->SetLeft(f->GetLeft());
                    } else {
                        df->SetOp(CONSTANT);
                        df->SetValue(T(0.0));
                    }
                    break;
                case COSH:
                    if (f->GetLeft()->HasId(id)) {

                        df->SetOp(MULTIPLY);
                        df->SetLeft(derivatives[f->GetLeft()]);


                        df->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->SetOp(SINH);
                        df->GetRight()->SetLeft(f->GetLeft());
                    } else {
                        df->SetOp(CONSTANT);
                        df->SetValue(T(0.0));
                    }
                    break;
                case TANH:
                    //f(x) = tanh(x)
                    //f'(x) =1- tanh(x)*tanh(x)

                    if (f->GetLeft()->HasId(id)) {

                        df->SetOp(MULTIPLY);

                        df->SetLeft(derivatives[f->GetLeft()]);


                        df->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->SetOp(MULTIPLY);

                        df->GetRight()->SetLeft(NEW_EXPRESSION(T) ());


                        df->GetRight()->GetLeft()->SetOp(DIVIDE);

                        df->GetRight()->GetLeft()->SetLeft(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetLeft()->GetLeft()->SetOp(CONSTANT);
                        df->GetRight()->GetLeft()->GetLeft()->SetValue(T(1.0));


                        df->GetRight()->GetLeft()->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->GetLeft()->GetRight()->SetOp(COSH);
                        df->GetRight()->GetLeft()->GetRight()->SetLeft(f->GetLeft());


                        df->GetRight()->SetRight(df->GetRight()->GetLeft());
                    } else {
                        df->SetOp(CONSTANT);
                        df->SetValue(T(0.0));

                    }
                    break;

                case FABS:

                    if (f->GetLeft()->HasId(id)) {

                        df->SetOp(DIVIDE);

                        df->SetLeft(NEW_EXPRESSION(T) ());
                        df->GetLeft()->SetOp(MULTIPLY);

                        df->GetLeft()->SetLeft(derivatives[f->GetLeft()]);
                        df->GetLeft()->SetRight(f->GetLeft());



                        df->SetRight(NEW_EXPRESSION(T) ());
                        df->GetRight()->SetOp(FABS);
                        df->GetRight()->SetLeft(f->GetLeft());
                    } else {
                        df->SetOp(CONSTANT);
                        df->SetValue(T(0.0));

                    }
                    break;
                case FLOOR:
                    df->SetOp(CONSTANT);
                    df->SetValue(T(0.0));
                    break;
                case NONE:

                default:
                    break;

            }


            it++;
        }

        return derivatives[exp];
    }
    //#define AD_TRACE
#ifdef AD_TRACE
#define ADDEBUG std::cout<<__func__<<":"<<__LINE__<<std::endl;
#else
#define ADDEBUG  
#endif

    template<class T>
    class ADNumber {
        T value;
        std::string name;
        unsigned long id;


    public:
        ExpressionPtr expression;

        /*!
         * Default constructor.
         */
        ADNumber() :
        expression(new Expression<T>()),
        value(T(0.0)),
        //name(std::string("na")),
        id(IDGenerator::instance()->next()) {
            ADDEBUG
            Initialize();
        }

        /*!
         * Constructor
         * @param value
         * @param derivative
         */
        ADNumber(const T &value) :
        expression(new Expression<T>()),
        value(value),
        // name(std::string("na")),
        id(IDGenerator::instance()->next()) {
            ADDEBUG
            Initialize();
        }

        /*!
         * Constructor
         * @param value
         * @param derivative
         */
        ADNumber(const T &value, Expression<T>* exp) :
        expression(exp),
        value(value),
        // name(std::string("na")),
        id(IDGenerator::instance()->next()) {
            expression->take();
            expression->SetId(id);
        }

        /*!
         * Constructor
         * 
         * @param name
         * @param value
         * @param derivative
         */
        ADNumber(const std::string &name, const T &value = T(0.0)) :
        value(value),
        name(name),
        expression(new Expression<T>()),
        id(IDGenerator::instance()->next()) {
            ADDEBUG
            Initialize();
        }

        /*!
         * Copy Constructor.
         * 
         * @param orig
         */
        ADNumber(const ADNumber& orig) :
        value(orig.value),
        name(orig.name),
        id(orig.id) {
            ADDEBUG
            expression = Clone(orig.expression);
            expression->take();

        }

        ADNumber(ExpressionPtr exp) :
        value(Evaluate(exp)),
        name(exp->GetName()),
        id(IDGenerator::instance()->next()) {
            ADDEBUG
            expression = exp;
            SetValue(Evaluate(exp));
            expression->take();

        }

        virtual ~ADNumber() {
            expression->release();
        }



        //    /*!
        //     * returns this value.
        //     */

        operator T() const {
            return value;
        }

        /*!
         * Returns this value.
         */
        operator ADNumber<T>() const {
            ADDEBUG
            return this;
        }

        //        const ADNumber<T> operator (const ADNumber<T> &val) {
        //
        //        }

        /**
         * In member assignment operator to set this 
         * equal to another ADNumber val.
         * 
         * @param val
         * @return ADNumber
         */
         ADNumber<T>& operator =(const ADNumber<T> &val)  {
            value = val.GetValue();

            ExpressionPtr exp = expression;


            expression = val.expression;
            expression->take();



            if (exp != NULL) {
                exp->release();
            }

            return *this;
        }

        /**
         * In member assignment operator to set this value 
         * equal to val with derivative set to 1.
         * 
         * @param val
         * @return ADNumber
         */
        ADNumber<T> & operator =(const T & val) {
            value = val;

            if (expression->GetLeft() != NULL) {
                expression->GetLeft()->release();
            }
            if (expression->GetRight() != NULL) {
                expression->GetRight()->release();
            }
            expression->SetLeft(NULL);
            expression->SetRight(NULL);
            expression->SetValue(val);


            return *this;
        }

        /**
         * In member addition operator.
         * Returns ADNumber<T> with this value + rhs value &
         * this derivative + rhs derivitive.
         * 
         * @param rhs
         * @return ADNumber
         */
        const ADNumber<T> operator +(const ADNumber<T>& rhs) const {

            T val = value + rhs.value;
            if (expression == rhs.expression) {

                return ADNumber<T > (val,
                        NEW_EXPRESSION(T)(val, 0, MULTIPLY,
                        NEW_EXPRESSION(T)(2.0, 0, CONSTANT, NULL, NULL),
                        expression));
            } else {

                return ADNumber<T > (val,
                        NEW_EXPRESSION(T)(val,
                        0, "", PLUS,expression, rhs.expression));
            }
        }

        /**
         * In member addition operator.
         * Returns ADNumber<T> with this value + rhs value &
         * this derivative + 0.
         * @param rhs
         * 
         * @return ADNumber
         */
        const ADNumber<T> operator +(const T & rhs) const {
            T val = value + rhs;
            return ADNumber<T > (val,
                    NEW_EXPRESSION(T)(val,
                    0, PLUS, expression,
                    NEW_EXPRESSION(T)(rhs, 0, CONSTANT, NULL, NULL)));

        }

        /**
         * In member subtraction operator.
         * Returns ADNumber<T> with this value - rhs value &
         * this derivative - rhs derivitive.
         * 
         * @param rhs
         * @return ADNmuber
         */
        const ADNumber<T> operator -(const ADNumber<T>& rhs) const {

            T val = value - rhs;
            return ADNumber<T > (val,
                    NEW_EXPRESSION(T)(val,
                    0, MINUS, expression,rhs.expression));
            //}
        }

        /**
         * In member subtraction operator.
         * Returns ADNumber<T>(this value - rhs value, this derivative - 0).
         * 
         * @param rhs
         * @return ADNumber
         */
        const ADNumber<T> operator -(const T & rhs) const {

            T val = value - rhs;
            return ADNumber<T > (val,
                    NEW_EXPRESSION(T)(val,
                    0, MINUS, expression,
                    NEW_EXPRESSION(T)(rhs, 0, CONSTANT, NULL, NULL)));
        }

        /*!
         * In member multiplication operator.
         * Returns ADNumber<T>(this value * rhs value,
         * this value_ * rhs derivative  + rhs value * this derivative).
         */
        const ADNumber<T> operator *(const ADNumber<T>& rhs) const {

            T val = value * rhs.value;
            if (expression == rhs.expression) {

                return ADNumber<T > (val,
                        NEW_EXPRESSION(T)(val, 0, POW,
                        expression,
                        NEW_EXPRESSION(T)(2.0, 0, CONSTANT, NULL, NULL)));
            } else {

                return ADNumber<T > (val,
                        NEW_EXPRESSION(T)(val,
                        0, MULTIPLY,expression, rhs.expression));
            }

        }

        /*!
         * In member multiplication operator.
         * Returns ADNumber<T>(this value * rhs value,
         * this value_ * 0  + rhs value * this derivative).
         */
        const ADNumber<T> operator *(const T & rhs) const {

            T val = value * rhs;
            return ADNumber<T > (val,
                    NEW_EXPRESSION(T)(val,
                    0, MULTIPLY, expression,
                    NEW_EXPRESSION(T)(rhs, 0, CONSTANT, NULL, NULL)));
        }

        /**
         * 
         * @param rhs
         * @return 
         */
        const ADNumber<T> operator /(const ADNumber<T>& rhs) const {

            T val = value / rhs.value;
            return ADNumber<T > (val,
                    NEW_EXPRESSION(T)(val,
                    0, DIVIDE,expression, rhs.expression ));

        }

        /*!
         * In member division operator.
         * Returns ADNumber<T>(this value * rhs value,
         * (this value_ * rhs derivative  - rhs value * 0)/(rhs value * rhs value)).
         */
        const ADNumber<T> operator /(const T & rhs) const {

            T val = value / rhs;
            return ADNumber<T > (val,
                    NEW_EXPRESSION(T)(val,
                    0, DIVIDE, expression,
                    NEW_EXPRESSION(T)(rhs, 0, CONSTANT, NULL, NULL)));
        }

        /*!
         * In member addition assignment operator.
         * @param rhs
         * @return 
         */


        const ADNumber<T>& operator +=(const ADNumber<T>& rhs) {

            T val = value + rhs.value;

            ExpressionPtr exp = NEW_EXPRESSION(T) (val, id, name, PLUS, expression, rhs.expression);
            if (expression != NULL) {
                expression->release();
            }

            expression = exp;


            expression->take();
            value = val;
            return *this;
        }

        /*!
         * In member addition subtraction operator.
         * 
         * @param rhs
         * @return 
         */
        const ADNumber<T>& operator -=(const ADNumber<T>& rhs) {
            ADDEBUG
            ExpressionPtr exp = NEW_EXPRESSION(T) (value - rhs.value, id, name, MINUS, expression, rhs.expression);
            if (expression != NULL) {
                expression->release();
            }
            expression = exp;
            expression->take();
            value = expression->GetValue();
            return *this;
        }

        /*!
         * In member multiplication assignment operator.
         * 
         * @param rhs
         * @return 
         */
        const ADNumber<T>& operator *=(const ADNumber<T>& rhs) {
            ADDEBUG
            ExpressionPtr temp = this->expression;
            if (this->expression == rhs.expression) {
                temp = ad::Clone(this->expression);
            }
            ExpressionPtr exp = NEW_EXPRESSION(T) (value * rhs.value, id, name, MULTIPLY, temp, rhs.expression);
            if (expression != NULL) {
                expression->release();
            }
            expression = exp;
            expression->take();
            value = expression->GetValue();
            return *this;
        }

        /*!
         * In member division assignment operator.
         * 
         * @param rhs
         * @return 
         */
        const ADNumber<T>& operator /=(const ADNumber<T>&rhs) {
            ADDEBUG
            ExpressionPtr exp = NEW_EXPRESSION(T) (value / rhs.value, id, name, DIVIDE, expression, rhs.expression);
            if (expression != NULL) {
                expression->release();
            }
            expression = exp;
            expression->take();
            value = expression->GetValue();
            return *this;
        }

        /*!
         * In member addition assignment operator.
         * 
         * @param rhs
         * @return 
         */
        const ADNumber<T>& operator +=(const T & rhs) {
            ADDEBUG
            ExpressionPtr c = NEW_EXPRESSION(T) ();
            c->SetOp(CONSTANT);
            c->SetValue(T(rhs));

            ExpressionPtr exp = NEW_EXPRESSION(T) (value + rhs, id, name, PLUS, expression, c);
            if (expression != NULL) {
                expression->release();
            }
            expression = exp;
            expression->take();
            value = expression->GetValue();
            return *this;
        }

        /*!
         * In member subtraction assignment operator.
         * 
         * @param rhs
         * @return 
         */
        const ADNumber<T>& operator -=(const T & rhs) {
            ADDEBUG
            ExpressionPtr c = NEW_EXPRESSION(T) ();
            c->SetOp(CONSTANT);
            c->SetValue(T(rhs));
            ExpressionPtr exp = NEW_EXPRESSION(T) (value - rhs, id, name, MINUS, expression, c);
            if (expression != NULL) {
                expression->release();
            }
            expression = exp;
            expression->take();
            value = expression->GetValue();
            return *this;
        }

        /*!
         * In member multiplication assignment operator.
         * 
         * @param rhs
         * @return 
         */
        ADNumber<T>& operator *=(const T & rhs) {
            ADDEBUG
            ExpressionPtr c = NEW_EXPRESSION(T) ();
            c->SetOp(CONSTANT);
            c->SetValue(T(rhs));
            ExpressionPtr exp = NEW_EXPRESSION(T) (value * rhs, id, name, MULTIPLY, expression, c);
            if (expression != NULL) {
                expression->release();
            }
            expression = exp;
            expression->take();
            value = expression->GetValue();
            return *this;
        }

        /*!
         * In member division assignment operator.
         * 
         * @param rhs
         * @return 
         */
        ADNumber<T>* operator /=(const T & rhs) {
            ADDEBUG
            ExpressionPtr c = NEW_EXPRESSION(T) ();
            c->SetOp(CONSTANT);
            c->SetValue(T(rhs));
            ExpressionPtr exp = NEW_EXPRESSION(T) (value / rhs, id, name, DIVIDE, expression, c);
            if (expression != NULL) {
                expression->release();
            }
            expression = exp;
            expression->take();
            value = expression->GetValue();
            return *this;
        }

        /*!
         * In member suffix increment operator.
         * 
         * @return 
         */
        const ADNumber<T>& operator ++() {
            ADDEBUG
            ExpressionPtr c = NEW_EXPRESSION(T) ();
            c->SetOp(CONSTANT);
            c->SetValue(T(1.0));

            ExpressionPtr exp = NEW_EXPRESSION(T) (value + 1, id, name, PLUS, expression, c);
            if (expression != NULL) {
                expression->release();
            }
            expression = exp;
            expression->take();
            value = expression->GetValue();
            return *this;

        }

        /*!
         * In member suffix decrement operator.
         * 
         * @return 
         */
        const ADNumber<T>& operator --() {
            ADDEBUG
            ExpressionPtr c = NEW_EXPRESSION(T) ();
            c->SetOp(CONSTANT);
            c->SetValue(T(1.0));
            ExpressionPtr exp = NEW_EXPRESSION(T) (value - 1, id, name, MINUS, expression, c);
            if (expression != NULL) {
                expression->release();
            }
            expression = exp;
            expression->take();
            value = expression->GetValue();
            return *this;
        }

        /*!
         * In member prefix increment operator.
         * 
         * @param 
         */
        const ADNumber<T>& operator ++(int) {
            ADDEBUG
            ExpressionPtr c = NEW_EXPRESSION(T) ();
            c->SetOp(CONSTANT);
            c->SetValue(T(1.0));

            ExpressionPtr exp = NEW_EXPRESSION(T) (value + 1, id, name, PLUS, expression, c);
            if (expression != NULL) {
                expression->release();
            }
            expression = exp;
            expression->take();
            value = expression->GetValue();
            return *this;
        }

        /*!
         * In member prefix decrement operator.
         * 
         * @param 
         */
        const ADNumber<T>& operator --(int) {
            ADDEBUG
            ExpressionPtr c = NEW_EXPRESSION(T) ();
            c->SetOp(CONSTANT);
            c->SetValue(T(1.0));

            ExpressionPtr exp = NEW_EXPRESSION(T) (value - 1, id, name, PLUS, expression, c);
            if (expression != NULL) {
                expression->release();
            }
            expression = exp;
            expression->take();
            value = expression->GetValue();
            return *this;
        }

        /*!
         * Returns the computed value.
         */
        const T GetValue() const {
            ADDEBUG
            return value;
        }

        /*!
         * 
         * @return 
         */
        const std::string GetName() const {
            if (name == "") {
                std::stringstream ss;
                ss << "x" << GetID();
                return ss.str();
            }
            return name;
        }

        void SetName(const std::string &name) {
            ADDEBUG
            expression->SetName(name);
            this->name = name;
        }

        void SetValue(const T &val) {
            value = val;
            expression->SetValue(value);
        }

        /*
         * Return the unique identifier for this ADNumber.
         */
        const uint32_t GetID() const {
            ADDEBUG
            return id;
        }

        /**
         * Derivative with respect to var0....var1 in order.
         *
         * @param var1
         * @return  derivative
         */
        const ADNumber<T> WRT(const ADNumber<T> &var1) {
            return ad::Derivative(*this, var1, 1);
        }

        /**
         * Derivative with respect to var0....var2 in order.
         *
         * @param var1
         * @param var2
         * @return  derivative
         */
        const ADNumber<T> WRT(const ADNumber<T> &var1,
                const ADNumber<T> &var2) {
            ADNumber<T> ret = ad::Derivative(*this, var1, 1);
            //std::cout << ret.expression->ToString(true) << std::flush;
            ADNumber<T> ret2 = ad::Derivative(ret, var2, 1);
            //std::cout<<ad::Evaluate(ret.expression)<<"\n\n\n\n\n\n";
            //std::cout << ret.expression->ToString()<<"\n\n\n\n\n\n" << std::flush;

            return ret2;
        }

        /**
         * Derivative with respect to var0....var3 in order.
         *
         * @param var1
         * @param var2
         * @param var3
         * @return  derivative
         */
        const ADNumber<T> WRT(const ADNumber<T> &var1,
                const ADNumber<T> &var2,
                const ADNumber<T> &var3) {
            ADNumber<T> ret = ad::Derivative(*this, var1, 1);
            ret = ad::Derivative(ret, var2, 1);
            return ad::Derivative(ret, var3, 1);
        }

        /**
         * Derivative with respect to var0....var4 in order.
         *
         * @param var1
         * @param var2
         * @param var3
         * @param var4
         * @return  derivative
         */
        const ADNumber<T> WRT(const ADNumber<T> &var1,
                const ADNumber<T> &var2,
                const ADNumber<T> &var3,
                const ADNumber<T> &var4) {
            ADNumber<T> ret = ad::Derivative(*this, var1, 1);
            ret = ad::Derivative(ret, var2, 1);
            ret = ad::Derivative(ret, var3, 1);
            return ad::Derivative(ret, var4, 1);
        }

        /**
         * Derivative with respect to var0....var5 in order.
         *
         * @param var1
         * @param var2
         * @param var3
         * @param var4
         * @param var5
         * @return  derivative
         */
        const ADNumber<T> WRT(const ADNumber<T> &var1,
                const ADNumber<T> &var2,
                const ADNumber<T> &var3,
                const ADNumber<T> &var4,
                const ADNumber<T> &var5) {
            ADNumber<T> ret = ad::Derivative(*this, var1, 1);
            ret = ad::Derivative(ret, var2, 1);
            ret = ad::Derivative(ret, var3, 1);
            ret = ad::Derivative(ret, var4, 1);
            return ad::Derivative(ret, var5, 1);
        }

        /**
         * Derivative with respect to var0....var6 in order.
         *
         * @param var1
         * @param var2
         * @param var3
         * @param var4
         * @param var5
         * @param var6
         * @return  derivative
         */
        const ADNumber<T> WRT(const ADNumber<T> &var1,
                const ADNumber<T> &var2,
                const ADNumber<T> &var3,
                const ADNumber<T> &var4,
                const ADNumber<T> &var5,
                const ADNumber<T> &var6) {
            ADNumber<T> ret = ad::Derivative(*this, var1, 1);
            ret = ad::Derivative(ret, var2, 1);
            ret = ad::Derivative(ret, var3, 1);
            ret = ad::Derivative(ret, var4, 1);
            ret = ad::Derivative(ret, var5, 1);
            return ad::Derivative(ret, var6, 1);
        }

        /**
         * Derivative with respect to var0....var7 in order.
         *
         * @param var1
         * @param var2
         * @param var3
         * @param var4
         * @param var5
         * @param var6
         * @param var7
         * @return  derivative
         */
        const ADNumber<T> WRT(const ADNumber<T> &var1,
                const ADNumber<T> &var2,
                const ADNumber<T> &var3,
                const ADNumber<T> &var4,
                const ADNumber<T> &var5,
                const ADNumber<T> &var6,
                const ADNumber<T> &var7) {
            ADNumber<T> ret = ad::Derivative(*this, var1, 1);
            ret = ad::Derivative(ret, var2, 1);
            ret = ad::Derivative(ret, var3, 1);
            ret = ad::Derivative(ret, var4, 1);
            ret = ad::Derivative(ret, var5, 1);
            ret = ad::Derivative(ret, var6, 1);
            return ad::Derivative(ret, var7, 1);
        }

        /**
         * Derivative with respect to var0....var8 in order.
         *
         * @param var1
         * @param var2
         * @param var3
         * @param var4
         * @param var5
         * @param var6
         * @param var7
         * @param var8
         * @return  derivative
         */
        const ADNumber<T> WRT(const ADNumber<T> &var1,
                const ADNumber<T> &var2,
                const ADNumber<T> &var3,
                const ADNumber<T> &var4,
                const ADNumber<T> &var5,
                const ADNumber<T> &var6,
                const ADNumber<T> &var7,
                const ADNumber<T> &var8) {
            ADNumber<T> ret = ad::Derivative(*this, var1, 1);
            ret = ad::Derivative(ret, var2, 1);
            ret = ad::Derivative(ret, var3, 1);
            ret = ad::Derivative(ret, var4, 1);
            ret = ad::Derivative(ret, var5, 1);
            ret = ad::Derivative(ret, var6, 1);
            ret = ad::Derivative(ret, var7, 1);
            return ad::Derivative(ret, var8, 1);
        }

        /**
         * Derivative with respect to var0....var9 in order.
         *
         * @param var1
         * @param var2
         * @param var3
         * @param var4
         * @param var5
         * @param var6
         * @param var7
         * @param var8
         * @param var9
         * @return  derivative
         */
        const ADNumber<T> WRT(const ADNumber<T> &var1,
                const ADNumber<T> &var2,
                const ADNumber<T> &var3,
                const ADNumber<T> &var4,
                const ADNumber<T> &var5,
                const ADNumber<T> &var6,
                const ADNumber<T> &var7,
                const ADNumber<T> &var8,
                const ADNumber<T> &var9) {
            ADNumber<T> ret = ad::Derivative(*this, var1, 1);
            ret = ad::Derivative(ret, var2, 1);
            ret = ad::Derivative(ret, var3, 1);
            ret = ad::Derivative(ret, var4, 1);
            ret = ad::Derivative(ret, var5, 1);
            ret = ad::Derivative(ret, var6, 1);
            ret = ad::Derivative(ret, var7, 1);
            ret = ad::Derivative(ret, var8, 1);
            return ad::Derivative(ret, var9, 1);
        }

        /**
         * Derivative with respect to var0....var10 in order.
         *
         * @param var1
         * @param var2
         * @param var3
         * @param var4
         * @param var5
         * @param var6
         * @param var7
         * @param var8
         * @param var9
         * @param var10
         * @return  derivative
         */
        const ADNumber<T> WRT(const ADNumber<T> &var1,
                const ADNumber<T> &var2,
                const ADNumber<T> &var3,
                const ADNumber<T> &var4,
                const ADNumber<T> &var5,
                const ADNumber<T> &var6,
                const ADNumber<T> &var7,
                const ADNumber<T> &var8,
                const ADNumber<T> &var9,
                const ADNumber<T> &var10) {
            ADNumber<T> ret = ad::Derivative(*this, var1, 1);
            ret = ad::Derivative(ret, var2, 1);
            ret = ad::Derivative(ret, var3, 1);
            ret = ad::Derivative(ret, var4, 1);
            ret = ad::Derivative(ret, var5, 1);
            ret = ad::Derivative(ret, var6, 1);
            ret = ad::Derivative(ret, var7, 1);
            ret = ad::Derivative(ret, var8, 1);
            ret = ad::Derivative(ret, var9, 1);
            return ad::Derivative(ret, var10, 1);
        }

        /**
         * Derivative with respect to var0....var11 in order.
         *
         * @param var1
         * @param var2
         * @param var3
         * @param var4
         * @param var5
         * @param var6
         * @param var7
         * @param var8
         * @param var9
         * @param var10
         * @param var11
         * @return  derivative
         */
        const ADNumber<T> WRT(const ADNumber<T> &var1,
                const ADNumber<T> &var2,
                const ADNumber<T> &var3,
                const ADNumber<T> &var4,
                const ADNumber<T> &var5,
                const ADNumber<T> &var6,
                const ADNumber<T> &var7,
                const ADNumber<T> &var8,
                const ADNumber<T> &var9,
                const ADNumber<T> &var10,
                const ADNumber<T> &var11) {
            ADNumber<T> ret = ad::Derivative(*this, var1, 1);
            ret = ad::Derivative(ret, var2, 1);
            ret = ad::Derivative(ret, var3, 1);
            ret = ad::Derivative(ret, var4, 1);
            ret = ad::Derivative(ret, var5, 1);
            ret = ad::Derivative(ret, var6, 1);
            ret = ad::Derivative(ret, var7, 1);
            ret = ad::Derivative(ret, var8, 1);
            ret = ad::Derivative(ret, var9, 1);
            ret = ad::Derivative(ret, var10, 1);
            return ad::Derivative(ret, var11, 1);
        }

        /**
         * Derivative with respect to var0....var12 in order.
         *
         * @param var1
         * @param var2
         * @param var3
         * @param var4
         * @param var5
         * @param var6
         * @param var7
         * @param var8
         * @param var9
         * @param var10
         * @param var11
         * @param var12
         * @return  derivative
         */
        const ADNumber<T> WRT(const ADNumber<T> &var1,
                const ADNumber<T> &var2,
                const ADNumber<T> &var3,
                const ADNumber<T> &var4,
                const ADNumber<T> &var5,
                const ADNumber<T> &var6,
                const ADNumber<T> &var7,
                const ADNumber<T> &var8,
                const ADNumber<T> &var9,
                const ADNumber<T> &var10,
                const ADNumber<T> &var11,
                const ADNumber<T> &var12) {
            ADNumber<T> ret = ad::Derivative(*this, var1, 1);
            ret = ad::Derivative(ret, var2, 1);
            ret = ad::Derivative(ret, var3, 1);
            ret = ad::Derivative(ret, var4, 1);
            ret = ad::Derivative(ret, var5, 1);
            ret = ad::Derivative(ret, var6, 1);
            ret = ad::Derivative(ret, var7, 1);
            ret = ad::Derivative(ret, var8, 1);
            ret = ad::Derivative(ret, var9, 1);
            ret = ad::Derivative(ret, var10, 1);
            ret = ad::Derivative(ret, var11, 1);
            return ad::Derivative(ret, var12, 1);
        }

        /**
         * Derivative with respect to var0....var13 in order.
         *
         * @param var1
         * @param var2
         * @param var3
         * @param var4
         * @param var5
         * @param var6
         * @param var7
         * @param var8
         * @param var9
         * @param var10
         * @param var11
         * @param var12
         * @param var13
         * @return  derivative
         */
        const ADNumber<T> WRT(const ADNumber<T> &var1,
                const ADNumber<T> &var2,
                const ADNumber<T> &var3,
                const ADNumber<T> &var4,
                const ADNumber<T> &var5,
                const ADNumber<T> &var6,
                const ADNumber<T> &var7,
                const ADNumber<T> &var8,
                const ADNumber<T> &var9,
                const ADNumber<T> &var10,
                const ADNumber<T> &var11,
                const ADNumber<T> &var12,
                const ADNumber<T> &var13) {
            ADNumber<T> ret = ad::Derivative(*this, var1, 1);
            ret = ad::Derivative(ret, var2, 1);
            ret = ad::Derivative(ret, var3, 1);
            ret = ad::Derivative(ret, var4, 1);
            ret = ad::Derivative(ret, var5, 1);
            ret = ad::Derivative(ret, var6, 1);
            ret = ad::Derivative(ret, var7, 1);
            ret = ad::Derivative(ret, var8, 1);
            ret = ad::Derivative(ret, var9, 1);
            ret = ad::Derivative(ret, var10, 1);
            ret = ad::Derivative(ret, var11, 1);
            ret = ad::Derivative(ret, var12, 1);
            return ad::Derivative(ret, var13, 1);
        }

        /**
         * Derivative with respect to var0....var14 in order.
         *
         * @param var1
         * @param var2
         * @param var3
         * @param var4
         * @param var5
         * @param var6
         * @param var7
         * @param var8
         * @param var9
         * @param var10
         * @param var11
         * @param var12
         * @param var13
         * @param var14
         * @return  derivative
         */
        const ADNumber<T> WRT(const ADNumber<T> &var1,
                const ADNumber<T> &var2,
                const ADNumber<T> &var3,
                const ADNumber<T> &var4,
                const ADNumber<T> &var5,
                const ADNumber<T> &var6,
                const ADNumber<T> &var7,
                const ADNumber<T> &var8,
                const ADNumber<T> &var9,
                const ADNumber<T> &var10,
                const ADNumber<T> &var11,
                const ADNumber<T> &var12,
                const ADNumber<T> &var13,
                const ADNumber<T> &var14) {
            ADNumber<T> ret = ad::Derivative(*this, var1, 1);
            ret = ad::Derivative(ret, var2, 1);
            ret = ad::Derivative(ret, var3, 1);
            ret = ad::Derivative(ret, var4, 1);
            ret = ad::Derivative(ret, var5, 1);
            ret = ad::Derivative(ret, var6, 1);
            ret = ad::Derivative(ret, var7, 1);
            ret = ad::Derivative(ret, var8, 1);
            ret = ad::Derivative(ret, var9, 1);
            ret = ad::Derivative(ret, var10, 1);
            ret = ad::Derivative(ret, var11, 1);
            ret = ad::Derivative(ret, var12, 1);
            ret = ad::Derivative(ret, var13, 1);
            return ad::Derivative(ret, var14, 1);
        }

        /**
         * Derivative with respect to var0....var15 in order.
         *
         * @param var1
         * @param var2
         * @param var3
         * @param var4
         * @param var5
         * @param var6
         * @param var7
         * @param var8
         * @param var9
         * @param var10
         * @param var11
         * @param var12
         * @param var13
         * @param var14
         * @param var15
         * @return  derivative
         */
        const ADNumber<T> WRT(const ADNumber<T> &var1,
                const ADNumber<T> &var2,
                const ADNumber<T> &var3,
                const ADNumber<T> &var4,
                const ADNumber<T> &var5,
                const ADNumber<T> &var6,
                const ADNumber<T> &var7,
                const ADNumber<T> &var8,
                const ADNumber<T> &var9,
                const ADNumber<T> &var10,
                const ADNumber<T> &var11,
                const ADNumber<T> &var12,
                const ADNumber<T> &var13,
                const ADNumber<T> &var14,
                const ADNumber<T> &var15) {
            ADNumber<T> ret = ad::Derivative(*this, var1, 1);
            ret = ad::Derivative(ret, var2, 1);
            ret = ad::Derivative(ret, var3, 1);
            ret = ad::Derivative(ret, var4, 1);
            ret = ad::Derivative(ret, var5, 1);
            ret = ad::Derivative(ret, var6, 1);
            ret = ad::Derivative(ret, var7, 1);
            ret = ad::Derivative(ret, var8, 1);
            ret = ad::Derivative(ret, var9, 1);
            ret = ad::Derivative(ret, var10, 1);
            ret = ad::Derivative(ret, var11, 1);
            ret = ad::Derivative(ret, var12, 1);
            ret = ad::Derivative(ret, var13, 1);
            ret = ad::Derivative(ret, var14, 1);
            return ad::Derivative(ret, var15, 1);
        }

        /**
         * Derivative with respect to var0....var16 in order.
         *
         * @param var1
         * @param var2
         * @param var3
         * @param var4
         * @param var5
         * @param var6
         * @param var7
         * @param var8
         * @param var9
         * @param var10
         * @param var11
         * @param var12
         * @param var13
         * @param var14
         * @param var15
         * @param var16
         * @return  derivative
         */
        const ADNumber<T> WRT(const ADNumber<T> &var1,
                const ADNumber<T> &var2,
                const ADNumber<T> &var3,
                const ADNumber<T> &var4,
                const ADNumber<T> &var5,
                const ADNumber<T> &var6,
                const ADNumber<T> &var7,
                const ADNumber<T> &var8,
                const ADNumber<T> &var9,
                const ADNumber<T> &var10,
                const ADNumber<T> &var11,
                const ADNumber<T> &var12,
                const ADNumber<T> &var13,
                const ADNumber<T> &var14,
                const ADNumber<T> &var15,
                const ADNumber<T> &var16) {
            ADNumber<T> ret = ad::Derivative(*this, var1, 1);
            ret = ad::Derivative(ret, var2, 1);
            ret = ad::Derivative(ret, var3, 1);
            ret = ad::Derivative(ret, var4, 1);
            ret = ad::Derivative(ret, var5, 1);
            ret = ad::Derivative(ret, var6, 1);
            ret = ad::Derivative(ret, var7, 1);
            ret = ad::Derivative(ret, var8, 1);
            ret = ad::Derivative(ret, var9, 1);
            ret = ad::Derivative(ret, var10, 1);
            ret = ad::Derivative(ret, var11, 1);
            ret = ad::Derivative(ret, var12, 1);
            ret = ad::Derivative(ret, var13, 1);
            ret = ad::Derivative(ret, var14, 1);
            ret = ad::Derivative(ret, var15, 1);
            return ad::Derivative(ret, var16, 1);
        }

        /**
         * Derivative with respect to var0....var17 in order.
         *
         * @param var1
         * @param var2
         * @param var3
         * @param var4
         * @param var5
         * @param var6
         * @param var7
         * @param var8
         * @param var9
         * @param var10
         * @param var11
         * @param var12
         * @param var13
         * @param var14
         * @param var15
         * @param var16
         * @param var17
         * @return  derivative
         */
        const ADNumber<T> WRT(const ADNumber<T> &var1,
                const ADNumber<T> &var2,
                const ADNumber<T> &var3,
                const ADNumber<T> &var4,
                const ADNumber<T> &var5,
                const ADNumber<T> &var6,
                const ADNumber<T> &var7,
                const ADNumber<T> &var8,
                const ADNumber<T> &var9,
                const ADNumber<T> &var10,
                const ADNumber<T> &var11,
                const ADNumber<T> &var12,
                const ADNumber<T> &var13,
                const ADNumber<T> &var14,
                const ADNumber<T> &var15,
                const ADNumber<T> &var16,
                const ADNumber<T> &var17) {
            ADNumber<T> ret = ad::Derivative(*this, var1, 1);
            ret = ad::Derivative(ret, var2, 1);
            ret = ad::Derivative(ret, var3, 1);
            ret = ad::Derivative(ret, var4, 1);
            ret = ad::Derivative(ret, var5, 1);
            ret = ad::Derivative(ret, var6, 1);
            ret = ad::Derivative(ret, var7, 1);
            ret = ad::Derivative(ret, var8, 1);
            ret = ad::Derivative(ret, var9, 1);
            ret = ad::Derivative(ret, var10, 1);
            ret = ad::Derivative(ret, var11, 1);
            ret = ad::Derivative(ret, var12, 1);
            ret = ad::Derivative(ret, var13, 1);
            ret = ad::Derivative(ret, var14, 1);
            ret = ad::Derivative(ret, var15, 1);
            ret = ad::Derivative(ret, var16, 1);
            return ad::Derivative(ret, var17, 1);
        }

        /**
         * Derivative with respect to var0....var18 in order.
         *
         * @param var1
         * @param var2
         * @param var3
         * @param var4
         * @param var5
         * @param var6
         * @param var7
         * @param var8
         * @param var9
         * @param var10
         * @param var11
         * @param var12
         * @param var13
         * @param var14
         * @param var15
         * @param var16
         * @param var17
         * @param var18
         * @return  derivative
         */
        const ADNumber<T> WRT(const ADNumber<T> &var1,
                const ADNumber<T> &var2,
                const ADNumber<T> &var3,
                const ADNumber<T> &var4,
                const ADNumber<T> &var5,
                const ADNumber<T> &var6,
                const ADNumber<T> &var7,
                const ADNumber<T> &var8,
                const ADNumber<T> &var9,
                const ADNumber<T> &var10,
                const ADNumber<T> &var11,
                const ADNumber<T> &var12,
                const ADNumber<T> &var13,
                const ADNumber<T> &var14,
                const ADNumber<T> &var15,
                const ADNumber<T> &var16,
                const ADNumber<T> &var17,
                const ADNumber<T> &var18) {
            ADNumber<T> ret = ad::Derivative(*this, var1, 1);
            ret = ad::Derivative(ret, var2, 1);
            ret = ad::Derivative(ret, var3, 1);
            ret = ad::Derivative(ret, var4, 1);
            ret = ad::Derivative(ret, var5, 1);
            ret = ad::Derivative(ret, var6, 1);
            ret = ad::Derivative(ret, var7, 1);
            ret = ad::Derivative(ret, var8, 1);
            ret = ad::Derivative(ret, var9, 1);
            ret = ad::Derivative(ret, var10, 1);
            ret = ad::Derivative(ret, var11, 1);
            ret = ad::Derivative(ret, var12, 1);
            ret = ad::Derivative(ret, var13, 1);
            ret = ad::Derivative(ret, var14, 1);
            ret = ad::Derivative(ret, var15, 1);
            ret = ad::Derivative(ret, var16, 1);
            ret = ad::Derivative(ret, var17, 1);
            return ad::Derivative(ret, var18, 1);
        }

        /**
         * Derivative with respect to var0....var19 in order.
         *
         * @param var1
         * @param var2
         * @param var3
         * @param var4
         * @param var5
         * @param var6
         * @param var7
         * @param var8
         * @param var9
         * @param var10
         * @param var11
         * @param var12
         * @param var13
         * @param var14
         * @param var15
         * @param var16
         * @param var17
         * @param var18
         * @param var19
         * @return  derivative
         */
        const ADNumber<T> WRT(const ADNumber<T> &var1,
                const ADNumber<T> &var2,
                const ADNumber<T> &var3,
                const ADNumber<T> &var4,
                const ADNumber<T> &var5,
                const ADNumber<T> &var6,
                const ADNumber<T> &var7,
                const ADNumber<T> &var8,
                const ADNumber<T> &var9,
                const ADNumber<T> &var10,
                const ADNumber<T> &var11,
                const ADNumber<T> &var12,
                const ADNumber<T> &var13,
                const ADNumber<T> &var14,
                const ADNumber<T> &var15,
                const ADNumber<T> &var16,
                const ADNumber<T> &var17,
                const ADNumber<T> &var18,
                const ADNumber<T> &var19) {
            ADNumber<T> ret = ad::Derivative(*this, var1, 1);
            ret = ad::Derivative(ret, var2, 1);
            ret = ad::Derivative(ret, var3, 1);
            ret = ad::Derivative(ret, var4, 1);
            ret = ad::Derivative(ret, var5, 1);
            ret = ad::Derivative(ret, var6, 1);
            ret = ad::Derivative(ret, var7, 1);
            ret = ad::Derivative(ret, var8, 1);
            ret = ad::Derivative(ret, var9, 1);
            ret = ad::Derivative(ret, var10, 1);
            ret = ad::Derivative(ret, var11, 1);
            ret = ad::Derivative(ret, var12, 1);
            ret = ad::Derivative(ret, var13, 1);
            ret = ad::Derivative(ret, var14, 1);
            ret = ad::Derivative(ret, var15, 1);
            ret = ad::Derivative(ret, var16, 1);
            ret = ad::Derivative(ret, var17, 1);
            ret = ad::Derivative(ret, var18, 1);
            return ad::Derivative(ret, var19, 1);
        }

        /**
         * Derivative with respect to var0....var20 in order.
         *
         * @param var1
         * @param var2
         * @param var3
         * @param var4
         * @param var5
         * @param var6
         * @param var7
         * @param var8
         * @param var9
         * @param var10
         * @param var11
         * @param var12
         * @param var13
         * @param var14
         * @param var15
         * @param var16
         * @param var17
         * @param var18
         * @param var19
         * @param var20
         * @return  derivative
         */
        const ADNumber<T> WRT(const ADNumber<T> &var1,
                const ADNumber<T> &var2,
                const ADNumber<T> &var3,
                const ADNumber<T> &var4,
                const ADNumber<T> &var5,
                const ADNumber<T> &var6,
                const ADNumber<T> &var7,
                const ADNumber<T> &var8,
                const ADNumber<T> &var9,
                const ADNumber<T> &var10,
                const ADNumber<T> &var11,
                const ADNumber<T> &var12,
                const ADNumber<T> &var13,
                const ADNumber<T> &var14,
                const ADNumber<T> &var15,
                const ADNumber<T> &var16,
                const ADNumber<T> &var17,
                const ADNumber<T> &var18,
                const ADNumber<T> &var19,
                const ADNumber<T> &var20) {
            ADNumber<T> ret = ad::Derivative(*this, var1, 1);
            ret = ad::Derivative(ret, var2, 1);
            ret = ad::Derivative(ret, var3, 1);
            ret = ad::Derivative(ret, var4, 1);
            ret = ad::Derivative(ret, var5, 1);
            ret = ad::Derivative(ret, var6, 1);
            ret = ad::Derivative(ret, var7, 1);
            ret = ad::Derivative(ret, var8, 1);
            ret = ad::Derivative(ret, var9, 1);
            ret = ad::Derivative(ret, var10, 1);
            ret = ad::Derivative(ret, var11, 1);
            ret = ad::Derivative(ret, var12, 1);
            ret = ad::Derivative(ret, var13, 1);
            ret = ad::Derivative(ret, var14, 1);
            ret = ad::Derivative(ret, var15, 1);
            ret = ad::Derivative(ret, var16, 1);
            ret = ad::Derivative(ret, var17, 1);
            ret = ad::Derivative(ret, var18, 1);
            ret = ad::Derivative(ret, var19, 1);
            return ad::Derivative(ret, var20, 1);
        }

#ifdef ADNUMBER_C11

        const ADNumber<T> operator ""d(ADNumber<T> &wrt) {

        }

#endif

        //Friends
        // relational operators
        template<class TT> friend const int operator==(const ADNumber<TT>& lhs, const ADNumber<TT>& rhs);
        template<class TT> friend const int operator!=(const ADNumber<TT>& lhs, const ADNumber<TT>& rhs);
        template<class TT> friend const int operator<(const ADNumber<TT>& lhs, const ADNumber<TT>& rhs);
        template<class TT> friend const int operator>(const ADNumber<TT>& lhs, const ADNumber<TT>& rhs);
        template<class TT> friend const int operator<=(const ADNumber<TT>& lhs, const ADNumber<TT>& rhs);
        template<class TT> friend const int operator>=(const ADNumber<TT>& lhs, const ADNumber<TT>& rhs);

        template<class TT> friend const int operator==(TT lhs, const ADNumber<TT>& rhs);
        template<class TT> friend const int operator!=(TT lhs, const ADNumber<TT>& rhs);
        template<class TT> friend const int operator<(TT lhs, const ADNumber<TT>& rhs);
        template<class TT> friend const int operator>(TT lhs, const ADNumber<TT>& rhs);
        template<class TT> friend const int operator<=(TT lhs, const ADNumber<TT>& rhs);
        template<class TT> friend const int operator>=(TT lhs, const ADNumber<TT>& rhs);

        template<class TT> friend const int operator==(const ADNumber<TT>& lhs, TT rhs);
        template<class TT> friend const int operator!=(const ADNumber<TT>& lhs, TT rhs);
        template<class TT> friend const int operator<(const ADNumber<TT>& lhs, TT rhs);
        template<class TT> friend const int operator>(const ADNumber<TT>& lhs, TT rhs);
        template<class TT> friend const int operator<=(const ADNumber<TT>& lhs, TT rhs);
        template<class TT> friend const int operator>=(const ADNumber<TT>& lhs, TT rhs);


        // binary
        template<class TT> friend const ADNumber<TT> operator-(const ADNumber<TT>& lhs, const ADNumber<TT>& rhs);
        template<class TT> friend const ADNumber<TT> operator/(const ADNumber<TT>& lhs, const ADNumber<TT>& rhs);
        template<class TT> friend const ADNumber<TT> operator+(const ADNumber<TT>& lhs, const ADNumber<TT>& rhs);
        template<class TT> friend const ADNumber<TT> operator*(const ADNumber<TT>& lhs, const ADNumber<TT>& rhs);


        template<class TT> friend const ADNumber<TT> operator-(TT lhs, const ADNumber<TT>& rhs);
        template<class TT> friend const ADNumber<TT> operator/(TT lhs, const ADNumber<TT>& rhs);
        template<class TT> friend const ADNumber<TT> operator+(TT lhs, const ADNumber<TT>& rhs);
        template<class TT> friend const ADNumber<TT> operator*(TT lhs, const ADNumber<TT>& rhs);


        template<class TT> friend const ADNumber<TT> operator-(const ADNumber<TT>& lhs, TT rhs);
        template<class TT> friend const ADNumber<TT> operator/(const ADNumber<TT>& lhs, TT rhs);
        template<class TT> friend const ADNumber<TT> operator+(const ADNumber<TT>& lhs, TT rhs);
        template<class TT> friend const ADNumber<TT> operator*(const ADNumber<TT>& lhs, TT rhs);

    private:

        void Initialize() {

            expression->take();
            expression->SetLeft(NULL);
            expression->SetRight(NULL);
            expression->SetValue(value);
            expression->SetId(id);
            expression->SetOp(VARIABLE);
            // expression->SetName(GetName());


        }

    };

    /*!
     * Equal to comparison operator.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> inline const int operator==(const ADNumber<T>& lhs, const ADNumber<T>& rhs) {

        return (lhs.GetValue() == rhs.GetValue());
    }

    /*!
     * Not equal to comparison operator.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> inline const int operator!=(const ADNumber<T>& lhs, const ADNumber<T>& rhs) {

        return (lhs.GetValue() != rhs.GetValue());
    }

    /*!
     * Less than comparison operator.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> inline const int operator<(const ADNumber<T>& lhs, const ADNumber<T>& rhs) {

        return (lhs.GetValue() < rhs.GetValue());
    }

    /*!
     * Greater than comparison operator.
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> inline const int operator>(const ADNumber<T>& lhs, const ADNumber<T>& rhs) {

        return (lhs.GetValue() > rhs.GetValue());
    }

    /*!
     * Less than equal to comparison operator.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> inline const int operator<=(const ADNumber<T>& lhs, const ADNumber<T>& rhs) {

        return (lhs.GetValue() <= rhs.GetValue());
    }

    /*!
     * Greater than equal to comparison operator.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> inline const int operator>=(const ADNumber<T>& lhs, const ADNumber<T>& rhs) {

        return (lhs.GetValue() >= rhs.GetValue());
    }

    /*!
     * Equal to comparison operator.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> inline const int operator==(T lhs, const ADNumber<T>& rhs) {

        return (lhs == rhs.GetValue());
    }

    /*!
     * Not equal to comparison operator.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> inline const int operator!=(T lhs, const ADNumber<T>& rhs) {

        return (lhs != rhs.GetValue());
    }

    /*!
     * Less than comparison operator.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> inline const int operator<(T lhs, const ADNumber<T>& rhs) {

        return (lhs < rhs.GetValue());
    }

    /*!
     * Greater than comparison operator.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> inline const int operator>(T lhs, const ADNumber<T>& rhs) {

        return (lhs > rhs.GetValue());
    }

    /*!
     * Less than equal to comparison operator.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> inline const int operator<=(T lhs, const ADNumber<T>& rhs) {

        return (lhs <= rhs.GetValue());
    }

    /*!
     * Greater than equal to comparison operator.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> inline const int operator>=(T lhs, const ADNumber<T>& rhs) {

        return (lhs >= rhs.GetValue());
    }

    /*!
     * Equal to comparison operator.
     *
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> inline const int operator==(const ADNumber<T>& lhs, T rhs) {

        return (lhs.GetValue() == rhs);
    }

    /*!
     * Not equal to comparison operator.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> inline const int operator!=(const ADNumber<T>& lhs, T rhs) {

        return (lhs.GetValue() != rhs);
    }

    /*!
     * Less than comparison operator.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> inline const int operator<(const ADNumber<T>& lhs, T rhs) {

        return (lhs.GetValue() < rhs);
    }

    /*!
     * Greater than comparison operator.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> inline const int operator>(const ADNumber<T>& lhs, T rhs) {

        return (lhs.GetValue() > rhs);
    }

    /*!
     * Less than equal to comparison operator.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> inline const int operator<=(const ADNumber<T>& lhs, T rhs) {

        return (lhs.GetValue() <= rhs);
    }

    /*!
     * Greater than equal to comparison operator.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> inline const int operator>=(const ADNumber<T>& lhs, T rhs) {

        return (lhs.GetValue() >= rhs);
    }


    // binary

    /*!
     * Outside class subtraction operator.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> const ADNumber<T> operator-(const ADNumber<T>& lhs, const ADNumber<T>& rhs) {
        ADNumber<T > ret((lhs.GetValue() - rhs.GetValue()));

        ret.expression->SetOp(MINUS);
        ret.expression->SetLeft(lhs.expression);
        ret.expression->SetRight(rhs.expression);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;
    }

    /*!
     * Outside class addition operator.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> const ADNumber<T> operator+(const ADNumber<T>& lhs, const ADNumber<T>& rhs) {
        ADNumber<T> ret((lhs.GetValue() + rhs.GetValue()));

        ret.expression->SetOp(PLUS);
        ret.expression->SetLeft(lhs.expression);
        ret.expression->SetRight(rhs.expression);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;
    }

    /*!
     * Outside class division operator.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> const ADNumber<T> operator/(const ADNumber<T>& lhs, const ADNumber<T>& rhs) {
        ADNumber<T > ret((lhs.GetValue() / rhs.GetValue()));

        ret.expression->SetOp(DIVIDE);
        ret.expression->SetLeft(lhs.expression);
        ret.expression->SetRight(rhs.expression);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;
    }

    /*!
     * Outside class multiplication operator.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> const ADNumber<T> operator*(const ADNumber<T>& lhs, const ADNumber<T>& rhs) {
        ADNumber<T > ret((lhs.GetValue() * rhs.GetValue()));

        ret.expression->SetOp(MULTIPLY);
        ret.expression->SetLeft(lhs.expression);
        ret.expression->SetRight(rhs.expression);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;
    }

    /*!
     * Outside class subtraction operator.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> const ADNumber<T> operator-(T lhs, const ADNumber<T>& rhs) {
        ADNumber<T > ret((lhs - rhs.GetValue()));

        Expression<T> *exp = NEW_EXPRESSION(T) ();
        exp->SetValue(lhs);
        exp->SetOp(CONSTANT);

        ret.expression->SetOp(MINUS);
        ret.expression->SetLeft(exp);
        ret.expression->SetRight(rhs.expression);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;
    }

    /*!
     * Outside class addition operator.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> const ADNumber<T> operator+(T lhs, const ADNumber<T>& rhs) {
        ADNumber<T > ret((lhs + rhs.GetValue()));

        Expression<T> *exp = NEW_EXPRESSION(T) ();
        exp->SetValue(lhs);
        exp->SetOp(CONSTANT);

        ret.expression->SetOp(PLUS);
        ret.expression->SetLeft(exp);
        ret.expression->SetRight(rhs.expression);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;
    }

    /*!
     * Outside class division operator.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> const ADNumber<T> operator/(T lhs, const ADNumber<T>& rhs) {
        ADNumber<T > ret((lhs / rhs.GetValue()));

        Expression<T> *exp = NEW_EXPRESSION(T) ();
        exp->SetValue(lhs);
        exp->SetOp(CONSTANT);

        ret.expression->SetOp(DIVIDE);
        ret.expression->SetLeft(exp);
        ret.expression->SetRight(rhs.expression);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;

    }

    /*!
     * Outside class multiplication operator.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> const ADNumber<T> operator*(T lhs, const ADNumber<T>& rhs) {
        ADNumber<T > ret((lhs * rhs.GetValue()));

        Expression<T> *exp = NEW_EXPRESSION(T) ();
        exp->SetValue(lhs);
        exp->SetOp(CONSTANT);

        ret.expression->SetOp(MULTIPLY);
        ret.expression->SetLeft(exp);
        ret.expression->SetRight(rhs.expression);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());


        return ret;
    }

    /*!
     * Outside class subtraction operator.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> const ADNumber<T> operator-(const ADNumber<T>& lhs, T rhs) {
        ADNumber<T > ret((lhs.GetValue() - rhs));

        Expression<T> *exp = NEW_EXPRESSION(T) ();
        exp->SetValue(rhs);
        exp->SetOp(CONSTANT);

        ret.expression->SetOp(MINUS);
        ret.expression->SetLeft(lhs.expression);
        ret.expression->SetRight(exp);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;
    }

    /*!
     * Outside class addition operator.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> const ADNumber<T> operator+(const ADNumber<T>& lhs, T rhs) {
        ADNumber<T> ret((lhs.GetValue() + rhs));

        Expression<T> *exp = NEW_EXPRESSION(T) ();
        exp->SetValue(rhs);
        exp->SetOp(CONSTANT);

        ret.expression->SetOp(PLUS);
        ret.expression->SetLeft(lhs.expression);
        ret.expression->SetRight(exp);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;
    }

    /*!
     * Outside class division operator.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> const ADNumber<T> operator/(const ADNumber<T>& lhs, T rhs) {
        ADNumber<T > ret((lhs.GetValue() / rhs));

        Expression<T> *exp = NEW_EXPRESSION(T) ();
        exp->SetValue(rhs);
        exp->SetOp(CONSTANT);

        ret.expression->SetOp(DIVIDE);
        ret.expression->SetLeft(lhs.expression);
        ret.expression->SetRight(exp);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;
    }

    /*!
     * Outside class multiplication operator.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> const ADNumber<T> operator*(const ADNumber<T>& lhs, T rhs) {
        ADNumber<T > ret((lhs.GetValue() * rhs));

        Expression<T> *exp = NEW_EXPRESSION(T) ();
        exp->SetValue(rhs);
        exp->SetOp(CONSTANT);

        ret.expression->SetOp(MULTIPLY);
        ret.expression->SetLeft(lhs.expression);
        ret.expression->SetRight(exp);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;
    }

    template<class T >
    std::ostream & operator<<(std::ostream &out, ADNumber<T> const &t) {
        out << t.GetValue();
        return out;
    }




}

namespace std {

    //Math Overloads

    /*!
     * Returns the arc tangent of the ADNumber number val.
     * 
     * @param val
     * @return 
     */
    template<class T> const ad::ADNumber<T> atan(const ad::ADNumber<T> &val) {
        ad::ADNumber<T> ret(atan(val.GetValue()));

        ret.expression->SetOp(ad::ATAN);
        ret.expression->SetLeft(val.expression);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;
    }

    /*!
     * Compute arc tangent with two parameters. 
     * 
     * @param val
     * @return 
     */
    template<class T> const ad::ADNumber<T> atan2(const ad::ADNumber<T> &lhs, const ad::ADNumber<T> &rhs) {

        T x = lhs.GetValue();
        T y = rhs.GetValue();
        T temp = x * x + y*y;
        ad::ADNumber<T> ret(atan2(x, y));

        ret.expression->SetOp(ad::ATAN2);
        ret.expression->SetLeft(lhs.expression);
        ret.expression->SetRight(rhs.expression);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;
    }

    /*!
     * Compute arc tangent with two parameters.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> const ad::ADNumber<T> atan2(T lhs, const ad::ADNumber<T> &rhs) {


        T x = lhs;
        T y = rhs.GetValue();
        T temp = x * x + y*y;

        ad::ADNumber<T> ret(atan2(x, y));

        ad::Expression<T> *exp = new ad::Expression<T > ();
        exp->SetValue(lhs);
        exp->SetOp(ad::CONSTANT);

        ret.expression->SetOp(ad::ATAN2);
        ret.expression->SetLeft(exp);
        ret.expression->SetRight(rhs.expression);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;
    }

    /*!
     * Compute arc tangent with two parameters.
     *  
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> const ad::ADNumber<T> atan2(const ad::ADNumber<T> &lhs, T rhs) {

        T x = lhs.GetValue();
        T y = rhs;
        T temp = x * x + y*y;
        ad::ADNumber<T> ret(atan2(x, y));

        ad::Expression<T> *exp = new ad::Expression<T > ();
        exp->SetValue(rhs);
        exp->SetOp(ad::CONSTANT);

        ret.expression->SetOp(ad::ATAN2);
        ret.expression->SetLeft(lhs.expression);
        ret.expression->SetRight(exp);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;
    }

    /*!
     * Returns the cosine of the ADNumber number val.
     * 
     * @param val
     * @return 
     */
    template<class T> const ad::ADNumber<T> cos(const ad::ADNumber<T> &val) {
        ad::ADNumber<T> ret(cos(val.GetValue()));

        ret.expression->SetOp(ad::COS);
        ret.expression->SetLeft(val.expression);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;
    }

    /*!
     * Compute exponential function for val.
     * 
     * @param val
     * @return 
     */
    template<class T> const ad::ADNumber<T> exp(const ad::ADNumber<T> &val) {
        ad::ADNumber<T> ret(exp(val.GetValue()));

        ret.expression->SetOp(ad::EXP);
        ret.expression->SetLeft(val.expression);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;
    }

    template<class T> const ad::ADNumber<T> mfexp(const ad::ADNumber<T> & x) {
        T b = T(60);
        if (x <= b && x >= T(-1) * b) {
            return std::exp(x);
        } else if (x > b) {
            return std::exp(b)*(T(1.) + T(2.) * (x - b)) / (T(1.) + x - b);
        } else {
            return std::exp(T(-1) * b)*(T(1.) - x - b) / (T(1.) + T(2.) * (T(-1) * x - b));
        }
    }

    /*!
     * Compute natural logarithm of val.
     * @param val
     * @return 
     */
    template<class T> const ad::ADNumber<T> log(const ad::ADNumber<T> &val) {
        ad::ADNumber<T> ret(log(val.GetValue()));

        ret.expression->SetOp(ad::LOG);
        ret.expression->SetLeft(val.expression);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;
    }

    /*!
     * Compute natural common logarithm of val.
     * @param val
     * @return 
     */
    template<class T> const ad::ADNumber<T> log10(const ad::ADNumber<T> &val) {
        ad::ADNumber<T> ret(log10(val.GetValue()));



        ret.expression->SetOp(ad::LOG10);
        ret.expression->SetLeft(val.expression);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());


        return ret;
    }

    /*!
     * Raise to power.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> const ad::ADNumber<T> pow(const ad::ADNumber<T> &lhs, const ad::ADNumber<T> &rhs) {
        ad::ADNumber<T> ret(pow(lhs.GetValue(), rhs.GetValue()));

        ret.expression->SetOp(ad::POW);
        ret.expression->SetLeft(lhs.expression);
        ret.expression->SetRight(rhs.expression);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;
    }

    /*!
     * Raise to power.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> const ad::ADNumber<T> pow(T lhs, const ad::ADNumber<T> & rhs) {
        ad::ADNumber<T> ret(pow(lhs, rhs.GetValue()));

        ad::ExpressionPtr exp = new ad::Expression<T > ();
        exp->SetValue(lhs);
        exp->SetOp(ad::CONSTANT);

        ret.expression->SetOp(ad::POW);
        ret.expression->SetLeft(exp);
        ret.expression->SetRight(rhs.expression);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;
    }

    /*!
     * Raise to power.
     * 
     * @param lhs
     * @param rhs
     * @return 
     */
    template<class T> const ad::ADNumber<T> pow(const ad::ADNumber<T> &lhs, T rhs) {
        T val = std::pow(lhs.GetValue(), rhs);
        return ad::ADNumber<T > (val,
                NEW_EXPRESSION(T)(val,
                0, ad::POW, lhs.expression,
                NEW_EXPRESSION(T)(rhs, 0, ad::CONSTANT, NULL, NULL)));
    }

    /*!
     * Returns the sine of val.
     * 
     * @param val
     * @return 
     */
    template<class T> const ad::ADNumber<T> sin(const ad::ADNumber<T> &val) {
        ad::ADNumber<T> ret(sin(val.GetValue()));

        ret.expression->SetOp(ad::SIN);
        ret.expression->SetLeft(val.expression);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;
    }

    /*!
     * Compute square root val.
     * 
     * @param val
     * @return 
     */
    template<class T> const ad::ADNumber<T> sqrt(const ad::ADNumber<T> &val) {
        T temp = sqrt(val.GetValue());
        ad::ADNumber<T> ret(temp);

        ret.expression->SetOp(ad::SQRT);
        ret.expression->SetLeft(val.expression);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;
    }

    /*!
     * Returns the tangent of val.
     * 
     * @param val
     * @return 
     */
    template<class T> const ad::ADNumber<T> tan(const ad::ADNumber<T> &val) {
        T temp = cos(val.GetValue());
        ad::ADNumber<T> ret(tan(val.GetValue()));

        ret.expression->SetOp(ad::TAN);
        ret.expression->SetLeft(val.expression);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;
    }

    /*!
     * Returns the arc cosine of val.
     * 
     * @param val
     * @return 
     */
    template<class T> const ad::ADNumber<T> acos(const ad::ADNumber<T> & val) {

        ad::ADNumber<T> ret(acos(val.GetValue()));

        ret.expression->SetOp(ad::ACOS);
        ret.expression->SetLeft(val.expression);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;
    }

    /*!
     * Returns the arc sine of val.
     * 
     * @param val
     * @return 
     */
    template<class T> const ad::ADNumber<T> asin(const ad::ADNumber<T> &val) {
        ad::ADNumber<T> ret(asin(val.GetValue()));

        ret.expression->SetOp(ad::ASIN);
        ret.expression->SetLeft(val.expression);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;
    }

    /*!
     * Returns the hyperbolic sin of val.
     * 
     * @param val
     * @return 
     */
    template<class T> const ad::ADNumber<T> sinh(const ad::ADNumber<T> &val) {

        ad::ADNumber<T> ret(sinh(val.GetValue()));

        ret.expression->SetOp(ad::SINH);
        ret.expression->SetLeft(val.expression);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;
    }

    /*!
     * Returns the hyperbolic cosine of val.
     * @param val
     * @return 
     */
    template<class T> const ad::ADNumber<T> cosh(const ad::ADNumber<T> &val) {
        ad::ADNumber<T> ret(cosh(val.GetValue()));

        ret.expression->SetOp(ad::COSH);
        ret.expression->SetLeft(val.expression);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;
    }

    /*!
     * Returns the hyperbolic tangent of val.
     * @param val
     * @return 
     */
    template<class T> const ad::ADNumber<T> tanh(const ad::ADNumber<T> &val) {
        T temp = cosh(val.GetValue());
        ad::ADNumber<T> ret(std::tanh(val.GetValue()));

        ret.expression->SetOp(ad::TANH);
        ret.expression->SetLeft(val.expression);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;
    }

    /*!
     * Compute absolute value of val.
     * @param val
     * @return 
     */
    template<class T> const ad::ADNumber<T> fabs(const ad::ADNumber<T> &val) {

        ad::ADNumber<T> ret(fabs(val.GetValue()));

        ret.expression->SetOp(ad::FABS);
        ret.expression->SetLeft(val.expression);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;
    }

    /*!
     * Round down value.
     * 
     * @param val
     * @return 
     */
    template<class T> const ad::ADNumber<T> floor(const ad::ADNumber<T> &val) {
        ad::ADNumber<T> ret(floor(val.GetValue()));
        ret.expression->SetOp(ad::FLOOR);
        ret.expression->SetLeft(val.expression);
        ret.expression->SetId(ret.GetID());
        ret.expression->SetValue(ret.GetValue());

        return ret;
    }

    /*!
     * Solve an equation in the form:
     * lhs = rhs, for variable var;
     * @param lhs
     * @param rhs
     * @param var
     * @return 
     */
    template<class T> const ad::ADNumber<T> solve(const ad::ADNumber<T> &lhs, const ad::ADNumber<T> &rhs, const ad::ADNumber<T> &var) {
        std::cout << "solve not yet implemented....\n";
        ad::ADNumber<T> ret;

        if (lhs.expression->HasId(var.GetID())) {
            std::cout << "left side contains var...\n";
        }

        if (rhs.expression->HasId(var.GetID())) {
            std::cout << "left side contains var...\n";
        }


        return ret;

    }


    //namespace std {

    template<class T>
    class numeric_limits<ad::ADNumber<T> > : public numeric_limits<T> {
    };





}


namespace ad {

    template<class T>
    static T DerivativeValue(const ADNumber<T> &x, const ADNumber<T> &wrt, unsigned int order) {

        if (order == 0) {
            return x.GetValue();
        }

        if (order == 1) {
            return EvaluateDerivative<T > (x.expression, wrt.GetID());
        }



        ExpressionPtr exp = Differentiate<T > (x.expression, wrt.GetID());
        exp->take();
        ExpressionPtr temp;

        if (order > 1) {

            size_t i;
            for (i = 1; i < order - 1; i++) {

                temp = Differentiate<T > (exp, wrt.GetID());
                temp->take();

                exp->release();
                exp = temp;


            }

        }

        T ret = ad::EvaluateDerivative<T > (exp, wrt.GetID());


        return ret;
    }

    template<class T>
    static const ADNumber<T> Derivative(const ADNumber<T> &x, const ADNumber<T> &wrt, unsigned int order) {


        if (order == 0) {
            return ADNumber<T > (x);
        }


        ExpressionPtr exp = Differentiate<T > (x.expression, wrt.GetID());
        exp->take();
        ExpressionPtr temp;

        if (order > 1) {

            size_t i;
            for (i = 1; i < order; i++) {

                temp = Differentiate<T > (exp, wrt.GetID());
                temp->take();

                exp->release();
                exp = temp;


            }

        }

        ADNumber<T> ret(exp);
        ret.SetValue(Evaluate(ret.expression));

        return ret;

    }

    template <typename TT >
    TT SwapBytes(const TT &u) {

        union {
            TT u;
            unsigned char u8[sizeof (TT)];
        } source, dest;

        source.u = u;

        for (size_t k = 0; k < sizeof (TT); k++)
            dest.u8[k] = source.u8[sizeof (TT) - k - 1];

        return dest.u;
    }

    template<class T>
    static void Serialize(ad::Expression<T>* expression, std::ostream &out) {


        bool little_endian = true;

        int num = 1;
        if (*(char *) &num == 1) {
            little_endian = true;
        } else {
            little_endian = false;
        }

        if (!out.good()) {
            std::cout << "Expression serialization stream not open!";
            return;
        }

        std::queue<Expression<T>* > Q;

        Q.push(expression);

        while (Q.size()) {
            Expression<T>* exp = (Expression<T>*)Q.front();

            if (exp == NULL) {
                Q.pop();
                continue;
            }

            unsigned long id = exp->GetId();


            if (!little_endian) {
                id = SwapBytes<unsigned long>(id);
            }

            out.write(reinterpret_cast<const char*> (&id), sizeof (id));

            int op = (int) exp->GetOp();

            if (!little_endian) {
                op = SwapBytes<int>(op);
            }


            out.write(reinterpret_cast<const char*> (&op), sizeof (int));

            T value = exp->GetValue();

            if (!little_endian) {
                value = SwapBytes<T > (value);
            }

            out.write(reinterpret_cast<const char*> (&value), sizeof ( T));

            size_t namesize = exp->GetName().size();
            out.write(reinterpret_cast<const char*> (&namesize), sizeof (namesize));
            out << exp->GetName().c_str();

            if (exp->GetLeft()) {
                out << '1';
            } else {
                out << '0';
            }

            if (exp->GetRight()) {
                out << '1';
            } else {
                out << '0';
            }

            if (exp->GetLeft() == NULL && exp->GetRight() == NULL) {
                Q.pop();
                continue;
            }


            Q.push(exp->GetLeft());
            Q.push(exp->GetRight());
            Q.pop();

        }

    }

    template<class T>
    static void Deserialize(ad::Expression<T>* exp, std::istream &in) {

        bool little_endian = true;

        int num = 1;
        if (*(char *) &num == 1) {
            little_endian = true;
        } else {
            little_endian = false;
        }

        if (!in.good()) {
            std::cout << "Expression serialization stream not open!";
            return;
        }

        if (NULL == exp) {
            exp = new ad::Expression<T > ();

            exp->SetLeft(NULL);
            exp->SetRight(NULL);
        }

        unsigned long* id;
        char idc[sizeof (id)];
        in.read(idc, sizeof (id));
        id = reinterpret_cast<unsigned long*> (idc);

        if (!little_endian) {
            exp->SetId(SwapBytes<unsigned long > (*id));
        } else {
            exp->SetId(*id);
        }




        int* op;
        char opc[sizeof (int) ];
        in.read(opc, sizeof (int));
        op = reinterpret_cast<int*> (opc);

        if (!little_endian) {
            exp->SetOp(static_cast<ad::Operation> (SwapBytes<int> (*op)));
        } else {
            exp->SetOp(static_cast<ad::Operation> (*op));
        }


        T* value;
        char valc[sizeof (T)];
        in.read(valc, sizeof (T));
        value = reinterpret_cast<T*> (valc);


        if (!little_endian) {
            exp->SetValue((SwapBytes<T > (*value)));
        } else {
            exp->SetValue(*value);
        }

        size_t* namesize;
        char namesizec[sizeof (size_t)];
        in.read(namesizec, sizeof (size_t));
        namesize = reinterpret_cast<size_t*> (namesizec);

        char namec[*namesize + 1];
        namec[*namesize] = '\0';

        in.read(namec, *namesize);
        exp->SetName(std::string(namec));

        char left;
        in >> left;

        if (left == '1') {
            exp->SetLeft(new Expression<T > ());
        }

        char right;
        in >> right;

        if (right == '1') {
            exp->SetRight(new Expression<T > ());
        }

        std::queue<Expression<T>* > Q;

        Q.push(exp);

        while (!Q.empty()) {


            Expression<T>* current = Q.front();


            Q.pop();



            if (current->GetLeft() != NULL) {

                Expression<T>* expression = current->GetLeft();
                expression->SetLeft(NULL);
                expression->SetRight(NULL);
                unsigned long* id;
                char idc[sizeof (id)];
                in.read(idc, sizeof (id));
                id = reinterpret_cast<unsigned long*> (idc);

                if (!little_endian) {
                    expression->SetId(SwapBytes<unsigned long > (*id));
                } else {
                    expression->SetId(*id);
                }




                int* op;
                char opc[sizeof (int) ];
                in.read(opc, sizeof (int));
                op = reinterpret_cast<int*> (opc);

                if (!little_endian) {
                    expression->SetOp(static_cast<ad::Operation> (SwapBytes<int> (*op)));
                } else {
                    expression->SetOp(static_cast<ad::Operation> (*op));
                }


                T* value;
                char valc[sizeof (T)];
                in.read(valc, sizeof (T));
                value = reinterpret_cast<T*> (valc);


                if (!little_endian) {
                    expression->SetValue((SwapBytes<T > (*value)));
                } else {
                    expression->SetValue(*value);
                }

                size_t* namesize;
                char namesizec[sizeof (size_t)];
                in.read(namesizec, sizeof (size_t));
                namesize = reinterpret_cast<size_t*> (namesizec);

                char namec[*namesize + 1];
                namec[*namesize] = '\0';

                in.read(namec, *namesize);
                expression->SetName(std::string(namec));

                char left;
                in >> left;

                if (left == '1') {
                    expression->SetLeft(new Expression<T > ());
                }

                char right;
                in >> right;

                if (right == '1') {
                    expression->SetRight(new Expression<T > ());
                }

                Q.push(expression);

            }


            if (current->GetRight() != NULL) {
                Expression<T>* expression = current->GetRight();
                expression->SetLeft(NULL);
                expression->SetRight(NULL);

                unsigned long* id;
                char idc[sizeof (id)];
                in.read(idc, sizeof (id));
                id = reinterpret_cast<unsigned long*> (idc);

                if (!little_endian) {
                    expression->SetId(SwapBytes<unsigned long > (*id));
                } else {
                    expression->SetId(*id);
                }




                int* op;
                char opc[sizeof (int) ];
                in.read(opc, sizeof (int));
                op = reinterpret_cast<int*> (opc);

                if (!little_endian) {
                    expression->SetOp(static_cast<ad::Operation> (SwapBytes<int> (*op)));
                } else {
                    expression->SetOp(static_cast<ad::Operation> (*op));
                }


                T* value;
                char valc[sizeof (T)];
                in.read(valc, sizeof (T));
                value = reinterpret_cast<T*> (valc);


                if (!little_endian) {
                    expression->SetValue((SwapBytes<T > (*value)));
                } else {
                    expression->SetValue(*value);
                }

                size_t* namesize;
                char namesizec[sizeof (size_t)];
                in.read(namesizec, sizeof (size_t));
                namesize = reinterpret_cast<size_t*> (namesizec);

                char namec[*namesize + 1];
                namec[*namesize] = '\0';

                in.read(namec, *namesize);
                expression->SetName(std::string(namec));

                char left;
                in >> left;

                if (left == '1') {
                    expression->SetLeft(new Expression<T > ());
                }

                char right;
                in >> right;

                if (right == '1') {
                    expression->SetRight(new Expression<T > ());
                }

                Q.push(expression);
            }

        }
    }

    template<class T>
    const std::string ToString(const ADNumber<T> &x, bool latex) {
        return x.expression->ToString(latex);
    }




    namespace cas {

        template<class T>
        static const ADNumber<T> Expand(const ADNumber<T> &x) {

        }

        template<class T>
        static const ADNumber<T> Factor(const ADNumber<T> &x) {

        }

        template<class T>
        static const ADNumber<T> Simplify(const ADNumber<T> &x) {

        }

        template<class T>
        static const ADNumber<T> Solve(const ADNumber<T> &exp, const ADNumber<T> &var) {

        }

        template<class T>
        static const std::vector<ADNumber<T> >Solve(const std::vector<ADNumber<T> > &system, const ADNumber<T> &var) {

        }

        template<class T>
        static const ADNumber<T> GCD(const ADNumber<T> &x, const ADNumber<T> &y) {

        }

        template<class T>
        static const ADNumber<T> LCM(const ADNumber<T> &x, const ADNumber<T> &y) {

        }



    }



}



typedef ad::ADNumber<double> addouble;
typedef ad::ADNumber<double> adfloat;

typedef ad::ADNumber<double> dvar;
typedef ad::ADNumber<double> fvar;



#ifdef ADNUMBER_MPI_SUPPORT

/**
 * Message Passing Interface support. Serializes the ADNumber using
 * ad::Serialize. Sends via char array.
 * @param x
 * @param dest
 * @param tag
 * @param comm
 * @return 
 */
template<class T>
int MPI_Send_ADNumber(const ad::ADNumber<T> &x,
int dest,
int tag,
MPI_Comm comm) {

    std::stringstream ss;
    ad::Serialize<T > (x.expression, ss);
    int size = ss.str().size();

    int error = MPI_Send(&size, 1, MPI_INT, dest, tag, comm);

    return MPI_Send((void*) ss.str().c_str(), ss.str().size(), MPI_CHAR, dest, tag, comm);
}

#include <fstream>

/**
 * Deserializes a char array using ad::Serialize and reconstructs a ADNumber.
 * 
 * @param x
 * @param source
 * @param tag
 * @param comm
 * @param status
 * @return 
 */
template<class T>
int MPI_Recv_ADNumber(ad::ADNumber<T> &x,
int source, int tag,
MPI_Comm comm, MPI_Status *status) {
    std::stringstream ss;
    int size = 0;

    MPI_Recv(&size, 1, MPI_INT, source, tag, comm, status);

    char* data = new char[size];
    int ret = MPI_Recv(data, size, MPI_CHAR, source, tag, comm, status);

    for (int i = 0; i < size; i++) {
        ss << data[i];
    }

    ad::Expression<T>* exp = new ad::Expression<T > ();
    ad::Deserialize<T > (exp, ss);

    delete data;
    //  std::cout<<exp->ToString();

    x = ad::ADNumber<T > (exp);
    return ret;

}

#endif




#endif	/* ADNUMBER_HPP */

