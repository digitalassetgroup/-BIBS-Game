////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GAFStaticObject.m
//  GAF Animation Library
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Imports

#import "GAFStaticObject.h"
#import "GAFNode.h"
#import "GAFSprite.h"
#import "GAFAsset.h"
#import "GAFHelperMethods.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constants

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private interface

@interface GAFStaticObject ()
@end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation

@implementation GAFStaticObject

#pragma mark -
#pragma mark Properties

#pragma mark -
#pragma mark Initialization & Release

+ (GAFStaticObject *)staticObjectWithAsset:(GAFAsset *)anAsset
{
    return [[[self alloc] initWithAsset:anAsset] ah_autorelease];
}

+ (GAFStaticObject *)staticObjectWithPath:(NSString *)aJSONPath
{
    return [[[self alloc] initWithPath:aJSONPath] ah_autorelease];
}

+ (GAFStaticObject *)staticObjectWithAsset:(GAFAsset *)anAsset
externalNamedPartsAtlas:(GAFTextureAtlas *)anAtlas
          atlasName:(NSString *)anExternalAtlasName
{
    return [[[self alloc] initWithAsset:anAsset
                externalNamedPartsAtlas:anAtlas
                              atlasName:anExternalAtlasName] ah_autorelease];
}

- (id)initWithAsset:(GAFAsset *)anAsset
{
    if ((self = [super initWithAsset:anAsset]))
    {
        [self start];
        [self gotoFrameAndStop:0];
    }
    return self;
}

- (id)initWithPath:(NSString *)aJSONPath runLooped:(BOOL)aLooped
{
    return [self initWithPath:aJSONPath];
}

- (id)initWithAsset:(GAFAsset *)anAsset
externalNamedPartsAtlas:(GAFTextureAtlas *)anAtlas
          atlasName:(NSString *)anExternalAtlasName
{
    if ((self = [super initWithAsset:anAsset]))
    {
        [self linkExternalNamedPartsAtlas:anAtlas forName:anExternalAtlasName];
        [self start];
        [self gotoFrameAndStop:0];
    }
    return self;
}

#pragma mark -
#pragma mark Public methods

#pragma mark -
#pragma mark Private methods

@end