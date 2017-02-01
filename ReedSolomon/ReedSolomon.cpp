/*
Copyright (c) 2017 Alexander W. Plummer. All rights reserved.
Copyright (c) 2015 Backblaze, Inc. All rights reserved.
The MIT License (MIT)
*/

#include "ReedSolomon.h"
#include "InputOutputByteTableCodingLoop.h"

ReedSolomon ReedSolomon::create(int dataShardCount, int parityShardCount)
{
	return ReedSolomon(dataShardCount, parityShardCount, new InputOutputByteTableCodingLoop());
}
ReedSolomon::ReedSolomon(int dataShardCount, int parityShardCount, CodingLoop* codingLoop)
{
	this->dataShardCount = dataShardCount;
	this->parityShardCount = parityShardCount;
	this->codingLoop = codingLoop;
	this->totalShardCount = dataShardCount + parityShardCount;
	matrix = buildMatrix(dataShardCount, this->totalShardCount);
	parityRows = CVec2D(parityShardCount);
	for (int i = 0; i < parityShardCount; i++) 
	{
		parityRows[i] = matrix.getRow(dataShardCount + i);
	}
}
void ReedSolomon::encodeParity(CVec2D &shards, int offset, int byteCount)
{
	// Check arguments.
	checkBuffersAndSizes(shards, offset, byteCount);

	// Build the array of output buffers.
	CVec2D outputs = CVec2D(parityShardCount);
	int sz = shards[0].size();
	for (int x = 0; x < parityShardCount; x++)
	{
		outputs[x] = shards[dataShardCount + x];
	}

	// Do the coding.
	codingLoop->codeSomeShards(
		parityRows,
		shards, dataShardCount,
		outputs, parityShardCount,
		offset, byteCount);

	for (int x = 0; x < parityShardCount; x++)
	{
		for (int y = 0; y < sz; y++)
		{
			shards[dataShardCount + x][y] = outputs[x][y];
		}
	}

}
bool ReedSolomon::isParityCorrect(CVec2D &shards, int firstByte, int byteCount)
{
	// Check arguments.
	checkBuffersAndSizes(shards, firstByte, byteCount);

	// Build the array of buffers being checked.
	CVec2D toCheck = CVec2D(parityShardCount);
	int sz = shards[0].size();
	for (int x = 0; x < parityShardCount; x++)
	{
		for (int y = 0; y < sz; y++)
		{
			toCheck[x][y] = shards[x][y];
		}
	}

	// Do the checking.
	return codingLoop->checkSomeShards(
		parityRows,
		shards, dataShardCount,
		toCheck, parityShardCount,
		firstByte, byteCount);
}

#include <iostream>
void output(CVec2D &shards,int size = 4)
{
	for (int x = 0; x < size; x++)
	{
		std::cout << (int)shards[x][0] << ","; 
	}
	std::cout << "\n";
}

void ReedSolomon::decodeMissing(CVec2D &shards, BVec &shardPresent, int offset, int byteCount)
{
	// Check arguments.
	checkBuffersAndSizes(shards, offset, byteCount);

	// Quick check: are all of the shards present?  If so, there's
	// nothing to do.
	int numberPresent = 0;
	for (int i = 0; i < totalShardCount; i++) {
		if (shardPresent[i]) {
			numberPresent += 1;
		}
	}
	if (numberPresent == totalShardCount) {
		// Cool.  All of the shards data data.  We don't
		// need to do anything.
		return;
	}

	// Pull out the rows of the matrix that correspond to the
	// shards that we have and build a square matrix.  This
	// matrix could be used to generate the shards that we have
	// from the original data.
	//
	// Also, pull out an array holding just the shards that
	// correspond to the rows of the submatrix.  These shards
	// will be the input to the decoding process that re-creates
	// the missing data shards.
	Matrix subMatrix = Matrix(dataShardCount, dataShardCount);
	CVec2D subShards = CVec2D(dataShardCount);
	{
		int subMatrixRow = 0;
		for (int matrixRow = 0; matrixRow < totalShardCount && subMatrixRow < dataShardCount; matrixRow++) {
			if (shardPresent[matrixRow]) {
				for (int c = 0; c < dataShardCount; c++) {
					subMatrix.set(subMatrixRow, c, matrix.get(matrixRow, c));
				}
				subShards[subMatrixRow] = shards[matrixRow];
				subMatrixRow += 1;
			}
		}
	}

	// Invert the matrix, so we can go from the encoded shards
	// back to the original data.  Then pull out the row that
	// generates the shard that we want to decode.  Note that
	// since this matrix maps back to the orginal data, it can
	// be used to create a data shard, but not a parity shard.
	Matrix dataDecodeMatrix = subMatrix.invert();

	// Re-create any data shards that were missing.
	//
	// The input to the coding is all of the shards we actually
	// have, and the output is the missing data shards.  The computation
	// is done using the special decode matrix we just built.
	CVec2D outputs = CVec2D(parityShardCount);
	CVec2D matrixRows = CVec2D(parityShardCount);
	int outputCount = 0;
	for (int iShard = 0; iShard < dataShardCount; iShard++) {
		if (!shardPresent[iShard]) {
			outputs[outputCount] = shards[iShard];
			matrixRows[outputCount] = dataDecodeMatrix.getRow(iShard);
			outputCount += 1;
		}
	}

	codingLoop->codeSomeShards(
		matrixRows,
		subShards, dataShardCount,
		outputs, outputCount,
		offset, byteCount);
	
	// Now that we have all of the data shards intact, we can
	// compute any of the parity that is missing.
	//
	// The input to the coding is ALL of the data shards, including
	// any that we just calculated.  The output is whichever of the
	// data shards were missing.
	outputCount = 0;
	for (int iShard = dataShardCount; iShard < totalShardCount; iShard++) {
		if (!shardPresent[iShard]) {
			outputs[outputCount] = shards[iShard];
			matrixRows[outputCount] = parityRows[iShard - dataShardCount];
			outputCount += 1;
		}
	}

	codingLoop->codeSomeShards(
		matrixRows,
		shards, dataShardCount,
		outputs, outputCount,
		offset, byteCount);


	int count = 0;
	for (int iShard = 0; iShard < dataShardCount; iShard++) {
		if (!shardPresent[iShard]) {
			shards[iShard] = outputs[count];
			count += 1;
		}
	}

}

void ReedSolomon::checkBuffersAndSizes(CVec2D &shards, int offset, int byteCount)
{
	// The number of buffers should be equal to the number of
	// data shards plus the number of parity shards.
	if (shards.size() != totalShardCount) {
		throw std::exception("wrong number of shards: " + shards.size());
	}

	// All of the shard buffers should be the same length.
	int shardLength = shards[0].size();
	for (int i = 1; i < shards.size(); i++) {
		if (shards[i].size() != shardLength) {
			throw std::exception("Shards are different sizes");
		}
	}

	// The offset and byteCount must be non-negative and fit in the buffers.
	if (offset < 0) {
		throw std::exception("offset is negative: " + offset);
	}
	if (byteCount < 0) {
		throw std::exception("byteCount is negative: " + byteCount);
	}
	if (shardLength < offset + byteCount) {
		throw std::exception("buffers to small: " + byteCount + offset);
	}
}

Matrix ReedSolomon::buildMatrix(int dataShards, int totalShards)
{
	// Start with a Vandermonde matrix.  This matrix would work,
	// in theory, but doesn't have the property that the data
	// shards are unchanged after encoding.
	Matrix vandermonde = ReedSolomon::vandermonde(totalShards, dataShards);

	// Multiple by the inverse of the top square of the matrix.
	// This will make the top square be the identity matrix, but
	// preserve the property that any square subset of rows  is
	// invertible.
	Matrix top = vandermonde.submatrix(0, 0, dataShards, dataShards);
	return vandermonde.times(top.invert());
}

Matrix ReedSolomon::vandermonde(int rows, int cols)
{
	Matrix result = Matrix(rows, cols);
	for (int r = 0; r < rows; r++) {
		for (int c = 0; c < cols; c++) {
			result.set(r, c, Galois::exp(r, c));
		}
	}
	return result;
}
