/* 
 * File:   ADNumber.hpp
 * Author: matthewsupernaw
 *
 * Created on March 20, 2014, 12:03 PM
 */

#ifndef ADNUMBER_HPP
#define	ADNUMBER_HPP

#include <queue>

#include <sys/resource.h>


#include "util/Expression.hpp"

namespace ad {

    static int SetRuntimeStackLimit(uint32_t limit_mb = 32) {
        const rlim_t kStackSize = limit_mb * 1024L * 1024L; // min stack size = 64 Mb
        struct rlimit rl;
        int result;

        result = getrlimit(RLIMIT_STACK, &rl);

        if (result == 0) {

            if (rl.rlim_cur < kStackSize) {


                rl.rlim_cur = kStackSize;

                if (rl.rlim_max < kStackSize) {
                    std::cout << "Warning: " << __func__ << ", requested stack size is greater than available limit.\n";
                    rl.rlim_cur = rl.rlim_max;
                }

                result = setrlimit(RLIMIT_STACK, &rl);
                if (result != 0) {
                    fprintf(stderr, "setrlimit returned result = %d\n", result);

                }
            }
        }
        return result;
    }



    template<class T> class ADNumber;

    template<class T>
    static T DerivativeValue(const ADNumber<T> &x, const ADNumber<T> &wrt, unsigned int order = 1);

    template<class T>
    static const ADNumber<T> Derivative(const ADNumber<T> &x, const ADNumber<T> &wrt, unsigned int order = 1);

    /*!
     * Creates a unique identifier.
     * @return 
     */
    class IDGenerator {
    public:
        static IDGenerator * instance();

        const unsigned long next() {
            return ++_id;
        }
    private:

        IDGenerator() : _id(0) {
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

    template<class T>
    class ADNumber {
        T value;
        std::string name;
        unsigned long id;
        Expression<T>* expression;
        static bool record_expressoion;
    public:

        /*!
         * Default constructor.
         */
        ADNumber() :
        expression(new Expression<T>()),
        value(T(0.0)),
        //name(std::string("na")),
        id(IDGenerator::instance()->next()) {

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

            expression = Clone(orig.expression);
            expression->take();

        }

        ADNumber(ExpressionPtr exp) :
        value(Evaluate(exp)),
        name(exp->GetName()),
        id(IDGenerator::instance()->next()) {

            expression = exp;
            SetValue(Evaluate(exp));
            expression->take();

        }

        virtual ~ADNumber() {
            if(ADNumber<T>::IsRecordingExpression()){
                 expression->release();
            }else{
                delete this->expression;
            }
           
        }

        operator T&() {
            return this->value;
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

            return this;
        }

        //        const ADNumber<T> operator (const ADNumber<T> &val) {
        //
        //        }

        //        /**
        //         * In member assignment operator to set this 
        //         * equal to another ADNumber val.
        //         * 
        //         * @param val
        //         * @return ADNumber
        //         */

        ADNumber<T>& operator =(const ADNumber<T> &val) {
            value = val.GetValue();
            if (ADNumber<T>::IsRecordingExpression()) {
                if (this->expression != NULL) {
                    this->expression->release();
                }
                //            ExpressionPtr exp = expression;

                val.GetExpression()->take();
                expression = val.GetExpression();
                //            expression->take();

            } else {
                this->SetValue(value);
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
            if (ADNumber<T>::IsRecordingExpression()) {
                this->expression->release();
                if (expression->GetLeft() != NULL) {
                    expression->GetLeft()->release();
                }
                if (expression->GetRight() != NULL) {
                    expression->GetRight()->release();
                }
                expression->SetLeft(NULL);
                expression->SetRight(NULL);
                expression->SetValue(val);
            } else {
                this->SetValue(value);
            }

            return *this;
        }

#ifdef USE_AD_POOL

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
            if (expression == rhs.GetExpression()) {

                return ADNumber<T > (val,
                        NEW_EXPRESSION(T)(val, 0, MULTIPLY,
                        NEW_EXPRESSION(T)(2.0, 0, CONSTANT, NULL, NULL),
                        expression));
            } else {

                return ADNumber<T > (val,
                        NEW_EXPRESSION(T)(val,
                        0, "", PLUS, expression, rhs.GetExpression()));
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
                    0, MINUS, expression, rhs.GetExpression()));
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
            if (expression == rhs.GetExpression()) {

                return ADNumber<T > (val,
                        NEW_EXPRESSION(T)(val, 0, POW,
                        expression,
                        NEW_EXPRESSION(T)(2.0, 0, CONSTANT, NULL, NULL)));
            } else {

                return ADNumber<T > (val,
                        NEW_EXPRESSION(T)(val,
                        0, MULTIPLY, expression, rhs.GetExpression()));
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
                    0, DIVIDE, expression, rhs.GetExpression()));

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


#else

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

            if (ADNumber<T >::IsRecordingExpression()) {
                if (expression == rhs.GetExpression()) {

                    return ADNumber<T > (val,
                            NEW_EXPRESSION(T)(val, 0, PLUS,
                            NEW_EXPRESSION(T)(2.0, 0, CONSTANT, NULL, NULL),
                            expression));
                } else {

                    return ADNumber<T > (val,
                            NEW_EXPRESSION(T)(val,
                            0, "", PLUS, expression, rhs.GetExpression()));
                }
            } else {
                return ADNumber<T > (val);
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
            if (ADNumber<T >::IsRecordingExpression()) {
                return ADNumber<T > (val,
                        NEW_EXPRESSION(T)(val,
                        0, PLUS, expression,
                        NEW_EXPRESSION(T)(rhs, 0, CONSTANT, NULL, NULL)));
            } else {
                return ADNumber<T > (val);
            }

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

            if (ADNumber<T >::IsRecordingExpression()) {
                return ADNumber<T > (val,
                        NEW_EXPRESSION(T)(val,
                        0, MINUS, expression, rhs.GetExpression()));
            } else {
                return ADNumber<T > (val);
            }
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

            if (ADNumber<T >::IsRecordingExpression()) {
                return ADNumber<T > (val,
                        NEW_EXPRESSION(T)(val,
                        0, MINUS, expression,
                        NEW_EXPRESSION(T)(rhs, 0, CONSTANT, NULL, NULL)));
            } else {
                return ADNumber<T > (val);
            }
        }

        /*!
         * In member multiplication operator.
         * Returns ADNumber<T>(this value * rhs value,
         * this value_ * rhs derivative  + rhs value * this derivative).
         */
        const ADNumber<T> operator *(const ADNumber<T>& rhs) const {

            T val = value * rhs.value;
            if (ADNumber<T >::IsRecordingExpression()) {
                if (expression == rhs.GetExpression()) {

                    return ADNumber<T > (val,
                            NEW_EXPRESSION(T)(val, 0, POW,
                            expression,
                            NEW_EXPRESSION(T)(2.0, 0, CONSTANT, NULL, NULL)));
                } else {

                    return ADNumber<T > (val,
                            NEW_EXPRESSION(T)(val,
                            0, MULTIPLY, expression, rhs.GetExpression()));
                }
            } else {
                return ADNumber<T > (val);
            }

        }

        /*!
         * In member multiplication operator.
         * Returns ADNumber<T>(this value * rhs value,
         * this value_ * 0  + rhs value * this derivative).
         */
        const ADNumber<T> operator *(const T & rhs) const {

            T val = value * rhs;
            if (ADNumber<T >::IsRecordingExpression()) {
                return ADNumber<T > (val,
                        NEW_EXPRESSION(T)(val,
                        0, MULTIPLY, expression,
                        NEW_EXPRESSION(T)(rhs, 0, CONSTANT, NULL, NULL)));
            } else {
                return ADNumber<T > (val);
            }
        }

        /**
         * 
         * @param rhs
         * @return 
         */
        const ADNumber<T> operator /(const ADNumber<T>& rhs) const {

            T val = value / rhs.value;
            if (ADNumber<T >::IsRecordingExpression()) {
                return ADNumber<T > (val,
                        NEW_EXPRESSION(T)(val,
                        0, DIVIDE, expression, rhs.GetExpression()));
            } else {
                return ADNumber<T > (val);
            }

        }

        /*!
         * In member division operator.
         * Returns ADNumber<T>(this value * rhs value,
         * (this value_ * rhs derivative  - rhs value * 0)/(rhs value * rhs value)).
         */
        const ADNumber<T> operator /(const T & rhs) const {

            T val = value / rhs;
            if (ADNumber<T >::IsRecordingExpression()) {
                return ADNumber<T > (val,
                        NEW_EXPRESSION(T)(val,
                        0, DIVIDE, expression,
                        NEW_EXPRESSION(T)(rhs, 0, CONSTANT, NULL, NULL)));
            } else {
                return ADNumber<T > (val);
            }
        }




#endif

        /*!
         * In member addition assignment operator.
         * @param rhs
         * @return 
         */

        const ADNumber<T>& operator +=(const ADNumber<T>& rhs) {

            T val = value + rhs.value;
            if (ADNumber<T>::IsRecordingExpression()) {
                ExpressionPtr exp = NEW_EXPRESSION(T) (val, id, name, PLUS, expression, rhs.GetExpression());
                if (expression != NULL) {
                    expression->release();
                }

                expression = exp;


                expression->take();
                value = val;
                return *this;
            } else {
                this->SetValue(val);
                return *this;
            }
        }

        /*!
         * In member addition subtraction operator.
         * 
         * @param rhs
         * @return 
         */
        const ADNumber<T>& operator -=(const ADNumber<T>& rhs) {

            ExpressionPtr exp = NEW_EXPRESSION(T) (value - rhs.value, id, name, MINUS, expression, rhs.GetExpression());
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

            ExpressionPtr temp = this->expression;
            if (this->expression == rhs.GetExpression()) {
                temp = ad::Clone(this->expression);
            }
            ExpressionPtr exp = NEW_EXPRESSION(T) (value * rhs.value, id, name, MULTIPLY, temp, rhs.GetExpression());
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

            ExpressionPtr exp = NEW_EXPRESSION(T) (value / rhs.value, id, name, DIVIDE, expression, rhs.GetExpression());
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

            if (ADNumber<T>::IsRecordingExpression()) {
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

            } else {
                this->SetValue(this->GetValue() + rhs);
                return *this;
            }
        }

        /*!
         * In member subtraction assignment operator.
         * 
         * @param rhs
         * @return 
         */
        const ADNumber<T>& operator -=(const T & rhs) {

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

        /**
         * Returns the underlying expression tree for this ADNumber.
         * @return 
         */
        ExpressionPtr GetExpression() const {
            return expression;
        }

        /**
         * Sets the underlying expression tree for this ADNumber.
         * @return 
         */
        void SetExpression(ExpressionPtr expression) {
            this->expression = expression;
        }

        /*!
         * Returns the computed value.
         */
        const T GetValue() const {

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

            expression->SetName(name);
            this->name = name;
        }

        void SetValue(const T &val) {
            value = val;
            expression->SetValue(value);
        }

        void Upate() {
            this->value = ad::Evaluate(this->expression);
        }

        /*
         * Return the unique identifier for this ADNumber.
         */
        const uint32_t GetID() const {

            return id;
        }

        static void SetRecordExpression(bool record) {
            ADNumber<T>::record_expressoion = record;
        }

        static bool IsRecordingExpression() {
            return ADNumber<T>::record_expressoion;
        }

        static bool IsUsingRecursion() {
            return Expression<T>::use_recusion_m;
        }

        static void SetUseRecurion(bool use_recursion) {
            Expression<T>::use_recusion_m = use_recursion;
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
            //std::cout << ret.GetExpression()->ToString(true) << std::flush;
            ADNumber<T> ret2 = ad::Derivative(ret, var2, 1);

            ExpressionPtr exp = ret2.expression;
            //std::cout<<ad::Evaluate(ret.expression)<<"\n\n\n\n\n\n";
            //std::cout << ret.GetExpression()->ToString()<<"\n\n\n\n\n\n" << std::flush;

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

        const T NthPartialValue(const ADNumber<T> &wrt, unsigned int order) {
            return ad::DerivativeValue<T > (*this, wrt, order);
        }

        const ADNumber<T> NthPartial(const ADNumber<T> &wrt, unsigned int order) {
            return ad::Derivative<T > (*this, wrt, order);
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

#ifdef USE_AD_POOL
        // binary
        template<class TT> friend ADNumber<TT>& operator-(const ADNumber<TT>& lhs, const ADNumber<TT>& rhs);
        template<class TT> friend ADNumber<TT>& operator/(const ADNumber<TT>& lhs, const ADNumber<TT>& rhs);
        template<class TT> friend ADNumber<TT>& operator+(const ADNumber<TT>& lhs, const ADNumber<TT>& rhs);
        template<class TT> friend ADNumber<TT>& operator*(const ADNumber<TT>& lhs, const ADNumber<TT>& rhs);


        template<class TT> friend ADNumber<TT>& operator-(TT lhs, const ADNumber<TT>& rhs);
        template<class TT> friend ADNumber<TT>& operator/(TT lhs, const ADNumber<TT>& rhs);
        template<class TT> friend ADNumber<TT>& operator+(TT lhs, const ADNumber<TT>& rhs);
        template<class TT> friend ADNumber<TT>& operator*(TT lhs, const ADNumber<TT>& rhs);


        template<class TT> friend ADNumber<TT>& operator-(const ADNumber<TT>& lhs, TT rhs);
        template<class TT> friend ADNumber<TT>& operator/(const ADNumber<TT>& lhs, TT rhs);
        template<class TT> friend ADNumber<TT>& operator+(const ADNumber<TT>& lhs, TT rhs);
        template<class TT> friend ADNumber<TT>& operator*(const ADNumber<TT>& lhs, TT rhs);

#else
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
#endif
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

    template<class T>
    bool ADNumber<T>::record_expressoion = true;

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
        if (ADNumber<T>::IsRecordingExpression()) {
            ret.GetExpression()->SetOp(MINUS);
            ret.GetExpression()->SetLeft(lhs.GetExpression());
            ret.GetExpression()->SetRight(rhs.GetExpression());
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
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
        if (ADNumber<T>::IsRecordingExpression()) {
            ret.GetExpression()->SetOp(PLUS);
            ret.GetExpression()->SetLeft(lhs.GetExpression());
            ret.GetExpression()->SetRight(rhs.GetExpression());
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
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
        if (ADNumber<T>::IsRecordingExpression()) {
            ret.GetExpression()->SetOp(DIVIDE);
            ret.GetExpression()->SetLeft(lhs.GetExpression());
            ret.GetExpression()->SetRight(rhs.GetExpression());
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
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
        if (ADNumber<T>::IsRecordingExpression()) {
            ret.GetExpression()->SetOp(MULTIPLY);
            ret.GetExpression()->SetLeft(lhs.GetExpression());
            ret.GetExpression()->SetRight(rhs.GetExpression());
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
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
        if (ADNumber<T>::IsRecordingExpression()) {
            Expression<T> *exp = NEW_EXPRESSION(T) ();
            exp->SetValue(lhs);
            exp->SetOp(CONSTANT);

            ret.GetExpression()->SetOp(MINUS);
            ret.GetExpression()->SetLeft(exp);
            ret.GetExpression()->SetRight(rhs.GetExpression());
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
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
        if (ADNumber<T>::IsRecordingExpression()) {
            Expression<T> *exp = NEW_EXPRESSION(T) ();
            exp->SetValue(lhs);
            exp->SetOp(CONSTANT);

            ret.GetExpression()->SetOp(PLUS);
            ret.GetExpression()->SetLeft(exp);
            ret.GetExpression()->SetRight(rhs.GetExpression());
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
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
        if (ADNumber<T>::IsRecordingExpression()) {
            Expression<T> *exp = NEW_EXPRESSION(T) ();
            exp->SetValue(lhs);
            exp->SetOp(CONSTANT);

            ret.GetExpression()->SetOp(DIVIDE);
            ret.GetExpression()->SetLeft(exp);
            ret.GetExpression()->SetRight(rhs.GetExpression());
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
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
        if (ADNumber<T>::IsRecordingExpression()) {
            Expression<T> *exp = NEW_EXPRESSION(T) ();
            exp->SetValue(lhs);
            exp->SetOp(CONSTANT);

            ret.GetExpression()->SetOp(MULTIPLY);
            ret.GetExpression()->SetLeft(exp);
            ret.GetExpression()->SetRight(rhs.GetExpression());
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());

        }
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
        if (ADNumber<T>::IsRecordingExpression()) {
            Expression<T> *exp = NEW_EXPRESSION(T) ();
            exp->SetValue(rhs);
            exp->SetOp(CONSTANT);

            ret.GetExpression()->SetOp(MINUS);
            ret.GetExpression()->SetLeft(lhs.GetExpression());
            ret.GetExpression()->SetRight(exp);
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
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
        if (ADNumber<T>::IsRecordingExpression()) {
            Expression<T> *exp = NEW_EXPRESSION(T) ();
            exp->SetValue(rhs);
            exp->SetOp(CONSTANT);

            ret.GetExpression()->SetOp(PLUS);
            ret.GetExpression()->SetLeft(lhs.GetExpression());
            ret.GetExpression()->SetRight(exp);
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
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
        if (ADNumber<T>::IsRecordingExpression()) {
            Expression<T> *exp = NEW_EXPRESSION(T) ();
            exp->SetValue(rhs);
            exp->SetOp(CONSTANT);

            ret.GetExpression()->SetOp(DIVIDE);
            ret.GetExpression()->SetLeft(lhs.GetExpression());
            ret.GetExpression()->SetRight(exp);
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
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
        if (ADNumber<T>::IsRecordingExpression()) {
            Expression<T> *exp = NEW_EXPRESSION(T) ();
            exp->SetValue(rhs);
            exp->SetOp(CONSTANT);

            ret.GetExpression()->SetOp(MULTIPLY);
            ret.GetExpression()->SetLeft(lhs.GetExpression());
            ret.GetExpression()->SetRight(exp);
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
        return ret;
    }

    template<class T >
    std::ostream & operator<<(std::ostream &out, ADNumber<T> const &t) {
        out << t.GetValue();
        return out;
    }

    template<class T>
    static T DerivativeValue(const ad::ADNumber<T> &x, const ad::ADNumber<T> &wrt, unsigned int order) {

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


        ExpressionPtr exp = Differentiate<T > (x.GetExpression(), wrt.GetID());
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
        ret.SetValue(Evaluate(ret.GetExpression()));
        exp->release();

        return ret;

    }

    template<class T>
    void Update(ad::ADNumber<T> &var, const ad::ADNumber<T> &wrt) {
        var.GetExpression()->Update(wrt.GetID(), wrt.GetValue());
        var.Upate();
    }

    template<class T>
    void Update(ad::ADNumber<T> &var, const T &wrt) {
        var.GetExpression()->Update(wrt.GetID(), wrt);
        var.Upate();
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
        if (ad::ADNumber<T>::IsRecordingExpression()) {
            ret.GetExpression()->SetOp(ad::ATAN);
            ret.GetExpression()->SetLeft(val.GetExpression());
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
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

        ad::ADNumber<T> ret(atan2(x, y));
        if (ad::ADNumber<T>::IsRecordingExpression()) {
            ret.GetExpression()->SetOp(ad::ATAN2);
            ret.GetExpression()->SetLeft(lhs.GetExpression());
            ret.GetExpression()->SetRight(rhs.GetExpression());
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
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
        ad::ADNumber<T> ret(atan2(x, y));
        if (ad::ADNumber<T>::IsRecordingExpression()) {


            ad::Expression<T> *exp = new ad::Expression<T > ();
            exp->SetValue(lhs);
            exp->SetOp(ad::CONSTANT);

            ret.GetExpression()->SetOp(ad::ATAN2);
            ret.GetExpression()->SetLeft(exp);
            ret.GetExpression()->SetRight(rhs.GetExpression());
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
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
        ad::ADNumber<T> ret(atan2(x, y));
        if (ad::ADNumber<T>::IsRecordingExpression()) {


            ad::Expression<T> *exp = new ad::Expression<T > ();
            exp->SetValue(rhs);
            exp->SetOp(ad::CONSTANT);

            ret.GetExpression()->SetOp(ad::ATAN2);
            ret.GetExpression()->SetLeft(lhs.GetExpression());
            ret.GetExpression()->SetRight(exp);
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
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
        if (ad::ADNumber<T>::IsRecordingExpression()) {
            ret.GetExpression()->SetOp(ad::COS);
            ret.GetExpression()->SetLeft(val.GetExpression());
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
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
        if (ad::ADNumber<T>::IsRecordingExpression()) {
            ret.GetExpression()->SetOp(ad::EXP);
            ret.GetExpression()->SetLeft(val.GetExpression());
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
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
        if (ad::ADNumber<T>::IsRecordingExpression()) {
            ret.GetExpression()->SetOp(ad::LOG);
            ret.GetExpression()->SetLeft(val.GetExpression());
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
        return ret;
    }

    /*!
     * Compute natural common logarithm of val.
     * @param val
     * @return 
     */
    template<class T> const ad::ADNumber<T> log10(const ad::ADNumber<T> &val) {
        ad::ADNumber<T> ret(log10(val.GetValue()));

        if (ad::ADNumber<T>::IsRecordingExpression()) {

            ret.GetExpression()->SetOp(ad::LOG10);
            ret.GetExpression()->SetLeft(val.GetExpression());
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());

        }
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
        if (ad::ADNumber<T>::IsRecordingExpression()) {
            ret.GetExpression()->SetOp(ad::POW);
            ret.GetExpression()->SetLeft(lhs.GetExpression());
            ret.GetExpression()->SetRight(rhs.GetExpression());
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
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
        if (ad::ADNumber<T>::IsRecordingExpression()) {
            ad::ExpressionPtr exp = new ad::Expression<T > ();
            exp->SetValue(lhs);
            exp->SetOp(ad::CONSTANT);

            ret.GetExpression()->SetOp(ad::POW);
            ret.GetExpression()->SetLeft(exp);
            ret.GetExpression()->SetRight(rhs.GetExpression());
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
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
        if (ad::ADNumber<T>::IsRecordingExpression()) {
            return ad::ADNumber<T > (val,
                    NEW_EXPRESSION(T)(val,
                    0, ad::POW, lhs.GetExpression(),
                    NEW_EXPRESSION(T)(rhs, 0, ad::CONSTANT, NULL, NULL)));
        } else {
            return ad::ADNumber<T > (val);
        }
    }

    /*!
     * Returns the sine of val.
     * 
     * @param val
     * @return 
     */
    template<class T> const ad::ADNumber<T> sin(const ad::ADNumber<T> &val) {
        ad::ADNumber<T> ret(sin(val.GetValue()));
        if (ad::ADNumber<T>::IsRecordingExpression()) {
            ret.GetExpression()->SetOp(ad::SIN);
            ret.GetExpression()->SetLeft(val.GetExpression());
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
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
        if (ad::ADNumber<T>::IsRecordingExpression()) {
            ret.GetExpression()->SetOp(ad::SQRT);
            ret.GetExpression()->SetLeft(val.GetExpression());
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
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
        if (ad::ADNumber<T>::IsRecordingExpression()) {
            ret.GetExpression()->SetOp(ad::TAN);
            ret.GetExpression()->SetLeft(val.GetExpression());
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
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
        if (ad::ADNumber<T>::IsRecordingExpression()) {
            ret.GetExpression()->SetOp(ad::ACOS);
            ret.GetExpression()->SetLeft(val.GetExpression());
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
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
        if (ad::ADNumber<T>::IsRecordingExpression()) {
            ret.GetExpression()->SetOp(ad::ASIN);
            ret.GetExpression()->SetLeft(val.GetExpression());
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
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
        if (ad::ADNumber<T>::IsRecordingExpression()) {
            ret.GetExpression()->SetOp(ad::SINH);
            ret.GetExpression()->SetLeft(val.GetExpression());
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
        return ret;
    }

    /*!
     * Returns the hyperbolic cosine of val.
     * @param val
     * @return 
     */
    template<class T> const ad::ADNumber<T> cosh(const ad::ADNumber<T> &val) {
        ad::ADNumber<T> ret(cosh(val.GetValue()));
        if (ad::ADNumber<T>::IsRecordingExpression()) {
            ret.GetExpression()->SetOp(ad::COSH);
            ret.GetExpression()->SetLeft(val.GetExpression());
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
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
        if (ad::ADNumber<T>::IsRecordingExpression()) {
            ret.GetExpression()->SetOp(ad::TANH);
            ret.GetExpression()->SetLeft(val.GetExpression());
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
        return ret;
    }

    /*!
     * Compute absolute value of val.
     * @param val
     * @return 
     */
    template<class T> const ad::ADNumber<T> fabs(const ad::ADNumber<T> &val) {

        ad::ADNumber<T> ret(fabs(val.GetValue()));
        if (ad::ADNumber<T>::IsRecordingExpression()) {
            ret.GetExpression()->SetOp(ad::FABS);
            ret.GetExpression()->SetLeft(val.GetExpression());
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
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
        if (ad::ADNumber<T>::IsRecordingExpression()) {
            ret.GetExpression()->SetOp(ad::FLOOR);
            ret.GetExpression()->SetLeft(val.GetExpression());
            ret.GetExpression()->SetId(ret.GetID());
            ret.GetExpression()->SetValue(ret.GetValue());
        }
        return ret;
    }
}


#endif	/* ADNUMBER_HPP */

