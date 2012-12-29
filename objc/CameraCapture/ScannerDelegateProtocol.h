//
//  ScannerDelegateProtocol.h
//  Shasta
//
//  Created by Ivan Klimchuk on 5/16/12.
//  Copyright (c) 2012 ENVIZIO Inc. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol ScannerDelegateProtocol <NSObject>

-(void) captureComplete:(NSString *)result;

@end
