/*
Copyright (c) 2017 Alexander W. Plummer. All rights reserved.
Copyright (c) 2015 Backblaze, Inc. All rights reserved.
The MIT License (MIT)
*/

#pragma once
#include "CodingLoop.h" 
class InputOutputByteTableCodingLoop : CodingLoop
{
public:
	void InputOutputByteTableCodingLoop::codeSomeShards(
		CVec2D &matrixRows,
		CVec2D &inputs, int inputCount,
		CVec2D &outputs, int outputCount,
		int offset, int byteCount) override
	{
		CVec2D table = Galois::MULTIPLICATION_TABLE;
		{
			int iInput = 0; 
			for (int iOutput = 0; iOutput < outputCount; iOutput++) {   
				for (int iByte = offset; iByte < offset + byteCount; iByte++) { 
					outputs[iOutput][iByte] = table[matrixRows[iOutput][iInput] & 0xFF][inputs[iInput][iByte] & 0xFF];
				}
			}
		}

		for (int iInput = 1; iInput < inputCount; iInput++) { 
			for (int iOutput = 0; iOutput < outputCount; iOutput++) { 
				for (int iByte = offset; iByte < offset + byteCount; iByte++) {
					outputs[iOutput][iByte] ^= table[matrixRows[iOutput][iInput] & 0xFF][inputs[iInput][iByte] & 0xFF];
				}
			}
		}
	}
};