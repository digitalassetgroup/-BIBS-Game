#import "GAFSpriteWithAlpha.h"

@class CCRenderTexture;

@interface GAFBlurredSprite : GAFSpriteWithAlpha
{
@private
    CGSize _originalTextureSize;
    CGSize _blurredTextureSize;
    
    CCRenderTexture *_originalTexture;
    CCRenderTexture *_dynamicTexture1;
    CCRenderTexture *_dynamicTexture2;
    
    float * _gaussianValuesX;
    float * _gaussianValuesY;
    CGSize _gaussianValuesArraySize;
    
    // Shader uniforms
    GLint _vertShader_kernelSizeUniformLocation;
    GLint _vertShader_blurDotSizeUniformLocation;
    GLint _vertShader_kernelValuesUniformLocation;
    
    GLint _horzShader_kernelSizeUniformLocation;
    GLint _horzShader_blurDotSizeUniformLocation;
    GLint _horzShader_kernelValuesUniformLocation;
}

// After changing this value [self updateDynamicTexture] should be called to update texture
@property (nonatomic, assign) CGSize blurSize;
- (CGSize)normalizedBlurSizeFromNeededBlurSize:(CGSize)aSize;

// Set if initWithGAFSprite:blurSize: was called
@property (nonatomic, retain) GAFSpriteWithAlpha *sourceSprite;

// One and only method that should be used to initialize this class
- (id)initWithTexture:(CCTexture2D *)texture rect:(CGRect)rect rotated:(BOOL)rotated blurSize:(CGSize)aBlurSize;

- (id)initWithGAFSprite:(GAFSpriteWithAlpha *)aSprite blurSize:(CGSize)aBlurSize;

- (void)updateDynamicTexture;

@end
