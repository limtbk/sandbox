//
//  ScannerProtocol.h
//  Shasta
//
//  Created by Ivan Klimchuk on 5/16/12.
//  Copyright (c) 2012 ENVIZIO Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "ScannerDelegateProtocol.h"
#import "ScannerDecoderProtocol.h"

@protocol ScannerProtocol <NSObject>

-(void) beginCapture;
-(void) endCapture;

-(void) setScannerDelegate:(NSObject<ScannerDelegateProtocol> *)delegate;
-(void) setScannerDecoder:(NSObject<ScannerDecoderProtocol> *)decoder;

@end
