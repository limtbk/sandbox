//
//  FingerSnap.m
//  FingerSnap
//
//  Created by Ivan Klimchuk on 7/27/12.
//  Copyright (c) 2012 Ivan Klimchuk. All rights reserved.
//

#import "FingerSnap.h"
#import "AudioCapture.h"

#define snapsInterval 300.0
#define maxVolumeTreshold 5.0
#define minVolumeTreshold 2.0
#define avgCoef 10.0 //new : old = 10 : 1

@interface FingerSnap () <AudioCaptureDelegate> {
    double avgDelta;
    BOOL wasSound;
}

@property (nonatomic, retain) AudioCapture *audioCapture;
@property (nonatomic, assign) double lastSnapTime;

-(void) fingerSnap;

@end

@implementation FingerSnap

@synthesize delegate = _delegate;
@synthesize audioCapture = _audioCapture;
@synthesize isListening = _isListening;
@synthesize lastSnapTime = _lastSnapTime;


#pragma mark Lifecycle
- (id) init
{
    self = [super init];
    if (self) {
        _isListening = NO;
        wasSound = NO;
        self.lastSnapTime = 1000.0 * [[NSDate date] timeIntervalSince1970];
        avgDelta = 0;
    }
    return self;
}

- (id) initWithDelegate:(id<FingerSnapDelegate>) delegate
{
    [self init];
    if (self) {
        self.delegate = delegate;
    }
    return self;
}

- (void)dealloc
{
    [super dealloc];
}

#pragma mark FingerSnap interface methods
- (void)startListening
{
    NSLog(@"StartListening!");
    [self.audioCapture startCapture];
    
}

- (void)stopListening
{
    NSLog(@"StopListening!");
    [self.audioCapture stopCapture];

}

#pragma mark AudioCaptureDelegate

-(void) processSample:(NSData *)sampleData
{
    SInt16 *data = (SInt16 *)[sampleData bytes];
    size_t dataLength = [sampleData length]/sizeof(SInt16);
    
    SInt16 max = 0;
    SInt16 min = 0;
    int avg = 0;
    for (int i = 0; i < dataLength; i++) {
        max = MAX(max, data[i]);
        min = MIN(min, data[i]);
        avg += data[i];
    }
    int delta = max-min;
    
    if (delta > avgDelta * maxVolumeTreshold) {
        wasSound = YES;
    } else {
        if ((delta < avgDelta * minVolumeTreshold) && wasSound) {
            [self fingerSnap];
        }
        wasSound = NO;
    }
    avgDelta = (avgCoef * avgDelta + delta) / (avgCoef + 1);
    NSLog(@"%f %d", avgDelta, delta);
}

-(void) fingerSnap
{
    double currentSnapTime = 1000.0 * [[NSDate date] timeIntervalSince1970];
    if ((currentSnapTime - self.lastSnapTime) > snapsInterval) {
        [self.delegate performSelectorOnMainThread:@selector(fingerSnapped) withObject:self.delegate waitUntilDone:NO];
        NSLog(@"Fingersnap!");
        self.lastSnapTime = currentSnapTime;
    }
    
}

#pragma mark Lazy loading properties

-(AudioCapture *) audioCapture
{
    if (!_audioCapture) {
        _audioCapture = [[AudioCapture alloc] initWithProcessor:self];
    }
    return _audioCapture;
}


@end
