////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GAFSubobjectState.m
//  GAF Animation Library
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#import "GAFAsset.h"
#import "GAFSubobjectState.h"
#import "GAFFilterData.h"
#import "ccMacros.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static NSString * const kStateName      = @"st";
static NSString * const kColorName      = @"c";
static NSString * const kMaskName       = @"m";
static NSString * const kEffectsName    = @"e";
static NSString * const kEffectTypeName = @"t";
static NSString * const kXName          = @"x";
static NSString * const kYName          = @"y";
static NSString * const kBlurEffectName = @"Fblur";

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

@implementation GAFSubobjectState

#pragma mark -
#pragma mark Properties

@synthesize objectId;
@synthesize zIndex;
@synthesize maskObjectId;
@synthesize atlasElementName;
@synthesize affineTransform;
@synthesize filters;

#pragma mark -
#pragma mark Initialization & Release

- (id)initEmptyStateWithObjectId:(NSString *)anObjectId
{
    if(anObjectId == nil)
    {
        NSAssert(NO, @"parameters should not be nil");
        CCLOGWARN(@"ERROR: initializing GAFSubobjectState. ObjectId is nil.");
        [self ah_release];
        return nil;
    }
    
    self = [super init];
    if (nil != self)
    {
		self.objectId = anObjectId;
		self.zIndex = 0;
		self.affineTransform = CGAffineTransformMake(1,0,0,1,0,0);
		_colorOffsets[0] = _colorOffsets[1] = _colorOffsets[2] = _colorOffsets[3] = _colorMults[GAFCTI_A] = 0;
		_colorMults[GAFCTI_R]   = _colorMults[GAFCTI_G]   = _colorMults[GAFCTI_B] = 1;
	}
	return self;
}

- (id)initWithStateDictionary:(NSDictionary *)aStateDictionary objectId:(NSString *)anObjectId
{
	self = [super init];
    if (nil != self)
    {
		self.objectId = anObjectId;
		if (nil == aStateDictionary)
		{
			CCLOGWARN(@"Error while creating GAFSubobjectState. Invalid aStateDictionary");
			[self ah_release];
			return nil;
		}
        if (nil == anObjectId)
		{
			CCLOGWARN(@"Error while creating GAFSubobjectState. Invalid objectId");
			[self ah_release];
			return nil;
		}
        
		NSString * state = (NSString*) aStateDictionary[kStateName];
		if (!state)
		{
			CCLOGWARN(@"Error while creating GAFSubobjectState. State is missing");
			[self ah_release];
			return nil;
		}
		NSRange s = [state rangeOfString:@"{"];
		NSRange e = [state rangeOfString:@"}"];
		//if (s.length == 0 || e.length == 0 || e.location < s.location)
        if (s.length == 0 || e.length == 0 || e.location < s.location || s.location < 2 || e.location > [state length] - 3)
		{
			CCLOGWARN(@"Error while creating GAFSubobjectState. Invalid state line");
			[self ah_release];
			return nil;
		}
		NSString * temp = [state substringWithRange:NSMakeRange(0, s.location - 1)];
		self.zIndex = [temp intValue];
		NSRange r = NSMakeRange(e.location + 2, [state length] - e.location - 2);
		temp = [state substringWithRange:r];
		_colorMults[GAFCTI_A] = [temp floatValue];
		temp = [state substringWithRange:NSMakeRange(s.location + 1, e.location - s.location - 1)];
		NSArray * transform = [temp componentsSeparatedByString:@","];
        if ( [transform count] == 6 )
        {
            self.affineTransform = CGAffineTransformMake(
                                                         [transform[0] floatValue],
                                                         [transform[1] floatValue],
                                                         [transform[2] floatValue],
                                                         [transform[3] floatValue],
                                                         [transform[4] floatValue],
                                                         [transform[5] floatValue]
                                                         );
        }
        else
        {
            CCLOGWARN(@"Error while creating GAFSubobjectState. wrong affineTransform.");
			[self ah_release];
			return nil;
        }
		
		NSString * color = (NSString*) aStateDictionary[kColorName];
		if (color != nil)
		{
			NSArray * clrs = [color componentsSeparatedByString:@","];
			if ([clrs count] < 7)
			{
				CCLOGWARN(@"Error while creating GAFSubobjectState. Invalid color data '%@'", state);
				[self ah_release];
                return nil;
			}
			_colorOffsets[GAFCTI_A] = [clrs[0] floatValue];
			
			_colorMults[GAFCTI_R]   = [clrs[1] floatValue];
			_colorOffsets[GAFCTI_R] = [clrs[2] floatValue];
			
			_colorMults[GAFCTI_G] =   [clrs[3] floatValue];
			_colorOffsets[GAFCTI_G] = [clrs[4] floatValue];
			
			_colorMults[GAFCTI_B]   = [clrs[5] floatValue];
			_colorOffsets[GAFCTI_B] = [clrs[6] floatValue];
		}
		else
		{
			_colorOffsets[0] = _colorOffsets[1] = _colorOffsets[2] = _colorOffsets[3] = 0;
			_colorMults[GAFCTI_R]   = _colorMults[GAFCTI_G]   = _colorMults[GAFCTI_B] = 1;
		}
		NSString * mask = (NSString*) aStateDictionary[kMaskName];
		if (mask != nil)
		{
			self.maskObjectId = mask;
		}
		NSArray * effects = (NSArray*) aStateDictionary[kEffectsName];
		if (effects != nil)
		{
			NSMutableDictionary * mutableFilters = [NSMutableDictionary dictionary];
			for (int i = 0; i < [effects count]; ++i)
			{
				NSDictionary * dict = (NSDictionary*)effects[i];
				NSString * type = (NSString*)dict[kEffectTypeName];
				if (type != nil)
				{
					if ([type isEqualToString:kBlurEffectName])
					{
						NSString * x = (NSString*)dict[kXName];
						NSString * y = (NSString*)dict[kYName];
						if (x != nil && y != nil)
						{
							GAFBlurFilterData * blurFilter = [GAFBlurFilterData new];
							blurFilter.blurSize = CGSizeMake([x floatValue], [y floatValue]);
							mutableFilters[kGAFBlurFilterName] = blurFilter;
							[blurFilter ah_release];
						}
					}
				}
				else
				{
					CCLOGINFO(@"Can not determine effect type, skipping it");
				}
			}
			self.filters = mutableFilters;
		}
	}
	return self;
}

- (void)dealloc
{
    [filters ah_release];
    [objectId ah_release];
    [maskObjectId ah_release];
    [atlasElementName ah_release];

#if !__has_feature(objc_arc)
    [super dealloc];
#endif
}

#pragma mark -
#pragma mark Overriden methods

- (id)copyWithZone:(NSZone *)zone
{
    GAFSubobjectState *newState = [[GAFSubobjectState allocWithZone:zone] init];
    newState.objectId = self.objectId;
    newState.zIndex = self.zIndex;
    newState.maskObjectId = self.maskObjectId;
    newState.atlasElementName = self.atlasElementName;
    newState.affineTransform = self.affineTransform;
    newState.filters = [[self.filters copyWithZone:zone] ah_autorelease];
    return newState;
}

- (BOOL)isEqual:(id)object
{
    if (object == nil || ![object isKindOfClass:[self class]])
        return NO;
    if (object == self)
        return YES;
    
    GAFSubobjectState *state = (GAFSubobjectState *)object;
    
    return ([state.objectId isEqualToString:self.objectId] &&
            state.zIndex == self.zIndex &&
            [state.maskObjectId isEqualToString:self.maskObjectId] &&
            [state.atlasElementName isEqualToString:self.atlasElementName] &&
            CGAffineTransformEqualToTransform(state.affineTransform, self.affineTransform) &&
            [state.filters isEqualToDictionary:self.filters]);
}

- (NSString *)description
{
    return [NSString stringWithFormat:@"< ID = %@ | zIndex = %ld | maskID = %@ | atlasElemenName = %@ | transform = (%@) | filters = %@ >",
            self.objectId, (long)self.zIndex, self.maskObjectId, self.atlasElementName,
            NSStringFromCGAffineTransform(self.affineTransform),
            [self.filters description]];
}

#pragma mark -
#pragma mark Public methods

- (BOOL)isVisible
{
	return _colorMults[GAFCTI_A] != 0.0f;
}

- (GLfloat *)colorMults
{
	return (GLfloat *) &_colorMults[0];
}

- (GLfloat *)colorOffsets
{
	return (GLfloat *) &_colorOffsets[0];
}

#pragma mark -
#pragma mark Private methods

@end