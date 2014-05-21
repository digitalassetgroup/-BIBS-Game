////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  NSError+GAFExtensions.h
//  GAF Animation Library
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constants

extern NSString * const kGAFLibErrorDomain;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Interface

@interface NSError (GAFExtensions)

+ (NSDictionary *)userInfoWithInnerError:(NSError *)anInnerError;

- (id)initWithDomain:(NSString *)domain code:(NSInteger)code innerError:(NSError *)anInnerError;
+ (id)errorWithDomain:(NSString *)domain code:(NSInteger)code innerError:(NSError *)anInnerError;

- (id)initWithDomain:(NSString *)domain code:(NSInteger)code description:(NSString *)description innerError:(NSError *)anInnerError;
+ (id)errorWithDomain:(NSString *)domain code:(NSInteger)code description:(NSString *)description innerError:(NSError *)anInnerError;

- (id)initWithDomain:(NSString *)domain code:(NSInteger)code description:(NSString *)description;
+ (id)errorWithDomain:(NSString *)domain code:(NSInteger)code description:(NSString *)description;

- (id)initWithDescription:(NSString *)aDescription innerError:(NSError *)anError;
+ (id)errorWithDescription:(NSString *)aDescription innerError:(NSError *)anError;

- (NSError *)innerError;

- (NSString *)localizedDescriptionWithExpandedInnerErrors;

@end
