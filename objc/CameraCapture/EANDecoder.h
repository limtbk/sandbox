//
//  EANDecoder.h
//  Shasta
//
//  Created by Ivan Klimchuk on 5/16/12.
//  Copyright (c) 2012 ENVIZIO Inc. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "ScannerDecoderProtocol.h"

@interface EANDecoder : NSObject <ScannerDecoderProtocol>

@property (nonatomic, retain) NSMutableArray *results;

@end
