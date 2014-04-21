#import <UIKit/UIKit.h>
#import <OpenGLES/EAGL.h>
#import <OpenGLES/EAGLDrawable.h>
#import <OpenGLES/ES1/gl.h>
#import <OpenGLES/ES1/glext.h>
#import "GameController.h"

//CLASSES:

@class EAGLView;

//PROTOCOLS:

@protocol EAGLViewDelegate <NSObject>
- (void) didResizeEAGLSurfaceForView:(EAGLView*)view; //Called whenever the EAGL surface has been resized
@end

//CLASS INTERFACE:

/*
 This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
 The view content is basically an EAGL surface you render your OpenGL scene into.
 Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
 */
@interface EAGLView : UIView
{
@private
	
	NSString*				_format;
	GLuint					_depthFormat;
	BOOL					_autoresize;
	EAGLContext				*_context;
	GLuint					_framebuffer;
	GLuint					_renderbuffer;
	GLuint					_depthBuffer;
	CGSize					_size;
	BOOL					_hasBeenCurrent;
	id<EAGLViewDelegate>	_delegate;
    
	// touch input data
    BOOL                _touch1Active;
    CGPoint             _touch1Location;    
    
    BOOL                _touch2Active;
    CGPoint             _touch2Location;
    
	BOOL				_gameStarted;
    
    NSTimer*				_timer;
	CFTimeInterval			_lastUpdateTimestamp;
	CFTimeInterval			_totalProcessingTime;
    
    GameController*         _gameController;


}
- (id)initWithCoder:(NSCoder*)coder; 

@property(readonly) GLuint framebuffer;
@property(readonly) NSString* pixelFormat;
@property(readonly) GLuint depthFormat;
@property(readonly) EAGLContext *context;

@property BOOL autoresizesSurface; //NO by default - Set to YES to have the EAGL surface automatically resized when the view bounds change, otherwise the EAGL surface contents is rendered scaled
@property(readonly, nonatomic) CGSize surfaceSize;

@property(assign) id<EAGLViewDelegate> delegate;

- (void) setCurrentContext;
- (BOOL) isCurrentContext;
- (void) clearCurrentContext;

- (void) swapBuffers; //This also checks the current OpenGL error and logs an error if needed


- (void)start;
- (void)stopRenderingTimer;
- (void)sendControlInput:(int)touchNum AndMode:(int)mode AndX:(float)x AndY:(float)y;
- (void)update;


@end
