//
//  Scanner.m
//  Shasta
//
//  Created by Ivan Klimchuk on 5/16/12.
//  Copyright (c) 2012 ENVIZIO Inc. All rights reserved.
//

#import "Scanner.h"
#import <AudioToolbox/AudioToolbox.h>

@implementation Scanner

@synthesize captureSession = _captureSession;
@synthesize customLayer = _customLayer;
@synthesize prevLayer = _prevLayer;

@synthesize delegate = _delegate;
@synthesize decoder = _decoder;

-(void) setup
{
    [self setBackgroundColor:[UIColor clearColor]];
    copyImageBuffer = YES;
    capturing = NO;
    captureDone = NO;
    [self beginCapture];
}

-(id) initWithCoder:(NSCoder *)aDecoder
{
    self = [super initWithCoder:aDecoder];
    if (self) {
        [self setup];
    }
    return self;
}

-(void) dealloc
{
    [self endCapture];
    [_prevLayer release];
    [_customLayer release];
    [_captureSession release];
    [super dealloc];
}

#pragma mark Camera methods
- (AVCaptureDevice *)captureDevice
{
    AVCaptureDevice* captureDevice = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
    cameraPresent = (captureDevice!=nil);
    
    if (cameraPresent) {
        [captureDevice lockForConfiguration:nil];
        
        if([captureDevice isWhiteBalanceModeSupported:AVCaptureWhiteBalanceModeContinuousAutoWhiteBalance]) {
            [captureDevice setWhiteBalanceMode:AVCaptureWhiteBalanceModeContinuousAutoWhiteBalance];
        }
        if([captureDevice isExposureModeSupported:AVCaptureExposureModeContinuousAutoExposure]) {
            [captureDevice setExposureMode:AVCaptureExposureModeContinuousAutoExposure];
        }
        if([captureDevice isFocusModeSupported:AVCaptureFocusModeContinuousAutoFocus]) {
            [captureDevice setFocusMode:AVCaptureFocusModeContinuousAutoFocus];
        }
    }
    return captureDevice;
}

- (AVCaptureVideoDataOutput *)captureOutput
{
    if (cameraPresent) {
        AVCaptureVideoDataOutput *captureOutput = [[[AVCaptureVideoDataOutput alloc] init] autorelease];
        captureOutput.alwaysDiscardsLateVideoFrames = YES;
        
        dispatch_queue_t queue;
        queue = dispatch_queue_create("cameraQueue", NULL);
        [captureOutput setSampleBufferDelegate:self queue:queue];
        dispatch_release(queue);
        
        NSString* key = (NSString *)kCVPixelBufferPixelFormatTypeKey; 
        NSNumber* value = [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA]; 
        NSDictionary* videoSettings = [NSDictionary dictionaryWithObject:value forKey:key]; 
        [captureOutput setVideoSettings:videoSettings];
        return captureOutput;
    }
    return nil;
}

-(AVCaptureSession *)captureSessionWithDevice:(AVCaptureDevice *)captureDevice andInput:(AVCaptureInput *)captureInput andOutput:(AVCaptureOutput *)captureOutput
{
    AVCaptureSession *captureSession = [[[AVCaptureSession alloc] init] autorelease];
    NSString *captureSessionPreset = AVCaptureSessionPreset640x480; //1280x720

    if([captureDevice supportsAVCaptureSessionPreset:captureSessionPreset])
        captureSession.sessionPreset = captureSessionPreset;
    else {
        self.captureSession.sessionPreset = AVCaptureSessionPresetMedium;
    }
    [captureSession addInput:captureInput];
    [captureSession addOutput:captureOutput];
    return captureSession;
}

- (void)beginCapture
{
    AVCaptureDevice *captureDevice = [self captureDevice];
    if (cameraPresent)
    {
        if (!capturing) {
            capturing = YES;
            captureDone = NO;
            
            
            AVCaptureDeviceInput *captureInput = [AVCaptureDeviceInput 
                                                  deviceInputWithDevice:captureDevice 
                                                  error:nil];
            
            AVCaptureVideoDataOutput *captureOutput = [self captureOutput];
            
            if (self.captureSession) {
                self.captureSession = nil; //releases old capture session
            }
            self.captureSession = [self captureSessionWithDevice:captureDevice andInput:captureInput andOutput:captureOutput];
            
            if (self.prevLayer) {
                [self.prevLayer removeFromSuperlayer];
                self.prevLayer = nil; //releases old preview layer
            }
            self.prevLayer = [AVCaptureVideoPreviewLayer layerWithSession:self.captureSession];
            self.prevLayer.frame = self.bounds;
            self.prevLayer.videoGravity = AVLayerVideoGravityResizeAspectFill;
            [self.layer addSublayer:self.prevLayer]; //replace old preview layer with new
            
            [self.captureSession startRunning];
        }
    }
}

-(void) endCapture
{
    if (cameraPresent) {
        if (capturing) {
            capturing = NO;
            captureDone = YES;
            [self.captureSession stopRunning];
            self.captureSession = nil; //release capture session
            [self.prevLayer removeFromSuperlayer];
            self.prevLayer = nil; //release capture session
        }
    }
}

- (void)captureOutput:(AVCaptureOutput *)captureOutput 
didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer 
	   fromConnection:(AVCaptureConnection *)connection 
{
    if (cameraPresent) {
        if (!captureDone) {
            NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
            
            CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer); 
            CVPixelBufferLockBaseAddress(imageBuffer,0);  //retain imageBuffer
            uint8_t *baseAddress = (uint8_t *)CVPixelBufferGetBaseAddress(imageBuffer);
            size_t width = CVPixelBufferGetWidth(imageBuffer); 
            size_t height = CVPixelBufferGetHeight(imageBuffer);
            
            NSData *data = nil;
            if (copyImageBuffer) {
                data = [NSData dataWithBytes:baseAddress length:width*height*4]; //copy imageBuffer to data;
                CVPixelBufferUnlockBaseAddress(imageBuffer,0); //release imageBuffer
            } else {
                data = [NSData dataWithBytesNoCopy:baseAddress length:width*height*4 freeWhenDone:NO]; //if we want to get faster, set data to imageBuffer, and release imageBuffer after decoding;
            }
            
            NSString *resultStr = nil;
            
            @try {
                resultStr = [self.decoder decodePictureInBuffer:data withWidth:width andHeight:height andBytesPerPixel:4];
            }
            @catch (NSException *exception) {
                NAELog("scanner", @"Exception %@", exception);
            }
            
            if (resultStr) {
                if ([self.decoder decodeComplete]) {
                    captureDone = YES;
                    [self performSelectorOnMainThread:@selector(captureComplete:) withObject:resultStr waitUntilDone:FALSE];
                    AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
                }
            }
            
            if (!copyImageBuffer) {
                CVPixelBufferUnlockBaseAddress(imageBuffer,0); //release imageBuffer if we use no_copy version
            }
            
            [pool drain];
        }
    }
    return;
}

-(void)captureComplete:(NSString *)resultStr
{
    if (cameraPresent) {
        captureDone = YES;
        [self endCapture];
        [self.delegate captureComplete:resultStr];
    }
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown);
    } else {
        return YES;
    }
}

-(void) setScannerDelegate:(NSObject<ScannerDelegateProtocol> *)delegate
{
    self.delegate = delegate;
}

-(void) setScannerDecoder:(NSObject<ScannerDecoderProtocol> *)decoder
{
    self.decoder = decoder;
}

@end
