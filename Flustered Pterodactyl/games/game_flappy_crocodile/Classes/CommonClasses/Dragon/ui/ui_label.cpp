#include "ui_label.h"

using namespace cocos2d;

namespace ig_interface
{

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	Label::Label(const std::string& desc, const FontSizeEnum fontSize, const float desiredWidth, const cocos2d::CCPoint& position) :
		m_description(desc),
        m_desiredWidth(desiredWidth),
        m_position(position),
		centerPiece(nullptr),
		m_size(fontSize),
		m_alignment(LABEL_ALIGN_CENTER)
	{      
        adjustGeometry();
    }

	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	Label::~Label(void)
	{
			
	}
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    void Label::setAligntment(const LabelAlignmentEnum& alighment)
    {
        m_alignment = alighment;
        adjustGeometry();
    }
    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
    
    void Label::setDisiredWidth(float desiredWidth)
    {
        m_desiredWidth = desiredWidth;
        adjustGeometry();
    }
    
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
    
    void Label::adjustGeometry()
    {
        if (centerPiece)
        {
            removeChild(centerPiece, true);
        }
	
        centerPiece = new CCLabelBMFont();
		
		const bool isIpad = false;//(LHSettings::sharedInstance()->isIpad());
        
		if(true == isIpad &&  FONT_SIZE_LARGE == m_size)
        {
            centerPiece->initWithString(m_description.c_str(), "fonts/arial16.fnt" /*"font_droid_large.fnt"*/);
        }
        else if((true == isIpad && FONT_SIZE_MEDIUM == m_size) || (false == isIpad && FONT_SIZE_LARGE == m_size))
        {
            centerPiece->initWithString(m_description.c_str(), "fonts/some_font.fnt" /*"font_droid_medium.fnt"*/);
        }
        else if(true == isIpad || (false == isIpad && FONT_SIZE_MEDIUM == m_size)) 
        {			
            centerPiece->initWithString(m_description.c_str(), "fonts/arial16.fnt"/*"font_droid_small.fnt"*/);
        }
		else
		{
			assert_if(true == isIpad, "Unexpected font size to be used!");
			centerPiece->initWithString(m_description.c_str(), "fonts/arial16.fnt" /*"font_droid_very_small.fnt"*/);	
		}
		
        centerPiece->setColor(ccc3(255, 255, 255));        
        addChild(static_cast<CCNode*>(centerPiece));
		centerPiece->release();
		
		centerPiece->boundingBox();

		auto sizeInPixels = CCSize(centerPiece->boundingBox().size.width+2, 
										centerPiece->boundingBox().size.height+2);
		//m_tContentSizeInPixels = sizeInPixels
		this->setContentSize(sizeInPixels);		
        
        if (m_desiredWidth < 0)
        {
            setPosition(m_position);
        }
        else
        {
            cocos2d::CCPoint p = m_position;						
            const float labelWidth(sizeInPixels.width);
			//const float labelHeight(sizeInPixels.height);
			if(labelWidth > m_desiredWidth)
            {	// not enought space for text - scaling down size and do not take alighment into account
                setScale(m_desiredWidth / labelWidth);
				printf("scale of label is now: %f\n", getScale());
                warning_if( (labelHeight * m_desiredWidth / labelWidth) < UNREADABLE_HEIGHT, "DracoGames Warning: unreadable height.");
            }
            else
            {
                if(m_alignment != LABEL_ALIGN_CENTER)
                {
                    float dx = 0.5f * (m_desiredWidth - sizeInPixels.width);
                    if(m_alignment == LABEL_ALIGN_LEFT)
                    {
                        p.x -= dx;
                    }
                    else /* LABEL_ALIGN_RIGHT */
                    {
                        p.x += dx;
                    }
                }
            }
            setPosition(p);                
        }
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
	const cocos2d::CCSize& Label::getContentSize() const
	{
		return centerPiece->getContentSize();
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void Label::setColor(const ccColor3B& color3)
	{
		centerPiece->setColor(color3);
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	void Label::setString(const std::string& text)
	{
        m_description = text;
		adjustGeometry();	
	}	
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	Label* Label::makeLabelWithKey(const std::string& textKey, const CCPoint& position, const FontSizeEnum fontSize, float desiredWidth /*= -1.0f*/)
	{
		const std::string& description = textKey;//DracoGames::igTextTranslator::getInstance()->translate(textKey);
		return new Label(description, fontSize, desiredWidth, position);
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	Label* Label::makeLabelWithText(const std::string& text, const CCPoint& position, const FontSizeEnum fontSize, float desiredWidth /*= -1.0f*/)
	{
		return new Label(text, fontSize, desiredWidth, position);
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
}; /* namespace ig_interface */
