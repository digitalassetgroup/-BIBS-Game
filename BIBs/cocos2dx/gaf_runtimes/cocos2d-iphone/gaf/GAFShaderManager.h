
extern NSString * const kPCShader_AlphaFilterProgramCacheKey;
@class CCGLProgram;

@interface GAFShaderManager : NSObject

+ (GAFShaderManager *)sharedManager;

- (BOOL)preloadAllShadersWithError:(NSError **)anError;

- (CCGLProgram *)programShaderForKey:(NSString *)aKey;

@end
