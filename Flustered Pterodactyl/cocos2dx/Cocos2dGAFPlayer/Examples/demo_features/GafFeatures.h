#import "cocos2d.h"

@class GAFAsset;

@interface GafFeatures : CCLayer
{
	@private
	GAFAsset * _asset;
	NSMutableArray * _objects;
	NSMutableArray * _jsons;
	int       _anim_index;
}

+(CCScene *) scene;

- (CCMenuItemImage*) addBtn:(NSString*) text px:(float)px py:(float)py  handler:(SEL)handler k:(float)k;

-(void) black;
-(void) white;
-(void) gray;
-(void) addOne;
-(void) prevFrame;
-(void) nextFrame;
-(void) removeOne;
-(void) set1;
-(void) set5;
-(void) set10;
-(void) set:(int)n;
-(void) addObjectsToScene:(int) aCount;
-(void) removeFromScene:(int) aCount;
-(void) restart;
-(void) playpause;
-(void) cleanup;
-(void) next_anim;
-(void) prev_anim;
-(int)  maxFrameNumber;
-(int)  frameNumber;
-(void) setFrameNumber:(int) aFrameNumber;

@end
