#include "ui_scrolling_sprite_field.h"
#include "ig_interface.h"

#include "../utilities/ig_utilities.h"
//#include "../../cannonboy.h"	/* for the ability to read gScreenSize */
#include "Core/CoreAll.h"
#include "CorePlatform/SpriteHelper.h"

using namespace cocos2d;
using namespace DracoGames;
using namespace DracoGames::Utils;

namespace ig_interface
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	ScrollingSpriteField::ScrollingSpriteField(void)
	{
		autorelease();
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	ScrollingSpriteField::~ScrollingSpriteField(void)
	{

	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	ScrollingSpriteField* ScrollingSpriteField::makeSpriteField(const std::string& shSheetName,
																const std::vector<std::string>& imageNames,
																const unsigned int numSpritesInField)
	{
		//SpriteHelperLoader shl(shSheetName);
		return makeSpriteField(imageNames, numSpritesInField);
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	ScrollingSpriteField* ScrollingSpriteField::makeSpriteField(const std::vector<std::string>& imageNames,
																const unsigned int numSpritesInField)
	{
		//assert_if(nullptr == shl, "Invalid value for parameter; shl - expected valid pointer.");
		assert_if(0 == imageNames.size(), "Invalid value for parameter; imageNames - expected at least 1 image name.");
		assert_if(0 == numSpritesInField, "Invalid value for parameter; numSpritesInField - expected greater than 0");
		
		ScrollingSpriteField* field = new ScrollingSpriteField();

		const CCPoint& scale(*g_convertRatio);
		const float FieldBorderX(200.0f * scale.x);
		const float FieldBorderY(200.0f * scale.y);

		field->setFieldBounds(-FieldBorderX, -FieldBorderY, 
							  gScreenSize.width + (2.0f * FieldBorderX), gScreenSize.height + (2.0f * FieldBorderY));

//		field->m_fieldBounds.minX = -FieldBorderAmountX;
//		field->m_fieldBounds.minY = -FieldBorderAmountY;
//		field->m_fieldBounds.maxX = gScreenSize.width + (2.0f * FieldBorderAmountX);
//		field->m_fieldBounds.maxY = gScreenSize.height + (2.0f * FieldBorderAmountX);
		
		for(unsigned int i = 0; i < numSpritesInField; ++i)
		{
			const std::string randomName(imageNames[static_cast<size_t>(randomInt()) % imageNames.size()]);
			const CCPoint randomPoint(randomFloat(field->m_fieldBounds.minX, field->m_fieldBounds.maxX),
									  randomFloat(field->m_fieldBounds.minY, field->m_fieldBounds.maxY));
			
			SlidingObject obj;
			obj.m_image = SpriteHelper::CreateSpriteWithTextureName(field, 0, randomName, ccp(0, 0));
			//obj.m_image = shl->spriteWithUniqueName(randomName, randomPoint);
			//field->addChild(obj.m_image);
			assert_if(nullptr == obj.m_image, "The image %s was not created!", randomName.c_str());
			obj.m_speedModifier = 1.0f;
			field->m_slidingObjects.push_back(obj);
		}
	
		return field;
	}	
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void ScrollingSpriteField::scrollBy(const float scrollAmountX, const float scrollAmountY)
	{		
		for_each_ref(SlidingObject& obj, m_slidingObjects)
		{	/* Move the object by the amount the sprite field scrolled by. */
			obj.m_image->setPositionX(obj.m_image->getPositionX() + (scrollAmountX * obj.m_speedModifier));
			obj.m_image->setPositionY(obj.m_image->getPositionY() + (scrollAmountY * obj.m_speedModifier));
            this->handle_clipping(obj);
        }
		for_each_ref_end
	}
    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    void ScrollingSpriteField::scrollByRotation(float deltaTime)
	{
		for_each_ref(SlidingObject& obj, m_slidingObjects)
		{	/* Move the object by the amount the sprite field scrolled by. */
			const float radians(DracoGames::Utils::DegreesToRadians(obj.m_image->getRotation()));
			obj.m_image->setPositionX(obj.m_image->getPositionX() + (deltaTime * obj.m_speedModifier * cosf(radians)));
			obj.m_image->setPositionY(obj.m_image->getPositionY() - (deltaTime * obj.m_speedModifier * sinf(radians)));
            this->handle_clipping(obj);
		}
		for_each_ref_end
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    void ScrollingSpriteField::handle_clipping(SlidingObject& obj)
    {
        /* Check if the object fell off horizontal bounds, place it on the other side and randomize vertically. */
        if(obj.m_image->getPositionX() < m_fieldBounds.minX)
        {
            obj.m_image->setPositionX(m_fieldBounds.maxX);
            obj.m_image->setPositionY(randomFloat(m_fieldBounds.minY, m_fieldBounds.maxY));
        }
        if(obj.m_image->getPositionX() > m_fieldBounds.maxX)
        {
            obj.m_image->setPositionX(m_fieldBounds.minX);
            obj.m_image->setPositionY(randomFloat(m_fieldBounds.minY, m_fieldBounds.maxY));
        }
        
        /* Check if the object fell off vertical bounds, place it on the other side and randomize horizontally. */			
        if(obj.m_image->getPositionY() < m_fieldBounds.minY)
        {
            obj.m_image->setPositionX(randomFloat(m_fieldBounds.minX, m_fieldBounds.maxX));
            obj.m_image->setPositionY(m_fieldBounds.maxY);
        }
        if(obj.m_image->getPositionY() > m_fieldBounds.maxY)
        {
            obj.m_image->setPositionX(randomFloat(m_fieldBounds.minX, m_fieldBounds.maxX));				
            obj.m_image->setPositionY(m_fieldBounds.minY);
        }

    }
    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void ScrollingSpriteField::randomizeOrientations(const float minAngleInDegrees, const float maxAngleInDegrees)
	{
		for_each_ref(SlidingObject& obj, m_slidingObjects)
		{
			obj.m_image->setRotation(randomFloat(minAngleInDegrees, maxAngleInDegrees));
		}
		for_each_ref_end
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void ScrollingSpriteField::randomizeOpacity(const float minOpacity, const float maxOpacity)
	{
		for_each_ref(SlidingObject& obj, m_slidingObjects)
		{
			obj.m_image->setOpacity(static_cast<GLubyte>(255.0f * randomFloat(minOpacity, maxOpacity)));
		}
		for_each_ref_end
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void ScrollingSpriteField::randomizeSpeedModifiers(const float minSpeed, const float maxSpeed)
	{
		for_each_ref(SlidingObject& obj, m_slidingObjects)
		{
			obj.m_speedModifier = randomFloat(minSpeed, maxSpeed);
		}
		for_each_ref_end
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void ScrollingSpriteField::randomizeScales(const float minScale, const float maxScale)
	{
		for_each_ref(SlidingObject& obj, m_slidingObjects)
		{
			const float newScale(randomFloat(minScale, maxScale));
			obj.m_image->setScaleX(obj.m_image->getScaleX() * newScale);
			obj.m_image->setScaleY(obj.m_image->getScaleY() * newScale);			
		}
		for_each_ref_end
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

    void ScrollingSpriteField::forceFlipX(bool flipX)
    {
        for_each_ref(SlidingObject& obj, m_slidingObjects)
		{
            obj.m_image->setFlipX(flipX);
        }
		for_each_ref_end
    }
    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
    
    void ScrollingSpriteField::forceFlipY(bool flipY)
    {
        for_each_ref(SlidingObject& obj, m_slidingObjects)
		{
            obj.m_image->setFlipY(flipY);
        }
		for_each_ref_end
    }
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	    
	
	void ScrollingSpriteField::setFieldBounds(const float minX, const float minY, const float maxX, const float maxY)
	{
		assert_if(minX > maxX, "Expected minX(%f) to be less than maxX(%f).\n", minX, maxX);
		assert_if(minY > maxY, "Expected minY(%f) to be less than maxY(%f).\n", minY, maxY);		
		
		m_fieldBounds.minX = minX;
		m_fieldBounds.minY = minY;
		m_fieldBounds.maxX = maxX;
		m_fieldBounds.maxY = maxY;
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
    
    void ScrollingSpriteField::randomizedFlips(bool flipX, bool flipY)
    {
		for_each_ref(SlidingObject& obj, m_slidingObjects)
		{
            if (flipX) 
            {
                obj.m_image->setFlipX(rand() & 1);
            }
            
            if (flipY) 
            {
                obj.m_image->setFlipY(rand() & 1);
            }			
		}
		for_each_ref_end
    }
    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

}; /* namespace ig_interface */
