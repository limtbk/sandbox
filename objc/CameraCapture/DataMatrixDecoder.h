//
//  DataMatrixDecoder.h
//  Shasta
//
//  Created by Ivan Klimchuk on 5/16/12.
//  Copyright (c) 2012 ENVIZIO Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "ScannerDecoderProtocol.h"
#import "DxScanner.h"

_USING_MOTREUS_DMTX;

@interface DataMatrixDecoder : NSObject <ScannerDecoderProtocol>

@property (nonatomic, readonly) DxScanner *scanner;

@end
