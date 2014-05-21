////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GAFSpriteWithAlpha.m
//  GAF Animation Library
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#import "GAFSpriteWithAlpha.h"
#import "GAFSprite_Protected.h"
#import "CCGLProgram+GAFExtensions.h"
#import "GAFTextureEffectsConverter.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static NSString * const kAlphaFragmentShaderFilename = @"pcShader_PositionTextureAlpha_frag.fs";
static NSString * const kGAFSpriteWithAlphaShaderProgramCacheKey = @"kGAFSpriteWithAlphaShaderProgramCache";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

@interface GAFSpriteWithAlpha ()

// When effect is applied texture is changed, next effect (e.g. change of blur radius) should be applied on the same
// initial texture, not modified one
@property (nonatomic, retain) CCTexture2D *initialTexture;
@property (nonatomic, assign) CGRect initialTextureRect;

- (CCGLProgram *)programForShader;
- (void)setBlendingFunc;

- (void)updateTextureWithEffects;

@end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

@implementation GAFSpriteWithAlpha

#pragma mark -
#pragma mark Properties

- (GLfloat *)getColorTransform
{
	return _colorTransform;
}

- (void)setColorTransformMult:(const GLfloat *) mults offsets:(const GLfloat *) offsets
{
	for (int i = 0; i < 4; ++i)
	{
		_colorTransform[i]     = mults[i];
		_colorTransform[i + 4] = offsets[i];
	}
    [self setBlendingFunc];
}

- (void)setColorTransform:(const GLfloat *) colorTransform
{
	for (int i = 0; i < 8; ++i)
	{
		_colorTransform[i] = colorTransform[i];
	}
	[self setBlendingFunc];
}

- (void)setBlurRadius:(CGSize)blurRadius
{
    if (!CGSizeEqualToSize(_blurRadius, blurRadius))
    {
        _blurRadius = blurRadius;
        [self updateTextureWithEffects];
    }
}

#pragma mark -
#pragma mark Initialization & Release

- (id)initWithTexture:(CCTexture2D *)texture rect:(CGRect)rect rotated:(BOOL)rotated
{
    if ((self = [super initWithTexture:texture rect:rect rotated:rotated]))
    {
        self.initialTexture = texture;
        self.initialTextureRect = rect;
        
        _blurRadius = CGSizeZero;
        
		for (int i = 0; i < 4; ++i)
		{
			_colorTransform[i]     = 1.0f;
			_colorTransform[i + 4] = 0;
		}
        [self setBlendingFunc];
        self.shaderProgram = [self programForShader];
    }
    return self;
}

- (void)dealloc
{
    [_initialTexture ah_release];
#if !__has_feature(objc_arc)
    [super dealloc];
#endif
}

#pragma mark -
#pragma mark Overriden methods

- (void)setUniformsForFragmentShader
{
	glUniform4fv(_colorTrasformLocation, 2, _colorTransform);
}

#pragma mark -
#pragma mark Public methods

#pragma mark -
#pragma mark Private methods

- (void)updateTextureWithEffects
{
    if (self.blurRadius.width == 0 && self.blurRadius.height == 0)
    {
        [self setTexture:self.initialTexture];
        [self setTextureRect:self.initialTextureRect rotated:NO untrimmedSize:self.initialTextureRect.size];
        self.flipY = NO;
    }
    else
    {
        GAFTextureEffectsConverter *converter = [GAFTextureEffectsConverter sharedConverter];
        CCRenderTexture *resultTex = [converter gaussianBlurredTextureFromTexture:self.initialTexture
                                                                             rect:self.initialTextureRect
                                                                      blurRadiusX:self.blurRadius.width
                                                                      blurRadiusY:self.blurRadius.height];
        if (resultTex != nil)
        {
            [self setTexture:resultTex.sprite.texture];
            self.flipY = YES;
            
            CGRect texureRect = CGRectMake(0, 0, resultTex.sprite.contentSize.width, resultTex.sprite.contentSize.height);
            [self setTextureRect:texureRect rotated:NO untrimmedSize:texureRect.size];
        }
    }
}

- (CCGLProgram *)programForShader
{
    CCGLProgram *program = [[CCShaderCache sharedShaderCache] programForKey:kGAFSpriteWithAlphaShaderProgramCacheKey];
    if (program == nil)
    {
        program = [[CCGLProgram alloc] initWithVertexShaderByteArray:ccPositionTextureColor_vert
                                              fragmentShaderFilename:kAlphaFragmentShaderFilename];
        
        if (program != nil)
        {
            [program addAttribute:kCCAttributeNamePosition index:kCCVertexAttrib_Position];
            [program addAttribute:kCCAttributeNameColor index:kCCVertexAttrib_Color];
            [program addAttribute:kCCAttributeNameTexCoord index:kCCVertexAttrib_TexCoords];
            
            [program link];
            [program updateUniforms];
            
            CHECK_GL_ERROR_DEBUG();
            
            [[CCShaderCache sharedShaderCache] addProgram:program forKey:kGAFSpriteWithAlphaShaderProgramCacheKey];
            [program ah_release];
        }
        else
        {
            CCLOGWARN(@"Cannot load program for GAFSpriteWithAlpha.");
            [self ah_release];
            return nil;
        }
    }
    
    [program use];
    
    _colorTrasformLocation = (GLuint)glGetUniformLocation(program->_program, "colorTransform");
    
    if (_colorTrasformLocation <= 0)
    {
        CCLOGWARN(@"Cannot get uniforms for kGAFSpriteWithAlphaShaderProgramCacheKey");
    }
    
    return program;
}

- (void)setBlendingFunc
{
	ccBlendFunc bf;
    bf.src = GL_ONE;
	bf.dst = GL_ONE_MINUS_SRC_ALPHA;
	self.blendFunc = bf;
}

@end