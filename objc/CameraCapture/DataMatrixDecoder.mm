//
//  DataMatrixDecoder.m
//  Shasta
//
//  Created by Ivan Klimchuk on 5/16/12.
//  Copyright (c) 2012 ENVIZIO Inc. All rights reserved.
//

#import "DataMatrixDecoder.h"

@implementation DataMatrixDecoder

@synthesize scanner = _scanner;

-(DxScanner *) scanner
{
    if (_scanner) {
        _scanner = new DxScanner();
    }
    return _scanner;
}

-(NSString *) decodePictureInBuffer:(NSData *)buffer withWidth:(int)width andHeight:(int)height andBytesPerPixel:(int)bps
{
    NSString *resultStr = nil;

    string *s = new string(self.scanner->Scan((unsigned char*)[buffer bytes], width, height, bps*8));
    
    if(s->size()>0) 
    {
        NAILog("datamatrix", @"Datamatrix found...");
        resultStr = [NSString stringWithCString:s->c_str() encoding:NSASCIIStringEncoding];
        NAILog("datamatrix", @"%@", resultStr);
    }
    else
    {
        NAILog("datamatrix", @"Datamatrix not found...");
    }
    delete s;
    
    return resultStr;
}

-(BOOL) decodeComplete
{
    return YES;
}

-(void) dealloc
{
    if (_scanner) {
        delete _scanner;
        _scanner = NULL;
    }
    [super dealloc];
}

@end