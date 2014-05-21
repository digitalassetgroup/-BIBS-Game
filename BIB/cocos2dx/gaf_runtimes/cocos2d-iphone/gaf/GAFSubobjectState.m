#import "GAFSubobjectState.h"
#import "GAFFilterData.h"
#import "ccMacros.h"

static NSString * const kStateName      = @"st";
static NSString * const kColorName      = @"c";
static NSString * const kMaskName       = @"m";
static NSString * const kEffectsName    = @"e";
static NSString * const kEffectTypeName = @"t";
static NSString * const kXName          = @"x";
static NSString * const kYName          = @"y";
static NSString * const kBlurEffectName = @"Fblur";

@interface GAFSubobjectState ()
@end

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

- (id)initWithObjectId:(NSString *)objId
{
    NSAssert(objId != nil, @"parameters should not be nil");    
    self = [super init];
    if (nil != self)
    {
		self.objectId = objId;
		self.zIndex = 0;
		self.affineTransform = CGAffineTransformMake(1,0,0,1,0,0);
		_colorOffsets[0] = _colorOffsets[1] = _colorOffsets[2] = _colorOffsets[3] = _colorMults[GAFCTI_A] = 0;
		_colorMults[GAFCTI_R]   = _colorMults[GAFCTI_G]   = _colorMults[GAFCTI_B] = 1;
	}
	return self;
}

- (id)initWithStateDictionary:(NSDictionary*)dict objectId:(NSString*)objId;
{

	self = [super init];
    if (nil != self)
    {
		self.objectId = objId;
		if (nil == objId)
		{
			CCLOGWARN(@"Error while creating GAFSubobjectState. Invalid objectId");
			[self release];
			return nil;
		}
		NSString * state = (NSString*) [dict objectForKey:kStateName];
		if (!state)
		{
			CCLOGWARN(@"Error while creating GAFSubobjectState. State state is missing");
			[self release];
			return nil;
		}
		NSRange s = [state rangeOfString:@"{"];
		NSRange e = [state rangeOfString:@"}"];
		if (s.length == 0 || e.length == 0 || e.location < s.location)
		{
			CCLOGWARN(@"Error while creating GAFSubobjectState. Invalid state line");
			[self release];
			return nil;
		}
		NSString * temp = [state substringWithRange:NSMakeRange(0, s.location - 1)];
		self.zIndex = [temp intValue];
		NSRange r = NSMakeRange(e.location + 2, [state length] - e.location - 2);
		temp = [state substringWithRange:r];
		_colorMults[GAFCTI_A] = [temp floatValue];
		temp = [state substringWithRange:NSMakeRange(s.location + 1, e.location - s.location - 1)];
		NSArray * transform = [temp componentsSeparatedByString:@","];
		self.affineTransform = CGAffineTransformMake(
													 [transform[0] floatValue],
													 [transform[1] floatValue],
													 [transform[2] floatValue],
													 [transform[3] floatValue],
													 [transform[4] floatValue],
													 [transform[5] floatValue]
													 );
		
		NSString * color = (NSString*) [dict objectForKey:kColorName];
		if (color != nil)
		{
			NSArray * clrs = [color componentsSeparatedByString:@","];
			if ([clrs count] < 7)
			{
				CCLOGWARN(@"Error while creating GAFSubobjectState. Invalid color data '%@'", state);
				return false;
			}
			_colorOffsets[GAFCTI_A] = [clrs[0] floatValue]/ 255.0f;
			
			_colorMults[GAFCTI_R]   = [clrs[1] floatValue];
			_colorOffsets[GAFCTI_R] = [clrs[2] floatValue] / 255.0f;
			
			_colorMults[GAFCTI_G] =   [clrs[3] floatValue];
			_colorOffsets[GAFCTI_G] = [clrs[4] floatValue] / 255.0f;
			
			_colorMults[GAFCTI_B]   = [clrs[5] floatValue];
			_colorOffsets[GAFCTI_B] = [clrs[6] floatValue] / 255.0f;
		}
		else
		{
			_colorOffsets[0] = _colorOffsets[1] = _colorOffsets[2] = _colorOffsets[3] = 0;
			_colorMults[GAFCTI_R]   = _colorMults[GAFCTI_G]   = _colorMults[GAFCTI_B] = 1;
		}
		NSString * mask = (NSString*) [dict objectForKey:kMaskName];
		if (mask != nil)
		{
			self.maskObjectId = mask;
		}
		NSArray * effects = (NSArray*) [dict objectForKey:kEffectsName];
		if (effects != nil)
		{
			NSMutableDictionary * mutableFilters = [NSMutableDictionary dictionary];
			for (int i = 0; i < [effects count]; ++i)
			{
				NSDictionary * dict = (NSDictionary*)[effects objectAtIndex:i];
				NSString * type = (NSString*)[dict objectForKey:kEffectTypeName];
				if (type != nil)
				{
					if ([type isEqualToString:kBlurEffectName])
					{
						NSString * x = (NSString*)[dict objectForKey:kXName];
						NSString * y = (NSString*)[dict objectForKey:kYName];
						if (x != nil && y != nil)
						{
							GAFBlurFilterData * blurFilter = [GAFBlurFilterData new];
							blurFilter.blurSize = CGSizeMake([x floatValue], [y floatValue]);
							[mutableFilters setObject:blurFilter forKey:kBlurFilterName];
							[blurFilter release];
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

- (id)copyWithZone:(NSZone *)zone
{
    GAFSubobjectState *newState = [[GAFSubobjectState allocWithZone:zone] init];
    newState.objectId = self.objectId;
    newState.zIndex = self.zIndex;
    newState.maskObjectId = self.maskObjectId;
    newState.atlasElementName = self.atlasElementName;
    newState.affineTransform = self.affineTransform;
    newState.filters = [[self.filters copyWithZone:zone] autorelease];    
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

- (void)dealloc
{
    [filters release];
    [objectId release];
    [maskObjectId release];
    [atlasElementName release];
    [super dealloc];
}

- (BOOL) isVisible
{
	return _colorMults[GAFCTI_A] != 0;
}

- (GLfloat*) colorMults
{
	return &_colorMults[0];
}

- (GLfloat*) colorOffsets
{
	return &_colorOffsets[0];
}
#pragma mark -
#pragma mark Public methods

#pragma mark -
#pragma mark Private methods

@end