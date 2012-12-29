//
//  UPCScannerInt.h
//  TestScanner
//
//  Created by Ivan Klimchuk on 3/22/12.
//  Copyright (c) 2012 ENVIZIO Inc. All rights reserved.
//

#ifndef TestScanner_UPCScannerInt_h
#define TestScanner_UPCScannerInt_h

void scanHorizLine(int *array, unsigned char *imgBytes, size_t imgWidth, size_t imgHeight, int scanThick);
void derivative(int *array, int *derivArray, size_t length);
void noiseReduction(int *array, int *filteredArray, size_t length, int noiseThreshold);


#endif
