// High level design note
// Our matrix should match the behavior of the glm library.
#ifndef MATRIX4F_H
#define MATRIX4F_H

#include <cmath>
#include <string>

// We need to Vector4f header in order to multiply a matrix
// by a vector.
#include "Vector4f.h"

// Matrix 4f represents 4x4 matrices in Math
struct Matrix4f{
private:
    float n[4][4];  // Store each value of the matrix

public:
    Matrix4f() = default;

    // ROW MAJOR ORDER!!!!
    // Matrix constructor with 9 scalar values.
    Matrix4f( float n00, float n01, float n02, float n03,
              float n10, float n11, float n12, float n13,
              float n20, float n21, float n22, float n23,
              float n30, float n31, float n32, float n33){

        n[0][0] = n00; n[0][1] = n01; n[0][2] = n02; n[0][3] = n03;
        n[1][0] = n10; n[1][1] = n11; n[1][2] = n12; n[1][3] = n13;
        n[2][0] = n20; n[2][1] = n21; n[2][2] = n22; n[2][3] = n23;
        n[3][0] = n30; n[3][1] = n30; n[3][2] = n32; n[3][3] = n33;
    }

    // Matrix constructor from four vectors.
    // Note: 'd' will almost always be 0,0,0,1
    Matrix4f(const Vector4f& a, const Vector4f& b, const Vector4f& c, const Vector4f& d){
      n[0][0] = a.x; n[0][1] = a.y; n[0][2] = a.z; n[0][3] = a.w;
      n[1][0] = b.x; n[1][1] = b.y; n[1][2] = b.z; n[1][3] = b.w;
      n[2][0] = c.x; n[2][1] = c.y; n[2][2] = c.z; n[2][3] = c.w;
      n[3][0] = d.x; n[3][1] = d.y; n[3][2] = d.z; n[3][3] = d.w;
    }

    // Makes the matrix an identity matrix
    void identity(){
		n[0][0] = 1.0f; n[0][1] = 0.0f; n[0][2] = 0.0f; n[0][3] = 0.0f;
		n[1][0] = 0.0f; n[1][1] = 1.0f; n[1][2] = 0.0f; n[1][3] = 0.0f;
		n[2][0] = 0.0f; n[2][1] = 0.0f; n[2][2] = 1.0f; n[2][3] = 0.0f;
		n[3][0] = 0.0f; n[3][1] = 0.0f; n[3][2] = 0.0f; n[3][3] = 1.0f;
    }

    // Index operator with two dimensions
    // Example: M(1,1) returns row 1 and column 1 of matrix M.
    float& operator ()(int i, int j){
      return (n[i][j]);
    }

    // Index operator with two dimensions
    // Example: M(1,1) returns row 1 and column 1 of matrix M.
    const float& operator ()(int i, int j) const{
      return (n[i][j]);
    }

    // Return a single row from the matrix
    Vector4f& operator [](int i){
      return (*reinterpret_cast<Vector4f *>(n[i]));
    }

    // Return a single row from the matrix
    const Vector4f& operator [](int i) const{
      return (*reinterpret_cast<const Vector4f *>(n[i]));
    }

    // NOTE: unlike above operator[] overrides, this DOES NOT
    //  return a reference to the matrix's internal 2D array
    // Return a column from a matrix as a vector.
    Vector4f col(int j){
    	return Vector4f(n[0][j], n[1][j], n[2][j], n[3][j]);
    }

    // NOTE: unlike above operator[] overrides, this DOES NOT
    //  return a reference to the matrix's internal 2D array
    // Return a column from a matrix as a vector.
    const Vector4f col(int j) const{
    	return Vector4f(n[0][j], n[1][j], n[2][j], n[3][j]);
    }

    // Make a matrix rotate about various axis
    static Matrix4f MakeRotationX(float t){
        return(Matrix4f(
			1.0f, 	0.0f, 			0.0f, 			0.0f,
			0.0f, 	std::cos(t),	std::sin(t),	0.0f,
			0.0f, 	-std::sin(t),	std::cos(t),	0.0f,
			0.0f, 	0.0f,			0.0f,			1.0f));
    }
    static Matrix4f MakeRotationY(float t){
        return(Matrix4f(
			std::cos(t), 	0.0f, 	-std::sin(t), 	0.0f,
			0.0f, 			1.0f,	0.0f,			0.0f,
			std::sin(t), 	0.0f,	std::cos(t),	0.0f,
			0.0f, 			0.0f,	0.0f,			1.0f));
    }
    static Matrix4f MakeRotationZ(float t){
        return(Matrix4f(
			std::cos(t), 	std::sin(t),	0.0f, 	0.0f,
			-std::sin(t), 	std::cos(t),	0.0f,	0.0f,
			0.0f, 			0.0f,			0.0f,	0.0f,
			0.0f, 			0.0f,			0.0f,	1.0f));
    }
    static Matrix4f MakeScale(float sx,float sy, float sz){
        return(Matrix4f(
			sx, 	0.0f,	0.0f, 	0.0f,
			0.0f, 	sy,		0.0f,	0.0f,
			0.0f, 	0.0f,	sz,		0.0f,
			0.0f, 	0.0f,	0.0f,	1.0f));
    }

	// For debugging purposes ONLY (it's not fast!)
	std::string toString() {
		std::string result;
		for (int ii = 0; ii < 4; ii++) {
			result += (*this)[ii].toString() + ",\n";
		}
		return result;
	}
};

// Matrix Multiplication
Matrix4f operator *(const Matrix4f& A, const Matrix4f& B){
	const Vector4f Arow1 = A[0];
	const Vector4f Arow2 = A[1];
	const Vector4f Arow3 = A[2];
	const Vector4f Arow4 = A[3];
	const Vector4f Bcol1 = B.col(0);
	const Vector4f Bcol2 = B.col(1);
	const Vector4f Bcol3 = B.col(2);
	const Vector4f Bcol4 = B.col(3);

	return Matrix4f(
		Dot(Arow1, Bcol1), Dot(Arow1, Bcol2), Dot(Arow1, Bcol3), Dot(Arow1, Bcol4),
		Dot(Arow2, Bcol1), Dot(Arow2, Bcol2), Dot(Arow2, Bcol3), Dot(Arow2, Bcol4),
		Dot(Arow3, Bcol1), Dot(Arow3, Bcol2), Dot(Arow3, Bcol3), Dot(Arow3, Bcol4),
		Dot(Arow4, Bcol1), Dot(Arow4, Bcol2), Dot(Arow4, Bcol3), Dot(Arow4, Bcol4));
}

// Multiply a vector by a matrix (vector goes on the left due to row major order)
Vector4f operator *(const Vector4f& v, const Matrix4f& M){
	const Vector4f Mcol1 = M.col(0);
	const Vector4f Mcol2 = M.col(1);
	const Vector4f Mcol3 = M.col(2);
	const Vector4f Mcol4 = M.col(3);

	return Vector4f(Dot(v, Mcol1), Dot(v, Mcol2), Dot(v, Mcol3), Dot(v, Mcol4));
}


#endif
