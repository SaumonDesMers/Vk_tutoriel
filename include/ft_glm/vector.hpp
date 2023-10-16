#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <iostream>
#include <type_traits>
#include <cmath>

namespace ft {

	template<size_t S>
	struct Empty {};

	template<size_t SIZE, typename T>
	union Vector {
	
	private:

		typedef typename ft::Vector<SIZE, T> vec;

		T data[SIZE];
	
	public:

		// struct {
		// 	T x;
		// 	T y;
		// 	[[no_unique_address]] std::conditional_t<SIZE >= 3, T, Empty<0>> z;
		// 	[[no_unique_address]] std::conditional_t<SIZE >= 4, T, Empty<1>> w;
		// };

		/*
		 * Constructors with 2 arguments available only for size 2 vectors.
		 */
		template <size_t n = SIZE, std::enable_if_t<(n == 2)>* = nullptr>
		Vector(const T& x, const T& y) {// requires (SIZE == 2) {
			this->data[0] = x;
			this->data[1] = y;
		}

		/*
		 * Constructors with 3 arguments available only for size 3 vectors.
		 */
		template <size_t n = SIZE, std::enable_if_t<(n == 3)>* = nullptr>
		Vector(const T& x, const T& y, const T& z) {// requires (SIZE == 3) { /* Requires is c++20 */
			this->data[0] = x;
			this->data[1] = y;
			this->data[2] = z;
		}

		template <size_t n = SIZE, std::enable_if_t<(n == 3)>* = nullptr>
		Vector(const Vector<2, T>& other, const T& z) {// requires (SIZE == 3) {
			this->data[0] = other[0];
			this->data[1] = other[1];
			this->data[2] = z;
		}

		/*
		 * Constructors with 4 arguments available only for size 4 vectors.
		 */
		template <size_t n = SIZE, std::enable_if_t<(n == 4)>* = nullptr>
		Vector(const T& x, const T& y, const T& z, const T& w) {// requires (SIZE == 4) {
			this->data[0] = x;
			this->data[1] = y;
			this->data[2] = z;
			this->data[3] = w;
		}

		template <size_t n = SIZE, std::enable_if_t<(n == 4)>* = nullptr>
		Vector(const Vector<3, T>& other, const T& w) {// requires (SIZE == 4) {
			this->data[0] = other[0];
			this->data[1] = other[1];
			this->data[2] = other[2];
			this->data[3] = w;
		}

		template <size_t n = SIZE, std::enable_if_t<(n == 4)>* = nullptr>
		Vector(const Vector<2, T>& other, const T& z, const T& w) {// requires (SIZE == 4) {
			this->data[0] = other[0];
			this->data[1] = other[1];
			this->data[2] = z;
			this->data[3] = w;
		}

		template <size_t n = SIZE, std::enable_if_t<(n == 4)>* = nullptr>
		Vector(const Vector<2, T>& other, const Vector<2, T>& other2) {// requires (SIZE == 4) {
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

		Vector(const vec& other) {
			memcpy(this->data, other.data, sizeof(T) * SIZE);
		}

		vec& operator=(const vec& other) {
			memcpy(this->data, other.data, sizeof(T) * SIZE);
			return *this;
		}

		vec& operator+=(const vec& other) {
			for (size_t i = 0; i < SIZE; i++) {
				this->data[i] += other[i];
			}
			return *this;
		}

		vec& operator-=(const vec& other) {
			for (size_t i = 0; i < SIZE; i++) {
				this->data[i] -= other[i];
			}
			return *this;
		}

		/* Binary plus operator */
		vec operator+(const vec& other) const {
			vec result;
			for (size_t i = 0; i < SIZE; i++) {
				result[i] = this->data[i] + other[i];
			}
			return result;
		}

		/* Binary minus operator */
		vec operator-(const vec& other) const {
			vec result;
			for (size_t i = 0; i < SIZE; i++) {
				result[i] = this->data[i] - other[i];
			}
			return result;
		}

		/* Unary minus operator */
		vec operator-() {
			vec result;
			for (size_t i = 0; i < SIZE; i++) {
				result[i] = -this->data[i];
			}
			return result;
		}

		vec operator*(float scalar) const {
			vec result;
			for (size_t i = 0; i < SIZE; i++) {
				result[i] = this->data[i] * scalar;
			}
			return result;
		}

		vec operator/(float scalar) const {
			vec result;
			for (size_t i = 0; i < SIZE; i++) {
				result[i] = this->data[i] / scalar;
			}
			return result;
		}

		bool operator==(const vec& other) const {
			return memcmp(this->data, other.data, sizeof(T) * SIZE) == 0;
		}

		T operator[](size_t index) const {
			return this->data[index];
		}

		T &operator[](size_t index) {
			return this->data[index];
		}

		float lengthSquared() const {
			float result = 0;
			for (size_t i = 0; i < SIZE; i++) {
				result += this->data[i] * this->data[i];
			}
			return result;
		}

		float length() const {
			return sqrt(this->lengthSquared());
		}

		vec normalize() const {
			return *this / this->length();
		}

		T dot(const vec& other) const {
			T result = 0;
			for (size_t i = 0; i < SIZE; i++) {
				result += this->data[i] * other[i];
			}
			return result;
		}

		template<size_t n = SIZE, std::enable_if_t<(n == 3)>* = nullptr>
		vec cross(const vec& other) const {
			double v1x = this->data[0], v1y = this->data[1], v1z = this->data[2];
			double v2x = other[0], v2y = other[1], v2z = other[2];
			return vec(
				v1y * v2z - v1z * v2y,
				v1z * v2x - v1x * v2z,
				v1x * v2y - v1y * v2x
			);
		}

		void log() const {
			for (size_t i = 0; i < SIZE; i++) {
				std::cout << this->data[i] << " ";
			}
			std::cout << std::endl;
		}
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