#ifndef STEREOV_UTIL_H
#define STEREOV_UTIL_H

#include <stdio.h>
#include <string.h>
#include "global.h"

#define MallocMat(var, Type, nRow, nCol) \
	var = new Type*[nRow]; \
	for (int iMallocMat = 0; iMallocMat < nRow; iMallocMat++) { \
		var[iMallocMat] = new Type[nCol]; \
	}

#define FreeMat(var, nRow) \
	for (int iFreeMat = 0; iFreeMat < nRow; iFreeMat++) \
		delete[] var[iFreeMat]; \
	delete[] var;

void errExit();
void markTime(int t);

#endif