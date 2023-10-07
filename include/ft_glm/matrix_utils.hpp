#ifndef MATRIX_UTILS_HPP
#define MATRIX_UTILS_HPP

#include "matrix.hpp"
#include "vector.hpp"

namespace ft {

	/* Create a translation matrix */
	template<typename T>
	Matrix<4, 4, T> translate(Vector<3, T> translation) {
		Matrix<4, 4, T> result(1.0f);
		result[0][3] = translation[0];
		result[1][3] = translation[1];
		result[2][3] = translation[2];
		return result;
	}

	/* Create a scaling matrix */
	template<typename T>
	Matrix<4, 4, T> scale(T scalar_x, T scalar_y, T scalar_z) {
		Matrix<4, 4, T> result(1.0f);
		result[0][0] = scalar_x;
		result[1][1] = scalar_y;
		result[2][2] = scalar_z;
		return result;
	}

	/* Create a rotation matrix */
	template<typename T>
	Matrix<4, 4, T> rotate(T angle, Vector<3, T> axis) {
		Matrix<4, 4, T> result(1.0f);
		T r = angle;
		T c = cos(r);
		T s = sin(r);
		T omc = 1.0f - c;
		result[0][0] = axis[0] * axis[0] * omc + c;
		result[0][1] = axis[0] * axis[1] * omc - axis[2] * s;
		result[0][2] = axis[0] * axis[2] * omc + axis[1] * s;
		result[1][0] = axis[1] * axis[0] * omc + axis[2] * s;
		result[1][1] = axis[1] * axis[1] * omc + c;
		result[1][2] = axis[1] * axis[2] * omc - axis[0] * s;
		result[2][0] = axis[2] * axis[0] * omc - axis[1] * s;
		result[2][1] = axis[2] * axis[1] * omc + axis[0] * s;
		result[2][2] = axis[2] * axis[2] * omc + c;
		return result;
	}

}

#endif // MATRIX_UTILS_HPP