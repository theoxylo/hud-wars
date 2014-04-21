//
//  HUDWarsAppDelegate.m
//  HUDWars
//
//  Created by Ted O'Neill on 8/29/09.
//  Copyright __MyCompanyName__ 2009. All rights reserved.
//

#import "HUDWarsAppDelegate.h"
#import "EAGLView.h"

@implementation HUDWarsAppDelegate

@synthesize window;
@synthesize glView;

- (void)applicationDidFinishLaunching:(UIApplication *)application {
    
	//Initialize OpenGL states
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	//glBindTexture(GL_TEXTURE_2D, [_textures[kTexture_Title] name]);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
    //Render the title splash full screen 320 x 480
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrthof(0, 320, 0, 480, -1, 1);
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glViewport(0, 0, 320, 480); // full screen
    glScissor(0, 0, 320, 480); // full screen
    
	//Load the title screen
    _splashScreen = [[Texture2D alloc] initWithImage: [UIImage imageNamed:@"Title.png"]];	
    [_splashScreen drawAtPoint:CGPointMake(160, 240)]; // offset to center
    [_splashScreen dealloc];
    
    //glDisable(GL_BLEND);
    // enable transparency
    //glEnable(GL_BLEND);
    
    // enable transparency
    glEnable(GL_BLEND);
    
    // commit the drawing to screen
	[glView swapBuffers];
}


// Release resources when they are no longer needed
- (void) dealloc 
{
	[glView release];
	[window release];
	
	[super dealloc];
}


// Changes the frame rate when the application is about to be inactive.
- (void)applicationWillResignActive:(UIApplication *)application {
	NSLog(@"applicationWillResignActive:");
}

// Resumes the initial frame rate when the application becomes active.
- (void)applicationDidBecomeActive:(UIApplication *)application {
	NSLog(@"applicationDidBecomeActive:");
}

@end
