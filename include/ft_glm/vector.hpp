#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <iostream>
#include <type_traits>

namespace ft {

	template<size_t SIZE, typename T>
	class Vector {
	
	public:

		Vector() {
			memcpy(this->data, 0, sizeof(T) * SIZE);
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

	typedef Vector<2, float> vec2;
	typedef Vector<3, float> vec3;
	typedef Vector<4, float> vec4;



	/* Specialization for size 2 vectors */
	template<typename T> class Vector<2, T> {
	
	public:

		Vector() {
			this->data[0] = 0;
			this->data[1] = 0;
		}

		Vector(const Vector<2, T>& other) {
			this->data[0] = other.data[0];
			this->data[1] = other.data[1];
		}

		Vector(const T& x, const T& y) {
			this->data[0] = x;
			this->data[1] = y;
		}

		Vector<2, T>& operator=(const Vector<2, T>& other) {
			this->data[0] = other.data[0];
			this->data[1] = other.data[1];
			return *this;
		}

		template<typename U>
		Vector<2, T> operator*(U scalar) {
			Vector<2, T> result;
			result[0] = this->data[0] * scalar;
			result[1] = this->data[1] * scalar;
			return result;
		}

		bool operator==(const Vector<2, T>& other) const {
			return this->data[0] == other.data[0]
				&& this->data[1] == other.data[1];
		}

		T operator[](size_t index) const {
			return this->data[index];
		}

		T &operator[](size_t index) {
			return this->data[index];
		}

		void log() {
			for (size_t i = 0; i < 2; i++) {
				std::cout << this->data[i] << " ";
			}
			std::cout << std::endl;
		}

	private:

		T data[2];

	};



	/* Specialization for size 3 vectors */
	template<typename T> class Vector<3, T> {

	public:

		Vector() {
			this->data[0] = 0;
			this->data[1] = 0;
			this->data[2] = 0;
		}

		Vector(const Vector<3, T>& other) {
			this->data[0] = other.data[0];
			this->data[1] = other.data[1];
			this->data[2] = other.data[2];
		}

		Vector(const T& x, const T& y, const T& z) {
			this->data[0] = x;
			this->data[1] = y;
			this->data[2] = z;
		}

		Vector(const Vector<2, T>& other, const T& z) {
			this->data[0] = other[0];
			this->data[1] = other[1];
			this->data[2] = z;
		}

		Vector<3, T>& operator=(const Vector<3, T>& other) {
			this->data[0] = other.data[0];
			this->data[1] = other.data[1];
			this->data[2] = other.data[2];
			return *this;
		}

		template<typename U>
		Vector<3, T> operator*(typename std::enable_if<std::is_arithmetic<U>::value, U>::type scalar) {
			Vector<3, T> result;
			result[0] = this->data[0] * scalar;
			result[1] = this->data[1] * scalar;
			result[2] = this->data[2] * scalar;
			return result;
		}

		bool operator==(const Vector<3, T>& other) const {
			return this->data[0] == other.data[0]
				&& this->data[1] == other.data[1]
				&& this->data[2] == other.data[2];
		}

		T operator[](size_t index) const {
			return this->data[index];
		}

		T &operator[](size_t index) {
			return this->data[index];
		}

		T dot(const Vector<3, T>& other) {
			return this->data[0] * other.data[0]
				+ this->data[1] * other.data[1]
				+ this->data[2] * other.data[2];
		}

		Vector<3, T> cross(const Vector<3, T>& other) {
			Vector<3, T> result;
			result[0] = this->data[1] * other.data[2] - this->data[2] * other.data[1];
			result[1] = this->data[2] * other.data[0] - this->data[0] * other.data[2];
			result[2] = this->data[0] * other.data[1] - this->data[1] * other.data[0];
			return result;
		}

		void log() {
			for (size_t i = 0; i < 3; i++) {
				std::cout << this->data[i] << " ";
			}
			std::cout << std::endl;
		}

	private:
	
		T data[3];

	};



	/* Specialization for size 4 vectors */
	template<typename T> class Vector<4, T> {

	public:

		Vector() {
			this->data[0] = 0;
			this->data[1] = 0;
			this->data[2] = 0;
			this->data[3] = 0;
		}

		Vector(const Vector<4, T>& other) {
			this->data[0] = other.data[0];
			this->data[1] = other.data[1];
			this->data[2] = other.data[2];
			this->data[3] = other.data[3];
		}

		Vector(const T& x, const T& y, const T& z, const T& w) {
			this->data[0] = x;
			this->data[1] = y;
			this->data[2] = z;
			this->data[3] = w;
		}

		Vector(const Vector<3, T>& other, const T& w) {
			this->data[0] = other[0];
			this->data[1] = other[1];
			this->data[2] = other[2];
			this->data[3] = w;
		}

		Vector(const Vector<2, T>& other, const T& z, const T& w) {
			this->data[0] = other[0];
			this->data[1] = other[1];
			this->data[2] = z;
			this->data[3] = w;
		}

		Vector<4, T>& operator=(const Vector<4, T>& other) {
			this->data[0] = other.data[0];
			this->data[1] = other.data[1];
			this->data[2] = other.data[2];
			this->data[3] = other.data[3];
			return *this;
		}

		template<typename U>
		Vector<4, T> operator*(typename std::enable_if<std::is_arithmetic<U>::value, U>::type scalar) {
			Vector<4, T> result;
			result[0] = this->data[0] * scalar;
			result[1] = this->data[1] * scalar;
			result[2] = this->data[2] * scalar;
			result[3] = this->data[3] * scalar;
			return result;
		}

		bool operator==(const Vector<4, T>& other) const {
			return this->data[0] == other.data[0]
				&& this->data[1] == other.data[1]
				&& this->data[2] == other.data[2]
				&& this->data[3] == other.data[3];
		}

		T operator[](size_t index) const {
			return this->data[index];
		}

		T &operator[](size_t index) {
			return this->data[index];
		}

		void log() {
			for (size_t i = 0; i < 4; i++) {
				std::cout << this->data[i] << " ";
			}
			std::cout << std::endl;
		}

	private:

		T data[4];

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