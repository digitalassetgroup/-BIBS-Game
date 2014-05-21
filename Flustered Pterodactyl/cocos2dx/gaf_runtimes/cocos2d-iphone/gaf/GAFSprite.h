#import "CCSprite.h"

typedef struct _ccBlendFuncSeparate
{
	// source blend function
	GLenum src;
	// destination blend function
	GLenum dst;
    // source alpha channel blend function
	GLenum srcAlpha;
    // destination alpha channel blend function
	GLenum dstAlpha;
    
} ccBlendFuncSeparate;

@interface GAFSprite : CCSprite
{
@private
    CGAffineTransform _externalTransform;
    CGAffineTransform _childTransform;
}

// By setting this parameter all other state values except 'anchorPoint' will be ignored
@property (nonatomic, assign) BOOL isLocator;
@property (nonatomic, assign) BOOL useExternalTransform;
@property (nonatomic, assign) CGAffineTransform externalTransform;

// TODO: test childTransform as it was not used previously anywhere
@property (nonatomic, assign) BOOL useChildTransform;
@property (nonatomic, assign) CGAffineTransform childTransform;

@property (nonatomic, copy) NSString *objectId;

@property (nonatomic, assign) BOOL useSeparateBlendFunc;
@property (nonatomic, assign) ccBlendFuncSeparate blendFuncSeparate;

@property (nonatomic, assign) GLint blendEquation;

@end
