#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <iostream>
#include <type_traits>

#include "vector.hpp"

namespace ft_glm {

	template<
		size_t ROWS,
		size_t COLS,
		typename T
	>
	class Matrix {

	public:

		Matrix() {
			memset(this->data, 0, sizeof(T) * ROWS * COLS);
		}

		Matrix(const Matrix<ROWS, COLS, T>& other) {
			memcpy(this->data, other.data, sizeof(T) * ROWS * COLS);
		}

		Matrix<ROWS, COLS, T>& operator=(const Matrix<ROWS, COLS, T>& other) {
			memcpy(this->data, other.data, sizeof(T) * ROWS * COLS);
			return *this;
		}

		template<typename U>
		Matrix<ROWS, COLS, T> operator*(typename std::enable_if<std::is_arithmetic<U>::value, U>::type scalar) {
			Matrix<ROWS, COLS, T> result;
			for (size_t i = 0; i < ROWS; i++) {
				for (size_t j = 0; j < COLS; j++) {
					result[i][j] = this->data[i][j] * scalar;
				}
			}
			return result;
		}

		T* operator[](size_t index) const {
			return this->data[index];
		}

		T* operator[](size_t index) {
			return this->data[index];
		}

		void log() {
			for (size_t i = 0; i < ROWS; i++) {
				for (size_t j = 0; j < COLS; j++) {
					std::cout << this->data[i][j] << " ";
				}
				std::cout << std::endl;
			}
		}

	private:

		T data[ROWS][COLS];
	};

	template<
		size_t ROWS, 
		size_t COLS, 
		typename T,
		typename U
	>
	Matrix<ROWS, COLS, T> operator*(U scalar, const Matrix<ROWS, COLS, T>& matrix) {
		return matrix * scalar;
	}


	/* Matrix multiplication */
	template<size_t ROWS, size_t COLS, typename T>
	Matrix<ROWS, COLS, T> operator*(Matrix<ROWS, COLS, T>& left, Matrix<ROWS, COLS, T>& right) {
		Matrix<ROWS, COLS, T> result;
		for (size_t i = 0; i < ROWS; i++) {
			for (size_t j = 0; j < COLS; j++) {
				for (size_t k = 0; k < COLS; k++) {
					result[i][j] += left[i][k] * right[k][j];
				}
			}
		}
		return result;
	}

	/* Matrix-vector multiplication */
	template<size_t ROWS, size_t COLS, typename T>
	Vector<ROWS, T> operator*(Matrix<ROWS, COLS, T>& matrix, Vector<COLS, T>& vector) {
		Vector<ROWS, T> result;
		for (size_t i = 0; i < ROWS; i++) {
			for (size_t j = 0; j < COLS; j++) {
				result[i] += matrix[i][j] * vector[j];
			}
		}
		return result;
	}

};

#endif // MATRIX_HPP