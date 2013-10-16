#ifndef ARRR_HPP
#define ARRR_HPP

#include <tuple>
#include <cmath>
#include <memory>
#include <immintrin.h>
#include <malloc.h>

namespace arrr {
    // constify is a workaround for g++ not allowing template dependent
    // integer constants in alignas.
    template<typename T>
    constexpr T constify(T value) {
        return value;
    }
#ifdef __INTEL_COMPILER
    #define ARRR_ALIGN(alignment) __attribute__ ((aligned (alignment)))
#else
    #define ARRR_ALIGN(alignment) alignas(constify(alignment))
#endif

    template<typename T>
    struct is_node {
        static const bool value = false;
    };

    #define ARITHMETIC_ARRAY_CREATE_UNARY(NAME,TAG)\
    struct TAG { };\
    template<typename T1>\
    struct is_node<std::tuple<TAG, T1>> {\
        static const bool value = true;\
    };\
    template<typename T1>\
    typename std::enable_if<is_node<T1>::value, std::tuple<TAG, const T1&>>::type\
    NAME(const T1 &a) {\
        return std::tuple<TAG, const T1&>(TAG(), a);\
    }

    #define ARITHMETIC_ARRAY_CREATE_BINARY(NAME,TAG)\
    struct TAG { };\
    template<typename T1, typename T2>\
    struct is_node<std::tuple<TAG, T1, T2>> {\
        static const bool value = true;\
    };\
    template<typename T1, typename T2>\
    typename std::enable_if<is_node<T1>::value || is_node<T2>::value, std::tuple<TAG, const T1&, const T2&>>::type\
    NAME(const T1 &a, const T2 &b) {\
        return std::tuple<TAG, const T1&, const T2&>(TAG(), a, b);\
    }

    ARITHMETIC_ARRAY_CREATE_BINARY(operator+, add_tag)
    ARITHMETIC_ARRAY_CREATE_BINARY(operator-, sub_tag)
    ARITHMETIC_ARRAY_CREATE_BINARY(operator*, mul_tag)
    ARITHMETIC_ARRAY_CREATE_BINARY(operator/, div_tag)
    ARITHMETIC_ARRAY_CREATE_BINARY(min, min_tag)
    ARITHMETIC_ARRAY_CREATE_BINARY(max, max_tag)

    ARITHMETIC_ARRAY_CREATE_UNARY(sqrt, sqrt_tag)
    ARITHMETIC_ARRAY_CREATE_UNARY(rsqrt, rsqrt_tag)
    ARITHMETIC_ARRAY_CREATE_UNARY(rcp, rcp_tag)

    #undef ARITHMETIC_ARRAY_CREATE_BINARY
    #undef ARITHMETIC_ARRAY_CREATE_UNARY
    
    struct store_tag { };
    template<typename T1, typename T2>
    struct is_node<std::tuple<store_tag, T1, T2>> {
        static const bool value = true;
    };
    template<typename T1, typename T2>
    std::tuple<store_tag, const T1&, const T2&>
    store(const T1 &a, const T2 &b) {
        return std::tuple<store_tag, const T1&, const T2&>(store_tag(), a, b);
    }

#include "instruction_sets.hpp"

    template<typename T>
    struct count;
    
    template<typename T, typename U, typename model>
    struct array_eval_t;

#include "loops.hpp"

    template<typename vector_model, typename scalar_model, typename T1>
    typename std::enable_if<is_node<T1>::value, void>::type execute(T1 expr, size_t N) {
        typedef count<T1> stats;
        loop<vector_model::registers/(stats::loads==0?1:stats::loads)>().template execute<vector_model, scalar_model>(expr, N);
    }

    template<typename T, size_t size_ = 0>
    class arithmetic_array {
    public:
        typedef T value_type;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;
        typedef T& reference;
        typedef const T& const_reference;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T* iterator;
        typedef const T* const_iterator;
        typedef vector_instruction_set<T> vector_model;
        typedef scalar_instruction_set<T> scalar_model;

        explicit arithmetic_array(T val = T()) {
            std::fill(data_, data_+size_, val);
        }

        size_type size() const { return size_; }
        pointer data() { return data_; }
        const_pointer data() const { return data_; }
        reference operator[](size_type i) { return data_[i]; }
        const_reference operator[](size_type i) const { return data_[i]; }
        iterator begin() { return data_; }
        iterator end() { return data_+size_; }
        const_iterator begin() const { return data_; }
        const_iterator end() const { return data_+size_; }
        void swap(arithmetic_array &other) { data_.swap(other.data_); std::swap(size_, other.size_); }

        arithmetic_array& operator=(const arithmetic_array &rhs) {
            execute<vector_model, scalar_model>(store(data_, rhs), size_);
            return *this;
        }
        template<typename T1>
        arithmetic_array& operator=(const T1 &rhs) {
            execute<vector_model, scalar_model>(store(static_cast<pointer>(data_), rhs), size_);
            return *this;
        }
        template<typename T1>
        arithmetic_array& operator+=(const T1 &rhs) {
            execute<vector_model, scalar_model>(store(static_cast<pointer>(data_), *this + rhs), size_);
            return *this;
        }
        template<typename T1>
        arithmetic_array& operator-=(const T1 &rhs) {
            execute<vector_model, scalar_model>(store(static_cast<pointer>(data_), *this - rhs), size_);
            return *this;
        }
        template<typename T1>
        arithmetic_array& operator*=(const T1 &rhs) {
            execute<vector_model, scalar_model>(store(static_cast<pointer>(data_), *this * rhs), size_);
            return *this;
        }
        template<typename T1>
        arithmetic_array& operator/=(const T1 &rhs) {
            execute<vector_model, scalar_model>(store(static_cast<pointer>(data_), *this / rhs), size_);
            return *this;
        }
    private:
        arithmetic_array(const arithmetic_array&) = delete;

        ARRR_ALIGN(vector_model::alignment) value_type data_[size_];
    };

    template<typename T>
    class arithmetic_array<T,0> {
    public:
        typedef T value_type;
        typedef std::size_t size_type;
        typedef std::ptrdiff_t difference_type;
        typedef T& reference;
        typedef const T& const_reference;
        typedef T* pointer;
        typedef const T* const_pointer;
        typedef T* iterator;
        typedef const T* const_iterator;
        typedef vector_instruction_set<T> vector_model;
        typedef scalar_instruction_set<T> scalar_model;

        explicit arithmetic_array(size_t size, T val = T())
        : size_(size), data_(static_cast<T*>(_mm_malloc(size_*sizeof(T), vector_model::alignment)), &_mm_free)
        {
            std::fill(data_.get(), data_.get()+size_, val);
        }

        size_type size() const { return size_; }
        pointer data() { return data_.get(); }
        const_pointer data() const { return data_.get(); }
        reference operator[](size_type i) { return data_[i]; }
        const_reference operator[](size_type i) const { return data_[i]; }
        iterator begin() { return data_.get(); }
        iterator end() { return data_.get()+size_; }
        const_iterator begin() const { return data_.get(); }
        const_iterator end() const { return data_.get()+size_; }
        void swap(arithmetic_array &other) { data_.swap(other.data_); std::swap(size_, other.size_); }

        arithmetic_array& operator=(const arithmetic_array &rhs) {
            execute<vector_model, scalar_model>(store(data_.get(), rhs), size_);
            return *this;
        }
        template<typename T1>
        arithmetic_array& operator=(const T1 &rhs) {
            execute<vector_model, scalar_model>(store(data_.get(), rhs), size_);
            return *this;
        }
        template<typename T1>
        arithmetic_array& operator+=(const T1 &rhs) {
            execute<vector_model, scalar_model>(store(data_.get(), *this + rhs), size_);
            return *this;
        }
        template<typename T1>
        arithmetic_array& operator-=(const T1 &rhs) {
            execute<vector_model, scalar_model>(store(data_.get(), *this - rhs), size_);
            return *this;
        }
        template<typename T1>
        arithmetic_array& operator*=(const T1 &rhs) {
            execute<vector_model, scalar_model>(store(data_.get(), *this * rhs), size_);
            return *this;
        }
        template<typename T1>
        arithmetic_array& operator/=(const T1 &rhs) {
            execute<vector_model, scalar_model>(store(data_.get(), *this / rhs), size_);
            return *this;
        }
    private:
        arithmetic_array(const arithmetic_array&) = delete;

        const size_type size_;
        std::unique_ptr<value_type[],void(*)(void*)> data_;
    };

    template<typename T1, size_t N>
    struct is_node<arithmetic_array<T1, N>> {
        static const bool value = true;
    };


    template<typename T>
    struct count {
        static const int loads = 0;
        static const int stores = 0;
        static const int operations = 0;
        static const int immediates = 1;
    };
    template<typename T>
    struct count<arithmetic_array<T>> {
        static const int loads = 1;
        static const int stores = 0;
        static const int operations = 0;
        static const int immediates = 0;
    };
    template<typename T1, typename T2>
    struct count<std::tuple<store_tag, const T1&, const T2&>> {
        static const int loads = count<T2>::loads;
        static const int stores = count<T2>::stores + 1;
        static const int operations = count<T2>::operations;
        static const int immediates = count<T2>::immediates;
    };
    template<typename tag, typename T1>
    struct count<std::tuple<tag, const T1&>> {
        static const int loads = count<T1>::loads;
        static const int stores = count<T1>::stores;
        static const int operations = count<T1>::operations+1;
        static const int immediates = count<T1>::immediates;
    };
    template<typename tag, typename T1, typename T2>
    struct count<std::tuple<tag, const T1&, const T2&>> {
        static const int loads = count<T1>::loads+count<T2>::loads;
        static const int stores = count<T1>::stores+count<T2>::stores;
        static const int operations = count<T1>::operations+count<T2>::operations+1;
        static const int immediates = count<T1>::immediates+count<T2>::immediates;
    };

    template<typename T, typename U, typename model>
    struct array_eval_t {
        typedef typename model::pack_type return_type;
        return_type tmp;
        void prepare(const T &node) { tmp = model::set(node); }
        void load(const T &, const U&) { }
        void store(const T &, const U&) { }
        return_type operator()(const T &node, const U&) {
            return tmp;
        }
    };


    template<typename T1, size_t N, typename U, typename model>
    struct array_eval_t<arithmetic_array<T1,N>,U,model> {
        typedef typename model::pack_type return_type;
        return_type tmp;
        typename arithmetic_array<T1,N>::const_pointer ptr;
        void prepare(const arithmetic_array<T1,N> &node) { ptr = node.data(); }
        void load(const arithmetic_array<T1,N> &node, const U &userdata) { tmp = model::load(ptr, userdata); }
        void store(const arithmetic_array<T1,N> &, const U &) { }
        return_type operator()(const arithmetic_array<T1,N> &, const U &) {
            return tmp;
        }
    };

    template<typename T1, typename T2, typename U, typename model>
    struct array_eval_t<std::tuple<store_tag, const T1&, const T2&>,U,model> {
        typedef typename model::pack_type return_type;
        return_type tmp;
        T1 ptr;

        array_eval_t<T2,U,model> right;

        void prepare(const std::tuple<store_tag, const T1&, const T2&> &node) {
            right.prepare(std::get<2>(node));
            ptr = std::get<1>(node);
        }
        void load(const std::tuple<store_tag, const T1&, const T2&> &node, const U& userdata) {
            right.load(std::get<2>(node), userdata);
        }
        void store(const std::tuple<store_tag, const T1&, const T2&> &node, const U& userdata) {
            right.store(std::get<2>(node), userdata);
            model::store(ptr, userdata, tmp);
        }
        return_type operator()(const std::tuple<store_tag, const T1&, const T2&> &node, const U& userdata) {
            return tmp = right(std::get<2>(node), userdata);
        }
    };


    template<typename tag, typename T1, typename U, typename model>
    struct array_eval_t<std::tuple<tag, const T1&>,U,model> {
        typedef typename model::pack_type return_type;
        array_eval_t<T1,U,model> child;

        void prepare(const std::tuple<tag, const T1&> &node) {
            child.prepare(std::get<1>(node));
        }
        void load(const std::tuple<tag, const T1&> &node, const U& userdata) {
            child.load(std::get<1>(node), userdata);
        }
        void store(const std::tuple<tag, const T1&> &node, const U& userdata) {
            child.store(std::get<1>(node), userdata);
        }
        return_type operator()(const std::tuple<tag, const T1&> &node, const U& userdata) {
            return model::template unary<tag>(
                child(std::get<1>(node), userdata)
            );
        }
    };

    template<typename tag, typename T1, typename T2, typename U, typename model>
    struct array_eval_t<std::tuple<tag, const T1&, const T2&>,U,model> {
        typedef typename model::pack_type return_type;
        array_eval_t<T1,U,model> left;
        array_eval_t<T2,U,model> right;

        void prepare(const std::tuple<tag, const T1&, const T2&> &node) {
            left.prepare(std::get<1>(node));
            right.prepare(std::get<2>(node));
        }
        void load(const std::tuple<tag, const T1&, const T2&> &node, const U& userdata) {
            left.load(std::get<1>(node), userdata);
            right.load(std::get<2>(node), userdata);
        }
        void store(const std::tuple<tag, const T1&, const T2&> &node, const U& userdata) {
            left.store(std::get<1>(node), userdata);
            right.store(std::get<2>(node), userdata);
        }
        return_type operator()(const std::tuple<tag, const T1&, const T2&> &node, const U& userdata) {
            return model::template binary<tag>(
                left(std::get<1>(node), userdata),
                right(std::get<2>(node), userdata)
            );
        }
    };
    #undef ARRR_ALIGN
}

#endif
