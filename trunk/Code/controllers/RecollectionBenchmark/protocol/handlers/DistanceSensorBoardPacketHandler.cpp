// Class automatically generated by Dev-C++ New Class wizard

#include <protocol/handlers/DistanceSensorBoardPacketHandler.h> // class's header file
#include <protocol/packets/DistanceSensorPacket.h>
#include <math.h>
#include <stdio.h>

namespace protocol {
namespace handlers {

// class constructor
DistanceSensorBoardPacketHandler::DistanceSensorBoardPacketHandler(PacketServer * ps, char groupid, char boardid)
{
	this->groupid = groupid;
	this->boardid = boardid;
	this->ps = ps;
	this->dsValue[0] = 0;
	this->dsValue[1] = 0;
	this->dsValue[2] = 0;
	this->dsValue[3] = 0;
	this->dsValue[4] = 0;
	this->dsValue[5] = 0;
}

// class destructor
DistanceSensorBoardPacketHandler::~DistanceSensorBoardPacketHandler()
{
	// insert your code here
}

void DistanceSensorBoardPacketHandler::handlePacket(Packet * p){

	packets::DistanceSensorPacket * dsp = new packets::DistanceSensorPacket(groupid,boardid);
	dsp->analysePacket(p);
	
	char cmd=dsp->getCommand();

	cmd=cmd & 0x7F;

	printf("Distance Sensor %d:%d -> ",  this->groupid, this->boardid);

	//ACK commands
	if ( cmd == CMD_ON ){
		printf("On sensor ACK received\n");
	} else
	if ( cmd == CMD_OFF ){
		printf("Off sensor ACK received\n");
	} else
	if ( cmd == CMD_SET_STATUS ){
		printf("Set status ACK received\n");
	} else
	if ( cmd == CMD_GET_STATUS ){
		printf("Get status ACK received\n");
	} else
	if ( cmd == CMD_GET_VALUE ){
		short * value = dsp->getSensorValues();

		// TODO convert from short to double

		// Lock Mutex
		#ifdef LINUX
		this->dsValueMutex->enterMutex();
		#endif
		
		this->dsValue[0] = value[0];
		this->dsValue[1] = value[1];
		this->dsValue[2] = value[2];
		this->dsValue[3] = value[3];
		this->dsValue[4] = value[4];
		this->dsValue[5] = value[5];
		
		// Release Mutex
		#ifdef LINUX
		this->dsValueMutex->leaveMutex();
		#endif

		printf("Get values response: ID0: %4d ID1: %4d ID2: %4d ID3: %4d ID4: %4d ID5: %5d\n", 
			this->dsValue[0],this->dsValue[1],this->dsValue[2],this->dsValue[3],this->dsValue[4],this->dsValue[5]);

	} else
	if ( cmd == CMD_GET_ONE_VALUE ){
		printf("Set direction ACK received\n");
	} else
	if ( cmd == CMD_ALARM_ON ){
		printf("Set direction ACK received\n");
	} else
	if ( cmd == CMD_SWITCH_ALARM ){
		printf("Set direction ACK received\n");
	} else {
		printf("Packet received unknown\n");
		p->print();
	}
	
	return;
}

void DistanceSensorBoardPacketHandler::enable(int dsId){
	packets::DistanceSensorPacket * p = new packets::DistanceSensorPacket(groupid,boardid);
	// TODO convert from double to char
	p->on(dsId);
	p->prepareToSend();
	this->ps->sendPacket(p);
}

void DistanceSensorBoardPacketHandler::disable(int dsId){
	packets::DistanceSensorPacket * p = new packets::DistanceSensorPacket(groupid,boardid);
	// TODO convert from double to char
	p->off(dsId);
	p->prepareToSend();
	this->ps->sendPacket(p);
}

int DistanceSensorBoardPacketHandler::getValue(int dsId){
	// TODO Put timestamps to prevent flooding
   	packets::DistanceSensorPacket * p = new packets::DistanceSensorPacket(groupid,boardid);
	p->getValue((char)0x0000003F);
	p->prepareToSend();
	this->ps->sendPacket(p);
	
	// Lock Mutex
	#ifdef LINUX
	this->dsValueMutex->enterMutex();
	#endif

	int value = this->dsValue[dsId];

	// Release Mutex
	#ifdef LINUX
	this->dsValueMutex->leaveMutex();
	#endif
	return value;
}

int DistanceSensorBoardPacketHandler::getValue(int* dsId, int count){
	// TODO Put timestamps to prevent flooding
   	packets::DistanceSensorPacket * p = new packets::DistanceSensorPacket(groupid,boardid);
	p->getValue((char)0x0000003F);
	p->prepareToSend();
	this->ps->sendPacket(p);
	
	// Lock Mutex
	#ifdef LINUX
	this->dsValueMutex->enterMutex();
	#endif

	for (int i = 0; i < count; i++) {
		 dsId[i] = this->dsValue[i];
	}

	// Release Mutex
	#ifdef LINUX
	this->dsValueMutex->leaveMutex();
	#endif
	
	return count;
}

int DistanceSensorBoardPacketHandler::getOneValue(int dsId){
	// TODO Put timestamps to prevent flooding
   	packets::DistanceSensorPacket * p = new packets::DistanceSensorPacket(groupid,boardid);
	p->getOneValue((char)0x0000003F);
	p->prepareToSend();
	this->ps->sendPacket(p);
	
	// Lock Mutex
	#ifdef LINUX
	this->dsValueMutex->enterMutex();
	#endif

	int value = this->dsValue[dsId];

	// Release Mutex
	#ifdef LINUX
	this->dsValueMutex->leaveMutex();
	#endif
	return value;
}


void DistanceSensorBoardPacketHandler::setMask(int dsId){
	packets::DistanceSensorPacket * p = new packets::DistanceSensorPacket(groupid,boardid);
	p->setMask(dsId);
	p->prepareToSend();
	this->ps->sendPacket(p);
	
}
		
int DistanceSensorBoardPacketHandler::getMask(){
	packets::DistanceSensorPacket * p = new packets::DistanceSensorPacket(groupid,boardid);
	p->getMask();
	p->prepareToSend();
	this->ps->sendPacket(p);
	
	// Lock Mutex
	#ifdef LINUX
	this->dsMaskMutex->enterMutex();
	#endif
	
	int mask=this->dsMask;
	
	// Release Mutex
	#ifdef LINUX
	this->dsValueMutex->leaveMutex();
	#endif
	return mask;

}

}
}

