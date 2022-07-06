#ifndef DYNAMIC_MATRIX_TPP
#define DYNAMIC_MATRIX_TPP

#include <stdexcept>
#include <string>

template<typename T>
typename dynamic_matrix<T>::reference
  dynamic_matrix<T>::row_wrapper::operator[](std::size_t index) {
    std::size_t pos = _index * _matrix->_cols + index;
    if (!_matrix->_is_initialized(pos))
        _matrix->_initialize(pos, _matrix->_default_value);
    return _matrix->_elements[pos];
}

template<typename T>
typename dynamic_matrix<T>::reference dynamic_matrix<T>::row_wrapper::at(std::size_t index) {
    _matrix->_check_col(index);
    std::size_t pos = _index * _matrix->_cols + index;
    if (!_matrix->_is_initialized(pos))
        _matrix->_initialize(pos, _matrix->_default_value);
    return _matrix->_elements[pos];
}

template<typename T>
dynamic_matrix<T>::row_wrapper::row_wrapper(dynamic_matrix<T>* m, std::size_t index) :
    _matrix(m), _index(index) {}

template<typename T>
typename dynamic_matrix<T>::const_reference
  dynamic_matrix<T>::const_row_wrapper::operator[](std::size_t index) const {
    std::size_t pos = _index * _matrix->_cols + index;
    return _matrix->_is_initialized(pos) ? _matrix->_elements[pos] : _matrix->_default_value;
}

template<typename T>
typename dynamic_matrix<T>::const_reference
  dynamic_matrix<T>::const_row_wrapper::at(std::size_t index) const {
    _matrix->_check_col(index);
    std::size_t pos = _index * _matrix->_cols + index;
    return _matrix->_is_initialized(pos) ? _matrix->_elements[pos] : _matrix->_default_value;
}

template<typename T>
dynamic_matrix<T>::const_row_wrapper::const_row_wrapper(const dynamic_matrix<T>* m,
                                                        std::size_t index) :
    _matrix(m), _index(index) {}

template<typename T>
template<typename, typename>
dynamic_matrix<T>::dynamic_matrix(std::size_t m, std::size_t n) : dynamic_matrix<T>(m, n, T()) {}

template<typename T>
dynamic_matrix<T>::dynamic_matrix(std::size_t m, std::size_t n, const T& default_value) :
    _allocator(),
    _check_allocator(),
    _check_size(0U),
    _default_value(default_value),
    _rows(m),
    _cols(n) {
    // out of bounds error
    std::size_t size = m * n;
    if (m != 0 && n != 0 && (size / m != n || size / n != m))
        throw std::overflow_error(std::to_string(m) + std::to_string(n));

    if (size == 0) {
        _elements = nullptr;
        _check_1 = nullptr;
        _check_2 = nullptr;
        return;
    }

    uint8_t tracker = 0;
    try {
        _elements = _allocator.allocate(size);
        ++tracker;
        _check_1 = _check_allocator.allocate(size);
        ++tracker;
        _check_2 = _check_allocator.allocate(size);
    } catch (...) {
        if (tracker == 2)
            _check_allocator.deallocate(_check_1, size);
        if (tracker >= 1)
            _allocator.deallocate(_elements, size);
        throw;
    }
}

template<typename T>
template<typename, typename>
dynamic_matrix<T>::dynamic_matrix(const std::initializer_list<std::initializer_list<T>>& src) :
    dynamic_matrix<T>(src.size(),
                      std::max(src, [](auto x, auto y) { return x.size() < y.size(); }).size()) {
    std::size_t row_num = 0;
    for (const std::initializer_list<T>& r : src) {
        std::size_t col_num = 0;
        for (const T& x : r) {
            (*this)[row_num][col_num] = x;
            ++col_num;
        }
        ++row_num;
    }
}

template<typename T> dynamic_matrix<T>::~dynamic_matrix() noexcept {
    for (std::size_t i = 0U; i < _check_size; ++i)
        std::destroy_at(_elements + _check_2[i]);
    std::size_t size = _rows * _cols;
    if (size != 0) {
        _check_allocator.deallocate(_check_2, size);
        _check_allocator.deallocate(_check_1, size);
        _allocator.deallocate(_elements, size);
    }
}

template<typename T>
dynamic_matrix<T>::dynamic_matrix(const dynamic_matrix<T>& src) :
    _allocator(),
    _check_allocator(),
    _check_size(0U),
    _default_value(src._default_value),
    _rows(src._rows),
    _cols(src._cols) {
    std::size_t size = src._rows * src._cols;

    if (size == 0) {
        _elements = nullptr;
        _check_1 = nullptr;
        _check_2 = nullptr;
        return;
    }

    uint8_t tracker = 0;
    try {
        _elements = _allocator.allocate(size);
        ++tracker;
        _check_1 = _check_allocator.allocate(size);
        ++tracker;
        _check_2 = _check_allocator.allocate(size);
        ++tracker;

        while (_check_size < src._check_size) {
            std::size_t next = src._check_2[_check_size];
            _initialize(next, src._elements[next]);
        }

    } catch (...) {
        for (std::size_t i = 0U; i < _check_size; ++i)
            std::destroy_at(_elements + _check_2[i]);

        if (tracker == 3)
            _check_allocator.deallocate(_check_2, size);
        if (tracker >= 2)
            _check_allocator.deallocate(_check_1, size);
        if (tracker >= 1)
            _allocator.deallocate(_elements, size);
        throw;
    }
}

template<typename T> dynamic_matrix<T>& dynamic_matrix<T>::operator=(const dynamic_matrix<T>& rhs) {
    if (this != &rhs) {
        dynamic_matrix<T> temp(rhs);
        *this = std::move(temp);
    }
    return *this;
}

template<typename T>
dynamic_matrix<T>::dynamic_matrix(dynamic_matrix<T>&& src) :
    _allocator(std::move(src._allocator)),
    _elements(std::move(src._elements)),
    _check_allocator(std::move(src._check_allocator)),
    _check_1(std::move(src._check_1)),
    _check_2(std::move(src._check_2)),
    _check_size(std::move(src._check_size)),
    _default_value(std::move(src._default_value)),
    _rows(std::move(src._rows)),
    _cols(std::move(src._cols)) {
    src._rows = src._cols = src._check_size = 0;
    src._elements = nullptr;
    src._check_1 = src._check_2 = nullptr;
}

template<typename T> dynamic_matrix<T>& dynamic_matrix<T>::operator=(dynamic_matrix<T>&& rhs) {
    if (this != &rhs) {
        this->~dynamic_matrix();
        _rows = _cols = _check_size = 0;
        _elements = nullptr;
        _check_1 = _check_2 = nullptr;

        _allocator = std::move(rhs._allocator);
        std::swap(_elements, rhs._elements);
        _check_allocator = std::move(rhs._check_allocator);
        std::swap(_check_1, rhs._check_1);
        std::swap(_check_2, rhs._check_2);
        std::swap(_check_size, rhs._check_size);
        _default_value = std::move(rhs._default_value);
        std::swap(_rows, rhs._rows);
        std::swap(_cols, rhs._cols);
    }
    return *this;
}

template<typename T>
typename dynamic_matrix<T>::row_wrapper dynamic_matrix<T>::operator[](std::size_t pos) {
    return row_wrapper(this, pos);
}

template<typename T>
typename dynamic_matrix<T>::row_wrapper dynamic_matrix<T>::at(std::size_t pos) {
    _check_row(pos);
    return operator[](pos);
}

template<typename T>
typename dynamic_matrix<T>::const_row_wrapper dynamic_matrix<T>::operator[](std::size_t pos) const {
    return const_row_wrapper(this, pos);
}

template<typename T>
typename dynamic_matrix<T>::const_row_wrapper dynamic_matrix<T>::at(std::size_t pos) const {
    _check_row(pos);
    return operator[](pos);
}

template<typename T> std::size_t dynamic_matrix<T>::num_rows() const {
    return _rows;
}

template<typename T> std::size_t dynamic_matrix<T>::num_cols() const {
    return _cols;
}

template<typename T>
dynamic_matrix<T> dynamic_matrix<T>::subarray(std::size_t rows, std::size_t cols,
                                              std::size_t first_row, std::size_t first_col) const {
    return subarray(rows, cols, first_row, first_col, _default_value);
}

template<typename T>
dynamic_matrix<T> dynamic_matrix<T>::subarray(std::size_t rows, std::size_t cols,
                                              std::size_t first_row, std::size_t first_col,
                                              const T& pad_value) const {
    _check_row(first_row);
    _check_col(first_col);

    std::size_t num_filled_rows = std::min(rows, _rows - first_row);
    std::size_t num_filled_cols = std::min(cols, _cols - first_col);
    dynamic_matrix<T> result(rows, cols);

    if (2 * num_filled_rows * num_filled_cols <= rows * cols && pad_value != _default_value) {
        result._default_value = pad_value;
        for (std::size_t i = first_row; i - first_row < num_filled_rows; ++i)
            for (std::size_t j = first_col; j - first_col < num_filled_cols; ++j)
                result[i - first_row][j - first_col] = (*this)[i][j];
    } else {
        result._default_value = _default_value;
        for (std::size_t i = first_row; i - first_row < num_filled_rows; ++i)
            for (std::size_t j = first_col; j - first_col < num_filled_cols; ++j)
                if (_is_initialized(i * _cols + j))
                    result[i - first_row][j - first_col] = (*this)[i][j];
        if (pad_value != _default_value) {
            for (std::size_t i = num_filled_rows; i < rows; ++i)
                for (std::size_t j = 0; j < cols; ++j)
                    result[i][j] = pad_value;

            for (std::size_t i = 0; i < num_filled_rows; ++i)
                for (std::size_t j = num_filled_cols; j < cols; ++j)
                    result[i][j] = pad_value;
        }
    }

    return result;
}

template<typename T>
void dynamic_matrix<T>::resize(std::size_t rows, std::size_t cols, std::size_t first_row,
                               std::size_t first_col) {
    *this = subarray(rows, cols, first_row, first_col);
}

template<typename T>
template<typename U>
dynamic_matrix<std::invoke_result_t<std::plus<>, T, U>>
  dynamic_matrix<T>::operator+(const dynamic_matrix<U>& rhs) const {
    if (_rows != rhs._rows || _cols != rhs._cols)
        throw std::invalid_argument("Dimension mismatch");

    dynamic_matrix<std::invoke_result_t<std::plus<>, T, U>> result(_rows, _cols);
    for (std::size_t i = 0; i < _rows; ++i)
        for (std::size_t j = 0; j < _cols; ++j)
            result[i][j] = (*this)[i][j] + rhs[i][j];

    return result;
}

template<typename T>
template<typename U, typename, typename>
dynamic_matrix<T>& dynamic_matrix<T>::operator+=(const dynamic_matrix<U>& rhs) noexcept(
  std::is_nothrow_invocable_r_v<T, std::plus<>, T, U>) {
    if (_rows != rhs._rows || _cols != rhs._cols)
        throw std::invalid_argument("Dimension mismatch");

    if constexpr (std::is_nothrow_invocable_r_v<T, std::plus<>, T, U>) {
        for (std::size_t i = 0; i < _rows; ++i)
            for (std::size_t j = 0; j < _cols; ++j)
                (*this)[i][j] = (*this)[i][j] + rhs[i][j];
    } else {
        dynamic_matrix<T> temp(*this);
        for (std::size_t i = 0; i < _rows; ++i)
            for (std::size_t j = 0; j < _cols; ++j)
                temp[i][j] = temp[i][j] + rhs[i][j];
        *this = std::move(temp);
    }

    return *this;
}

template<typename T>
template<typename U>
dynamic_matrix<std::invoke_result_t<std::minus<>, T, U>>
  dynamic_matrix<T>::operator-(const dynamic_matrix<U>& rhs) const {
    if (_rows != rhs._rows || _cols != rhs._cols)
        throw std::invalid_argument("Dimension mismatch");

    dynamic_matrix<std::invoke_result_t<std::minus<>, T, U>> result(_rows, _cols);
    for (std::size_t i = 0; i < _rows; ++i)
        for (std::size_t j = 0; j < _cols; ++j)
            result[i][j] = (*this)[i][j] - rhs[i][j];

    return result;
}

template<typename T>
template<typename U, typename, typename>
dynamic_matrix<T>& dynamic_matrix<T>::operator-=(const dynamic_matrix<U>& rhs) noexcept(
  std::is_nothrow_invocable_r_v<T, std::minus<>, T, U>) {
    if (_rows != rhs._rows || _cols != rhs._cols)
        throw std::invalid_argument("Dimension mismatch");

    if constexpr (std::is_nothrow_invocable_r_v<T, std::minus<>, T, U>) {
        for (std::size_t i = 0; i < _rows; ++i)
            for (std::size_t j = 0; j < _cols; ++j)
                (*this)[i][j] = (*this)[i][j] - rhs[i][j];
    } else {
        dynamic_matrix<T> temp(*this);
        for (std::size_t i = 0; i < _rows; ++i)
            for (std::size_t j = 0; j < _cols; ++j)
                temp[i][j] = temp[i][j] - rhs[i][j];
        *this = std::move(temp);
    }

    return *this;
}

template<typename T> dynamic_matrix<T> dynamic_matrix<T>::operator*(const T& rhs) const {
    if constexpr (std::is_nothrow_invocable_r_v<T, std::multiplies<>, T, T>) {
        dynamic_matrix<T> temp(*this);
        temp *= rhs;
        return temp;
    } else {
        dynamic_matrix<T> result(*this);
        result._default_value = result._default_value * rhs;
        for (std::size_t i = 0; i < _check_size; ++i)
            result._elements[_check_2[i]] = result._elements[_check_2[i]] * _default_value;
        return result;
    }
}

template<typename T>
dynamic_matrix<T>& dynamic_matrix<T>::operator*=(const T& rhs) noexcept(
  std::is_nothrow_invocable_r_v<T, std::multiplies<>, T, T>) {
    if constexpr (std::is_nothrow_invocable_r_v<T, std::multiplies<>, T, T>) {
        _default_value = _default_value * rhs;
        for (std::size_t i = 0; i < _check_size; ++i)
            _elements[_check_2[i]] = _elements[_check_2[i]] * _default_value;
    } else {
        *this = *this * rhs;
    }
    return *this;
}

template<typename T> dynamic_matrix<T> dynamic_matrix<T>::operator/(const T& rhs) const {
    if constexpr (std::is_nothrow_invocable_r_v<T, std::divides<>, T, T>) {
        dynamic_matrix<T> temp(*this);
        temp /= rhs;
        return temp;
    } else {
        dynamic_matrix<T> result(*this);
        result._default_value = result._default_value / rhs;
        for (std::size_t i = 0; i < _check_size; ++i)
            result._elements[_check_2[i]] = result._elements[_check_2[i]] / _default_value;
        return result;
    }
}

template<typename T>
dynamic_matrix<T>& dynamic_matrix<T>::operator/=(const T& rhs) noexcept(
  std::is_nothrow_invocable_r_v<T, std::divides<>, T, T>) {
    if constexpr (std::is_nothrow_invocable_r_v<T, std::divides<>, T, T>) {
        _default_value = _default_value / rhs;
        for (std::size_t i = 0; i < _check_size; ++i)
            _elements[_check_2[i]] = _elements[_check_2[i]] / _default_value;
    } else {
        *this = *this / rhs;
    }
    return *this;
}

template<typename T>
template<typename U>
dynamic_matrix<std::invoke_result_t<std::multiplies<>, T, U>>
  dynamic_matrix<T>::operator*(const dynamic_matrix<U>& rhs) const {
    if (_cols != rhs._rows)
        throw std::invalid_argument("Dimension mismatch");
    static const std::size_t CUTOFF = 12;
    typedef std::invoke_result_t<std::multiplies<>, T, U> result_t;

    dynamic_matrix<result_t> result(_rows, rhs._cols);

    // For small matrices, switch to brute force
    if (_rows < CUTOFF && _cols < CUTOFF && rhs._cols < CUTOFF) {
        for (std::size_t i = 0; i < _rows; ++i) {
            for (std::size_t k = 0; k < rhs._cols; ++k) {
                result_t current = result_t();
                for (std::size_t j = 0; j < _cols; ++j) {
                    current = current + (*this)[i][j] * rhs[j][k];
                }
                result[i][k] = current;
            }
        }
        return result;
    }

    // If highly non-square, break into more square-like matrices and recurse
    std::size_t dimensions[3] = {_rows, _cols, rhs._cols};
    std::sort(dimensions, dimensions + 3);
    if (dimensions[1] >= 2 * dimensions[0] ||
        (dimensions[1] == dimensions[0] && dimensions[2] >= 2 * dimensions[0])) {
        // Case 1: skinny LHS
        if (dimensions[0] == _rows) {
            std::size_t num_large_cols(_cols / _rows), num_rhs_large_cols(rhs._cols / _rows);
            if (_cols % _rows != 0)
                ++num_large_cols;
            if (rhs._cols % _rows != 0)
                ++num_rhs_large_cols;

            std::vector<dynamic_matrix<T>> lhs_sub(num_large_cols);
            for (std::size_t i = 0; i < num_large_cols; ++i)
                lhs_sub[i] = this->subarray(_rows, _rows, 0, i * _rows, T());

            dynamic_matrix<dynamic_matrix<U>> rhs_sub(num_large_cols, num_rhs_large_cols);
            for (std::size_t i = 0; i < num_large_cols; ++i)
                for (std::size_t j = 0; j < num_rhs_large_cols; ++j)
                    rhs_sub[i][j] = rhs.subarray(_rows, _rows, i * _rows, j * _rows, U());

            for (std::size_t i = 0; i < num_rhs_large_cols; ++i) {
                dynamic_matrix<result_t> subresult(_rows, _rows);
                for (std::size_t j = 0; j < num_large_cols; ++j)
                    subresult += lhs_sub[j] * rhs_sub[j][i];

                std::size_t col_bound = std::min(rhs._cols - i * _rows, _rows);
                for (std::size_t j = 0; j < _rows; ++j)
                    for (std::size_t k = 0; k < col_bound; ++k)
                        result[j][i * _rows + k] = subresult[j][k];
            }
            return result;
        }
        // Case 2: Lots of small multiplications into a large result
        if (dimensions[0] == _cols) {
            std::size_t num_large_rows(_rows / _cols), num_large_cols(rhs._cols / _cols);
            if (_rows % _cols != 0)
                ++num_large_rows;
            if (rhs._cols % _cols != 0)
                ++num_large_cols;

            std::vector<dynamic_matrix<T>> lhs_sub(num_large_rows);
            for (std::size_t i = 0; i < num_large_rows; ++i)
                lhs_sub[i] = this->subarray(_cols, _cols, i * _cols, 0, T());

            std::vector<dynamic_matrix<U>> rhs_sub(num_large_cols);
            for (std::size_t i = 0; i < num_large_cols; ++i)
                rhs_sub[i] = rhs.subarray(_cols, _cols, 0, i * _cols, U());

            for (std::size_t i = 0; i < num_large_rows; ++i) {
                for (std::size_t j = 0; j < num_large_cols; ++j) {
                    dynamic_matrix<result_t> subresult = lhs_sub[i] * rhs_sub[j];
                    std::size_t row_bound(std::min(_cols, _rows - i * _cols)),
                      col_bound(std::min(_cols, rhs._cols - j * _cols));
                    for (std::size_t k = 0; k < row_bound; ++k)
                        for (std::size_t l = 0; l < col_bound; ++l)
                            result[i * _cols + k][j * _cols + l] = subresult[k][l];
                }
            }

            return result;
        }

        // Case 3: Skinny rhs
        if (dimensions[0] == rhs._cols) {
            std::size_t num_large_rows(_rows / rhs._cols), num_large_cols(_cols / rhs._cols);
            if (_rows % rhs._cols != 0)
                ++num_large_rows;
            if (_cols % rhs._cols != 0)
                ++num_large_cols;

            dynamic_matrix<dynamic_matrix<T>> lhs_sub(num_large_rows, num_large_cols);
            for (std::size_t i = 0; i < num_large_rows; ++i)
                for (std::size_t j = 0; j < num_large_cols; ++j)
                    lhs_sub[i][j] =
                      this->subarray(rhs._cols, rhs._cols, i * rhs._cols, j * rhs._cols, T());

            std::vector<dynamic_matrix<U>> rhs_sub(num_large_cols);
            for (std::size_t i = 0; i < num_large_cols; ++i)
                rhs_sub[i] = rhs.subarray(rhs._cols, rhs._cols, i * rhs._cols, 0, U());

            for (std::size_t i = 0; i < num_large_rows; ++i) {
                dynamic_matrix<result_t> subresult(rhs._cols, rhs._cols);
                for (std::size_t j = 0; j < num_large_cols; ++j)
                    subresult += lhs_sub[i][j] * rhs_sub[j];

                std::size_t row_bound = std::min(rhs._cols, _rows - i * rhs._cols);
                for (std::size_t j = 0; j < row_bound; ++j)
                    for (std::size_t k = 0; k < rhs._cols; ++k)
                        result[i * rhs._cols + j][k] = subresult[j][k];
            }

            return result;
        }
    }

    // Strassen-Winograd algorithm
    // split each matrix in four
    std::size_t row_split(_rows / 2 + _rows % 2), mid_split(_cols / 2 + _cols % 2),
      col_split(rhs._cols / 2 + rhs._cols % 2);

    if constexpr (std::is_same_v<T, U> && std::is_same_v<T, result_t>) {
        // only need in this case 10 temp variables
        dynamic_matrix<T> temp[10];
        temp[0] = this->subarray(row_split, mid_split, 0, mid_split, T()); // A12
        temp[1] = rhs.subarray(mid_split, col_split, mid_split, 0, T());   // B21
        temp[2] = temp[0] * temp[1];                                       // P2 = A12 * B21
        temp[3] = this->subarray(row_split, mid_split, 0, 0);              // A11
        temp[4] = rhs.subarray(mid_split, col_split, 0, 0);                // B11
        temp[5] = temp[3] * temp[4];                                       // P1 = A11 * B11

        temp[6] = this->subarray(row_split, mid_split, row_split, 0, T());         // A21
        temp[7] = this->subarray(row_split, mid_split, row_split, mid_split, T()); // A22
        temp[8] = temp[6] + temp[7];                                               // S1 = A21 + A22
        temp[9] = rhs.subarray(mid_split, col_split, 0, col_split, T());           // B12
        temp[2] = temp[5] + temp[2]; // U1 = P1 + P2 = C11
        for (std::size_t i = 0; i < row_split; ++i)
            for (std::size_t j = 0; j < col_split; ++j)
                result[i][j] = temp[2][i][j];
        temp[4] = temp[9] - temp[4];                                             // T1 = B12 - B11
        temp[6] = temp[3] - temp[6];                                             // S3 = A11 - A21
        temp[2] = rhs.subarray(mid_split, col_split, mid_split, col_split, T()); // B22
        temp[3] = temp[8] - temp[3];                                             // S2 = S1 - A11
        temp[8] = temp[8] * temp[4];                                             // P5 = S1 * T1
        temp[0] -= temp[3];                                                      // S4 = A12 - S2
        temp[4] = temp[2] - temp[4];                                             // T2 = B22 - T1
        temp[0] = temp[0] * temp[2];                                             // P3 = S4 * B22
        temp[1] = temp[4] - temp[1];                                             // T4 = T2 - B21
        temp[2] -= temp[9];                                                      // T3 = B22 - B12
        temp[3] = temp[3] * temp[4];                                             // P6 = S2 * T2
        temp[1] = temp[7] * temp[1];                                             // P4 = A22 * T4
        temp[2] = temp[6] * temp[2];                                             // P7 = S3 * T3
        temp[3] = temp[5] + temp[3];                                             // U2 = P1 + P6
        temp[4] = temp[3] + temp[8];                                             // U4 = U2 + P5
        temp[0] = temp[4] + temp[0]; // U5 = U4 + P3 = C12
        for (std::size_t i = 0; i < row_split; ++i)
            for (std::size_t j = 0; j < col_split && col_split + j < rhs._cols; ++j)
                result[i][col_split + j] = temp[0][i][j];
        temp[2] = temp[3] + temp[2]; // U3 = U2 + P7
        temp[1] = temp[2] - temp[1]; // U6 = U3 - P4 = C21
        for (std::size_t i = 0; i < row_split && row_split + i < _rows; ++i)
            for (std::size_t j = 0; j < col_split; ++j)
                result[row_split + i][j] = temp[1][i][j];
        temp[2] += temp[8]; // U7 = U3 + P5 = C22
        for (std::size_t i = 0; i < row_split && row_split + i < _rows; ++i)
            for (std::size_t j = 0; j < col_split && col_split + j < rhs._cols; ++j)
                result[row_split + i][col_split + j] = temp[2][i][j];
    } else {
        // Three sets of temp variables for A/S, B/T, P/U
        dynamic_matrix<T> lhs_sub[5];
        dynamic_matrix<U> rhs_sub[4];
        dynamic_matrix<result_t> res_sub[4];
        lhs_sub[0] = this->subarray(row_split, mid_split, 0, 0);                      // A11
        lhs_sub[1] = this->subarray(row_split, mid_split, 0, mid_split, T());         // A12
        lhs_sub[2] = this->subarray(row_split, mid_split, row_split, 0);              // A21
        lhs_sub[3] = this->subarray(row_split, mid_split, row_split, mid_split, T()); // A22
        rhs_sub[0] = rhs.subarray(mid_split, col_split, 0, 0);                        // B11
        rhs_sub[1] = rhs.subarray(mid_split, col_split, 0, col_split, U());           // B12
        rhs_sub[2] = rhs.subarray(mid_split, col_split, mid_split, 0, U());           // B21
        rhs_sub[3] = rhs.subarray(mid_split, col_split, mid_split, col_split, U());   // B22

        res_sub[1] = lhs_sub[0] * rhs_sub[0]; // P1 = A11 * B11
        res_sub[0] = lhs_sub[1] * rhs_sub[2]; // P2 = A12 * B21
        res_sub[0] = res_sub[1] + res_sub[0]; // U1 = P1 + P2
        for (std::size_t i = 0; i < row_split; ++i)
            for (std::size_t j = 0; j < col_split; ++j)
                result[i][j] = res_sub[0][i][j];

        lhs_sub[4] = lhs_sub[2] + lhs_sub[3]; // S1 = A21 + A22
        rhs_sub[0] = rhs_sub[1] - rhs_sub[0]; // T1 = B12 - B11
        res_sub[2] = lhs_sub[4] * rhs_sub[0]; // P5 = S1 * T1
        lhs_sub[4] -= lhs_sub[0];             // S2 = S1 - A11
        rhs_sub[0] = rhs_sub[3] - rhs_sub[0]; // T2 = B22 - T1
        res_sub[3] = lhs_sub[4] * rhs_sub[0]; // P6 = S2 * T2
        res_sub[1] += res_sub[3];             // U2 = P1 + P6
        lhs_sub[0] -= lhs_sub[2];             // S3 = A11 - A21
        rhs_sub[1] = rhs_sub[3] - rhs_sub[1]; // T3 = B22 - B12
        res_sub[0] = lhs_sub[0] * rhs_sub[1]; // P7 = S3 * T3
        lhs_sub[1] -= lhs_sub[4];             // S4 = A12 - S2
        rhs_sub[0] -= rhs_sub[2];             // T4 = T2 - B21
        res_sub[3] = lhs_sub[1] * rhs_sub[3]; // P3 = S4 * B22
        res_sub[0] = res_sub[1] + res_sub[0]; // U3 = U2 + P7
        res_sub[1] += res_sub[2];             // U4 = U2 + P5
        res_sub[3] = res_sub[1] + res_sub[3]; // U5 = U4 + P3
        for (std::size_t i = 0; i < row_split; ++i)
            for (std::size_t j = 0; j < col_split && col_split + j < rhs._cols; ++j)
                result[i][col_split + j] = res_sub[3][i][j];

        res_sub[2] = res_sub[0] + res_sub[2]; // U7 = U3 + P5
        for (std::size_t i = 0; i < row_split && row_split + i < _rows; ++i)
            for (std::size_t j = 0; j < col_split && col_split + j < rhs._cols; ++j)
                result[row_split + i][col_split + j] = res_sub[2][i][j];

        res_sub[1] = lhs_sub[3] * rhs_sub[0]; // P4 = A22 * T4
        res_sub[2] = res_sub[0] - res_sub[1]; // U6 = U3 - P4
        for (std::size_t i = 0; i < row_split && row_split + i < _rows; ++i)
            for (std::size_t j = 0; j < col_split; ++j)
                result[row_split + i][j] = res_sub[2][i][j];
    }

    return result;
}

template<typename T> bool dynamic_matrix<T>::operator==(const dynamic_matrix<T>& rhs) const {
    if (!(_rows == rhs._rows && _cols == rhs._cols))
        return false;
    for (std::size_t i = 0; i < _rows; ++i)
        for (std::size_t j = 0; j < _cols; ++j)
            if ((*this)[i][j] != rhs[i][j])
                return false;
    return true;
}

#if __cplusplus <= 201703L
template<typename T> bool dynamic_matrix<T>::operator!=(const dynamic_matrix<T>& rhs) const {
    return !operator==(rhs);
}
#endif

template<typename T> void dynamic_matrix<T>::_check_row(std::size_t pos) const {
    if (pos >= _rows)
        throw std::out_of_range(std::to_string(pos));
}

template<typename T> void dynamic_matrix<T>::_check_col(std::size_t pos) const {
    if (pos >= _cols)
        throw std::out_of_range(std::to_string(pos));
}

template<typename T> bool dynamic_matrix<T>::_is_initialized(std::size_t pos) const noexcept {
    return _check_1[pos] < _check_size && _check_2[_check_1[pos]] == pos;
}

template<typename T> void dynamic_matrix<T>::_initialize(std::size_t pos, const T& value) {
    if (_is_initialized(pos)) {
        _elements[pos] = value;
    } else {
        std::construct_at(_elements + pos, value);
        _check_1[pos] = _check_size;
        _check_2[_check_size] = pos;
        ++_check_size;
    }
}

#endif // DYNAMIC_MATRIX_TPP
