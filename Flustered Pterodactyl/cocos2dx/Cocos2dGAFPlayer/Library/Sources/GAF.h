////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GAF.h
//  GAF Animation Library
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Imports

#import "GAFAnimatedObject.h"
#import "GAFStaticObject.h"
#import "GAFAnimationFrame.h"
#import "GAFAnimationSequence.h"
#import "GAFAsset.h"
#import "GAFFilterData.h"
#import "GAFNode.h"
#import "GAFShaderManager.h"
#import "GAFSprite.h"
#import "GAFSpriteWithAlpha.h"
#import "GAFSprite_Protected.h"
#import "GAFStencilMaskSprite.h"
#import "GAFSubobjectState.h"
#import "GAFTextureAtlas.h"
#import "GAFTextureAtlasElement.h"
#import "GAFAssetExtendedDataObject.h"
#import "GAFTextureEffectsConverter.h"

#import "GAFHelperMethods.h"

//#define CAPTURE_TIME(t) (t = [[NSDate date] timeIntervalSinceReferenceDate]);
//#define SHOW_PASSED_TIME(t, desc) (NSLog(@"%.3fms - (%@)", ([[NSDate date] timeIntervalSinceReferenceDate] - t) * 1000, desc));
