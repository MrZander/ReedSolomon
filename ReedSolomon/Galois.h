#pragma once
#include "Shared.h"

/*
Copyright (c) 2017 Alexander W. Plummer. All rights reserved.
Copyright (c) 2015 Backblaze, Inc. All rights reserved.
The MIT License (MIT)
*/

class LogTableGenerationException : std::exception {};

static class Galois
{
public:  
	static const int FIELD_SIZE = 256;
	static const int GENERATING_POLYNOMIAL = 29;
	static const short LOG_TABLE[256];
	static const char EXP_TABLE[510];
	static const CVec2D MULTIPLICATION_TABLE;

	static char add(char a, char b);
	static char subtract(char a, char b);
	static char multiply(char a, char b);
	static char divide(char a, char b);
	static char exp(char a, char n);

	static SVec generateLogTable(int polynomial);
	static CVec generateExpTable(SVec &logTable);
	static CVec2D generateMultiplicationTable();
	static IVec allPossiblePolynomials();
};