// Class automatically generated by Dev-C++ New Class wizard

#ifndef behaviours_GoToBaseGroup_H
#define behaviours_GoToBaseGroup_H

#include "AbstractBehaviour.h"
#include "WorldInfo.h"
#include <list>
#include <behaviours/GoToBase.h>
#include <behaviours/FindLine.h>
#include <behaviours/PositionInLine.h>
#include <behaviours/Recharge.h>

//#define LINE_THRESHOLD 420
#define LINE_THRESHOLD 350
#define GO_TO_BASE_ORIENTATION_TOLE (PI/4)
#define BASE_FIX_ORIENTATION_TOLE 0.3

namespace behaviours {

/**
 * No description
 */
class GoToBaseGroup : public AbstractBehaviour {

	public:
		GoToBaseGroup(WorldInfo * wi, robotapi::IRobot * robot, robotapi::IBattery * robotBatt, robotapi::IBattery * pcBatt, robotapi::IDifferentialWheels * wheels, std::vector<robotapi::IDistanceSensor*> & fss);

		~GoToBaseGroup();

    	void sense();

	    void action();

	private:
		bool onMark();

		bool inLine();

		bool inPosition();

		void correctOrientation();

	    behaviours::AbstractBehaviour * myBehaviours [4];
		robotapi::IDifferentialWheels * wheels;
	    robotapi::IBattery * robotBattery;
		robotapi::IBattery * pcBattery;
		std::vector<robotapi::IDistanceSensor*> * fss;
		WorldInfo * wi;
};

}

#endif // GOTOBASEGROUP_H
