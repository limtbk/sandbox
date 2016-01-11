//
//  ViewController.h
//  TestBLE
//
//  Created by lim on 1/10/16.
//  Copyright © 2016 lim. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <CoreBluetooth/CoreBluetooth.h>

@interface ViewController : UIViewController <CBCentralManagerDelegate, CBPeripheralDelegate>

@end

