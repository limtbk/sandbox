//
//  Scanner.h
//  Shasta
//
//  Created by Ivan Klimchuk on 5/16/12.
//  Copyright (c) 2012 ENVIZIO Inc. All rights reserved.
//

#ifdef __APPLE__
#include "TargetConditionals.h"
#endif

#import <UIKit/UIKit.h>
#import "ScannerProtocol.h"

#import <UIKit/UIKit.h>
#import <CoreFoundation/CoreFoundation.h>
#import <AVFoundation/AVFoundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import <CoreVideo/CoreVideo.h>
#import <CoreMedia/CoreMedia.h>


@interface Scanner : UIView <ScannerProtocol
#if !(TARGET_IPHONE_SIMULATOR)
,AVCaptureVideoDataOutputSampleBufferDelegate
#endif
> {
    BOOL captureDone;
    int captureFrame;
    BOOL curFrameCaptured;
    BOOL capturing;
    BOOL copyImageBuffer;
    BOOL cameraPresent;
}

@property (nonatomic, retain) AVCaptureSession *captureSession;
@property (nonatomic, retain) CALayer *customLayer;
@property (nonatomic, retain) AVCaptureVideoPreviewLayer *prevLayer;

@property (nonatomic, retain) IBOutlet NSObject<ScannerDecoderProtocol> *decoder;
@property (nonatomic, assign) IBOutlet NSObject<ScannerDelegateProtocol> *delegate;


@end
