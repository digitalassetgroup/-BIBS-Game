

#import "GafFeatures.h"

#include "cocos2d.h"

#import "GAFAsset.h"
#import "GAFAnimatedObject.h"

@interface GafFeatures ()

@property (nonatomic, strong) CCLabelTTF *objectCountLabel;
@property (nonatomic, strong) CCLabelTTF *animationNameLabel;

@end

@implementation GafFeatures

+ (CCScene *) scene
{
	CCScene *scene = [CCScene node];
	GafFeatures *layer = [GafFeatures node];
	[scene addChild: layer];
	
	return scene;
}

- (id) init
{
	if( (self=[super init]) )
	{
		CGSize size = [[CCDirector sharedDirector] winSizeInPixels];
		float s = size.height / 640;
		if (s > 1) s = 1;
		float dp = 0.13;
		
        [self addObjectCountLabel];
        [self addAnimationNameLabel];
        
		NSMutableArray * items = [[NSMutableArray alloc] init];

		[items addObject: [self addBtn:@"Play/Pause" px:0.95 py:0.95 handler:@selector(playpause) k:s]];
		[items addObject: [self addBtn:@"Restart" px:0.95 py:0.95-dp handler:@selector(restart) k:s]];
		[items addObject: [self addBtn:@"B" px:0.75 py:0.95- dp* 2 handler:@selector(black) k:s]];
		[items addObject: [self addBtn:@"W" px:0.85 py:0.95- dp* 2 handler:@selector(white) k:s]];
		[items addObject: [self addBtn:@"G" px:0.95 py:0.95- dp* 2 handler:@selector(gray) k:s]];
		[items addObject: [self addBtn:@"-" px:0.85 py:0.95- dp* 3 handler:@selector(removeOne) k:s]];
		[items addObject: [self addBtn:@"+" px:0.95 py:0.95- dp* 3 handler:@selector(addOne) k:s]];
		[items addObject: [self addBtn:@"1" px:0.75 py:0.95- dp* 4 handler:@selector(set1) k:s]];
		[items addObject: [self addBtn:@"5" px:0.85 py:0.95- dp* 4 handler:@selector(set5) k:s]];
		[items addObject: [self addBtn:@"10" px:0.95 py:0.95- dp* 4 handler:@selector(set10) k:s]];
		[items addObject: [self addBtn:@"fr. -" px:0.85 py:0.95- dp* 5 handler:@selector(prevFrame) k:s]];
		[items addObject: [self addBtn:@"fr. +" px:0.95 py:0.95- dp* 5 handler:@selector(nextFrame) k:s]];
		[items addObject: [self addBtn:@"Full cleanup" px:0.95 py:0.95- dp* 6 handler:@selector(cleanup) k:s]];
		[items addObject: [self addBtn:@"<<" px:0.85 py:0.95- dp* 7 handler:@selector(prev_anim) k:s]];
		[items addObject: [self addBtn:@">>" px:0.95 py:0.95- dp* 7 handler:@selector(next_anim) k:s]];
		
		CCMenu * pMenu = [CCMenu menuWithArray:items];
		
		pMenu.position = CGPointZero;
		[self addChild:pMenu z:10000];		
		_anim_index = 0;
        
        _jsons = @[@"SampleAnimations/critter_hit/critter_hit.json",
                  @"SampleAnimations/critter_hit/dirt_dig01_all.json",
                  @"SampleAnimations/critter_hit/dirt_dig02_all.json",
                  @"SampleAnimations/critter_hit/dirt_dig03_all.json",
                  @"SampleAnimations/critter_hit/dirt_dig_move.json",
                  @"SampleAnimations/critter_hit/dirt_growing_all.json",
                  @"SampleAnimations/critter_hit/dirt_static01_idle.json",
                  @"SampleAnimations/critter_hit/dirt_static02_idle.json",
                  @"SampleAnimations/critter_hit/dirt_static03_idle.json",
                  @"SampleAnimations/critter_hit/gem_blue_all.json",
                  @"SampleAnimations/critter_hit/gem_green_all.json",
                  @"SampleAnimations/critter_hit/gem_purple_all.json",
                  @"SampleAnimations/critter_hit/gem_rainbow_all.json",
                  @"SampleAnimations/critter_hit/gem_red_all.json",
                  @"SampleAnimations/critter_hit/gem_vanish.json",
                  @"SampleAnimations/critter_hit/gem_yellow_all.json",
                  @"SampleAnimations/critter_hit/piece_item.json",
                  @"SampleAnimations/critter_hit/skull_moving_all.json",
                  @"SampleAnimations/critter_hit/skull_static_all.json",
                  @"SampleAnimations/critter_hit/supergem_vfx_all.json"];
        
        //_jsons = [@[@"SampleAnimations/blur/blur.json"] retain];
        
		[self addObjectsToScene:1];
		[self white];
		[self setTouchEnabled:YES];
	}
	return self;
}


- (CCMenuItemImage*) addBtn:(NSString*) text px:(float)px py:(float)py  handler:(SEL)handler k:(float)k
{
	CGSize size = [[CCDirector sharedDirector] winSize];
	
	CCMenuItemImage *res = [CCMenuItemImage itemWithNormalImage:@"CloseNormal.png" selectedImage:@"CloseSelected.png" target:self selector:handler];

    res.position = ccp(size.width * px, size.height * py);
	res.scale = k;
	
	CCLabelTTF* pLabel = [CCLabelTTF labelWithString:text fontName:@"Thonburi" fontSize:34];
	pLabel.color = ccc3(0, 0, 255);
	
	pLabel.anchorPoint = ccp(1, 0.5);
	pLabel.scale = k;
    pLabel.position = ccp(res.position.x - [res contentSize].width * k * 0.5, res.position.y) ;
	[self addChild:pLabel z:100000];
	
	return res;
}

- (void)addObjectCountLabel
{
	self.objectCountLabel = [CCLabelTTF labelWithString:@"" fontName:@"Thonburi" fontSize:20];
	self.objectCountLabel.color = ccc3(0, 0, 0);
	self.objectCountLabel.anchorPoint = ccp(0, 0);
    self.objectCountLabel.position = ccp(80, 0);
	[self addChild:self.objectCountLabel z:100000];
}

- (void)setObjectCountTo:(int)anObjectCount
{
    self.objectCountLabel.string = [NSString stringWithFormat:@"Instance count: %d", anObjectCount];
}

- (void)addAnimationNameLabel
{
	self.animationNameLabel = [CCLabelTTF labelWithString:@"" fontName:@"Thonburi" fontSize:20];
	self.animationNameLabel.color = ccc3(0, 0, 0);
	self.animationNameLabel.anchorPoint = ccp(0, 0);
    self.animationNameLabel.position = ccp(20, 768 - 40);
	[self addChild:self.animationNameLabel z:100000];
}

- (void)setAnimationLabelText:(NSString *)aText
{
    self.animationNameLabel.string = [NSString stringWithFormat:@"Animation path: %@", aText];
}

- (void) black
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

- (void) white
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

- (void) gray
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
}


- (void) prevFrame
{
	int f = [self frameNumber];
	if (-1 == f)
	{
		return;
	}
	if (f != [self maxFrameNumber])
	{
		[self setFrameNumber:f - 1];
	}
}

- (void) nextFrame
{
	int f = [self frameNumber];
	if (-1 == f)
	{
		return;
	}
	if (f != [self maxFrameNumber])
	{
		[self setFrameNumber:f + 1];
	}
}

- (void) addOne
{
	[self addObjectsToScene:1];
}

- (void) removeOne
{
	[self removeFromScene:1];
}

- (void) set:(int)n
{
	if (_objects == nil)
	{
		_objects = [[NSMutableArray alloc] init];
	}
	int c = [_objects count];
	if (c == n)
	{
		return;
	}
	if (n > c)
	{
		[self addObjectsToScene:n - c];
	}
	else
	{
		[self removeFromScene:c - n];
	}
}

- (void)ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	if (_objects == nil || 0 == [_objects count])
	{
		return;
	}
	
	CCNode * node = (CCNode*) _objects[0];
	UITouch * pTouch = (UITouch *)[touches anyObject];
	CGPoint pt = [pTouch locationInView: [CCDirector sharedDirector].view ];
	pt.y = [CCDirector sharedDirector].winSize.height - pt.y;
	node.position = pt;
}

- (void) addObjectsToScene:(int) aCount
{
	if (_asset == nil)
	{
		_asset = [[GAFAsset alloc] initWithJSONAtPath:_jsons[_anim_index] keepImagesInAtlas:NO];
        
        [self setAnimationLabelText:_jsons[_anim_index]];
	}
	if (_objects == nil)
	{
		_objects = [[NSMutableArray alloc] init];
	}
	if (_asset != nil)
	{
		int initialCount = [_objects count];
		for (int i = initialCount; i < initialCount + aCount; ++i)
		{
			GAFAnimatedObject *object = [GAFAnimatedObject animatedObjectWithAsset:_asset];
            object.isLooped = YES;
            [object start];
			object.zOrder = 100 * i;
			object.scale = 1.0f;
			[self addChild:object z:i + 100];
			CGSize winSize = [[CCDirector sharedDirector] winSize];
            
            if (initialCount == 0)
            {
                object.position = ccp(winSize.width * 0.5, winSize.height * 0.5);
            }
            else
            {
                const int rightOffset = 300;
                const int border = 100;
                object.position = ccp(arc4random()%(int)(winSize.width - 2 * border - rightOffset) + border,
                                      arc4random()%(int)(winSize.height - 2 * border) + border);
            }
            
			[_objects addObject:object];
			// will work only if animation has sequence
			[object playSequenceWithName:@"walk" looped:YES];
		}
	}
    
    [self setObjectCountTo:_objects.count];
}

- (void) removeFromScene:(int) aCount
{
	if (_objects == nil)
	{
		return;
	}
	if (aCount > [_objects count])
	{
		aCount = [_objects count];
	}
	for (int i = 0; i < aCount; ++i)
	{
		GAFAnimatedObject *obj = (GAFAnimatedObject *)[_objects lastObject];
		[obj removeFromParentAndCleanup:YES];
		[_objects removeLastObject];
	}
    
    [self setObjectCountTo:_objects.count];
}

- (void) set1
{
	[self set:1];
}

- (void) set5
{
	[self set:5];
}

- (void) set10
{
	[self set:10];
}

- (void) restart
{
	if (_objects == nil || 0 == [_objects count])
	{
		return;
	}
	GAFAnimatedObject *object = (GAFAnimatedObject *)_objects[0];
	[object stop];
	[object start];
}

- (void) playpause
{
	if (_objects == nil || 0 == [_objects count])
	{
		return;
	}
	GAFAnimatedObject *object = (GAFAnimatedObject *)_objects[0];
	
	if (object.isRunning)
	{
		[object pause];
	}
	else
	{
		[object resume];
	}
}

- (void) cleanup
{
	_asset = nil;
	if (_objects == nil || 0 == [_objects count])
	{
		return;
	}
	[self removeFromScene:[_objects count]];
	
	_objects = nil;
}

- (int) maxFrameNumber
{
	if (_objects == nil || 0 == [_objects count])
	{
		return -1;
	}
	GAFAnimatedObject *object = (GAFAnimatedObject *)_objects[0];
	return object.totalFrameCount;
}

- (void) setFrameNumber:(int) aFrameNumber
{
	if (_objects == nil || 0 == [_objects count])
	{
		return;
	}
	GAFAnimatedObject *object = (GAFAnimatedObject *)_objects[0];
	[object setFrame:aFrameNumber];
}

- (int) frameNumber
{
	if (_objects == nil || 0 == [_objects count])
	{
		return -1;
	}
	GAFAnimatedObject *object = (GAFAnimatedObject *)_objects[0];
	return object.currentFrameIndex;
}

- (void) next_anim
{
	if (_jsons == nil || 0 == [_jsons count])
	{
		return;
	}
	[self cleanup];
	++_anim_index;
	if (_anim_index >= [_jsons count])
	{
		_anim_index = 0;
	}
	[self addObjectsToScene:1];
}

- (void) prev_anim
{
	if (_jsons == nil || 0 == [_jsons count])
	{
		return;
	}
	[self cleanup];
	--_anim_index;
	if (_anim_index < 0)
	{
		_anim_index = [_jsons count] - 1;
	}
	[self addObjectsToScene:1];
}

@end
