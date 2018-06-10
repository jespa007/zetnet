#pragma once



//---------------------------------------------------------------------
//  DEFINES
//---------------------------------------------------------------------

#define  MAX_BYTES_TO_SEND	512


#define  TIME_TO_RECONNECT	3000	//  ms

#define  MAX_TRIES_BAD_MESSAGE	3
//#define  TIME_POLLIGN		1000
#define  TIME_WAIT_ACK		3000	//  ms

#define  MAX_SOCKETS					100
#define  MAX_CLIENTS					MAX_SOCKETS-1  // -1 because socket server is included.




#define  MAX_LENGTH_MESSAGE 16384
#define SOCKET_CLIENT_NOT_AVAILABLE -1

typedef  struct  {
	void  *socket;
	int    idxClient;
	bool    header_sent; // for streaming purposes!
}  tClientSocket;



class  CNetTcp
{

	std::thread *thread;
	bool end_loop_mdb;

	bool  TCP_GetConnection();
	void  TCP_GetIpAddressFromSocket(TCPsocket  sock,  char  *buffer);

	void  update();  //  Receive  messages,  gest  &  send...

	static void mainLoop(CNetTcp *);

protected:



	void  TCP_GestClient();

	void  *socket;

	int   TCP_getMsg(TCPsocket  sock,  Uint8  *buf);
	int   TCP_putMsg(TCPsocket  sock,  Uint8  *buf,  Uint32  len);


	//---------------------------------------------------------------------
	IPaddress  ip;
	bool configured;
	SDLNet_SocketSet socketSet;
	char  *message,*host;
	Uint32  ipaddr;
	Uint8  buffer[MAX_LENGTH_MESSAGE];

	int	      src_port,dst_port;
	int        timeout;

	bool  	connected
	,RequestToConnect
	,RequestToDisConnect
	,Want_reconnection;


	Uint32  TimeToReconnect, TimerActivityNet,TimerPolling;
	string  ValueVariableHost;

	unsigned  initialized;


	bool  createSocketSet();
	bool  		sendMessageToServer(Uint8  *data,  unsigned  len);//,  unsigned  NumeroMensaje=0);


	void  internal_disconnect();
	bool  internal_connect();
	virtual void  getMessage();
	void  gestClient();

	virtual bool gestServerBase();
	virtual void  gestServer();

	Uint8	searchClient(const char *name_client);
	void sendAll(Uint8  ID_Message,  Uint8  *message=NULL,  int  len=0);
	bool  setupHost();




	tClientSocket 		*getFreeSlot(); // adds client
	bool 				freeSlot(tClientSocket * sockClient); // removes client...

	virtual bool 		gestMessage(void *in_socket, Uint8 *buffer, unsigned int len)=0;

	int getMsg(void *sock,Uint8  *buf);
	int putMsg(void *sock,Uint8  *buf,  Uint32  len);

	void socketClose(void *sock);
	int socketAdd(void *sock);
	int socketDel(void *sock);
	void * socketAccept(void *sock);
	int socketReady(void *sock);




public:


	//bool          IsServer();

	void  unLoad();

	CNetTcp();

	void  setup(  int _src_port,int _dst_port, const char *name_client="Server");  //  Reads  configuration  of  machine  &  init  sdl_net...
	//void  setupAsClient(  const char *ip, int _src_port, int _dst_port, const char *name_client="Client");

	bool  DisconnectedCable();
	void  WaitToDisconnect();
	bool  ChangeIpReceived();

	bool  isConnected();
	virtual void  connect();
	virtual void  disconnect();
	void  Reconnection();

	void resume();
	void pause();




	tClientSocket  	clientSocket[MAX_CLIENTS];
	int 			freeSocket[MAX_CLIENTS];
	int        		n_freeSockets; // 1-n_freeSlot => TOTAL ACTIVE CLIENTS!!!


	void  PrintStatus();
	virtual ~CNetTcp();
};





int CNET_Demo();




