#ifndef DYNAMIC_MATRIX_H
#define DYNAMIC_MATRIX_H

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

// An implementation of a dynamic 2-dimensional array
// Takes advantage of the space-initialization trick
template<typename T> class dynamic_matrix {
    public:
    typedef T value_type;
    typedef T* pointer;
    typedef T& reference;
    typedef const T* const_pointer;
    typedef const T& const_reference;

    class row_wrapper {
        public:
        reference operator[](std::size_t index);
        reference at(std::size_t index);

        private:
        row_wrapper(dynamic_matrix<T>* m, std::size_t size);

        friend class dynamic_matrix<T>;

        dynamic_matrix<T>* _matrix;
        std::size_t _index;
    };

    class const_row_wrapper {
        public:
        const_reference operator[](std::size_t index) const;
        const_reference at(std::size_t index) const;

        private:
        const_row_wrapper(const dynamic_matrix<T>* m, std::size_t size);

        friend class dynamic_matrix<T>;

        const dynamic_matrix<T>* _matrix;
        std::size_t _index;
    };

    // Generate a m * n matrix "initialized" to contain initial_value in every position
    template<typename _T = T, typename = std::enable_if_t<std::is_default_constructible_v<_T>>>
    dynamic_matrix(std::size_t m = 0, std::size_t n = 0);
    dynamic_matrix(std::size_t m, std::size_t n, const T& default_value);

    // Initializing with an initializer list
    template<typename _T = T, typename = std::enable_if_t<std::is_default_constructible_v<_T>>>
    dynamic_matrix(const std::initializer_list<std::initializer_list<T>>&);

    ~dynamic_matrix() noexcept;

    dynamic_matrix(const dynamic_matrix<T>&);
    dynamic_matrix<T>& operator=(const dynamic_matrix<T>&);

    dynamic_matrix(dynamic_matrix<T>&&);
    dynamic_matrix& operator=(dynamic_matrix<T>&&);

    // Element accessors; at() supplies bounds checking
    row_wrapper operator[](std::size_t);
    row_wrapper at(std::size_t);
    const_row_wrapper operator[](std::size_t) const;
    const_row_wrapper at(std::size_t) const;

    std::size_t num_rows() const;
    std::size_t num_cols() const;

    /*
    Return a subarray of the matrix
    First two parameters are the size of the subarray, while the last two indicate the position of
    the "top left" element If subarray goes out of bounds, pads with the default value Ex. m = 1 2 3
    4 5 6 7 8 9 1 2 3 3 4 5 7 8 9 m default value is 0

    m.subarray(3, 3, 1, 4) =
    2 3 0
    8 9 0
    0 0 0
     */
    dynamic_matrix<T> subarray(std::size_t rows, std::size_t cols, std::size_t first_row = 0,
                               std::size_t first_col = 0) const;
    // Specifies a different pad value
    dynamic_matrix<T> subarray(std::size_t rows, std::size_t cols, std::size_t first_row,
                               std::size_t first_col, const T& pad_value) const;

    // Equivalent to *this = this->subarray(new_rows, new_cols, first_row, first_col)
    void resize(std::size_t new_rows, std::size_t new_cols, std::size_t first_row = 0,
                std::size_t first_col = 0);

    /*********************
     * MATRIX OPERATIONS *
     *********************/

    // Matrix addition
    template<typename U>
    dynamic_matrix<std::invoke_result_t<std::plus<>, T, U>>
      operator+(const dynamic_matrix<U>&) const;
    template<typename U, typename _Type = T,
             typename _R = std::enable_if_t<std::is_invocable_r_v<_Type, std::plus<>, _Type, U>>>
    dynamic_matrix<T>& operator+=(const dynamic_matrix<U>&) noexcept(
      std::is_nothrow_invocable_r_v<T, std::plus<>, T, U>);

    // Matrix subtraction
    template<typename U>
    dynamic_matrix<std::invoke_result_t<std::minus<>, T, U>>
      operator-(const dynamic_matrix<U>&) const;
    template<typename U, typename _Type = T,
             typename = std::enable_if_t<std::is_invocable_r_v<_Type, std::minus<>, _Type, U>>>
    dynamic_matrix<T>& operator-=(const dynamic_matrix<U>&) noexcept(
      std::is_nothrow_invocable_r_v<T, std::minus<>, T, U>);

    // Scalar multiplication / division
    dynamic_matrix<T> operator*(const T&) const;
    dynamic_matrix<T>&
      operator*=(const T&) noexcept(std::is_nothrow_invocable_r_v<T, std::multiplies<>, T, T>);
    dynamic_matrix<T> operator/(const T&) const;
    dynamic_matrix<T>&
      operator/=(const T&) noexcept(std::is_nothrow_invocable_r_v<T, std::divides<>, T, T>);

    // Matrix multiplication
    // Requirements: T, U, and V are default constructible and return the additive identity for the
    // data type (0) Let V be the result type of T * U. V also must support addition and
    // distributive property on both T and U i.e. for any T a,b and U c,d: (a + b) * (c + d) == a *
    // c + a * d + b * c + b * d;
    template<typename U>
    dynamic_matrix<std::invoke_result_t<std::multiplies<>, T, U>>
      operator*(const dynamic_matrix<U>&) const;

    /**************
     * COMPARISON *
     **************/

    bool operator==(const dynamic_matrix<T>&) const;
#if __cplusplus <= 201703L
    bool operator!=(const dynamic_matrix<T>&) const;
#endif

    friend class row_wrapper;
    friend class const_row_wrapper;

    private:
    void _check_row(std::size_t pos) const;
    void _check_col(std::size_t pos) const;
    bool _is_initialized(std::size_t pos) const noexcept;
    void _initialize(std::size_t, const T&);

    std::allocator<T> _allocator;
    T* _elements;

    std::allocator<std::size_t> _check_allocator;
    std::size_t* _check_1;
    std::size_t* _check_2;
    std::size_t _check_size;

    T _default_value;

    std::size_t _rows, _cols;
};

#include "../../src/structures/dynamic_matrix.tpp"

#endif // DYNAMIC_MATRIX_H
