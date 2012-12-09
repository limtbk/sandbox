//
//  FingerSnap.h
//  FingerSnap
//
//  Created by Ivan Klimchuk on 7/27/12.
//  Copyright (c) 2012 Ivan Klimchuk. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol FingerSnapDelegate <NSObject>

-(void) fingerSnapped;

@end

@interface FingerSnap : NSObject

- (id) initWithDelegate:(id<FingerSnapDelegate>) delegate;
-(void) startListening;
-(void) stopListening;

@property (nonatomic, readonly) BOOL isListening;
@property (nonatomic, assign) NSObject<FingerSnapDelegate> *delegate;

@end
