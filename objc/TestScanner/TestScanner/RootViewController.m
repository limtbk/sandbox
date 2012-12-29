//
//  RootViewController.m
//  TestScanner
//
//  Created by Ivan Klimchuk on 3/13/12.
//  Copyright (c) 2012 ENVIZIO Inc. All rights reserved.
//

#import "RootViewController.h"
#import "UPCScanner.h"

@implementation RootViewController

@synthesize captureSession = _captureSession;
@synthesize customLayer = _customLayer;
@synthesize videoView = _videoView;
@synthesize resultLabel = _resultLabel;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}

#pragma mark - View lifecycle

- (void)viewDidLoad
{
    //[super viewDidLoad];
    [self initCapture];
    // Do any additional setup after loading the view from its nib.
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}


#pragma mark -
#pragma mark Initialization
- (id)init {
	self = [super init];
	if (self) {
		/*We initialize some variables (they might be not initialized depending on what is commented or not)*/
		self.customLayer = nil;
	}
	return self;
}

- (void)initCapture {
	/*We setup the input*/
	AVCaptureDeviceInput *captureInput = [AVCaptureDeviceInput 
										  deviceInputWithDevice:[AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo] 
										  error:nil];
	/*We setupt the output*/
	AVCaptureVideoDataOutput *captureOutput = [[AVCaptureVideoDataOutput alloc] init];
	/*While a frame is processes in -captureOutput:didOutputSampleBuffer:fromConnection: delegate methods no other frames are added in the queue.
	 If you don't want this behaviour set the property to NO */
	captureOutput.alwaysDiscardsLateVideoFrames = YES; 
	/*We specify a minimum duration for each frame (play with this settings to avoid having too many frames waiting
	 in the queue because it can cause memory issues). It is similar to the inverse of the maximum framerate.
	 In this example we set a min frame duration of 1/10 seconds so a maximum framerate of 10fps. We say that
	 we are not able to process more than 10 frames per second.*/
	//captureOutput.minFrameDuration = CMTimeMake(1, 10);
    
	/*We create a serial queue to handle the processing of our frames*/
	dispatch_queue_t queue;
	queue = dispatch_queue_create("cameraQueue", NULL);
	[captureOutput setSampleBufferDelegate:self queue:queue];
	dispatch_release(queue);
	// Set the video output to store frame in BGRA (It is supposed to be faster)
	NSString* key = (NSString*)kCVPixelBufferPixelFormatTypeKey; 
	NSNumber* value = [NSNumber numberWithUnsignedInt:kCVPixelFormatType_32BGRA]; 
	NSDictionary* videoSettings = [NSDictionary dictionaryWithObject:value forKey:key]; 
	[captureOutput setVideoSettings:videoSettings]; 
	/*And we create a capture session*/
	self.captureSession = [[AVCaptureSession alloc] init];
	/*We add input and output*/
	[self.captureSession addInput:captureInput];
	[self.captureSession addOutput:captureOutput];
//    [self.captureSession setSessionPreset:AVCaptureSessionPresetHigh];
    [self.captureSession setSessionPreset:AVCaptureSessionPresetPhoto];
	/*We add the Custom Layer (We need to change the orientation of the layer so that the video is displayed correctly)*/
	self.customLayer = [CALayer layer];
	self.customLayer.frame = CGRectMake(self.videoView.frame.origin.x+20, self.videoView.frame.origin.y, self.videoView.frame.size.width, self.videoView.frame.size.height);
    
	self.customLayer.transform = CATransform3DRotate(CATransform3DIdentity, M_PI/2.0f, 0, 0, 1);
	self.customLayer.contentsGravity = kCAGravityResizeAspectFill;
	[self.videoView.layer addSublayer:self.customLayer];
	[self.captureSession startRunning];
    
}

#pragma mark -
#pragma mark AVCaptureSession delegate

- (void)captureOutput:(AVCaptureOutput *)captureOutput 
didOutputSampleBuffer:(CMSampleBufferRef)sampleBuffer 
	   fromConnection:(AVCaptureConnection *)connection 
{ 
	/*We create an autorelease pool because as we are not in the main_queue our code is
	 not executed in the main thread. So we have to create an autorelease pool for the thread we are in*/
    
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    
    CVImageBufferRef imageBuffer = CMSampleBufferGetImageBuffer(sampleBuffer); 
    /*Lock the image buffer*/
    CVPixelBufferLockBaseAddress(imageBuffer,0); 
    /*Get information about the image*/
    uint8_t *baseAddress = (uint8_t *)CVPixelBufferGetBaseAddress(imageBuffer); 
    size_t bytesPerRow = CVPixelBufferGetBytesPerRow(imageBuffer); 
    size_t width = CVPixelBufferGetWidth(imageBuffer); 
    size_t height = CVPixelBufferGetHeight(imageBuffer);
    
    int resCStrMaxLength = 12;
    NSMutableData *resCStrData = [NSMutableData dataWithLength:sizeof(char) * (resCStrMaxLength + 1)];
    char *resCStr = [resCStrData mutableBytes];

    int resCStrLength = 0;
//    NSLog(@"start");
    resCStrLength = scanAndReadUPC(baseAddress, height, width, resCStr, resCStrMaxLength);
//    NSLog(@"stop");
    
    if (resCStrLength==12)
    {
        NSLog(@"RESULT = %s", resCStr);
        NSString *resStr = [NSString stringWithCString:resCStr encoding:NSUTF8StringEncoding];
        [self.resultLabel performSelectorOnMainThread:@selector(setText:) withObject: (id) resStr waitUntilDone:YES];
    }
    
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef newContext = CGBitmapContextCreate(baseAddress, width, height, 8, bytesPerRow, colorSpace, kCGBitmapByteOrder32Little | kCGImageAlphaPremultipliedFirst);
    CGImageRef newImage = CGBitmapContextCreateImage(newContext); 
    CGContextRelease(newContext); 
    CGColorSpaceRelease(colorSpace);
	[self.customLayer performSelectorOnMainThread:@selector(setContents:) withObject: (id) newImage waitUntilDone:YES];
	CGImageRelease(newImage);
	CVPixelBufferUnlockBaseAddress(imageBuffer,0);
	[pool drain];
} 

#pragma mark -
#pragma mark Memory management

- (void)viewDidUnload {
	self.customLayer = nil;
}

- (void)dealloc {
	[self.captureSession release];
    [super dealloc];
}

-(IBAction)clearResult:(id)sender {
    self.resultLabel.text = @"";
}

@end
