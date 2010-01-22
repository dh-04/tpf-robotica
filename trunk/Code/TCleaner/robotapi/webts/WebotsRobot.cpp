#include "WebotsRobot.h"

#include <robotapi/webts/WebotsCamera.h>
#include <robotapi/webts/WebotsDistanceSensor.h>
#include <robotapi/webts/WebotsServo.h>
#include <robotapi/webts/WebotsBattery.h>
#include <robotapi/webts/WebotsDifferentialWheels.h>
#include <webots/Camera.hpp>
#include <webots/Robot.hpp>

namespace robotapi {
namespace webts {

	webots::Robot * robot;

    WebotsRobot::WebotsRobot( webots::DifferentialWheels & dw){
		robot = &dw;
	}

    std::string WebotsRobot::getName(){
        return robot->getName();
    }
    
    double WebotsRobot::getTime(){
        return robot->getTime();
    }

    int WebotsRobot::getMode(){
        return robot->getMode();
    }

    bool WebotsRobot::getSynchronization(){
        return robot->getSynchronization();
    }
    
    double WebotsRobot::getBasicTimeStep(){
        return robot->getBasicTimeStep();
    }
    
    ICamera & WebotsRobot::getCamera(std::string name){
//        const_cast<webots::Camera>(
        WebotsCamera * cam = new WebotsCamera(*robot->getCamera(name));
        return * cam;
    }

    IDistanceSensor & WebotsRobot::getDistanceSensor(std::string name){
        WebotsDistanceSensor * ds = new WebotsDistanceSensor(*robot->getDistanceSensor(name));
        return * ds;
    }

    IServo & WebotsRobot::getServo(std::string name){
        WebotsServo * s = new WebotsServo(*robot->getServo(name));
        return * s;
    }

    IDevice & WebotsRobot::getDevice(std::string name){
        IWbDeviceTag * wdt = new WebotsDeviceTag(3);
        WebotsDevice * d = new WebotsDevice(name, *wdt);
        return * d;
    }

    IDifferentialWheels & WebotsRobot::getDifferentialWheels(std::string name){
        WebotsDifferentialWheels * df = new WebotsDifferentialWheels((webots::DifferentialWheels&)*robot);
        return * df;
    }

    IBattery & WebotsRobot::getBattery(std::string name){
		IWbDeviceTag * wdt = new WebotsDeviceTag(3);
        WebotsBattery * b = new WebotsBattery(name,*wdt);
        return * b;
    }

	void WebotsRobot::step(int ms){
		robot->step(ms);
		return ;
	}
    
} /* End of namespace robotapi::webts */
} /* End of namespace robotapi */
