#import "cocos2d.h"

@interface CCGLProgram (Extensions)

- (id)initWithVertexShaderFilename:(NSString *)vShaderFilename fragmentShaderByteArray:(const GLchar *)fShaderByteArray;
- (id)initWithVertexShaderByteArray:(const GLchar *)vShaderByteArray fragmentShaderFilename:(NSString *)fShaderFilename;

@end
