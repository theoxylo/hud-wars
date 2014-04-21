//
//  HUDWarsAppDelegate.h
//  HUDWars
//
//  Created by Ted O'Neill on 8/29/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "Texture2D.h"

@class EAGLView;

@interface HUDWarsAppDelegate : NSObject <UIApplicationDelegate, UIAlertViewDelegate>
{
	UIWindow* window;
	EAGLView* glView;
    
    Texture2D*				_splashScreen;
}


@property (nonatomic, retain) IBOutlet UIWindow *window;
@property (nonatomic, retain) IBOutlet EAGLView *glView;


@end

