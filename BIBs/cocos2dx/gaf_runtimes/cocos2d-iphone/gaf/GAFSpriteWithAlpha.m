#import "GAFSpriteWithAlpha.h"
#import "GAFSprite_Protected.h"
#import "CCGLProgram+Extensions.h"
#import "ccMacros.h"

static NSString * const kAlphaFragmentShaderFilename = @"pcShader_PositionTextureAlpha_frag.fs";
static NSString * const kGAFSpriteWithAlphaShaderProgramCacheKey = @"kGAFSpriteWithAlphaShaderProgramCache";

@interface GAFSpriteWithAlpha ()

- (CCGLProgram *)programForShader;

@end


@implementation GAFSpriteWithAlpha

#pragma mark -
#pragma mark Properties

- (GLfloat *) getColorTransform
{
	return _colorTransform;
}

- (void) setColorTransformMult:(const GLfloat *) mults offsets:(const GLfloat *) offsets
{
	for (int i = 0; i < 4; ++i)
	{
		_colorTransform[i]     = mults[i];
		_colorTransform[i + 4] = offsets[i];
	}
}

- (void) setColorTransform:(const GLfloat *) colorTransform
{
	for (int i = 0; i < 8; ++i)
	{
		_colorTransform[i] = colorTransform[i];
	}
}

#pragma mark -
#pragma mark Initialization & Release

- (id)initWithTexture:(CCTexture2D *)texture rect:(CGRect)rect rotated:(BOOL)rotated
{
    if ((self = [super initWithTexture:texture rect:rect rotated:rotated]))
    {
		for (int i = 0; i < 4; ++i)
		{
			_colorTransform[i]     = 1.0f;
			_colorTransform[i + 4] = 0;
		}        
        self.shaderProgram = [self programForShader];
    }
    return self;
}

- (void)dealloc
{
    [super dealloc];
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
            [program release];
        }
        else
        {
            CCLOGWARN(@"Cannot load program for GAFSpriteWithAlpha.");
            [self release];
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

@end