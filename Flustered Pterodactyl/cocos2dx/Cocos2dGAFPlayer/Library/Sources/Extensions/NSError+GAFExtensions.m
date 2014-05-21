////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  NSError+GAFExtensions.m
//  GAF Animation Library
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Imports

#import "NSError+GAFExtensions.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constants

NSString * const kGAFLibErrorDomain = @"com.CatalystApps.GAF";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation

@implementation NSError (GAFExtensions)

#pragma mark -
#pragma mark Public methods

+ (NSDictionary *)userInfoWithInnerError:(NSError *)anInnerError
{
    return anInnerError == nil ? nil : @{NSUnderlyingErrorKey: anInnerError};
}

- (id)initWithDomain:(NSString *)domain code:(NSInteger)code innerError:(NSError *)anInnerError
{
    return [self initWithDomain:domain code:code userInfo:[[self class] userInfoWithInnerError:anInnerError]];
}

+ (id)errorWithDomain:(NSString *)domain code:(NSInteger)code innerError:(NSError *)anInnerError
{
    return [self errorWithDomain:domain code:code userInfo:[self userInfoWithInnerError:anInnerError]];
}

- (id)initWithDomain:(NSString *)domain code:(NSInteger)code description:(NSString *)description innerError:(NSError *)anInnerError
{
    NSDictionary *userInfo = anInnerError != nil ?
        @{NSLocalizedDescriptionKey:description, NSUnderlyingErrorKey:anInnerError} :
        @{NSLocalizedDescriptionKey:description};
    return [self initWithDomain:domain
                           code:code
                       userInfo:userInfo];
}

+ (id)errorWithDomain:(NSString *)domain code:(NSInteger)code description:(NSString *)description innerError:(NSError *)anInnerError
{
    return [[[NSError alloc] initWithDomain:domain code:code description:description innerError:anInnerError] ah_autorelease];
}

- (id)initWithDomain:(NSString *)domain code:(NSInteger)code description:(NSString *)description
{
    return [self initWithDomain:domain
                           code:code
                       userInfo:@{NSLocalizedDescriptionKey:description}];
}

+ (id)errorWithDomain:(NSString *)domain code:(NSInteger)code description:(NSString *)description
{
    return [[[NSError alloc] initWithDomain:domain code:code description:description] ah_autorelease];
}

- (id)initWithDescription:(NSString *)aDescription innerError:(NSError *)anError
{
    return [self initWithDomain:kGAFLibErrorDomain code:-1 description:aDescription innerError:anError];
}

+ (id)errorWithDescription:(NSString *)aDescription innerError:(NSError *)anError
{
    return [[[NSError alloc] initWithDescription:aDescription innerError:anError] ah_autorelease];
}

- (NSError *)innerError
{
    return (self.userInfo)[NSUnderlyingErrorKey];
}

- (NSString *)localizedDescriptionWithExpandedInnerErrors
{
    if (self.innerError == nil)
    {
        return [self localizedDescription];
    }
    else
    {
        return [NSString stringWithFormat:@"%@. [InnerError: %@]",
                [self localizedDescription],
                [self.innerError localizedDescriptionWithExpandedInnerErrors]];
    }
}

@end