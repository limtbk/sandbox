//
//  AudioCapture.h
//  FingerSnapTest
//
//  Created by Ivan Klimchuk on 8/2/12.
//  Copyright (c) 2012 Ivan Klimchuk Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol AudioCaptureDelegate <NSObject>

-(void) processSample:(NSData *)sampleData;

@end

@interface AudioCapture : NSObject

-(void) startCapture;
-(void) stopCapture;
- (id) initWithProcessor:(id<AudioCaptureDelegate>) processor;

@property (nonatomic, readonly) BOOL isCapturing;
@property (nonatomic, assign) id<AudioCaptureDelegate> audioProcessor;

@end
