#pragma once
#ifndef _cannonboy_ui_label_h_
#define _cannonboy_ui_label_h_ "$Rev: 2345 $"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//#include "../objects/obj_base.h"
#include "cocos2d.h"
#include <string>

//#include "../cannonboy.h"

#include "../effects/effect_manager.h"	/* Dependent on this for the button scale up effect to complete itself. */
#include "../effects/node_effect_button_slide_in.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define UNREADABLE_HEIGHT 10

namespace ig_interface
{
	enum LabelAlignmentEnum { LABEL_ALIGN_CENTER, LABEL_ALIGN_LEFT, LABEL_ALIGN_RIGHT};
    
	enum FontSizeEnum { FONT_SIZE_LARGE, FONT_SIZE_MEDIUM, FONT_SIZE_SMALL };
	

	
	class Label : public cocos2d::CCNode
	{
	public:
		
		virtual ~Label(void);
		
		const cocos2d::CCSize& getContentSize() const;

		/**
		 *	Sets the font color of the text being displayed.
		 */
		void setColor(const cocos2d::ccColor3B& color3);

		/**
		 *	Changes the string of the label.
		 *
		 *	@note the cocos label implementation will render the new string to an image and use that for future use.
		 *	Only call this when the string changes as it is a fairly expensive operation!
		 */		
		virtual void setString(const std::string& text);
		const std::string& getString() { return m_description; }
        
        void setAligntment(const LabelAlignmentEnum& alighment);
        
        
        void setDisiredWidth(float desiredWidth);
		/**
		 *	Create a label from a key that will be used to look up the actual string from the text_translator.
		 */
		static Label* makeLabelWithKey(const std::string& key, const cocos2d::CCPoint& position = ccp(0,0), 
									   const FontSizeEnum fontSize = defaultFontSize(), float desiredWidth = -1.0f);
		
		/**
		 *	Create a label from a direct string, performing no lookup with the text translator.
		 *
		 *	@note using makeLabelWithKey is preferred to this variation.
		 */
		static Label* makeLabelWithText(const std::string& text, const cocos2d::CCPoint& position = ccp(0,0), 
										const FontSizeEnum fontSize = defaultFontSize(), float desiredWidth = -1.0f);		
		
		/**
		 *	Returns the default font size that should be used... by default.  Increase if necessary.
		 */
		static inline FontSizeEnum defaultFontSize(void) { return FONT_SIZE_MEDIUM; }

		/**
		 *	Returns the size of the font that will be used for titles, headers, etc.
		 */
		static inline FontSizeEnum headerFontSize(void) { return FONT_SIZE_LARGE; }
		
		/**
		 *	Returns the font size that should be used on buttons... by default.
		 */		
		static inline FontSizeEnum buttonFontSize(void) { return FONT_SIZE_MEDIUM; }
		
		/**
		 *	Returns the font size that should be used in paragraph or lengthy descriptions.  Use sparingly.
		 */
		static inline FontSizeEnum descriptionFontSize(void) { return FONT_SIZE_SMALL; }
		
		/**
		 *	Returns the font size for small labels etc.
		 */
		static inline FontSizeEnum smallFontSize(void) { return FONT_SIZE_SMALL; }
		
		virtual void setPosition(const cocos2d::CCPoint& var) { cocos2d::CCNode::setPosition(var); m_position = var; }

	protected:
		/* To create a label use the static Factory methods. */
		Label(const std::string& desc, const FontSizeEnum fontSize, const float desiredWidth, const cocos2d::CCPoint& position);
		
	private:
		/* Prevent accidental use. */
		Label(const Label& obj);
		Label& operator=(const Label& obj);
        void adjustGeometry();

		virtual void setScaleX(float xScale) { cocos2d::CCNode::setScaleX(xScale); }
		virtual void setScaleY(float yScale) { cocos2d::CCNode::setScaleY(yScale); }
		virtual void setScale(float scale) { cocos2d::CCNode::setScale(scale); }
		
        cocos2d::CCPoint     m_position;
		std::string m_description;
		float m_desiredWidth;
        LabelAlignmentEnum m_alignment;
		cocos2d::CCLabelBMFont* centerPiece;
		
		FontSizeEnum m_size;
	};
	
}; /* namespace ig_interface */

extern ig_interface::Label* gMediumLabelPreCache;
extern ig_interface::Label* gLargeLabelPreCache;
extern ig_interface::Label* gSmallLabelPreCache;

#endif /* _cannonboy_ui_label_h_ */
