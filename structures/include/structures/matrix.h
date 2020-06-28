#if __cplusplus >= 201703L
#ifndef MATRIX_H
#define MATRIX_H

#include <iostream>
#include <algorithm>
#include <array>

template <typename T, std::size_t Rows, std::size_t Cols>
using matrix = std::array<std::array<T, Cols>, Rows>;

template <typename T, std::size_t Rows, std::size_t Cols>
matrix<T, Rows, Cols>& operator+=(matrix<T, Rows, Cols>& x, const matrix<T, Rows, Cols>& y) {
    for(std::size_t i = 0; i < Rows; ++i)
        std::transform(x[i].begin(), x[i].end(), y[i].begin(), x[i].begin(), std::plus<T>());
    return x;
}

template <typename T, std::size_t Rows, std::size_t Cols>
matrix<T, Rows, Cols> operator+(const matrix<T, Rows, Cols>& x, const matrix<T, Rows, Cols>& y) {
    matrix<T, Rows, Cols> temp = x;
    temp += y;
    return temp;
}

template <typename T, std::size_t Rows, std::size_t Cols>
matrix<T, Rows, Cols>& operator-=(matrix<T, Rows, Cols>& x, const matrix<T, Rows, Cols>& y) {
    for(std::size_t i = 0; i < Rows; ++i)
        std::transform(x[i].begin(), x[i].end(), y[i].begin(), x[i].begin(), std::minus<T>());
    return x;
}

template <typename T, std::size_t Rows, std::size_t Cols>
matrix<T, Rows, Cols> operator-(const matrix<T, Rows, Cols>& x, const matrix<T, Rows, Cols>& y) {
    matrix<T, Rows, Cols> temp = x;
    temp -= y;
    return temp;
}

// Scalar multiplication
template <typename T, std::size_t Rows, std::size_t Cols>
matrix<T, Rows, Cols>& operator*=(matrix<T, Rows, Cols>& m, const T& s) {
    for (std::size_t i = 0; i < Rows; ++i)
        for (std::size_t j = 0; j < Cols; ++j)
            m[i][j] *= s;
    return m;
}

template <typename T, std::size_t Rows, std::size_t Cols>
matrix<T, Rows, Cols>& operator*(const matrix<T, Rows, Cols>& m, const T& s) {
    matrix<T, Rows, Cols> temp = m;
    temp *= s;
    return temp;
}

template <typename T, std::size_t Rows, std::size_t Cols>
matrix<T, Rows, Cols>& operator*(const T& s, const matrix<T, Rows, Cols>& m) {
    return m * s;
}

// Self multiplication: only works with square matrices
template <typename T, std::size_t Dim>
matrix<T, Dim, Dim>& operator*=(matrix<T, Dim, Dim>& x, const matrix<T, Dim, Dim>& y) {
    // Due to the complexity of matrix multiplication, better to just assign from matrix multiplication algorithm
    x = x * y;
    return x;
}

template <typename T, std::size_t Rows1, std::size_t Cols1, std::size_t Cols2>
matrix<T, Rows1, Cols2> operator*(const matrix<T, Rows1, Cols1>& x, const matrix<T, Cols1, Cols2>& y) {
    // Pad to even
    constexpr std::size_t padded_Cols1 = (Cols1 % 2 == 0) ? Cols1 : Cols1 + 1;
    constexpr std::size_t padded_Rows1 = (Rows1 % 2 == 0) ? Rows1 : Rows1 + 1;
    constexpr std::size_t padded_Cols2 = (Cols2 % 2 == 0) ? Cols2 : Cols2 + 1;

    // At a certain threshold, just use normal matrix multiplication
    if constexpr(Cols1 <= 12 && (Rows1 <= 12 || Cols2 <= 12)) {
        matrix<T, Rows1, Cols2> result;
        for(std::size_t i = 0; i < Rows1; ++i) {
            for(std::size_t j = 0; j < Cols2; ++j) {
                T& entry = result[i][j];
                entry = T();

                for(std::size_t k = 0; k < Cols1; ++k)
                    entry += x[i][k] * y[k][j];
            }
        }
        return result;
    }
    
    // quick check: if disproportionately long, split and recurse
    else if constexpr (2 * Rows1 <= padded_Cols1) {
        constexpr std::size_t matrix_size = padded_Cols1 / 2;
        matrix<T, Rows1, matrix_size> x_sub_1, x_sub_2;
        matrix<T, matrix_size, Cols2> y_sub_1, y_sub_2;
        for(std::size_t i = 0; i < Rows1; ++i) {
            std::size_t j = 0;
            for(; j < padded_Cols1; ++j) 
                x_sub_1[i][j] = x[i][j];
            for(; j < Cols1; ++j)
                x_sub_2[i][j - padded_Cols1] = x[i][j];
        }
        for(std::size_t j = 0; j < Cols2; ++j) {
            std::size_t i = 0;
            for(; i < padded_Cols1; ++i)
                y_sub_1[i][j] = y[i][j];
            for(; i < Cols1; ++i)
                y_sub_2[i - padded_Cols1][j] = y[i][j];
        }
        return x_sub_1 * y_sub_1 + x_sub_2 * y_sub_2;
    }

    // At this point, we can use Strassen's algorithm; but need to pad if necessary
    // x -> {{x_div[0], x_div[1]}, {x_div[2], x_div[3]}}, similar for y
    else {
        // the divide part: split into 8 matrices of equal size
        constexpr std::size_t split_i = padded_Rows1 / 2;
        constexpr std::size_t split_j = padded_Cols1 / 2;
        constexpr std::size_t split_k = padded_Cols2 / 2;
        matrix<T, split_i, split_j> x_div[4];
        matrix<T, split_j, split_k> y_div[4];

        std::size_t i = 0, j, k;
        for(; i < split_i; ++i) {
            for(j = 0; j < split_j; ++j)
                x_div[0][i][j] = x[i][j];
            for(; j < Cols1; ++j)
                x_div[1][i][j - split_j] = x[i][j];
            for(; j < padded_Cols1; ++j)
                x_div[1][i][j - split_j] = T();
        }
        for(; i < Rows1; ++i) {
            for(j = 0; j < split_j; ++j)
                x_div[2][i - split_i][j] = x[i][j];
            for(; j < Cols1; ++j)
                x_div[3][i - split_i][j - split_j] = x[i][j];
            for(; j < padded_Cols1; ++j)
                x_div[3][i - split_i][j - split_j] = T();
        }
        for(; i < padded_Rows1; ++i)
            for(j = 0; j < split_j; ++j)
                x_div[3][i - split_i][j] = T();

        for(j = 0; j < split_j; ++j) {
            for(k = 0; k < split_k; ++k)
                y_div[0][j][k] = y[j][k];
            for(; k < Cols2; ++k)
                y_div[1][j][k - split_k] = y[j][k];
            for(; k < padded_Cols2; ++k)
                y_div[1][j][k - split_k] = T();
        }
        for(; j < Cols1; ++j) {
            for(k = 0; k < split_k; ++k)
                y_div[2][j - split_j][k] = y[j][k];
            for(; k < Cols2; ++k)
                y_div[3][j - split_j][k - split_k] = y[j][k];
            for(; k < padded_Cols2; ++k)
                y_div[3][j - split_j][k - split_k] = T();
        }
        for(; j < padded_Cols1; ++j)
            for(k = 0; k < split_k; ++k)
                y_div[3][j - split_j][k] = T();

        // The seven multiplications in Strassen's algorithm
        matrix<T, split_i, split_k> subresults[7] = {
            (x_div[0] + x_div[3]) * (y_div[0] + y_div[3]),
            (x_div[2] + x_div[3]) *  y_div[0],
             x_div[0]             * (y_div[1] - y_div[3]),
             x_div[3]             * (y_div[2] - y_div[0]),
            (x_div[0] + x_div[1]) *  y_div[3],
            (x_div[2] - x_div[0]) * (y_div[0] + y_div[1]),
            (x_div[1] - x_div[3]) * (y_div[2] + y_div[3])
        };

        matrix<T, split_i, split_k> submatrices[4] = {
            subresults[0] + subresults[3] - subresults[4] + subresults[6],
            subresults[2] + subresults[4],
            subresults[1] + subresults[3],
            subresults[0] - subresults[1] + subresults[2] + subresults[5]
        };

        matrix<T, Rows1, Cols2> result;
        for(i = 0; i < split_i; ++i) {
            for(k = 0; k < split_k; ++k)
                result[i][k] = submatrices[0][i][k];
            for(; k < Cols2; ++k)
                result[i][k] = submatrices[1][i][k - split_k];
        }
        for(; i < Rows1; ++i) {
            for(k = 0; k < split_k; ++k)
                result[i][k] = submatrices[2][i - split_i][k];
            for(; k < Cols2; ++k)
                result[i][k] = submatrices[3][i - split_i][k - split_k];
        }
        return result;
    }
}

#endif // MATRIX_H
#endif // __cplusplus >= 201703L
