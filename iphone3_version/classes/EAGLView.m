#import <QuartzCore/QuartzCore.h>
#import "EAGLView.h"
#import "HUDWarsAppDelegate.h"

#define FRAMERATE_FPS				30.0 // Hz

//CLASS IMPLEMENTATIONS:

@implementation EAGLView

@synthesize delegate=_delegate, autoresizesSurface=_autoresize, surfaceSize=_size, framebuffer = _framebuffer, pixelFormat = _format, depthFormat = _depthFormat, context = _context;

+ (Class) layerClass
{
	return [CAEAGLLayer class];
}

- (BOOL) _createSurface
{
	CAEAGLLayer*			eaglLayer = (CAEAGLLayer*)[self layer];
	CGSize					newSize;
	GLuint					oldRenderbuffer;
	GLuint					oldFramebuffer;
	
	if(![EAGLContext setCurrentContext:_context]) {
		return NO;
	}
	
	newSize = [eaglLayer bounds].size;
	newSize.width = roundf(newSize.width);
	newSize.height = roundf(newSize.height);
	
	glGetIntegerv(GL_RENDERBUFFER_BINDING_OES, (GLint *) &oldRenderbuffer);
	glGetIntegerv(GL_FRAMEBUFFER_BINDING_OES, (GLint *) &oldFramebuffer);
	
	glGenRenderbuffersOES(1, &_renderbuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, _renderbuffer);
	
	if(![_context renderbufferStorage:GL_RENDERBUFFER_OES fromDrawable:eaglLayer]) {
		glDeleteRenderbuffersOES(1, &_renderbuffer);
		glBindRenderbufferOES(GL_RENDERBUFFER_BINDING_OES, oldRenderbuffer);
		return NO;
	}
	
	glGenFramebuffersOES(1, &_framebuffer);
	glBindFramebufferOES(GL_FRAMEBUFFER_OES, _framebuffer);
	glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_COLOR_ATTACHMENT0_OES, GL_RENDERBUFFER_OES, _renderbuffer);
	if (_depthFormat) {
		glGenRenderbuffersOES(1, &_depthBuffer);
		glBindRenderbufferOES(GL_RENDERBUFFER_OES, _depthBuffer);
		glRenderbufferStorageOES(GL_RENDERBUFFER_OES, _depthFormat, newSize.width, newSize.height);
		glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES, GL_DEPTH_ATTACHMENT_OES, GL_RENDERBUFFER_OES, _depthBuffer);
	}
	
	_size = newSize;
	if(!_hasBeenCurrent) {
		glViewport(0, 0, newSize.width, newSize.height);
		glScissor(0, 0, newSize.width, newSize.height);
		_hasBeenCurrent = YES;
	}
	else {
		glBindFramebufferOES(GL_FRAMEBUFFER_OES, oldFramebuffer);
	}
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, oldRenderbuffer);
	
	
	
	[_delegate didResizeEAGLSurfaceForView:self];
	
	return YES;
}

- (void) _destroySurface
{
	EAGLContext *oldContext = [EAGLContext currentContext];
	
	if (oldContext != _context)
		[EAGLContext setCurrentContext:_context];
	
	if(_depthFormat) {
		glDeleteRenderbuffersOES(1, &_depthBuffer);
		_depthBuffer = 0;
	}
	
	glDeleteRenderbuffersOES(1, &_renderbuffer);
	_renderbuffer = 0;
	
	glDeleteFramebuffersOES(1, &_framebuffer);
	_framebuffer = 0;
	
	if (oldContext != _context)
		[EAGLContext setCurrentContext:oldContext];
}

//The GL view is stored in the nib file. When it's unarchived it's sent -initWithCoder:
- (id)initWithCoder:(NSCoder*)coder 
{	
	if ((self = [super initWithCoder:coder])) 
    {
		CAEAGLLayer *eaglLayer = (CAEAGLLayer*)[self layer];
		
		[eaglLayer setDrawableProperties:[NSDictionary dictionaryWithObjectsAndKeys:[NSNumber numberWithBool:NO], kEAGLDrawablePropertyRetainedBacking, kEAGLColorFormatRGB565, kEAGLDrawablePropertyColorFormat, nil]];
		_format = kEAGLColorFormatRGB565;
		_depthFormat = 0;
		
		_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES1];
		if(_context == nil) {
			[self release];
			return nil;
		}
		
		if(![self _createSurface]) {
			[self release];
			return nil;
		}
	}
	return self;
}


- (void) dealloc
{
	[self _destroySurface];
	
	[_context release];
	_context = nil;
	
	[super dealloc];
}

- (void) layoutSubviews
{
	CGRect				bounds = [self bounds];
	
	if(_autoresize && ((roundf(bounds.size.width) != _size.width) || (roundf(bounds.size.height) != _size.height))) {
		[self _destroySurface];
#if __DEBUG__
		REPORT_ERROR(@"Resizing surface from %fx%f to %fx%f", _size.width, _size.height, roundf(bounds.size.width), roundf(bounds.size.height));
#endif
		[self _createSurface];
	}
}

- (void) setAutoresizesEAGLSurface:(BOOL)autoresizesEAGLSurface;
{
	_autoresize = autoresizesEAGLSurface;
	if(_autoresize)
		[self layoutSubviews];
}

- (void) setCurrentContext
{
	if(![EAGLContext setCurrentContext:_context]) {
		printf("Failed to set current context %p in %s\n", _context, __FUNCTION__);
	}
}

- (BOOL) isCurrentContext
{
	return ([EAGLContext currentContext] == _context ? YES : NO);
}

- (void) clearCurrentContext
{
	if(![EAGLContext setCurrentContext:nil])
		printf("Failed to clear current context in %s\n", __FUNCTION__);
}

- (void) swapBuffers
{
	EAGLContext *oldContext = [EAGLContext currentContext];
	GLuint oldRenderbuffer;
	
	if(oldContext != _context)
		[EAGLContext setCurrentContext:_context];
	
	
	
	glGetIntegerv(GL_RENDERBUFFER_BINDING_OES, (GLint *) &oldRenderbuffer);
	glBindRenderbufferOES(GL_RENDERBUFFER_OES, _renderbuffer);
	
	if(![_context presentRenderbuffer:GL_RENDERBUFFER_OES])
		printf("Failed to swap renderbuffer in %s\n", __FUNCTION__);
	
	if(oldContext != _context)
		[EAGLContext setCurrentContext:oldContext];
}

- (BOOL) isMultipleTouchEnabled
{
    return YES;
}

// from UIResponder interface
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    if (!_gameStarted) 
    {
        NSLog(@"Starting game...");
        _gameStarted = YES;
        [self start];
    } 
    
    NSArray* touchArray = [touches allObjects];
    
    int numTouchesBegan = [touchArray count];
    if (DEBUG_TOUCH) NSLog(@"Num touches down: %d", numTouchesBegan);
    
    // both touches already active, ignore touches beyond 2
    if (_touch1Active && _touch2Active) return; 
    
    // no current touches, so populate one or both
    if (!_touch1Active && !_touch2Active) 
    {
        _touch1Active = YES;
        _touch1Location = [[touchArray objectAtIndex:0] locationInView:self];
        [self sendControlInput:1 AndMode:iMode_touchDown AndX:_touch1Location.x AndY:_touch1Location.y];   
        
        if (numTouchesBegan > 1)
        {
            // second touch
            _touch2Active = YES;
            _touch2Location = [[touchArray objectAtIndex:1] locationInView:self];
            [self sendControlInput:2 AndMode:iMode_touchDown AndX:_touch2Location.x AndY:_touch2Location.y];   
        }
        return;
    }
    
    // exactly one touch already active, so populate the other
    if (!_touch1Active)
    {
        _touch1Active = YES;
        _touch1Location = [[touchArray objectAtIndex:0] locationInView:self];
        [self sendControlInput:1 AndMode:iMode_touchDown AndX:_touch1Location.x AndY:_touch1Location.y];   
    }    
    else
    {
        _touch2Active = YES;
        _touch2Location = [[touchArray objectAtIndex:0] locationInView:self];
        [self sendControlInput:2 AndMode:iMode_touchDown AndX:_touch2Location.x AndY:_touch2Location.y];   
    }
    return;
} // end touchesBegan

// from UIResponder interface
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{  	
    NSArray* touchArray = [touches allObjects];
    
    int numTouchesMoved = [touchArray count];
    //NSLog(@"Num touches moved: %d", numTouchesMoved);
    
    // neither touch active, ignore move
    if (!_touch1Active && !_touch2Active) return; 
    
    // both touches active, but only 1 moved
    if (_touch1Active && _touch2Active && numTouchesMoved == 1)
    {
        CGPoint touchMove = [[touchArray objectAtIndex:0] locationInView:self];
        int deltaX1 = touchMove.x - _touch1Location.x;
        int deltaY1 = touchMove.y - _touch1Location.y;        
        int deltaX2 = touchMove.x - _touch2Location.x;
        int deltaY2 = touchMove.y - _touch2Location.y;
        
        if ((deltaX1 * deltaX1 + deltaY1 * deltaY1) < (deltaX2 * deltaX2 + deltaY2 * deltaY2)) // closest to 1
        {
            _touch1Location = touchMove;
            [self sendControlInput:1 AndMode:iMode_touchMoved AndX:_touch1Location.x AndY:_touch1Location.y];
        }
        else // closest to 2
        {
            _touch2Location = touchMove;
            [self sendControlInput:2 AndMode:iMode_touchMoved AndX:_touch2Location.x AndY:_touch2Location.y];
        }
        return;
    }
    
    if (_touch1Active && _touch2Active && numTouchesMoved > 1) // both touches active, both moved
    {
        CGPoint touchMove = [[touchArray objectAtIndex:0] locationInView:self];
        int deltaX1 = touchMove.x - _touch1Location.x;
        int deltaY1 = touchMove.y - _touch1Location.y;        
        int deltaX2 = touchMove.x - _touch2Location.x;
        int deltaY2 = touchMove.y - _touch2Location.y;
        
        if ((deltaX1 * deltaX1 + deltaY1 * deltaY1) < (deltaX2 * deltaX2 + deltaY2 * deltaY2))
        {
            // first touch is closest to touch 1
            _touch1Location = touchMove;
            _touch2Location = [[touchArray objectAtIndex:1] locationInView:self];
        }
        else
        {
            // first touch is closest to touch 2
            _touch2Location = touchMove;
            _touch1Location = [[touchArray objectAtIndex:1] locationInView:self];
        }
        
        [self sendControlInput:1 AndMode:iMode_touchMoved AndX:_touch1Location.x AndY:_touch1Location.y];
        [self sendControlInput:2 AndMode:iMode_touchMoved AndX:_touch2Location.x AndY:_touch2Location.y];
        
        return;
    }    
    
    if (_touch1Active) // only touch 1 active
    {
        _touch1Location = [[touchArray objectAtIndex:0] locationInView:self];
        [self sendControlInput:1 AndMode:iMode_touchMoved AndX:_touch1Location.x AndY:_touch1Location.y];        
        return;
    }    
    
    if (_touch2Active) // only touch 2 active
    {
        _touch2Location = [[touchArray objectAtIndex:0] locationInView:self];
        [self sendControlInput:2 AndMode:iMode_touchMoved AndX:_touch2Location.x AndY:_touch2Location.y];        
        return;
    }
} // end touchesMoved

// from UIResponder interface
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    NSArray* touchArray = [touches allObjects];
    
    int numTouchesEnded = [touchArray count];
    if (DEBUG_TOUCH) NSLog(@"Num touches ended: %d", numTouchesEnded);
    
    
    if (!_touch1Active && !_touch2Active) 
    {
        // neither touch active, ignore event
        return;
    }
    
    if (_touch1Active && _touch2Active && numTouchesEnded == 1) // both touches active, only 1 ended
    {
        CGPoint touchMove = [[touchArray objectAtIndex:0] locationInView:self];
        int deltaX1 = touchMove.x - _touch1Location.x;
        int deltaY1 = touchMove.y - _touch1Location.y;        
        int deltaX2 = touchMove.x - _touch2Location.x;
        int deltaY2 = touchMove.y - _touch2Location.y;
        
        if ((deltaX1 * deltaX1 + deltaY1 * deltaY1) < (deltaX2 * deltaX2 + deltaY2 * deltaY2))
        {
            _touch1Active = NO; // closest to touch 1
            _touch1Location = touchMove;
            [self sendControlInput:1 AndMode:iMode_touchReleased AndX:_touch1Location.x AndY:_touch1Location.y];            
        }
        else
        {
            _touch2Active = NO; // closest to touch 2
            _touch2Location = touchMove;
            [self sendControlInput:2 AndMode:iMode_touchReleased AndX:_touch2Location.x AndY:_touch2Location.y];            
        }
        return;
    }
    
    if (_touch1Active && _touch2Active && numTouchesEnded > 1) // both touches active, both ended
    {
        _touch1Active = NO;
        _touch1Location = [[touchArray objectAtIndex:0] locationInView:self];
        [self sendControlInput:1 AndMode:iMode_touchReleased AndX:_touch1Location.x AndY:_touch1Location.y];            
        
        _touch2Active = NO;
        _touch2Location = [[touchArray objectAtIndex:1] locationInView:self];
        [self sendControlInput:2 AndMode:iMode_touchReleased AndX:_touch2Location.x AndY:_touch2Location.y];            
        
        return;
    }    
    
    if (_touch1Active) // only touch 1 active, so end it
    {
        _touch1Active = NO;
        _touch1Location = [[touchArray objectAtIndex:0] locationInView:self];
        [self sendControlInput:1 AndMode:iMode_touchReleased AndX:_touch1Location.x AndY:_touch1Location.y];            
        
        return;
    }    
    
    if (_touch2Active) // only touch 2 active, so end it
    {
        _touch2Active = NO;
        _touch2Location = [[touchArray objectAtIndex:0] locationInView:self];
        [self sendControlInput:2 AndMode:iMode_touchReleased AndX:_touch2Location.x AndY:_touch2Location.y];            
        
        return;
    }       
} // end touchesEnded


// from UIResponder interface
- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    // ?
    NSArray* touchArray = [touches allObjects];
    
    int numTouchesCanceled = [touchArray count];
    if (DEBUG_TOUCH) NSLog(@"Num touches canceled: %d", numTouchesCanceled);
}

- (void)start
{
    _lastUpdateTimestamp = 0; // 0 means "never been updated"
    
    _gameController = [GameController alloc];
    [_gameController initGame];
    
	
    
	// Either the user tapped to start a new game or the user successfully landed the rocket.
	//Stop rendering timer
	[_timer invalidate];
	_timer = nil;
	[UIApplication sharedApplication].idleTimerDisabled = NO;
	
	//Start rendering timer
	_timer = [NSTimer scheduledTimerWithTimeInterval:(1.0 / FRAMERATE_FPS) target:self selector:@selector(update) userInfo:nil repeats:YES];
	[UIApplication sharedApplication].idleTimerDisabled = YES;
    
}

- (void)stopRenderingTimer
{
    [_timer invalidate];
    _timer = nil;
    [UIApplication sharedApplication].idleTimerDisabled = NO;
}

- (void)sendControlInput:(int)touchNum AndMode:(int)mode AndX:(float)x AndY:(float)y
{
    if (DEBUG_TOUCH) NSLog(@"Touch %d, mode %d location x, y: %f, %f", touchNum, mode, x, y);
    [_gameController sendTouchControlInput:touchNum AndMode:mode AndX:x AndY:y];
}

// called by timer that is configured in start method above
- (void)update
{
    CFTimeInterval time = CFAbsoluteTimeGetCurrent();
    
    if (_lastUpdateTimestamp == 0) // 0 means "never been updated"
    {
        _lastUpdateTimestamp = time - (1.0 / FRAMERATE_FPS);
    }
    
    // iterate model
    [_gameController updateGame:(time - _lastUpdateTimestamp)];
    
    // save current time as last time for next iteration
    _lastUpdateTimestamp = time;
    
    // commit the drawing to screen
    [self swapBuffers];
}



@end
