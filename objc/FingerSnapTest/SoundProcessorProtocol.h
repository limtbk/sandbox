//
//  SoundProcessorProtocol.h
//  FingerSnapTest
//
//  Created by Ivan Klimchuk on 8/2/12.
//  Copyright (c) 2012 Ivan Klimchuk Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol SoundProcessorProtocol <NSObject>

- (void) processAudioSample:(NSData *)sample;

@end
