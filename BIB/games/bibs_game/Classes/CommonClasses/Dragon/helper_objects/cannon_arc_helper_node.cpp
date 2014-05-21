#include "cannon_arc_helper_node.h"
#include "cocos2d.h"
#include "../utilities/ig_assert.h"
//#include "cannonboy.h"

#include <limits.h>

void CannonArcHelperNode::draw(void)
{
#if COCOS2D_VERSION < 0x00020000    
	/** 
	 explainations :
	 
	 Override this method to draw your own node.
	 The following GL states will be enabled by default:
	 - glEnableClientState(GL_VERTEX_ARRAY);
	 - glEnableClientState(GL_COLOR_ARRAY);
	 - glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	 - glEnable(GL_TEXTURE_2D);
	 
	 AND YOU SHOULD NOT DISABLE THEM AFTER DRAWING YOUR NODE
	 
	 But if you enable any other GL state, you should disable it after drawing your node.
	 */
	
	if(nullptr != m_arcPath && m_drawPath)
	{				
		glDisable(GL_TEXTURE_2D);
		glDisableClientState(GL_COLOR_ARRAY);
		
		glLineWidth(5.0f/* * g_convertRatio->x*/);
	
		b2Color color = b2Color(0.0f, 0.0f, 0.0f);
		
		glColor4f(color.r, color.g, color.b, 0.5f);
		
		size_t vertexCount(m_arcPath->size());
		
		DragonVec2* vertices(new DragonVec2[vertexCount]);
		
		for (size_t i(0);i<vertexCount;i++)
		{
			vertices[i] = ((*m_arcPath)[i]);
		}		

		glVertexPointer(2, GL_FLOAT, 0, vertices);

		assert_if(vertexCount > INT_MAX, "Unsafe cast below.\n");
		glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(vertexCount));
		
		
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_COLOR_ARRAY);
		
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	}
#endif
}



