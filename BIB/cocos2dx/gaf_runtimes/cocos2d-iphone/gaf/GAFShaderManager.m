////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GAFShaderManager.m
//  PetCity
//
//  Created by Gregory Maksyuk on 3/15/13.
//  Copyright 2013 Catalyst Apps. All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Imports

#import "GAFShaderManager.h"
#import "CCShaderCache.h"
#import "OpenGL_Internal.h"
#import "CCGLProgram.h"
#import "ccShaders.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constants

NSString * const kPCShader_AlphaFilterProgramCacheKey = @"kPCShader_AlphaFilterProgramCacheKey";

static NSString * const kPCShader_AlphaFilterFragmentShaderFilename = @"pcShader_PositionTexture_alphaFilter.fs";

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
    [super dealloc];
}

#pragma mark -
#pragma mark Public methods

- (BOOL)loadAlphaFilterShaderProgramWithError:(NSError **)anError
{
    CCGLProgram *program = [[CCShaderCache sharedShaderCache] programForKey:kPCShader_AlphaFilterProgramCacheKey];
    if (program == nil)
    {
        program = [[CCGLProgram alloc] initWithVertexShaderByteArray:ccPositionTextureColor_vert
                                              fragmentShaderFilename:kPCShader_AlphaFilterFragmentShaderFilename];
        
        if (program != nil)
        {
            [program addAttribute:kCCVertexAttrib_Position index:kCCVertexAttrib_Position];
            [program addAttribute:kCCVertexAttrib_Color index:kCCVertexAttrib_Color];
            [program addAttribute:kCCVertexAttrib_TexCoords index:kCCVertexAttrib_TexCoords];
            
            [program link];
            [program updateUniforms];
            
            CHECK_GL_ERROR_DEBUG();
            
            [[CCShaderCache sharedShaderCache] addProgram:program forKey:kPCShader_AlphaFilterProgramCacheKey];
            [program release];
            
            return YES;
        }
        else
        {
            if (anError != nil)
            {
                *anError = [NSError errorWithDescription:[NSString stringWithFormat:@"Cannot load program named: %@", kPCShader_AlphaFilterProgramCacheKey]
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