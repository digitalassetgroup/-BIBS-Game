#ifndef CCLabelWithShadow_h
#define CCLabelWithShadow_h

#include "base_nodes/CCNode.h"
#include <string>
using std::string;
using namespace cocos2d;
namespace cocos2d { class CCLabelBMFont; }


class CCLabelWithShadow : public CCNode, public CCRGBAProtocol
{
private:
	CCLabelBMFont* _label;
	CCLabelBMFont* _labelShadow;

public:
	static CCLabelWithShadow* create(const char* str, const char* fntFile, 
		const CCPoint& shadowOffset, CCTextAlignment alignment = kCCTextAlignmentLeft);
	void SetString(const string& str);
	void SetLabelAnchorPoint(const CCPoint& anchorPoint);

	//--------CCRGBAProtocol-----------
	virtual void setColor(const ccColor3B& color);
	virtual const ccColor3B& getColor(void);
	virtual const ccColor3B& getDisplayedColor(void);
	virtual GLubyte getDisplayedOpacity(void);
	virtual GLubyte getOpacity(void);
	virtual void setOpacity(GLubyte opacity);
	virtual void setOpacityModifyRGB(bool bValue);
	virtual bool isOpacityModifyRGB(void);
	virtual bool isCascadeColorEnabled(void);
	virtual void setCascadeColorEnabled(bool cascadeColorEnabled);
	virtual void updateDisplayedColor(const ccColor3B& color);
	virtual bool isCascadeOpacityEnabled(void);
	virtual void setCascadeOpacityEnabled(bool cascadeOpacityEnabled);
	virtual void updateDisplayedOpacity(GLubyte opacity);
	//-------------

private:
	CCLabelWithShadow();
};

#endif