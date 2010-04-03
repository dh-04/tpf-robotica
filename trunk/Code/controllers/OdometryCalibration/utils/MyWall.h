// Class automatically generated by Dev-C++ New Class wizard

#ifndef utils_MyWall_h
#define utils_MyWall_h

#include <utils/MyPoint.h>

namespace utils {

/**
 * No description
 */
class MyWall
{
	public:
		// class constructor
		MyWall(double length, double width, double x, double y, bool horizontal);
		// class destructor
		~MyWall();
		
		bool isHorizontal();
		double getX();
		double getY();
		double getLength();
		double getWidth();
		
	private:
		double length;
		double x;
		double y;
		bool horizontal;
		double width;
};

}
#endif // utils_MyWall_h
