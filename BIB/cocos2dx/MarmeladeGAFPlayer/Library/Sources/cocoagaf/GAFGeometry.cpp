#include "GAFGeometry.h"

namespace GAF
{

CCPoint::CCPoint()
:
x(0), y(0)
{
}
	
CCPoint::CCPoint(float _x, float _y)
:
x(_x), y(_y)
{
}

CCSize::CCSize()
:
width(0), height(0)
{
}
	
CCSize::CCSize(float w, float h)
:
width(w), height(h)
{
}
	
CCRect::CCRect()
{

}
	
CCRect::CCRect(float x, float y, float width, float height)
:
origin(x, y),
size(width, height)
{
}
	
	
	
} // namespace GAF