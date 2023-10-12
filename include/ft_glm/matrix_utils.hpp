#ifndef MATRIX_UTILS_HPP
#define MATRIX_UTILS_HPP

#include "matrix.hpp"
#include "vector.hpp"

namespace ft {

	/* Create a translation matrix
	 *
	 * Should look like this:
	 * 	| 1 0 0 x |
	 * 	| 0 1 0 y |
	 * 	| 0 0 1 z |
	 * 	| 0 0 0 1 |
	 * 
	 * but it doesn't work for some reason.
	 * So I'm copying the glm implementation which works and looks like this:
	 * 	| 1 0 0 0 |
	 * 	| 0 1 0 0 |
	 * 	| 0 0 1 0 |
	 * 	| x y z 1 |
	 * 
	 * I don't know why it works, but it does.
	 */
	template<typename T>
	Matrix<4, 4, T> translate(Vector<3, T> translation) {
		Matrix<4, 4, T> result(1.0f);
		result[3][0] = translation[0];
		result[3][1] = translation[1];
		result[3][2] = translation[2];
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

	template<typename T>
	Matrix<4, 4, T> scale(Vector<3, T> scalar) {
		Matrix<4, 4, T> result(1.0f);
		result[0][0] = scalar[0];
		result[1][1] = scalar[1];
		result[2][2] = scalar[2];
		return result;
	}

	/* Create a rotation matrix.
	 * Angle must be in radians.

		We want to rotate vector V around axis A normalized by theta T radians.

			| cos T + (1 - cos T) * A.x^2				(1 - cos T) * A.x * A.y - sin T * A.z		(1 - cos T) * A.x * A.z + sin T * A.y	0 |
		R = | (1 - cos T) * A.x * A.y + sin T * A.z		cos T + (1 - cos T) * A.y^2					(1 - cos T) * A.y * A.z - sin T * A.x	0 |
			| (1 - cos T) * A.x * A.z - sin T * A.y		(1 - cos T) * A.y * A.z + sin T * A.x		cos T + (1 - cos T) * A.z^2				0 |

	 */
	template<typename T>
	Matrix<4, 4, T> rotate(float theta, Vector<3, T> a) {
		a = a.normalize();
		T c = cos(theta);
		T s = sin(theta);
		T omc = 1.0f - c;
		return Matrix<4, 4, T>({
			a[0] * a[0] * omc + c,			a[0] * a[1] * omc - a[2] * s,	a[0] * a[2] * omc + a[1] * s,	0.0f,
			a[1] * a[0] * omc + a[2] * s,	a[1] * a[1] * omc + c,			a[1] * a[2] * omc - a[0] * s,	0.0f,
			a[2] * a[0] * omc - a[1] * s,	a[2] * a[1] * omc + a[0] * s,	a[2] * a[2] * omc + c,			0.0f,
			0.0f,							0.0f,							0.0f,							1.0f
		});
	}

	/* Create lookat matrix.
	 * The camera is at position eye and looks at target.
	 * The up vector is up.
	 */
	template<typename T>
	Matrix<4, 4, T> lookAt(Vector<3, T> eye, Vector<3, T> target, Vector<3, T> up) {
		Vector<3, T> dir = (eye - target).normalize();
		Vector<3, T> right = up.cross(dir).normalize();
		Vector<3, T> newUp = dir.cross(right);
		return Matrix<4, 4, T>({
			right[0],			newUp[0],			dir[0],			0.0f,
			right[1],			newUp[1],			dir[1],			0.0f,
			right[2],			newUp[2],			dir[2],			0.0f,
			-right.dot(eye),	-newUp.dot(eye),	-dir.dot(eye),	1.0f
		});
	}

	/* Create perspective projection matrix.
	 * The field of view is fov in radians.
	 * The aspect ratio is aspect.
	 * The near plane is near.
	 * The far plane is far.
	 */
	template<typename T>
	Matrix<4, 4, T> perspective(float fov, float aspect, float near, float far) {
		float tanHalfFov = tan(fov / 2.0f);
		return Matrix<4, 4, T>({
			1.0f / (aspect * tanHalfFov),	0.0f,				0.0f,									0.0f,
			0.0f,							1.0f / tanHalfFov,	0.0f,									0.0f,
			0.0f,							0.0f,				-(far + near) / (far - near),			-1.0f,
			0.0f,							0.0f,				-(2.0f * far * near) / (far - near),	0.0f
		});
	}


	template<typename T>
	void log_mat4(T mat) {
		for (size_t i = 0; i < 4; i++) {
			for (size_t j = 0; j < 4; j++)
				std::cout << mat[i][j] << " ";
			std::cout << std::endl;
		}
		std::cout << std::endl;
	}

}

#endif // MATRIX_UTILS_HPP