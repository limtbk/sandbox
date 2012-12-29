//
//  EANDecoder.m
//  Shasta
//
//  Created by Ivan Klimchuk on 5/16/12.
//  Copyright (c) 2012 ENVIZIO Inc. All rights reserved.
//

#import "EANDecoder.h"
#import "EAN_Decoder.h"

@implementation EANDecoder

@synthesize results = _results;

-(NSString *) decodePictureInBuffer:(NSData *)buffer withWidth:(int)width andHeight:(int)height andBytesPerPixel:(int)bps
{
    NSString *resultStr = nil;
    char resStr[30];
    resStr[0]='*';
    
    int res = EAN_Decoder ((const unsigned char*) [buffer bytes], height, width, bps*8, resStr, true, true); //int ns, int nc, int pf, char *BarStr, bool scanVertical, bool bigEndian)

    if(res==0)
    {
        resultStr = [NSString stringWithCString:resStr encoding:NSASCIIStringEncoding];
        [self.results addObject:resultStr];
        NAILog("ean", @"Scanned EAN: %@", resultStr);
    }
    
    return resultStr;
}

-(BOOL) decodeComplete
{
    if (self.results.count>0) {
        int lastIndex = self.results.count-1;
        NSString *lastResult = [self.results objectAtIndex:lastIndex];
        int matchCount = 0;
        for (NSString *res in self.results) {
            if ([res isEqualToString:lastResult]) {
                matchCount++;
            }
        }
        if (matchCount>2) { //The same code must be scanned more than 2 times
            self.results = nil;
            return YES;
        } else {
            return NO;
        }
    }
    else {
        return NO;
    }
}

-(NSMutableArray *)results
{
    if (!_results)
    {
        _results = [[NSMutableArray alloc] init];
    }
    return _results;
}

-(void) dealloc
{
    [_results retain];
    [super dealloc];
}

@end
