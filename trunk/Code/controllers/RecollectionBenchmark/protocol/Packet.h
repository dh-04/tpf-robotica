// Class automatically generated by Dev-C++ New Class wizard

#ifndef PACKET_H
#define PACKET_H

#define PACKET_SIZE_BYTES 1
#define MAX_PACKET_SIZE 257
#define MIN_PACKET_SIZE 5

#define DESTINY_FIELD 1
#define ORIGIN_FIELD 2
#define COMMAND_FIELD 3
#define DATA_FIELD 4

namespace protocol {

/**
 * No description
 */
class Packet
{
	public:
		// class constructors
		Packet();
        Packet(char * data, unsigned char length);
		// class destructor
		~Packet();
		void setDestinationId(char id);
		void setDestinationGroup(char group);
		void setOriginId(char id);
		void setOriginGroup(char group);
		char getDestinationId();
		char getDestinationGroup();
		char getOriginId();
		char getOriginGroup();
		bool checkCRC();
		void calculateCRC();
		void setBroadcast();
		void setGroupBroadcast();
		bool isResponse();
		void setResponse();
		void setCommand(char command);
		char getCommand();
		bool isACK();
		char * getData();
		char getDataLength();
		char getCharData();
		short getShortData();
		int getIntData();
		void addData(char * data,char length);
		void addData(char data);
		void addData(short data);
		void addData(int data);
		char * getPacket();
		char getActualLength();
		void refresh();
		void clear();
		void print();
	protected:
		void resetDataIdx();
		void resetDataIdx(int offset);
	private:
		char packet [MAX_PACKET_SIZE];
		char actualLength;
		int dataIdx;
		void setGroup(int field, char group);
		void setId(int field, char id);
		char getGroup(int field);
		char getId(int field);
};

}

#endif // PACKET_H
