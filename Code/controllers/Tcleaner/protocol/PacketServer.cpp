// Class automatically generated by Dev-C++ New Class wizard

#include <protocol/PacketServer.h> // class's header file
#include <protocol/handlers/DefaultBoardPacketHandler.h>
#include <stdio.h>
#include <string.h>

#ifdef __linux__
	#include <termios.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <signal.h>
	#include <errno.h>
	#include <sys/types.h>
	#include <sys/stat.h>
#endif


namespace protocol {
	
// class constructor
PacketServer::PacketServer()
{
	defaultHandler = new protocol::handlers::DefaultBoardPacketHandler(this);
	this->debug = false;
	#ifdef __linux__
	this->init();
	#endif
}

// class destructor
PacketServer::~PacketServer()
{
	// insert your code here
}

void PacketServer::sendPacket(Packet * p){
	// Apply mutex
	#ifdef __linux__
	this->toSendMutex.enterMutex();
	#endif

	//this->toSend.push(p);
	this->sendAPacket(p);

	// Release mutex
	#ifdef __linux__
	this->toSendMutex.leaveMutex();
	#endif
}

void PacketServer::sendAPacket(Packet * p){
	unsigned char i;
	p->calculateCRC();
	char * packet = p->getPacket();
	if (debug == true)
		p->print();
	#ifdef __linux__
	//printf("escribi : %d bytes en el pipe\n",write(pipes[PIPE_OUT],packet,p->getActualLength()));
	write(pipes[PIPE_OUT],packet,p->getActualLength());
	#endif
	this->waitingForResponse.push_back(p);
}

void PacketServer::run(void){
    #ifdef __linux__
    int maxfd = 0, select_resp = 0;
    struct timeval timeout;
    fd_set readfd, writefd;
    fd_set readfd_b, writefd_b;
	if ( pipe(this->pipes) == -1 ){
		perror("PIPE ERROR");
		return;
	}

    // Set file descriptors
//    FD_SET(serfd,&readfd);
    FD_SET(this->pipes[PIPE_IN],&readfd);
    maxfd = MAXOF(serfd,this->pipes[PIPE_IN]) + 1;
    readfd_b = readfd;
    writefd_b = writefd;
    
	
	char a = 0xFF;
	
    while ( 1 ){

        if ((select_resp = select(maxfd, &readfd, &writefd, NULL, NULL)) == 0)
        {
            // Select timed out!
            // TODO: retransmit waitingForResponse list...
            printf("TIME OUT!\n");
        }

        if (errno == EINTR)
        {
            // A signal was delivered before time_out 
            errno = 0;
            continue;
        }

        // SERIAL PORT
        if ( FD_ISSET(serfd, &readfd) )
        {
            unsigned char s;
            char ser_buf[256];
            read(serfd, ser_buf, 1);
            this->readNBytes(serfd, &(ser_buf[1]), ser_buf[0]);
            //read(serfd, &(ser_buf[1]), ser_buf[0]);
            Packet * p = new Packet(ser_buf, ser_buf[0]+1);
			BoardPacketHandler * bph = this->getHandler(p->getOriginGroup(),p->getOriginId());
			bph->handlePacket(p);
			if (debug == true)
				defaultHandler->handlePacket(p);
/*            if (((a + 1) & 0x000000FF) != (c & 0x000000FF))
			char c;
            // Have things in buffer! :P
            read(serfd, &c, 1);
			printf(" ERROR! %02X %02X ", a & 0x000000FF, c & 0x000000FF);
			a = c & 0x000000FF;
			fflush(stdout);
			printf(":%02X", c & 0x000000FF);
*/        
        }

        // PIPE
        if ( FD_ISSET(this->pipes[PIPE_IN], &readfd) )
        {
			unsigned char c;
/*			int pipe_buf[256];
            // Have things in buffer! :P
            read(pipes[PIPE_IN], &c, 1);
			write(serfd,&c,1);
            read(pipes[PIPE_IN], pipe_buf, c);
			printf("escribi : %d bytes en el serial\n",write(serfd,pipe_buf,c)+1);
*/
			read(pipes[PIPE_IN], &c, 1);
			
			write(serfd,&c,1);
		
		}

        // Restore structures from backup
        readfd = readfd_b;
        writefd = writefd_b;
	}
	#endif
}

void PacketServer::readNBytes(int fd, char * buff, int nBytes){
    #ifdef __linux__

	int bytesRead = 0;
	int bytesLeft = nBytes - bytesRead;

	while ( bytesLeft ){
		int thisread = read( fd, buff+(char)bytesRead , bytesLeft );
		if ( thisread <= 0 )
			continue;
		bytesRead = bytesRead + thisread;
		bytesLeft = nBytes - bytesRead;
	}
	return;
	#endif

}

void PacketServer::registerHandler(BoardPacketHandler * bph,int groupid,int boardid){
	char gid = groupid;
	char bid = boardid;
   	this->handlers[gid][bid] = bph;
}

BoardPacketHandler * PacketServer::getHandler(unsigned char groupid , unsigned char boardid){
	BoardPacketHandler * handler = this->handlers[groupid][boardid];
	
	if ( handler == NULL )
		handler = defaultHandler;
	return handler;
}

bool PacketServer::init()
{
    #ifdef __linux__
    this->serfd = open(SERIAL_PORT, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
//    this->serfd = open(SERIAL_PORT, O_RDWR | O_NOCTTY | O_NDELAY);
    
    if (this->serfd < 0)
    {
        perror("/dev/ttyUSB0");
        fprintf(stderr, "cannot connect to the serial port %s\n", SERIAL_PORT);
        return false;
    }
    
    char ini[20] = "\nSesion iniciada\n";

    write(serfd,ini,strlen(ini)+1);


/*    struct termios tc; // 115200 baud, 8n1, no flow control
    
    tc.c_iflag = IGNBRK;
    tc.c_oflag = 0;
    tc.c_cflag = CS8 | CREAD | CLOCAL | CSTOPB;
    tc.c_lflag = 0;
 
    cfsetispeed(&tc, B115200);
    cfsetospeed(&tc, B115200);

    tcsetattr(this->serfd, TCSANOW, &tc);
    
    
  */  
    

    
    #endif
    return true;
}

void PacketServer::setDebug(bool mode) {
	this->debug = mode;
}

}
