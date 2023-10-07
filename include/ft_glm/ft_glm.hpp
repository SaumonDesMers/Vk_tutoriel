#ifndef FT_GLM_HPP
#define FT_GLM_HPP

#include "utils.hpp"
#include "matrix.hpp"
#include "vector.hpp"
#include "matrix_utils.hpp"

namespace ft {

	typedef Vector<2, float> vec2;
	typedef Vector<3, float> vec3;
	typedef Vector<4, float> vec4;

	typedef Matrix<2, 2, float> mat2;
	typedef Matrix<3, 3, float> mat3;
	typedef Matrix<4, 4, float> mat4;

};

#endif // FT_GLM_HPP