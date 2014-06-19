#pragma once
#ifndef SUPER_KID_SCENE_ASSERT_H
#define SUPER_KID_SCENE_ASSERT_H "$Rev: 695 $"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "scene_base.h"
#include <string>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CAssertScene : public CBaseScene
{
public:
	CAssertScene(const std::string& assertMessage, const std::string &fileName, const unsigned int line);
	virtual ~CAssertScene(void);

	/* CBaseScene Methods/Overrides.  See obj_base.h for documentation */	
	virtual std::string GetSceneName(void) const { return "scene_assert"; }
	virtual void OnEnter(CBaseScene* prevScene);
	virtual void OnLeave(CBaseScene* nextScene);

private:
	/* Prevent accidental use */
	CAssertScene(const CAssertScene& obj);
	CAssertScene& operator=(const CAssertScene& obj);
	
	std::string fixStringToLength(const std::string& msg, const unsigned int length);
	
	std::string m_assertMessage;
	const std::string m_fileName;
	const unsigned int m_lineNumber;	
};


#endif /* SUPER_KID_SCENE_ASSERT_H */

