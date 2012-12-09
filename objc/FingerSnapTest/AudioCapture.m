//
//  AudioCapture.m
//  FingerSnapTest
//
//  Created by Ivan Klimchuk on 8/2/12.
//  Copyright (c) 2012 Ivan Klimchuk Inc. All rights reserved.
//

#import "AudioCapture.h"
#import <AVFoundation/AVFoundation.h>
#import <CoreMedia/CoreMedia.h>

@interface AudioCapture () <AVCaptureAudioDataOutputSampleBufferDelegate>

@property (nonatomic, assign) BOOL audioInitDone;
@property (nonatomic, retain) AVCaptureSession *capSession;

- (void)audioInit;

@end

@implementation AudioCapture

@synthesize isCapturing = _isCapturing;
@synthesize audioProcessor = _audioProcessor;
@synthesize audioInitDone = _audioInitDone;
@synthesize capSession = _capSession;

#pragma mark Lifecycle
- (id) init
{
    self = [super init];
    if (self) {
        self.audioInitDone = NO;
    }
    return self;
}

- (id) initWithProcessor:(id<AudioCaptureDelegate>) processor
{
    [self init];
    if (self) {
        self.audioProcessor = processor;
    }
    return self;
}

- (void)dealloc
{
    [_capSession release];
    [super dealloc];
}


#pragma mark FingerSnap interface methods
- (void)startCapture
{
    if (!self.audioInitDone) {
        [self audioInit];
    }
    if (self.audioInitDone) {
        [self.capSession startRunning];
        _isCapturing = YES;
    }
}

- (void)stopCapture
{
    if (self.audioInitDone) {
        [self.capSession stopRunning];
        _isCapturing = NO;
    }
}

#pragma mark Internal methods
- (void)audioInit
{
    if (!self.audioInitDone) {
        NSError *error = nil;
        
        // Setup the audio input
        AVCaptureDevice *audioDevice     = [AVCaptureDevice defaultDeviceWithMediaType: AVMediaTypeAudio];
        AVCaptureDeviceInput *audioInput = [AVCaptureDeviceInput deviceInputWithDevice:audioDevice error:&error ];     
        // Setup the audio output
        AVCaptureAudioDataOutput *audioOutput = [[AVCaptureAudioDataOutput alloc] init];
        
        // Create the session
        self.capSession = [[AVCaptureSession alloc] init];
        [self.capSession addInput:audioInput];
        [self.capSession addOutput:audioOutput];
        
        self.capSession.sessionPreset = AVCaptureSessionPresetLow;     
        
        // Setup the queue
        dispatch_queue_t queue = dispatch_queue_create("AudioCaptureQueue", NULL);
        [audioOutput setSampleBufferDelegate:self queue:queue];
        dispatch_release(queue);
    }
    
    self.audioInitDone = YES;
}

#pragma mark AVCaptureAudioDataOutputSampleBufferDelegate
- (void)captureOutput:(AVCaptureOutput *)captureOutput
didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer
       fromConnection:(AVCaptureConnection *)connection
{
    if ([self isCapturing]) {
        CMBlockBufferRef blockBuff = CMSampleBufferGetDataBuffer(sampleBuffer);
        
        size_t lengthAtOffset = 0;
        size_t totalLength = 0;
        char *dataRef = NULL;
        
        CMBlockBufferGetDataPointer(blockBuff, 0, &lengthAtOffset, &totalLength, &dataRef);
        
        NSData *data = [NSData dataWithBytes:dataRef length:lengthAtOffset];
        
        [self.audioProcessor processSample:data];
    }
}


@end
