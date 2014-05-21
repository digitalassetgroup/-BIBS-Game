#import "GafApi.h"

#import "AppDelegate.h"
#import "GAFAsset.h"
#import "GAFSprite.h"
#import "GAFAnimatedObject.h"


static NSString * const kAnimationPath      = @"SampleAnimations/enemy_slime/export_enemy_slime.json";
static NSString * const kNamedAnimationPath = @"SampleAnimations/named/named.json";

@implementation GafApi

+(CCScene *) scene
{
	CCScene *scene = [CCScene node];
	GafApi *layer = [GafApi node];
	[scene addChild: layer];
	return scene;
}

-(id) init
{
	if( (self=[super init]) )
	{
		CGSize size = [[CCDirector sharedDirector] winSizeInPixels];
		float s = size.height / 640;
		if (s > 1) s = 1;
		//float dp = 0.13;
		
		NSMutableArray * items = [[[NSMutableArray alloc] init] autorelease];

		[items addObject: [self addBtn:@"B" px:0.75 py:0.95 handler:@selector(black) k:s]];
		[items addObject: [self addBtn:@"W" px:0.85 py:0.95 handler:@selector(white) k:s]];
		[items addObject: [self addBtn:@"G" px:0.95 py:0.95 handler:@selector(gray) k:s]];
				
		CCMenu * pMenu = [CCMenu menuWithArray:items];
		pMenu.position = CGPointZero;
		[self addChild:pMenu z:10000];
		
		[self demo];
		[self white];
		[self setTouchEnabled:YES];

	}
	return self;
}

-(void) demo
{
	// one-liner to create animation and run it
	GAFAnimatedObject * object = [GAFAnimatedObject createAndRun:kAnimationPath looped:YES];
	[self addToScene:object px:0.1 py:0.6 sequenceName:nil];
	object.tag = 10;
	object.sequenceDelegate = self;

	// using GAFAsset to spawn multiple instances of GAFAnimatedObject
	_asset = [GAFAsset create:kAnimationPath];
	
	[self addToScene:[_asset createObjectAndRun:YES] px:0.4 py:0.2 sequenceName:@"walk"];
	[self addToScene:[_asset createObjectAndRun:YES] px:0.4 py:0.5 sequenceName:@"attack"];
	[self addToScene:[_asset createObjectAndRun:YES] px:0.4 py:0.8 sequenceName:@"death"];
	
	object = [GAFAnimatedObject createAndRun:kNamedAnimationPath looped:YES];
	// you can set name of any movieclip in flash and, sprite in code, and use in any way
	GAFSprite * blue = [object getNamedSubobject:@"Blue"];
	[self addToScene:object px:0.5 py:0.5 sequenceName:nil];
	if (blue != nil)
	{
		GAFAnimatedObject * pathObject = [_asset createObjectAndRun:YES];
		// common pattern to attach nodes to your animation is making attachment child of
		// named GAFSprite and settting that sprite as locator
		[blue addChild:pathObject];
		[pathObject setPosition:CGPointZero];
		blue.isLocator = YES;
	}
}

-(void) addToScene:(GAFAnimatedObject *)object px:(float) px py:(float) py sequenceName:(NSString*)sequenceName
{
	[self addChild:object];
	CGSize size = [CCDirector sharedDirector].winSize;
	[object setPosition:CGPointMake(size.width * px, size.height * py)];
	if ([sequenceName length] > 0)
	{
		[object setSequenceWithName:sequenceName looped:YES];
	}
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

-(void) black
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

-(void) white
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

-(void) gray
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
}

- (void)ccTouchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{

}

- (void) dealloc
{
	[_asset release];
	[super dealloc];
}

- (void)onFinishSequence:(GAFAnimatedObject *)object sequenceName:(NSString*)sequenceName
{
	NSLog(@"onFinishSequence TAG:%d NAME:%@\n", object.tag, sequenceName);
}

@end
