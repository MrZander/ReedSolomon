/*
Copyright (c) 2017 Alexander W. Plummer. All rights reserved.
Copyright (c) 2015 Backblaze, Inc. All rights reserved.
The MIT License (MIT)
*/

#include "Matrix.h"
#include "Galois.h"

Matrix::Matrix(int initRows, int initColumns)
{
	rows = initRows;
	columns = initColumns;
	data = CVec2D(initRows);
	for (int x = 0; x < initRows; x++)
	{
		data[x] = CVec(initColumns);
	}
}
Matrix::Matrix(CVec2D initData)
{
	rows = initData.size();
	columns = initData[0].size();
	data = initData;
}
Matrix Matrix::identity(int size)
{
	Matrix result(size, size);
	for (int i = 0; i < size; i++) {
		result.set(i, i, 1);
	}
	return result;
}
char Matrix::get(int r, int c)
{
	return data[r][c];
}
void Matrix::set(int r, int c, char value)
{
	data[r][c] = value;
}
bool Matrix::equals(Matrix &other)
{
	if (rows != other.getRows() || columns != other.getColumns()) return false;
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < columns; c++)
		{
			if (get(r, c) != other.get(r, c)) return false;
		}
	}
	return true;
}
Matrix Matrix::times(Matrix &right)
{
	Matrix result = Matrix(getRows(), right.getColumns());
	for (int r = 0; r < getRows(); r++) {
		for (int c = 0; c < right.getColumns(); c++) {
			char value = 0;
			for (int i = 0; i < getColumns(); i++) {
				value ^= Galois::multiply(get(r, i), right.get(i, c));
			}
			result.set(r, c, value);
		}
	}
	return result;
}
Matrix Matrix::augment(Matrix &right)
{
	Matrix result = Matrix(rows, columns + right.columns);
	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < columns; c++) {
			result.data[r][c] = data[r][c];
		}
		for (int c = 0; c < right.columns; c++) {
			result.data[r][columns + c] = right.data[r][c];
		}
	}
	return result;
}
Matrix Matrix::submatrix(int rmin, int cmin, int rmax, int cmax)
{
	Matrix result = Matrix(rmax - rmin, cmax - cmin);
	for (int r = rmin; r < rmax; r++) {
		for (int c = cmin; c < cmax; c++) {
			result.data[r - rmin][c - cmin] = data[r][c];
		}
	}
	return result;
}
CVec Matrix::getRow(int row)
{
	CVec result(columns);
	for (int c = 0; c < columns; c++) {
		result[c] = get(row, c);
	}
	return result;
}
void Matrix::swapRows(int r1, int r2)
{
	CVec tmp = data[r1];
	data[r1] = data[r2];
	data[r2] = tmp;
}
Matrix Matrix::invert()
{
	Matrix work = augment(identity(rows));
	work.gaussianElimination();
	return work.submatrix(0, rows, columns, columns * 2);
} 

void Matrix::gaussianElimination()
{
	// Clear out the part below the main diagonal and scale the main
	// diagonal to be 1.
	for (int r = 0; r < rows; r++) {
		// If the element on the diagonal is 0, find a row below
		// that has a non-zero and swap them.
		if (data[r][r] == 0) {
			for (int rowBelow = r + 1; rowBelow < rows; rowBelow++) {
				if (data[rowBelow][r] != 0) {
					swapRows(r, rowBelow);
					break;
				}
			}
		}

		// Scale to 1.
		if (data[r][r] != 1) {
			char scale = Galois::divide(1, data[r][r]);
			for (int c = 0; c < columns; c++) {
				data[r][c] = Galois::multiply(data[r][c], scale);
			}
		}
		// Make everything below the 1 be a 0 by subtracting
		// a multiple of it.  (Subtraction and addition are
		// both exclusive or in the Galois field.)
		for (int rowBelow = r + 1; rowBelow < rows; rowBelow++) {
			if (data[rowBelow][r] != 0) {
				char scale = data[rowBelow][r];
				for (int c = 0; c < columns; c++) {
					data[rowBelow][c] ^= Galois::multiply(scale, data[r][c]);
				}
			}
		}
	}

	// Now clear the part above the main diagonal.
	for (int d = 0; d < rows; d++) {
		for (int rowAbove = 0; rowAbove < d; rowAbove++) {
			if (data[rowAbove][d] != 0) {
				char scale = data[rowAbove][d];
				for (int c = 0; c < columns; c++) {
					data[rowAbove][c] ^= Galois::multiply(scale, data[d][c]);
				}

			}
		}
	}
}