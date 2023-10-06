#ifndef UTILS_HPP
#define UTILS_HPP

#include "matrix.hpp"
#include "vector.hpp"

namespace ft_glm {

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

}

#endif // UTILS_HPP