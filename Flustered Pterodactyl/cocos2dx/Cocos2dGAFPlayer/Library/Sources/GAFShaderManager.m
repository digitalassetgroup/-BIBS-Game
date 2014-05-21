////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GAFShaderManager.m
//  GAF Animation Library
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Imports

#import "GAFShaderManager.h"
#import "CCShaderCache.h"
#import "Support/OpenGL_Internal.h"
#import "CCGLProgram.h"
#import "ccShaders.h"
#import "NSError+GAFExtensions.h"
#import "CCGLProgram+GAFExtensions.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constants

NSString * const kGAFShader_AlphaFilterProgramCacheKey = @"kGAFShader_AlphaFilterProgramCacheKey";

static NSString * const kGAFShader_AlphaFilterFragmentShaderFilename = @"pcShader_PositionTexture_alphaFilter.fs";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private interface

@interface GAFShaderManager ()

- (BOOL)loadAlphaFilterShaderProgramWithError:(NSError **)anError;

@end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation

@implementation GAFShaderManager


+ (GAFShaderManager *)sharedManager
{                                          
	static GAFShaderManager *obj;               
	static dispatch_once_t onceToken;        
	dispatch_once(&onceToken,                
				  ^{                                       
					  obj = [[self alloc] init];            
				  });                                      
	return obj;                              
}

#pragma mark -
#pragma mark Properties

#pragma mark -
#pragma mark Initialization & Release

- (BOOL)preloadAllShadersWithError:(NSError **)anError
{
    NSError *innerError = nil;
    
    if (innerError == nil)
        [self loadAlphaFilterShaderProgramWithError:&innerError];

    if (innerError != nil)
    {
        if (anError != nil)
        {
            *anError = [NSError errorWithDescription:@"Failed to preload shaders" innerError:innerError];
        }
        return NO;
    }
    return YES;
}

- (CCGLProgram *)programShaderForKey:(NSString *)aKey
{
    return [[CCShaderCache sharedShaderCache] programForKey:aKey];
}

- (void)dealloc
{
#if !__has_feature(objc_arc)
    [super dealloc];
#endif
}

#pragma mark -
#pragma mark Public methods

- (BOOL)loadAlphaFilterShaderProgramWithError:(NSError **)anError
{
    CCGLProgram *program = [[CCShaderCache sharedShaderCache] programForKey:kGAFShader_AlphaFilterProgramCacheKey];
    if (program == nil)
    {
        program = [[CCGLProgram alloc] initWithVertexShaderByteArray:ccPositionTextureColor_vert
                                              fragmentShaderFilename:kGAFShader_AlphaFilterFragmentShaderFilename];
        
        if (program != nil)
        {
            [program addAttribute:kCCAttributeNamePosition index:kCCVertexAttrib_Position];
            [program addAttribute:kCCAttributeNameColor index:kCCVertexAttrib_Color];
            [program addAttribute:kCCAttributeNameTexCoord index:kCCVertexAttrib_TexCoords];
            
            [program link];
            [program updateUniforms];
            
            CHECK_GL_ERROR_DEBUG();
            
            [[CCShaderCache sharedShaderCache] addProgram:program forKey:kGAFShader_AlphaFilterProgramCacheKey];
            [program ah_release];
            
            return YES;
        }
        else
        {
            if (anError != nil)
            {
                *anError = [NSError errorWithDescription:[NSString stringWithFormat:@"Cannot load program named: %@", kGAFShader_AlphaFilterProgramCacheKey]
                                              innerError:nil];
            }
            return NO;
        }
    }
    return YES;
}

#pragma mark -
#pragma mark Private methods

@end