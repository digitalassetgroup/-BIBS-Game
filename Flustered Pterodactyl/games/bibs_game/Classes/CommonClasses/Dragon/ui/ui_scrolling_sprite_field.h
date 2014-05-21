#pragma once
#ifndef SUPER_KID_CANNON_SCROLLING_SPRITE_FIELD_H
#define SUPER_KID_CANNON_SCROLLING_SPRITE_FIELD_H "$Rev: 518 $"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "cocos2d.h"
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace ig_interface
{
	
	class ScrollingSpriteField : public cocos2d::CCNode
	{
	public:
		virtual ~ScrollingSpriteField(void);

		/**
		 *	Create a scrolling backdrop by calling this factor method.  The object will clean up after itself much
		 *	like the ig_interface::Label.
		 *
		 *	@param sheetName is the name of the Sprite Helper scene that the scrolling backdrop image exists on.
		 *	@param imageNames should contain the images to be created at random to scroll over the sprite field.
		 *
		 *	@note this is not as efficient as using an already loaded sprite helper scene as a temporary scene will be
		 *	loaded and destroyed within.
		 */
		static ScrollingSpriteField* makeSpriteField(const std::string& sheetName,
													 const std::vector<std::string>& imageNames,
													 const unsigned int numSpritesInField);

		/**
		 *	Create a scrolling backdrop by calling this factor method.  The object will clean up after itself much
		 *	like the ig_interface::Label.
		 *
		 *	@param loader is a pointer to an already existing sprite helper scene.
		 *	@param imageNames should contain the images to be created at random to scroll over the sprite field.
		 */		
		static ScrollingSpriteField* makeSpriteField(const std::vector<std::string>& imageNames,
													 const unsigned int numSpritesInField);
		
		/**
		 *	Move the the sprites by the amount specified;  Each sprite will have a speed modifier so the actual
		 *	distance moved can be more or less than the scrollAmount.
		 *	
		 *	@param scrollAmount is the amount to scroll by.  Absolute value cannot exceed backdrop width or height.
		 */
		void scrollBy(const float scrollAmountX, const float scrollAmountY);
        
        void scrollByRotation(float dt);

		void randomizeOrientations(const float minAngleInDegrees, const float maxAngleInDegrees);
		void randomizeOpacity(const float minOpacity, const float maxOpacity);
		void randomizeSpeedModifiers(const float minSpeed, const float maxSpeed);
		void randomizeScales(const float minScale, const float maxScale);
		void randomizedFlips(bool flipX, bool flipY);
        void forceFlipX(bool flipX);
        void forceFlipY(bool flipY);
		
		/**
		 *	Use this to setup the bounds of the field, by default the full screen + some overlap is used.
		 */
		void setFieldBounds(const float minX, const float minY, const float maxX, const float maxY);
		
	private:
		/* Prevent accidental use.  To Create a scrolling backdrop use the static Factory methods. */
		ScrollingSpriteField(void);
		ScrollingSpriteField(const ScrollingSpriteField& obj);
		ScrollingSpriteField& operator=(const ScrollingSpriteField& obj);
        
		struct SlidingObject
		{
			cocos2d::CCSprite* m_image;
			float m_speedModifier;
		};
		
        void handle_clipping(SlidingObject& obj);
		typedef std::vector<SlidingObject> SlidingObjectsContainer;
		typedef SlidingObjectsContainer::const_iterator SlidingObjectsCItr;
		SlidingObjectsContainer m_slidingObjects;
		
		struct
		{
			float minX;
			float minY;
			float maxX;
			float maxY;
		} m_fieldBounds;
	};
	
}; /* namespace ig_interface */

#endif /* SUPER_KID_CANNON_SCROLLING_SPRITE_FIELD_H */
