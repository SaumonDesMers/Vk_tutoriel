#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <iostream>
#include <type_traits>

namespace ft {

	template<size_t SIZE, typename T>
	class Vector {
	
	public:

		/*
		 * Constructors with 2 arguments available only for size 2 vectors.
		 * Which means that the following code will not compile for size 3 or 4 vectors but will compile for size 2 vectors
		 */
		template <size_t n = SIZE, std::enable_if_t<(n == 2)>* = nullptr>
		Vector(const T& x, const T& y) {
			this->data[0] = x;
			this->data[1] = y;
		}

		/*
		 * Constructors with 3 arguments available only for size 3 vectors.
		 * Which means that the following code will not compile for size 2 or 4 vectors but will compile for size 3 vectors
		 */
		template <size_t n = SIZE, std::enable_if_t<(n == 3)>* = nullptr>
		Vector(const T& x, const T& y, const T& z) {
			this->data[0] = x;
			this->data[1] = y;
			this->data[2] = z;
		}

		template <size_t n = SIZE, std::enable_if_t<(n == 3)>* = nullptr>
		Vector(const Vector<2, T>& other, const T& z) {
			this->data[0] = other[0];
			this->data[1] = other[1];
			this->data[2] = z;
		}

		/*
		 * Constructors with 4 arguments available only for size 4 vectors.
		 * Which means that the following code will not compile for size 2 or 3 vectors but will compile for size 4 vectors
		 */
		template <size_t n = SIZE, std::enable_if_t<(n == 4)>* = nullptr>
		Vector(const T& x, const T& y, const T& z, const T& w) {
			this->data[0] = x;
			this->data[1] = y;
			this->data[2] = z;
			this->data[3] = w;
		}

		template <size_t n = SIZE, std::enable_if_t<(n == 4)>* = nullptr>
		Vector(const Vector<3, T>& other, const T& w) {
			this->data[0] = other[0];
			this->data[1] = other[1];
			this->data[2] = other[2];
			this->data[3] = w;
		}

		template <size_t n = SIZE, std::enable_if_t<(n == 4)>* = nullptr>
		Vector(const Vector<2, T>& other, const T& z, const T& w) {
			this->data[0] = other[0];
			this->data[1] = other[1];
			this->data[2] = z;
			this->data[3] = w;
		}

		template <size_t n = SIZE, std::enable_if_t<(n == 4)>* = nullptr>
		Vector(const Vector<2, T>& other, const Vector<2, T>& other2) {
			this->data[0] = other[0];
			this->data[1] = other[1];
			this->data[2] = other2[0];
			this->data[3] = other2[1];
		}


		/*
		 * Generic member function for all sizes
		 */

		Vector() {
			memset(this->data, 0, sizeof(T) * SIZE);
		}

		Vector(const Vector<SIZE, T>& other) {
			memcpy(this->data, other.data, sizeof(T) * SIZE);
		}

		Vector<SIZE, T>& operator=(const Vector<SIZE, T>& other) {
			memcpy(this->data, other.data, sizeof(T) * SIZE);
			return *this;
		}

		template<typename U>
		Vector<SIZE, T> operator*(typename std::enable_if<std::is_arithmetic<U>::value, U>::type scalar) {
			Vector<SIZE, T> result;
			for (size_t i = 0; i < SIZE; i++) {
				result[i] = this->data[i] * scalar;
			}
			return result;
		}

		bool operator==(const Vector<SIZE, T>& other) const {
			for (size_t i = 0; i < SIZE; i++) {
				if (this->data[i] != other.data[i]) {
					return false;
				}
			}
			return true;
		}

		T operator[](size_t index) const {
			return this->data[index];
		}

		T &operator[](size_t index) {
			return this->data[index];
		}

		void log() {
			for (size_t i = 0; i < SIZE; i++) {
				std::cout << this->data[i] << " ";
			}
			std::cout << std::endl;
		}

	private:

		T data[SIZE];
	};

};

/* Custom hash constructor for vectors */
namespace std {
	template<size_t SIZE, typename T> struct hash<ft::Vector<SIZE, T>> {
		size_t operator()(ft::Vector<SIZE, T> const& vector) const {
			size_t hash = 0;
			for (size_t i = 0; i < SIZE; i++) {
				hash ^= std::hash<T>()(vector[i]) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
			}
			return hash;
		}
	};
}

#endif // VECTOR_HPP