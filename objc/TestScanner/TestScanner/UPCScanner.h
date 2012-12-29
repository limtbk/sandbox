//
//  UPCScanner.h
//  TestScanner
//
//  Created by Ivan Klimchuk on 3/16/12.
//  Copyright (c) 2012 ENVIZIO Inc. All rights reserved.
//

#ifndef TestScanner_UPCScanner_h
#define TestScanner_UPCScanner_h

int scanAndReadUPC(uint8_t *imgAddress, size_t imgHeight, size_t imgWidth, char *resultCString, int resultMaxLength);

#endif
