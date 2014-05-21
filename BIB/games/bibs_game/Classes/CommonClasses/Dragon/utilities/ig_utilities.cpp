#include "ig_utilities.h"
#include <math.h>
#include "cocos2d.h"

static cocos2d::CCPoint g_convertRatioReal(1, 1);
const cocos2d::CCPoint* g_convertRatio = &g_convertRatioReal;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace DracoGames 
{
    namespace Utils 
	{
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        float DistanceTo(const DragonVec2 &a, const DragonVec2 &b)
        {
            float xd(a.x - b.x);
            float yd(a.y - b.y);
            return sqrtf((xd * xd) + (yd * yd));
        }
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        float DistanceTo(float x1, float y1, float x2, float y2)
        {
            float xd(x2 - x1);
            float yd(y2 - y1);
            return sqrtf((xd * xd) + (yd * yd));
        }
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        float DistanceToSq(float x1, float y1, float x2, float y2)
        {
            float xd(x2 - x1);
            float yd(y2 - y1);
            return ((xd * xd) + (yd * yd));
        }
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        float DistanceFromLine(float x1, float y1, float x2, float y2, float xPt, float yPt)
        { /* Resource used: http://paulbourke.net/geometry/pointline/ */ 
            float u ((((xPt - x1) * (x2 - x1)) + ((yPt - y1) * (y2 - y1))) / DistanceToSq (x1, y1, x2, y2));
            float xIntersect(x1 + (u * (x2 - x1)));
            float yIntersect(y1 + (u * (y2 - y1)));
            return DistanceTo(xIntersect, yIntersect, xPt, yPt);
        }
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        float AngleFromVec2(float x, float y)
        {	/* DotProduct between x,y and 0,-1 vector: angle = acos ( (x * 0.0f) + (y * 1.0f)); (or more simply;) */
            float angle((acosf (y) / igPI) * 180.0f);
            if (x < 0.0) angle = 360.0f - angle;
            return angle;
        }
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        float AngleFromVec2(float *xy)
        {
            return AngleFromVec2(xy[0], xy[1]);
        }
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void Vec2FromAngle(float *x, float *y, float angle)
        {
            float angleInRadians((angle / 360.0f) * igPI * 2.0f);
            *x = sinf(angleInRadians);
            *y = cosf(angleInRadians);
        }
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        float* Vec2FromAngle(float *xy, float angle)
        {
            Vec2FromAngle(&xy[0], &xy[1], angle);
            return xy;
        }
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        DragonVec2 Vec2FromAngle (float angle)
        {
            DragonVec2 toReturn;
            float angleInRadians((angle / 360.0f) * igPI * 2.0f);
            toReturn.x = sinf(angleInRadians);
            toReturn.y = cosf(angleInRadians);

            return toReturn;
        }
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        float AngleBetween2Vectors(const DragonVec2 &a, const DragonVec2 &b)
        {
            /* perhaps this is the wrong formula */
            float ang( (atan2 ( (a.x * b.y) - (a.y * b.x), (a.x * b.x) + (a.y * b.y))) / igPI * 180.0f);

            if(0 > ang)
            {
                ang = - ang;
            }

            return ang;
        }
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        int randomInt(void)
        {
            return rand();
        }
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        int randomInt(int min, int max)
        {
            assert_if(min == max, "Cannot produce random number with range of 0.  (min == max)");
			assert_if(min > max, "min should never be greater than max.");
            return (rand() % ((max - min) + 1)) + min;
        }
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
        float randomFloat(void)
        {
            return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        }
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        float randomFloat(float min, float max)
        {
			assert_if(min > max, "min should never be greater than max.");			
            return (randomFloat() * (max - min)) + min;
        }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		bool isEqual(const float lhs, const float rhs)
		{
			return (abs(lhs - rhs) <= igEpsilon) ? true : false;
		}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////		
		
		bool isZero(const float value)
		{
			return (abs(value) <= igEpsilon) ? true : false;
		}		
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        Spherical_Coord spherical_from_cartesian(float x, float y, float z)
        {
            static const float epsilon(powf(2,-23));
			
            const float phi((x > epsilon || x < -epsilon) ? acosf (y / x) : 0.0f);

            const float ess(x * x + y * y);
            const float rho(sqrtf (ess + z * z));

            const float theta_partial(asinf (y / sqrtf (ess)));
            const float theta((0 <= x) ? theta_partial : (float)M_PI - theta_partial);

            Spherical_Coord s = { theta, phi == phi ? phi : 0.0f, rho };
            return s;
        }
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		
		bool compare_y_ccpoint(const cocos2d::CCPoint& first, const cocos2d::CCPoint& second)
		{
			if(first.y <= second.y)
			{
				return true;
			}

			return false;
		}
		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    }; /* namespace Utils */
}; /* namespace DracoGames */
