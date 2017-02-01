#include "CodingLoop.h"

class InputOutputByteTableCodingLoop : CodingLoop
{
	void InputOutputByteTableCodingLoop::codeSomeShards(
		CVec2D matrixRows,
		CVec2D inputs, int inputCount,
		CVec2D outputs, int outputCount,
		int offset, int byteCount) override
	{
		CVec2D table = Galois::MULTIPLICATION_TABLE; 
		{
			int iInput = 0;
			CVec inputShard = inputs[iInput];
			for (int iOutput = 0; iOutput < outputCount; iOutput++) {
				CVec outputShard = outputs[iOutput];
				CVec matrixRow = matrixRows[iOutput];
				CVec multTableRow = table[matrixRow[iInput] & 0xFF];
				for (int iByte = offset; iByte < offset + byteCount; iByte++) {
					outputShard[iByte] = multTableRow[inputShard[iByte] & 0xFF];
				}
			}
		}

		for (int iInput = 1; iInput < inputCount; iInput++) {
			CVec inputShard = inputs[iInput];
			for (int iOutput = 0; iOutput < outputCount; iOutput++) {
				CVec outputShard = outputs[iOutput];
				CVec matrixRow = matrixRows[iOutput];
				CVec multTableRow = table[matrixRow[iInput] & 0xFF];
				for (int iByte = offset; iByte < offset + byteCount; iByte++) {
					outputShard[iByte] ^= multTableRow[inputShard[iByte] & 0xFF];
				}
			}
		}
	}
};