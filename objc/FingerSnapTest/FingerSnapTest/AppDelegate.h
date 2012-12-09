//
//  AppDelegate.h
//  FingerSnapTest
//
//  Created by Ivan Klimchuk on 8/2/12.
//  Copyright (c) 2012 ENVIZIO Inc. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "FingerSnap.h"

@interface AppDelegate : UIResponder <UIApplicationDelegate, FingerSnapDelegate>

@property (strong, nonatomic) UIWindow *window;
@property (strong, nonatomic) FingerSnap *fingerSnap;

@end
