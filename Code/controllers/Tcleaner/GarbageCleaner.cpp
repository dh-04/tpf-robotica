#include "GarbageCleaner.h"
#include <behaviours/AvoidObstacle.h>
#include <behaviours/CollectGarbage.h>
#include <behaviours/FocusGarbage.h>
#include <behaviours/GoToBaseGroup.h>
#include <behaviours/GoToDisposal.h>
#include <behaviours/GoToGarbage.h>
#include <behaviours/Recharge.h>
#include <behaviours/UnloadGarbage.h>
#include <behaviours/Wander.h>
#include <robotapi/IDistanceSensor.h>
#include <robotapi/IServo.h>
#include <robotapi/IBattery.h>
#include <robotapi/IDifferentialWheels.h>
#include <robotapi/ITrashBin.h>
#include <vector>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_PROCESSING_STEPS 10
#define MIN_PROCESSING_STEPS 1

//#define INITIAL_PROCESSING_STEPS ((MAX_PROCESSING_STEPS+MIN_PROCESSING_STEPS)/2)
#define INITIAL_PROCESSING_STEPS 2

std::vector<robotapi::IDistanceSensor*> dss;
std::vector<robotapi::IDistanceSensor*> fss;
robotapi::IServo* servoFront;
robotapi::IServo* servoBottom;
robotapi::IServo* servoRear;
robotapi::IServo* servoInside;
robotapi::ITrashBin* trashBin;
robotapi::IDifferentialWheels* wheels;
robotapi::IBattery* robotBattery;
robotapi::IBattery* pcBattery;
robotapi::ICamera* camera;
robotapi::IRobot* robot;
utils::GarbageRecognition * gr;

int steps = 0;
int currentProcessingSteps = INITIAL_PROCESSING_STEPS;

GarbageCleaner::GarbageCleaner(WorldInfo * wi, robotapi::IRobot & robot) : myIRobot(robot)
// don't delete the following line as it's needed to preserve source code of this autogenerated element
// section -64--88-1-100--1230920c:125cb7ecd6f:-8000:0000000000000DB6 begin
{
	this->initializeSensors();
	this->initializeBehaviours(wi);
}
// section -64--88-1-100--1230920c:125cb7ecd6f:-8000:0000000000000DB6 end
// don't delete the previous line as it's needed to preserve source code of this autogenerated element

void GarbageCleaner::initializeSensors(){
	for( int i=0 ; i < DISTANCE_SENSORS ; i++ ){
		std::stringstream sstr;
		sstr << "ps" << i;
		robotapi::IDistanceSensor * ids = &myIRobot.getDistanceSensor(sstr.str());
		ids->enable(TIME_STEP);
		dss.push_back(ids);
	}

	for( int i=0 ; i < FLOOR_SENSORS ; i++ ){
		std::stringstream sstr;
		sstr << "fs" << i;
		robotapi::IDistanceSensor * fds = &myIRobot.getDistanceSensor(sstr.str());
		fds->enable(TIME_STEP);
		fss.push_back(fds);
	}

	wheels = &myIRobot.getDifferentialWheels("df");
	wheels->enableEncoders(TIME_STEP);

/*
	servoBottom = &myIRobot.getServo("servo0");
	servoBottom->enablePosition(TIME_STEP);
 */
	servoFront = &myIRobot.getServo("servo2");
	servoFront->enablePosition(TIME_STEP);
//	servoFront->setForce(90);
//	servoFront->setMotorForce(90);
//	servoFront->setVelocity(4);
	servoRear = &myIRobot.getServo("servo1");
	servoRear->enablePosition(TIME_STEP);


	trashBin = &myIRobot.getTrashBin("ts0");
	trashBin->enable(TIME_STEP);

	robotBattery = &myIRobot.getBattery("b0");
	robotBattery->enable(TIME_STEP);
	
	pcBattery = &myIRobot.getBattery("b1");
	pcBattery->enable(TIME_STEP);

	camera = &myIRobot.getCamera("camera0");
	camera->enable(TIME_STEP);

	
}

void GarbageCleaner::initializeBehaviours(WorldInfo * wi){
	gr = new utils::GarbageRecognition(wi);
	gr->setCamera(*camera);

    behaviours::AbstractBehaviour * ab;

	ab = new behaviours::Wander(wi, wheels);
	myAbstractBehaviours.push_back(ab);

	ab = new behaviours::FocusGarbage( gr, wheels );
	myAbstractBehaviours.push_back(ab);

	ab = new behaviours::GoToGarbage( gr, wheels );
	myAbstractBehaviours.push_back(ab);

	ab = new behaviours::CollectGarbage( gr, &myIRobot, trashBin, wheels, wi, servoFront );
	myAbstractBehaviours.push_back(ab);

	ab = new behaviours::GoToDisposal( wi, &myIRobot, trashBin , wheels, fss , servoRear );
	myAbstractBehaviours.push_back(ab);

	ab = new behaviours::GoToBaseGroup( wi, &myIRobot, robotBattery , pcBattery , wheels, fss );
	myAbstractBehaviours.push_back(ab);

	ab = new behaviours::AvoidObstacle(wheels,dss);
	myAbstractBehaviours.push_back(ab);

}

void GarbageCleaner::cleanGarbage()
// don't delete the following line as it's needed to preserve source code of this autogenerated element
// section -64--88-1-100--1230920c:125cb7ecd6f:-8000:0000000000000DBA begin
{
    std::list<behaviours::AbstractBehaviour*>::iterator it;

	for ( int i = 0; i < 20 ; i++ )
		this->myIRobot.step(TIME_STEP);

	while ( 1 ){
		std::list<behaviours::AbstractBehaviour*>::iterator it;
		behaviours::AbstractBehaviour::resetStimulusPresent();
		
		if ( steps % currentProcessingSteps == 0 ){
			gr->stepDone();
			steps = 0;
		}
		//std::cout << "mylist contains:";

		for ( it=myAbstractBehaviours.begin() ; it != myAbstractBehaviours.end(); it++ ){
			//std::cout << "\n" << (*it)->toString() << "\n";
			(*it)->sense();
		}
		wheels->computeOdometry();
		for ( it=myAbstractBehaviours.begin() ; it != myAbstractBehaviours.end(); it++ ){
			//std::cout << "\n" << (*it)->toString() << "\n";
			(*it)->act();
		}

		this->myIRobot.step(TIME_STEP);
		steps++;

	}
}
// section -64--88-1-100--1230920c:125cb7ecd6f:-8000:0000000000000DBA end
// don't delete the previous line as it's needed to preserve source code of this autogenerated element

