// Class automatically generated by Dev-C++ New Class wizard

#include <behaviours/PositionInLine.h> // class's header file

namespace behaviours{

// class constructor
PositionInLine::PositionInLine(WorldInfo * wi, robotapi::IDifferentialWheels * wheels, std::vector<robotapi::IDistanceSensor*> & fss) : AbstractBehaviour("Position In Line")
{
	this->wheels = wheels;
	this->fss = &fss;
	this->wi = wi;
}

// class destructor
PositionInLine::~PositionInLine()
{
	// insert your code here
}


void PositionInLine::sense(){
}

void PositionInLine::action(){
}

}
