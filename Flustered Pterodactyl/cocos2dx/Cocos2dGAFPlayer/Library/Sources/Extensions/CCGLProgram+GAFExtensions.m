////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  CCGLProgram+GAFExtensions.m
//  GAF Animation Library
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Imports

#import "CCGLProgram+GAFExtensions.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation

@implementation CCGLProgram (GAFExtensions)

- (id)initWithVertexShaderFilename:(NSString *)vShaderFilename fragmentShaderByteArray:(const GLchar *)fShaderByteArray
{
	const GLchar * vertexSource = (GLchar*) [[NSString stringWithContentsOfFile:[[CCFileUtils sharedFileUtils] fullPathFromRelativePath:vShaderFilename] encoding:NSUTF8StringEncoding error:nil] UTF8String];
    
    if (vertexSource == nil)
    {
#ifdef DEBUG
        NSString *message = [NSString stringWithFormat:@"Cannot load vertex shader with name %@", vShaderFilename];
        NSAssert(false, message);
#endif
    }
    
	return [self initWithVertexShaderByteArray:vertexSource fragmentShaderByteArray:fShaderByteArray];
}

- (id)initWithVertexShaderByteArray:(const GLchar *)vShaderByteArray fragmentShaderFilename:(NSString *)fShaderFilename
{
	const GLchar * fragmentSource = (GLchar*) [[NSString stringWithContentsOfFile:[[CCFileUtils sharedFileUtils] fullPathFromRelativePath:fShaderFilename] encoding:NSUTF8StringEncoding error:nil] UTF8String];
    
    if (fragmentSource == nil)
    {
#ifdef DEBUG
        NSString *message = [NSString stringWithFormat:@"Cannot load fragment shader with name %@", fShaderFilename];
        NSAssert(false, message);
#endif
    }
    
	return [self initWithVertexShaderByteArray:vShaderByteArray fragmentShaderByteArray:fragmentSource];
}

@end