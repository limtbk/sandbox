//
//  RootViewController.h
//  TestScanner
//
//  Created by Ivan Klimchuk on 3/13/12.
//  Copyright (c) 2012 ENVIZIO Inc. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <AVFoundation/AVFoundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import <CoreVideo/CoreVideo.h>
#import <CoreMedia/CoreMedia.h>

@interface RootViewController : UIViewController <AVCaptureVideoDataOutputSampleBufferDelegate> {
	AVCaptureSession *_captureSession;
	CALayer *_customLayer;
}

/*!
 @brief	The capture session takes the input from the camera and capture it
 */
@property (nonatomic, retain) AVCaptureSession *captureSession;

/*!
 @brief	The CALayer we use to display the CGImageRef generated from the imageBuffer
 */
@property (nonatomic, retain) CALayer *customLayer;

@property (nonatomic, retain) IBOutlet UIView *videoView;

@property (nonatomic, retain) IBOutlet UILabel *resultLabel;

/*!
 @brief	This method initializes the capture session
 */
- (void)initCapture;

-(IBAction)clearResult:(id)sender;


@end
