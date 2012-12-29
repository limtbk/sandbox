//
//  ScannerDecoderProtocol.h
//  Shasta
//
//  Created by Ivan Klimchuk on 5/16/12.
//  Copyright (c) 2012 ENVIZIO Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol ScannerDecoderProtocol <NSObject>

-(NSString *) decodePictureInBuffer:(NSData *)buffer withWidth:(int)width andHeight:(int)height andBytesPerPixel:(int)bps;
-(BOOL) decodeComplete;

@end
