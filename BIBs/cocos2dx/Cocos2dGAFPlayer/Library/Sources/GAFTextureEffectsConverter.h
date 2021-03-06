////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GAFTextureEffectsConverter.h
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Imports

#import "cocos2d.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Predeclarations

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constants

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Interface

@interface GAFTextureEffectsConverter : NSObject

+ (GAFTextureEffectsConverter *)sharedConverter;

- (CCRenderTexture *)gaussianBlurredTextureFromTexture:(CCTexture2D *)aTexture
                                                  rect:(CGRect)rect
                                           blurRadiusX:(CGFloat)aBlurRadiusX
                                           blurRadiusY:(CGFloat)aBlurRadiusY;

@end
