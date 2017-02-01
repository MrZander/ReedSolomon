/*
 Copyright (c) 2017 Alexander W. Plummer. All rights reserved.
 Copyright (c) 2015 Backblaze, Inc. All rights reserved.
 The MIT License (MIT)
*/

#pragma once
#include "Shared.h"
#include "Galois.h"

class CodingLoop
{
public:
	virtual void codeSomeShards(
		CVec2D &matrixRows,
		CVec2D &inputs,
		int inputCount,
		CVec2D &outputs,
		int outputCount,
		int offset,
		int byteCount) = 0;
	bool checkSomeShards(
		CVec2D &matrixRows,
		CVec2D &inputs,
		int inputCount,
		CVec2D &toCheck,
		int checkCount,
		int offset,
		int byteCount);
};