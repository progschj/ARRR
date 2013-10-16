
template<typename T>
struct scalar_instruction_set {
    typedef T value_type;
    typedef T pack_type;
    static const size_t pack_size = 1;
    static const size_t alignment = 16;
    static const size_t registers = 8;
    
    template<typename T2>
    static pack_type set(T2 value) { return value; }
    static pack_type load(const value_type *ptr, size_t index) { return ptr[index]; }
    static pack_type store(value_type *ptr, size_t index, pack_type val) { ptr[index] = val; return val; }
    static pack_type stream(value_type *ptr, size_t index, pack_type val) { ptr[index] = val; return val; }
    
    template<typename T2, typename tag> struct unary_op { };
    template<typename T2> struct unary_op<T2,sqrt_tag> { T2 operator()(T2 a) { return std::sqrt(a); } };
    template<typename T2> struct unary_op<T2,rsqrt_tag> { T2 operator()(T2 a) { return value_type(1)/std::sqrt(a); } };
    template<typename T2> struct unary_op<T2,rcp_tag> { T2 operator()(T2 a) { return value_type(1)/a; } };
    
    template<class tag>
    static pack_type unary(pack_type a) { return unary_op<pack_type,tag>()(a); }
    
    template<typename T2, typename tag> struct binary_op { };
    template<typename T2> struct binary_op<T2,add_tag> { T2 operator()(T2 a, T2 b) { return a+b; } };
    template<typename T2> struct binary_op<T2,sub_tag> { T2 operator()(T2 a, T2 b) { return a-b; } };
    template<typename T2> struct binary_op<T2,mul_tag> { T2 operator()(T2 a, T2 b) { return a*b; } };
    template<typename T2> struct binary_op<T2,div_tag> { T2 operator()(T2 a, T2 b) { return a/b; } };
    template<typename T2> struct binary_op<T2,min_tag> { T2 operator()(T2 a, T2 b) { return a<b?a:b; } };
    template<typename T2> struct binary_op<T2,max_tag> { T2 operator()(T2 a, T2 b) { return a>b?a:b; } };
    
    template<class tag>
    static pack_type binary(pack_type a, pack_type b) { return binary_op<pack_type,tag>()(a,b); }
};

template<typename T>
struct vector_instruction_set : public scalar_instruction_set<T> { };

#if defined(__AVX__)
template<>
struct vector_instruction_set<float> {
    typedef float value_type;
    typedef __m256 pack_type;
    static const size_t pack_size = 8;
    static const size_t alignment = 32;
    static const size_t registers = 16;
    
    template<typename T2>
    static pack_type set(T2 value) { return _mm256_set1_ps(value); }
    static pack_type load(const value_type *ptr, size_t index) { return _mm256_load_ps(ptr+index); }
    static pack_type store(value_type *ptr, size_t index, pack_type val) { _mm256_store_ps(ptr+index, val); return val; }
    static pack_type stream(value_type *ptr, size_t index, pack_type val) { _mm256_stream_ps(ptr+index, val); return val; }
    
    template<typename T2, typename tag> struct unary_op { };
    template<typename T2> struct unary_op<T2,sqrt_tag> { T2 operator()(T2 a) { return _mm256_sqrt_ps(a); } };
    template<typename T2> struct unary_op<T2,rsqrt_tag> { T2 operator()(T2 a) { return _mm256_rsqrt_ps(a); } };
    template<typename T2> struct unary_op<T2,rcp_tag> { T2 operator()(T2 a) { return _mm256_rcp_ps(a); } };
    
    template<class tag>
    static pack_type unary(pack_type a) { return unary_op<pack_type,tag>()(a); }
    
    template<typename T2, typename tag> struct binary_op { };
    template<typename T2> struct binary_op<T2,add_tag> { T2 operator()(T2 a, T2 b) { return _mm256_add_ps(a, b); } };
    template<typename T2> struct binary_op<T2,sub_tag> { T2 operator()(T2 a, T2 b) { return _mm256_sub_ps(a, b); } };
    template<typename T2> struct binary_op<T2,mul_tag> { T2 operator()(T2 a, T2 b) { return _mm256_mul_ps(a, b); } };
    template<typename T2> struct binary_op<T2,div_tag> { T2 operator()(T2 a, T2 b) { return _mm256_div_ps(a, b); } };
    template<typename T2> struct binary_op<T2,min_tag> { T2 operator()(T2 a, T2 b) { return _mm256_min_ps(a, b); } };
    template<typename T2> struct binary_op<T2,max_tag> { T2 operator()(T2 a, T2 b) { return _mm256_max_ps(a, b); } };
    
    template<class tag>
    static pack_type binary(pack_type a, pack_type b) { return binary_op<pack_type,tag>()(a,b); }
};

template<>
struct vector_instruction_set<double> {
    typedef double value_type;
    typedef __m256d pack_type;
    static const size_t pack_size = 4;
    static const size_t alignment = 32;
    static const size_t registers = 16;
    
    template<typename T2>
    static pack_type set(T2 value) { return _mm256_set1_pd(value); }
    static pack_type load(const value_type *ptr, size_t index) { return _mm256_load_pd(ptr+index); }
    static pack_type store(value_type *ptr, size_t index, pack_type val) { _mm256_store_pd(ptr+index, val); return val; }
    static pack_type stream(value_type *ptr, size_t index, pack_type val) { _mm256_stream_pd(ptr+index, val); return val; }
    
    template<typename T2, typename tag> struct unary_op { };
    template<typename T2> struct unary_op<T2,sqrt_tag> { T2 operator()(T2 a) { return _mm256_sqrt_pd(a); } };
    template<typename T2> struct unary_op<T2,rsqrt_tag> { T2 operator()(T2 a) { return _mm256_rsqrt_pd(a); } };
    template<typename T2> struct unary_op<T2,rcp_tag> { T2 operator()(T2 a) { return _mm256_rcp_pd(a); } };
    
    template<class tag>
    static pack_type unary(pack_type a) { return unary_op<pack_type,tag>()(a); }
    
    template<typename T2, typename tag> struct binary_op { };
    template<typename T2> struct binary_op<T2,add_tag> { T2 operator()(T2 a, T2 b) { return _mm256_add_pd(a, b); } };
    template<typename T2> struct binary_op<T2,sub_tag> { T2 operator()(T2 a, T2 b) { return _mm256_sub_pd(a, b); } };
    template<typename T2> struct binary_op<T2,mul_tag> { T2 operator()(T2 a, T2 b) { return _mm256_mul_pd(a, b); } };
    template<typename T2> struct binary_op<T2,div_tag> { T2 operator()(T2 a, T2 b) { return _mm256_div_pd(a, b); } };
    template<typename T2> struct binary_op<T2,min_tag> { T2 operator()(T2 a, T2 b) { return _mm256_min_pd(a, b); } };
    template<typename T2> struct binary_op<T2,max_tag> { T2 operator()(T2 a, T2 b) { return _mm256_max_pd(a, b); } };
    
    template<class tag>
    static pack_type binary(pack_type a, pack_type b) { return binary_op<pack_type,tag>()(a,b); }
};
#elif defined(__SSE2__)
template<>
struct vector_instruction_set<float> {
    typedef float value_type;
    typedef __m128 pack_type;
    static const size_t pack_size = 4;
    static const size_t alignment = 16;
    static const size_t registers = 8;
    
    template<typename T2>
    static pack_type set(T2 value) { return _mm_set1_ps(value); }
    static pack_type load(const value_type *ptr, size_t index) { return _mm_load_ps(ptr+index); }
    static pack_type store(value_type *ptr, size_t index, pack_type val) { _mm_store_ps(ptr+index, val); return val; }
    static pack_type stream(value_type *ptr, size_t index, pack_type val) { _mm_stream_ps(ptr+index, val); return val; }
    
    template<typename T2, typename tag> struct unary_op { };
    template<typename T2> struct unary_op<T2,sqrt_tag> { T2 operator()(T2 a) { return _mm_sqrt_ps(a); } };
    template<typename T2> struct unary_op<T2,rsqrt_tag> { T2 operator()(T2 a) { return _mm_rsqrt_ps(a); } };
    template<typename T2> struct unary_op<T2,rcp_tag> { T2 operator()(T2 a) { return _mm_rcp_ps(a); } };
    
    template<class tag>
    static pack_type unary(pack_type a) { return unary_op<pack_type,tag>()(a); }
    
    template<typename T2, typename tag> struct binary_op { };
    template<typename T2> struct binary_op<T2,add_tag> { T2 operator()(T2 a, T2 b) { return _mm_add_ps(a, b); } };
    template<typename T2> struct binary_op<T2,sub_tag> { T2 operator()(T2 a, T2 b) { return _mm_sub_ps(a, b); } };
    template<typename T2> struct binary_op<T2,mul_tag> { T2 operator()(T2 a, T2 b) { return _mm_mul_ps(a, b); } };
    template<typename T2> struct binary_op<T2,div_tag> { T2 operator()(T2 a, T2 b) { return _mm_div_ps(a, b); } };
    template<typename T2> struct binary_op<T2,min_tag> { T2 operator()(T2 a, T2 b) { return _mm_min_ps(a, b); } };
    template<typename T2> struct binary_op<T2,max_tag> { T2 operator()(T2 a, T2 b) { return _mm_max_ps(a, b); } };
    
    template<class tag>
    static pack_type binary(pack_type a, pack_type b) { return binary_op<pack_type,tag>()(a,b); }
};

template<>
struct vector_instruction_set<double> {
    typedef double value_type;
    typedef __m128d pack_type;
    static const size_t pack_size = 2;
    static const size_t alignment = 16;
    static const size_t registers = 8;
    
    template<typename T2>
    static pack_type set(T2 value) { return _mm_set1_pd(value); }
    static pack_type load(const value_type *ptr, size_t index) { return _mm_load_pd(ptr+index); }
    static pack_type store(value_type *ptr, size_t index, pack_type val) { _mm_store_pd(ptr+index, val); return val; }
    static pack_type stream(value_type *ptr, size_t index, pack_type val) { _mm_stream_pd(ptr+index, val); return val; }
    
    template<typename T2, typename tag> struct unary_op { };
    template<typename T2> struct unary_op<T2,sqrt_tag> { T2 operator()(T2 a) { return _mm_sqrt_pd(a); } };
    template<typename T2> struct unary_op<T2,rsqrt_tag> { T2 operator()(T2 a) { return _mm_rsqrt_pd(a); } };
    template<typename T2> struct unary_op<T2,rcp_tag> { T2 operator()(T2 a) { return _mm_rcp_pd(a); } };
    
    template<class tag>
    static pack_type unary(pack_type a) { return unary_op<pack_type,tag>()(a); }
            
    template<typename T2, typename tag> struct binary_op { };
    template<typename T2> struct binary_op<T2,add_tag> { T2 operator()(T2 a, T2 b) { return _mm_add_pd(a, b); } };
    template<typename T2> struct binary_op<T2,sub_tag> { T2 operator()(T2 a, T2 b) { return _mm_sub_pd(a, b); } };
    template<typename T2> struct binary_op<T2,mul_tag> { T2 operator()(T2 a, T2 b) { return _mm_mul_pd(a, b); } };
    template<typename T2> struct binary_op<T2,div_tag> { T2 operator()(T2 a, T2 b) { return _mm_div_pd(a, b); } };
    template<typename T2> struct binary_op<T2,min_tag> { T2 operator()(T2 a, T2 b) { return _mm_min_pd(a, b); } };
    template<typename T2> struct binary_op<T2,max_tag> { T2 operator()(T2 a, T2 b) { return _mm_max_pd(a, b); } };
    
    template<class tag>
    static pack_type binary(pack_type a, pack_type b) { return binary_op<pack_type,tag>()(a,b); }
};
#endif
