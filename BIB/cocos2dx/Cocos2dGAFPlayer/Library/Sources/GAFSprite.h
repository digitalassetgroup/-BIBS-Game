////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GAFSprite.h
//  GAF Animation Library
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#import "CCSprite.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct _gafBlendFuncSeparate
{
	// source blend function
	GLenum src;
	// destination blend function
	GLenum dst;
    // source alpha channel blend function
	GLenum srcAlpha;
    // destination alpha channel blend function
	GLenum dstAlpha;
    
} gafBlendFuncSeparate;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
@property (nonatomic, assign) float atlasScale;

// TODO: test childTransform as it was not used previously anywhere
@property (nonatomic, assign) BOOL useChildTransform;
@property (nonatomic, assign) CGAffineTransform childTransform;

@property (nonatomic, copy) NSString *objectId;
@property (nonatomic, copy) NSString *atlasElementId;

@property (nonatomic, assign) BOOL useSeparateBlendFunc;
@property (nonatomic, assign) gafBlendFuncSeparate blendFuncSeparate;

@property (nonatomic, assign) GLint blendEquation;

@end
