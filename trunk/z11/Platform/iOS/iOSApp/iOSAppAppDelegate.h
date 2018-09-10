//
//  iOSAppAppDelegate.h
//  iOSApp
//
//  Created by Wang ZhiPeng on 11-5-24.
//  Copyright 2011年 Zhejiang University. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MIKit.h"

@class z11AppViewController;

using namespace z11;

@interface iOSAppAppDelegate : NSObject <UIApplicationDelegate> {
    GuiContainer* gui_container;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, assign) z11AppViewController *viewController;
@property (nonatomic) GuiContainer* gui_container;

@end

