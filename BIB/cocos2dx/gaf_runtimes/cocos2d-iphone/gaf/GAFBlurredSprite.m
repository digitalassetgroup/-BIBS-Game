#import "GAFBlurredSprite.h"
#import "CCRenderTexture.h"
#import "CCShaderCache.h"
#import "CCGLProgram.h"
#import "ccMacros.h"
#import "ccShaders.h"

static bool isOdd (int number)
{
    return (number % 2) != 0;
}

static bool isEven (int number)
{
    return (number % 2) == 0;
}

static float gaussianValue (int x, float ro)
{
    return (exp(- (x * x) / (2 * ro * ro)) / sqrt(2 * M_PI * ro * ro));
}

static float * gaussianMatrixRow (int size, float ro)
{
    assert(size != 0);
    //assert((size - 1) % 2 == 0);
    
    float * result = (float*) malloc(sizeof(float) * size);
    int center = (size - 1) / 2;
    
    bool oddSize = ((size - 1) % 2 == 0);
    if (!oddSize)
    {
        size -= 1;
    }
    
    if (size == 1)
    {
        result[0] = 1.0;
    }
    else
    {
        for (int x = 0; (center - x) >= 0; x++)
        {
            result[center + x] = result[center - x] = gaussianValue(x, ro);
        }
    }
    
    // Workaround for even sizes
    if (!oddSize)
    {
        // actually result's size == size + 1
        result[size] = 0;
    }
    
    return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constants

static NSString * const kBlurredSpriteHorizontalBlurShaderFilename = @"pcBlurredSprite_Shader_HorizontalBlur.fs";
static NSString * const kBlurredSpriteVerticalBlurShaderFilename = @"pcBlurredSprite_Shader_VerticalBlur.fs";

static NSString * const kGAFBlurredSpriteHorizontalBlurShaderProgramCacheKey = @"kGAFBlurredSpriteHorizontalBlurShaderProgramCacheKey";
static NSString * const kGAFBlurredSpriteVerticalBlurShaderProgramCacheKey = @"kGAFBlurredSpriteVerticalBlurShaderProgramCacheKey";

static int const kDefaultBlurSize = 7;
static int const kMaxGaussianMatrixSize = 51;
static int const kMaxBlurSize = 25; // kMaxBlurSize * 2 + 1 = kMaxGaussianMatrixSize

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private interface

@interface GAFBlurredSprite ()

@property (nonatomic, retain) CCRenderTexture *originalTexture;
@property (nonatomic, retain) CCRenderTexture *dynamicTexture1;
@property (nonatomic, retain) CCRenderTexture *dynamicTexture2;
@property (nonatomic, retain) CCSprite *dynamicSprite;

- (CGRect)blurTextureRectForOriginalRect:(CGRect)anOriginalRect;

- (void)prepareDynamicsForSize:(CGSize)aSize;
- (void)createGaussianMatrixWithSize:(CGSize)aSize;

@end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation

@implementation GAFBlurredSprite

#pragma mark -
#pragma mark Properties

@synthesize blurSize = _blurSize;
@synthesize originalTexture = _originalTexture;
@synthesize dynamicTexture1 = _dynamicTexture1;
@synthesize dynamicTexture2 = _dynamicTexture2;
@synthesize dynamicSprite = _dynamicSprite;
@synthesize sourceSprite;

- (void)setBlurSize:(CGSize)blurSize
{
    BOOL adjusted = NO;
    if (blurSize.width > kMaxBlurSize)
    {
        blurSize.width = kMaxBlurSize;
        adjusted = YES;
    }
    if (blurSize.height > kMaxBlurSize)
    {
        blurSize.height = kMaxBlurSize;
        adjusted = YES;
    }
    if (adjusted)
    {
        CCLOGWARN(@"blur size should not be greater than %d. Using maximum value instead.", kMaxBlurSize);
    }
    
    if (!CGSizeEqualToSize(_blurSize, blurSize))
    {
        _blurSize = [self normalizedBlurSizeFromNeededBlurSize:blurSize];
        [self createGaussianMatrixWithSize:CGSizeMake((_blurSize.width * 2) + 1,
                                                      (_blurSize.height * 2) + 1)];
    }
}

- (CGSize)normalizedBlurSizeFromNeededBlurSize:(CGSize)aSize
{
    CGSize result = CGSizeMake(MIN(aSize.width, kMaxGaussianMatrixSize),
                               MIN(aSize.height, kMaxGaussianMatrixSize));
    
    result.width = result.width + (isOdd((int)result.width) ? 0 : 1);
    result.height = result.height + (isOdd((int)result.height) ? 0 : 1);
    return result;
}

- (CGRect)blurTextureRectForOriginalRect:(CGRect)anOriginalRect
{
    return CGRectMake(anOriginalRect.origin.x,
                      anOriginalRect.origin.y,
                      anOriginalRect.size.width + (_blurSize.width - 1) * 2,
                      anOriginalRect.size.height + (_blurSize.height - 1) * 2);
}

- (CGSize)blurTextureSizeForOriginalSize:(CGSize)anOriginalSize
{
    return CGSizeMake(anOriginalSize.width + (_blurSize.width - 1) * 2,
                      anOriginalSize.height + (_blurSize.height - 1) * 2);
}

#pragma mark -
#pragma mark Initialization & Release

- (id)initWithTexture:(CCTexture2D *)texture rect:(CGRect)rect rotated:(BOOL)rotated blurSize:(CGSize)aBlurSize
{
    self = [super initWithTexture:texture rect:rect rotated:rotated];
    if (self != nil)
    {
        _originalTextureSize = rect.size;
        
        _blurSize = [self normalizedBlurSizeFromNeededBlurSize:aBlurSize];
        [self createGaussianMatrixWithSize:_blurSize];
        
        CGSize blurredSize = [self blurTextureSizeForOriginalSize:_originalTextureSize];
        [self prepareDynamicsForSize:blurredSize];
        
        // Prepare originalTexture
        self.originalTexture = [CCRenderTexture renderTextureWithWidth:blurredSize.width
                                                                height:blurredSize.height];
        
        [_originalTexture beginWithClear:1.0 g:1.0 b:1.0 a:0.0];
        
        GAFSprite *dynamicSprite = [[GAFSprite alloc] initWithTexture:texture rect:rect rotated:rotated];
        
        // Adjust sprite position (anchor is 0.5, 0.5 default)
        dynamicSprite.anchorPoint = CGPointMake(0.5, 0.5);
        dynamicSprite.position = CGPointMake(blurredSize.width / 2.0, blurredSize.height / 2.0);
        [dynamicSprite visit];
        [dynamicSprite release];
        
        [_originalTexture end];
    }
    return self;
}

- (id)initWithTexture:(CCTexture2D *)texture rect:(CGRect)rect rotated:(BOOL)rotated
{
    return [self initWithTexture:texture rect:rect rotated:rotated blurSize:CGSizeMake(kDefaultBlurSize, kDefaultBlurSize)];
}

- (id)initWithGAFSprite:(GAFSpriteWithAlpha *)aSprite blurSize:(CGSize)aBlurSize
{
    self = [self initWithTexture:aSprite.texture rect:aSprite->_rect rotated:aSprite->_rectRotated blurSize:aBlurSize];
    if (self != nil)
    {
        self.sourceSprite = aSprite;
        
		[self setColorTransform:[aSprite getColorTransform]];
        self.useExternalTransform = aSprite.useExternalTransform;
        self.anchorPoint = aSprite.anchorPoint;
        self.externalTransform = aSprite.externalTransform;
        self.visible = aSprite.visible;
        self.zOrder = aSprite.zOrder;
        self.objectId = aSprite.objectId;
        self.vertexZ = aSprite.vertexZ;
        
        self.blendFunc = aSprite.blendFunc;
    }
    return self;
}

- (void)dealloc
{
    [_dynamicTexture1 release];
    [_dynamicTexture2 release];
    [_originalTexture release];
    free(_gaussianValuesX);
    free(_gaussianValuesY);
    [sourceSprite release];
    [super dealloc];
}

#pragma mark -
#pragma mark Public methods

- (void)updateDynamicTexture
{
    // Preparing dynamic textures
    CGSize blurredSize = [self blurTextureSizeForOriginalSize:_originalTextureSize];
    [self prepareDynamicsForSize:blurredSize];
 
    /* We will use two-pass gaussian filter, first horizontal, then vertical.
     * 1. Render 'original' texture to 'dynamicTexture1' using horizontal blur pixel shader
     * 2. Render 'dynamicTexture1' to 'dynamicTexture2' using vertical blur pixel shader
     * 3. Use 'dynamicTexture2' as texture for our sprite
     */
    
    // Caching shaders
    CCGLProgram *program1 = [self programForHorizontalBlur];
    CCGLProgram *program2 = [self programForVerticalBlur];
    if (program1 == nil || program2 == nil)
    {
        CCLOGWARN(@"Failed to compile shaders for GAFBlurredSprite. Will display incorrectly.");
        return;
    }
    
    CHECK_GL_ERROR_DEBUG();
    
    //////////
    // Step 0 - render original to tex2
    [_dynamicTexture2 beginWithClear:1.0 g:1.0 b:1.0 a:0.0];
    
    // Draw sprite
    _originalTexture.sprite.position = CGPointMake(_dynamicTexture1.sprite.contentSize.width / 2,
                                                   _dynamicTexture1.sprite.contentSize.height / 2);
    
    [_originalTexture.sprite setBlendFunc:(ccBlendFunc){ GL_ONE, GL_ZERO }];
    [_originalTexture.sprite visit];
    
    [_dynamicTexture2 end];
        
    //////////
    // Step 1 - render tex2 to tex1. Use shader.
    [_dynamicTexture1 beginWithClear:1.0 g:1.0 b:1.0 a:0.0];
    
    // Set shader to sprite
    _dynamicTexture2.sprite.shaderProgram = [self programForHorizontalBlur];
    
    CHECK_GL_ERROR_DEBUG();
    // Set uniform values
    glUniform1i(_horzShader_kernelSizeUniformLocation, _gaussianValuesArraySize.width);
    glUniform1f(_horzShader_blurDotSizeUniformLocation, (1.0 / _blurredTextureSize.width));
    glUniform1fv(_horzShader_kernelValuesUniformLocation, _gaussianValuesArraySize.width, (GLfloat *)_gaussianValuesX);
    
    // Draw sprite
    _dynamicTexture2.sprite.position = CGPointMake(_dynamicTexture1.sprite.contentSize.width / 2,
                                                   _dynamicTexture1.sprite.contentSize.height / 2);
    
    [_dynamicTexture2.sprite setBlendFunc:(ccBlendFunc){ GL_ONE, GL_ZERO }];
    [_dynamicTexture2.sprite visit];
    
    [_dynamicTexture1 end];
    
    //////////
    // Step 2 - render tex1 to tex2. Use shader.
    [_dynamicTexture2 beginWithClear:1.0 g:1.0 b:1.0 a:0.0];
    
    // Set shader to sprite
    _dynamicTexture1.sprite.shaderProgram = [self programForVerticalBlur];
    
    CHECK_GL_ERROR_DEBUG();
    // Set uniform values
    glUniform1i(_vertShader_kernelSizeUniformLocation, _gaussianValuesArraySize.height);
    glUniform1f(_vertShader_blurDotSizeUniformLocation, (1.0 / _blurredTextureSize.height));
    glUniform1fv(_vertShader_kernelValuesUniformLocation, _gaussianValuesArraySize.height, (GLfloat *)_gaussianValuesY);
    
    // Draw sprite
    _dynamicTexture1.sprite.position = CGPointMake(_dynamicTexture1.sprite.contentSize.width / 2,
                                                   _dynamicTexture1.sprite.contentSize.height / 2);
     
    [_dynamicTexture1.sprite setBlendFunc:(ccBlendFunc){ GL_ONE, GL_ZERO }];
    [_dynamicTexture1.sprite visit];
    
    [_dynamicTexture2 end];
     
    //////////
    // Step 3
    
    //self.texture = _originalTexture.sprite.texture;
    //self.texture = _dynamicTexture1.sprite.texture;
    self.texture = _dynamicTexture2.sprite.texture;
    self.flipY = YES;
    
    // Adjust this sprite's data
    CGRect textureRect = CGRectMake(0, 0, _blurredTextureSize.width, _blurredTextureSize.height);
    [self setTextureRect:textureRect rotated:_rectRotated untrimmedSize:textureRect.size];
    
    // Adjust pivot point due to texture size change (making new object positioned at the same place as old)
    CGPoint pivotPoint = self.sourceSprite.anchorPoint;
    CGPoint scale = CGPointMake(_blurredTextureSize.width / self.sourceSprite.contentSize.width,
                                _blurredTextureSize.height / self.sourceSprite.contentSize.height);
    pivotPoint.x = 0.5 - ((0.5 - pivotPoint.x) / scale.x);
    pivotPoint.y = 0.5 - ((0.5 - pivotPoint.y) / scale.y);
    
    self.anchorPoint = pivotPoint;
}

#pragma mark -
#pragma mark Private methods

- (CCGLProgram *)programForVerticalBlur
{
    CCGLProgram *program = [[CCShaderCache sharedShaderCache] programForKey:kGAFBlurredSpriteVerticalBlurShaderProgramCacheKey];
    if (program == nil)
    {
        program = [[CCGLProgram alloc] initWithVertexShaderByteArray:ccPositionTextureColor_vert
                                              fragmentShaderFilename:kBlurredSpriteVerticalBlurShaderFilename];
        
        if (program != nil)
        {
            [program addAttribute:kCCAttributeNamePosition index:kCCVertexAttrib_Position];
            [program addAttribute:kCCAttributeNameColor index:kCCVertexAttrib_Color];
            [program addAttribute:kCCAttributeNameTexCoord index:kCCVertexAttrib_TexCoords];
            
            [program link];
            [program updateUniforms];
            
            CHECK_GL_ERROR_DEBUG();
            
            [[CCShaderCache sharedShaderCache] addProgram:program forKey:kGAFBlurredSpriteVerticalBlurShaderProgramCacheKey];
            [program release];
        }
        else
        {
            CCLOGWARN(@"Cannot load program for kGAFBlurredSpriteVerticalBlurShaderProgramCacheKey.");
            [self release];
            return nil;
        }
    }
    
    [program use];
    
    _vertShader_kernelSizeUniformLocation = (GLint)glGetUniformLocation(program->_program, "u_matrixRowSize");
    _vertShader_blurDotSizeUniformLocation = (GLint)glGetUniformLocation(program->_program, "dotSize");
    _vertShader_kernelValuesUniformLocation = (GLint)glGetUniformLocation(program->_program, "u_matrixRowValues");
    
    if (_vertShader_kernelSizeUniformLocation <= 0 ||
        _vertShader_blurDotSizeUniformLocation <= 0 ||
        _vertShader_kernelValuesUniformLocation <= 0)
    {
        CCLOGWARN(@"Cannot get uniforms for kGAFBlurredSpriteVerticalBlurShaderProgramCacheKey");
    }
    
    return program;
}

- (CCGLProgram *)programForHorizontalBlur
{
    CCGLProgram *program = [[CCShaderCache sharedShaderCache] programForKey:kGAFBlurredSpriteHorizontalBlurShaderProgramCacheKey];
    if (program == nil)
    {
        program = [[CCGLProgram alloc] initWithVertexShaderByteArray:ccPositionTextureColor_vert
                                             fragmentShaderFilename:kBlurredSpriteHorizontalBlurShaderFilename];
        
        if (program != nil)
        {
            [program addAttribute:kCCAttributeNamePosition index:kCCVertexAttrib_Position];
            [program addAttribute:kCCAttributeNameColor index:kCCVertexAttrib_Color];
            [program addAttribute:kCCAttributeNameTexCoord index:kCCVertexAttrib_TexCoords];
            
            [program link];
            [program updateUniforms];
            
            CHECK_GL_ERROR_DEBUG();
            
            [[CCShaderCache sharedShaderCache] addProgram:program forKey:kGAFBlurredSpriteHorizontalBlurShaderProgramCacheKey];
            [program release];
        }
        else
        {
			CCLOGWARN(@"Cannot load program for kGAFBlurredSpriteHorizontalBlurShaderProgramCacheKey.");
            [self release];
            return nil;
        }
    }
    
    [program use];
    
    _horzShader_kernelSizeUniformLocation = (GLint)glGetUniformLocation(program->_program, "u_matrixRowSize");
    _horzShader_blurDotSizeUniformLocation = (GLint)glGetUniformLocation(program->_program, "dotSize");
    _horzShader_kernelValuesUniformLocation = (GLint)glGetUniformLocation(program->_program, "u_matrixRowValues");
    
    if (_horzShader_kernelSizeUniformLocation <= 0 ||
        _horzShader_blurDotSizeUniformLocation <= 0 ||
        _horzShader_kernelValuesUniformLocation <= 0)
    {
        CCLOGWARN(@"Cannot get uniforms for kGAFBlurredSpriteHorizontalBlurShaderProgramCacheKey");
    }
    
    return program;
}

- (void)prepareDynamicsForSize:(CGSize)aSize
{
    if (self.dynamicTexture1 == nil || !CGSizeEqualToSize(_blurredTextureSize, aSize))
    {
        self.dynamicTexture1 = [CCRenderTexture renderTextureWithWidth:aSize.width height:aSize.height];
    }
    
    if (self.dynamicTexture2 == nil || !CGSizeEqualToSize(_blurredTextureSize, aSize))
    {
        self.dynamicTexture2 = [CCRenderTexture renderTextureWithWidth:aSize.width height:aSize.height];
    }
    
    _blurredTextureSize = aSize;
}

- (void)createGaussianMatrixWithSize:(CGSize)aSize
{
    NSAssert(aSize.width > 0 && aSize.height > 0, @"");
    NSAssert(isOdd((int)aSize.width) && isOdd((int)aSize.height), @"");
    
    const float roCoeff = 3;
    
    if (aSize.width != _gaussianValuesArraySize.width)
    {
        if (_gaussianValuesX != nil)
            free(_gaussianValuesX);
        
        float ro = (aSize.width / 2) / roCoeff;
        _gaussianValuesX = gaussianMatrixRow(aSize.width, ro);
    }
    
    if (aSize.height != _gaussianValuesArraySize.height)
    {
        if (_gaussianValuesY != nil)
            free(_gaussianValuesY);
        
        float ro = (aSize.height / 2) / roCoeff;
        _gaussianValuesY = gaussianMatrixRow(aSize.height, ro);
    }
    
    _gaussianValuesArraySize = aSize;
}

@end