////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GAFTextureEffectsConverter.m
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Imports

#import "GAFTextureEffectsConverter.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constants

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private interface

@interface GAFTextureEffectsConverter ()

@property (nonatomic, retain) NSMutableDictionary *vertexShaderUniforms;

@end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation

@implementation GAFTextureEffectsConverter

#pragma mark -
#pragma mark Properties

#pragma mark -
#pragma mark Initialization & Release

+ (GAFTextureEffectsConverter *)sharedConverter
{
    static dispatch_once_t once;
    static id instance = nil;
    _dispatch_once(&once, ^{
        instance = [[self alloc] init];
    });
    return instance;
}

- (id)init
{
    if ((self = [super init]))
    {
        self.vertexShaderUniforms = [NSMutableDictionary dictionary];
    }
    return self;
}

- (void)dealloc
{
    [_vertexShaderUniforms ah_release];

#if !__has_feature(objc_arc)
    [super dealloc];
#endif
}

#pragma mark -
#pragma mark Public methodsя

- (CCRenderTexture *)gaussianBlurredTextureFromTexture:(CCTexture2D *)aTexture
                                                  rect:(CGRect)rect
                                           blurRadiusX:(CGFloat)aBlurRadiusX
                                           blurRadiusY:(CGFloat)aBlurRadiusY
{
    const int kGaussianKernelSize = 9;
    
    CGSize rTextureSize = CGSizeMake(rect.size.width + 2 * (kGaussianKernelSize / 2) * aBlurRadiusX,
                                     rect.size.height + 2 * (kGaussianKernelSize / 2) * aBlurRadiusY);
    
    CCRenderTexture *rTexture1 = [CCRenderTexture renderTextureWithWidth:rTextureSize.width height:rTextureSize.height];
    CCRenderTexture *rTexture2 = [CCRenderTexture renderTextureWithWidth:rTextureSize.width height:rTextureSize.height];
    
    //NSTimeInterval time = 0;
    //CAPTURE_TIME(time);
    
    // Loading shader
    CCGLProgram *shader = [self programForBlurShaderWithName:@"GaussianBlur"
                                            vertexShaderFile:@"GaussianBlurVertexShader.vs"
                                          fragmentShaderFile:@"GaussianBlurFragmentShader.fs"];
    if (shader == nil)
    {
        return nil;
    }
    
    GLint texelWidthOffset = (GLint)glGetUniformLocation(shader->_program, "texelWidthOffset");
    GLint texelHeightOffset = (GLint)glGetUniformLocation(shader->_program, "texelHeightOffset");
    
    CHECK_GL_ERROR_DEBUG();
    
    //SHOW_PASSED_TIME(time, @"load shaders");
    //CAPTURE_TIME(time);
    
    {
        // Render texture to rTexture2 without shaders
        CCSprite *sprite = [CCSprite spriteWithTexture:aTexture rect:rect];
        sprite.position = CGPointMake(rTextureSize.width / 2,
                                      rTextureSize.height / 2);
        
        [sprite setBlendFunc:(ccBlendFunc){ GL_ONE, GL_ZERO }];
        
        [rTexture2 beginWithClear:0.0 g:0.0 b:0.0 a:0.0];
        [sprite visit];
        [rTexture2 end];
    }
    
    CHECK_GL_ERROR_DEBUG();
    
    //SHOW_PASSED_TIME(time, @"texture -> rTexture2");
    //CAPTURE_TIME(time);
    
    {
        // Render rTexture2 to rTexture1 (horizontal)
        GLfloat texelWidthValue = aBlurRadiusX / (GLfloat)rTextureSize.width;
        GLfloat texelHeightValue = 0;
        
        rTexture2.sprite.position = CGPointMake(rTextureSize.width / 2,
                                                rTextureSize.height / 2);
        
        rTexture2.sprite.shaderProgram = shader;
        
        [shader use];
        glUniform1f(texelWidthOffset, texelWidthValue);
        glUniform1f(texelHeightOffset, texelHeightValue);
        
        [rTexture2.sprite setBlendFunc:(ccBlendFunc){ GL_ONE, GL_ZERO }];
        
        [rTexture1 beginWithClear:0.0 g:0.0 b:0.0 a:0.0];
        [rTexture2.sprite visit];
        [rTexture1 end];
    }
    
    CHECK_GL_ERROR_DEBUG();
    
    //SHOW_PASSED_TIME(time, @"rTexture2 -> rTexture1");
    //CAPTURE_TIME(time);
    
    {
        // Render rTexture1 to rTexture2 (vertical)
        GLfloat texelWidthValue = 0;
        GLfloat texelHeightValue = aBlurRadiusY / (GLfloat)rTextureSize.height;
        
        rTexture1.sprite.position = CGPointMake(rTextureSize.width / 2,
                                                rTextureSize.height / 2);
        
        rTexture1.sprite.shaderProgram = shader;
        
        [shader use];
        glUniform1f(texelWidthOffset, texelWidthValue);
        glUniform1f(texelHeightOffset, texelHeightValue);
        
        [rTexture1.sprite setBlendFunc:(ccBlendFunc){ GL_ONE, GL_ZERO }];
        
        [rTexture2 beginWithClear:0.0 g:0.0 b:0.0 a:0.0];
        [rTexture1.sprite visit];
        [rTexture2 end];
    }
    
    CHECK_GL_ERROR_DEBUG();
    
    //SHOW_PASSED_TIME(time, @"rTexture1 -> rTexture2");
    
    return rTexture2;
}

#pragma mark -
#pragma mark Private methods

- (CCGLProgram *)programForBlurShaderWithName:(NSString *)aShaderName
                             vertexShaderFile:(NSString *)aVertexShaderFile
                           fragmentShaderFile:(NSString *)aFragmentShaderFile
{
    CCGLProgram *program = [[CCShaderCache sharedShaderCache] programForKey:aShaderName];
    if (program == nil)
    {
        program = [[[CCGLProgram alloc] initWithVertexShaderFilename:aVertexShaderFile
                                              fragmentShaderFilename:aFragmentShaderFile] ah_autorelease];
        
        if (program != nil)
        {
            [program addAttribute:@"position" index:kCCVertexAttrib_Position];
            [program addAttribute:@"inputTextureCoordinate" index:kCCVertexAttrib_TexCoords];
            
            [program link];
            [program updateUniforms];
            
            CHECK_GL_ERROR_DEBUG();
            
            [[CCShaderCache sharedShaderCache] addProgram:program forKey:aShaderName];
        }
        else
        {
            CCLOGWARN(@"Cannot load program for %@.", aShaderName);
            return nil;
        }
    }
    
    return program;
}

@end