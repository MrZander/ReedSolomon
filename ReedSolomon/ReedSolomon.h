/*
Copyright (c) 2017 Alexander W. Plummer. All rights reserved.
Copyright (c) 2015 Backblaze, Inc. All rights reserved.
The MIT License (MIT)
*/

#pragma once

#include "Shared.h"
#include "Galois.h"
#include "Matrix.h"
#include "CodingLoop.h"

class ReedSolomon
{
private:
	int dataShardCount;
	int parityShardCount;
	int totalShardCount;
	Matrix matrix;
	CodingLoop* codingLoop;
	CVec2D parityRows;
public:
	static ReedSolomon create(int dataShardCount, int parityShardCount);
	ReedSolomon(int dataShardCount, int parityShardCount, CodingLoop* codingLoop);
	int getDataShardCount() { return dataShardCount; }
	int getParityShardCount() { return parityShardCount; }
	int getTotalShardCount() { return totalShardCount; }
	void encodeParity(CVec2D &shards, int offset, int byteCount);
	bool isParityCorrect(CVec2D &shards, int firstByte, int byteCount);
	void decodeMissing(CVec2D &shards, BVec &shardPresent, int offset, int byteCount);
	void checkBuffersAndSizes(CVec2D &shards, int offset, int byteCount);
	static Matrix buildMatrix(int dataShards, int totalShards);
	static Matrix vandermonde(int rows, int cols);
};