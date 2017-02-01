/*
Copyright (c) 2017 Alexander W. Plummer. All rights reserved.
Copyright (c) 2015 Backblaze, Inc. All rights reserved.
The MIT License (MIT)
*/

#pragma once

#include "Shared.h"

class Matrix
{
private:
	int rows;
	int columns;
	CVec2D data;
	void gaussianElimination();
public:
	Matrix() {}
	Matrix(int initRows, int initColumns);
	Matrix(CVec2D initData);
	static Matrix identity(int size);
	int getRows() { return rows; }
	int getColumns() { return columns; }
	char get(int r, int c);
	void set(int r, int c, char value);
	bool equals(Matrix &matrix);
	Matrix times(Matrix &right);
	Matrix augment(Matrix &right);
	Matrix submatrix(int rmin, int cmin, int rmax, int cmax);
	CVec getRow(int row);
	void swapRows(int r1, int r2);
	Matrix invert(); 
};