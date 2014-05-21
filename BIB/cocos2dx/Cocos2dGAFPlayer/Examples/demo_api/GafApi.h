#import "cocos2d.h"
#import "GAFAnimatedObject.h"

@class GAFAsset;
@class GAFAnimatedObject;

@interface GafApi : CCLayer <GAFSequenceDelegate>
{
	@private
	GAFAsset * _asset;
}
+(CCScene *) scene;
- (CCMenuItemImage*) addBtn:(NSString*) text px:(float)px py:(float)py  handler:(SEL)handler k:(float)k;
-(void) black;
-(void) white;
-(void) gray;
-(void) demo;

-(void) addToScene:(GAFAnimatedObject *)object px:(float) px py:(float) py sequenceName:(NSString*)sequenceName;

@end
