typedef enum
{
	GAFCTI_R,
	GAFCTI_G,
	GAFCTI_B,
	GAFCTI_A,
} GAFColorTransformIndex;

@interface GAFSubobjectState : NSObject <NSCopying>
{
	CGFloat _colorMults[4];
	CGFloat _colorOffsets[4];
}
@property (nonatomic,   copy) NSString *objectId;
@property (nonatomic, assign) NSInteger zIndex;
@property (nonatomic,   copy) NSString *maskObjectId;
@property (nonatomic,   copy) NSString *atlasElementName;
@property (nonatomic, assign) CGAffineTransform affineTransform;
@property (nonatomic, retain) NSDictionary *filters;

- (id)initWithObjectId:(NSString *)objectId;
- (id)initWithStateDictionary:(NSDictionary*)dict objectId:(NSString*)objectId;

- (id)copyWithZone:(NSZone *)zone;
- (BOOL) isVisible;
- (GLfloat*) colorMults;
- (GLfloat*) colorOffsets;

@end
