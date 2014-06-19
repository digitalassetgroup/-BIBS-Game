////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GAFNode.m
//  AnimationTest
//
//  Created by Gregory Maksyuk on 11/29/12.
//  Copyright 2012 Catalyst Apps. All rights reserved.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Imports

#import "GAFNode.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Constants

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private interface

@interface GAFNode ()
@end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Implementation

@implementation GAFNode

#pragma mark -
#pragma mark Properties

@synthesize useExternalTransform;
@synthesize externalTransform;

- (void)setExternalTransform:(CGAffineTransform)anExternalTransform
{
    if (!CGAffineTransformEqualToTransform(externalTransform, anExternalTransform))
    {
        externalTransform = anExternalTransform;
        _isTransformDirty = _isInverseDirty = YES;
        self.useExternalTransform = YES;
    }
}

#pragma mark -
#pragma mark Overriden methods

- (CGAffineTransform)nodeToParentTransform
{
    if (self.useExternalTransform)
    {
        if (_isTransformDirty)
        {
            _transform = CGAffineTransformTranslate(externalTransform, -self.anchorPoint.x, -self.anchorPoint.y);
            
            _isTransformDirty = NO;
            _isInverseDirty = YES;
        }
        return _transform;
	}
    else
    {
        return [super nodeToParentTransform];
    }
}

@end