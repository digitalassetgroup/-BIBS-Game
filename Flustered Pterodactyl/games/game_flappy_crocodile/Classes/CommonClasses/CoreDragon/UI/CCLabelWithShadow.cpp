#include "CCLabelWithShadow.h"
#include "label_nodes/CCLabelBMFont.h"

CCLabelWithShadow::CCLabelWithShadow() 
	: _label(),
	_labelShadow()
{
}

CCLabelWithShadow* CCLabelWithShadow::create(const char* str, const char* fntFile, 
	const CCPoint& shadowOffset, CCTextAlignment alignment)
{
	auto result = new CCLabelWithShadow();
	result->autorelease();
	result->_label = CCLabelBMFont::create(str, fntFile, -1, alignment);
	result->_labelShadow = CCLabelBMFont::create(str, fntFile, -1, alignment);
	result->_labelShadow->addPositionX(shadowOffset.x);
	result->_labelShadow->addPositionY(shadowOffset.y);
	result->_labelShadow->setColor(ccc3(0, 0, 0));

	result->addChild(result->_label);	
	result->addChild(result->_labelShadow, -1);

	return result;
}

void CCLabelWithShadow::SetString(const string& str)
{
	_label->setString(str.c_str());
	_labelShadow->setString(str.c_str());
}

void CCLabelWithShadow::SetLabelAnchorPoint(const CCPoint& anchorPoint)
{
	setAnchorPoint(anchorPoint);
	_label->setAnchorPoint(anchorPoint);
	_labelShadow->setAnchorPoint(anchorPoint);
}


//--------CCRGBAProtocol-----------
void CCLabelWithShadow::setColor(const ccColor3B& color)
{
	_label->setColor(color);
}

const ccColor3B& CCLabelWithShadow::getColor(void)
{
	return _label->getColor();
}

const ccColor3B& CCLabelWithShadow::getDisplayedColor(void)
{
	return _label->getDisplayedColor();
}

GLubyte CCLabelWithShadow::getDisplayedOpacity(void)
{
	return _label->getDisplayedOpacity();
}

GLubyte CCLabelWithShadow::getOpacity(void)
{
	return _label->getOpacity();
}

void CCLabelWithShadow::setOpacity(GLubyte opacity)
{
	_label->setOpacity(opacity);
	_labelShadow->setOpacity(opacity);
}

void CCLabelWithShadow::setOpacityModifyRGB(bool bValue)
{
	_label->setOpacityModifyRGB(bValue);
	_labelShadow->setOpacityModifyRGB(bValue);
}

bool CCLabelWithShadow::isOpacityModifyRGB(void)
{
	return _label->isOpacityModifyRGB();
}

bool CCLabelWithShadow::isCascadeColorEnabled(void)
{
	return _label->isCascadeColorEnabled();
}

void CCLabelWithShadow::setCascadeColorEnabled(bool cascadeColorEnabled)
{
	_label->setCascadeColorEnabled(cascadeColorEnabled);
}

void CCLabelWithShadow::updateDisplayedColor(const ccColor3B& color)
{
	_label->updateDisplayedColor(color);
}

bool CCLabelWithShadow::isCascadeOpacityEnabled(void)
{
	return _label->isCascadeOpacityEnabled();
}

void CCLabelWithShadow::setCascadeOpacityEnabled(bool cascadeOpacityEnabled)
{
	_label->setCascadeOpacityEnabled(cascadeOpacityEnabled);
}

void CCLabelWithShadow::updateDisplayedOpacity(GLubyte opacity)
{
	_label->updateDisplayedOpacity(opacity);
}

