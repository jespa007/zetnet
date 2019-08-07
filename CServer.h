#pragma once


#define  MAX_LENGTH_MESSAGE 16384
#define  MAX_SOCKETS					100
namespace zetnet{


	typedef  struct  {
		SOCKET socket;
		int    idxClient;
		bool    header_sent; // for streaming purposes!

	}  ClientSocket;

	class  CServer
	{
		 //set of socket descriptors
		  fd_set readfds;

	#ifdef _WIN32
		WSADATA wsaData;
	#endif

	#ifdef __GNUC__
		struct timeval timeout;
	#endif

		std::thread *thread;
		bool end_loop_mdb;
		unsigned long time_delay_ms;

		// only linux ...

		struct addrinfo  serv_addr, cli_addr;
		SOCKET sockfd;
		int portno;

		//bool  TCP_GetConnection();
		//void  TCP_GetIpAddressFromSocket(TCPsocket  sock,  char  *buffer);

		void  update();  //  Receive  messages,  gest  &  send...

		static void mainLoop(CServer *);

	protected:



		//void  TCP_GestClient();

		//void  *socket;


		bool IsStreamingServer;
		//---------------------------------------------------------------------
		//IPaddress  ip;
		bool configured;
		//SDLNet_SocketSet socketSet;
		char  *message;
		const char *host;
		uint32_t  ipaddr;
		uint8_t  buffer[MAX_LENGTH_MESSAGE];

		int	      src_port,dst_port;


		bool  	connected
		,RequestToConnect
		,RequestToDisConnect
		,Want_reconnection;


		//Uint32  TimeToReconnect, TimerActivityNet,TimerPolling;
		std::string  value_variable_host;

		unsigned  initialized;


		//bool  createSocketSet();
		//bool  		sendMessageToServer(Uint8  *data,  unsigned  len);//,  unsigned  NumeroMensaje=0);
		void setTimeDelay(unsigned long delay);

		void  internal_disconnect();
		bool  internal_connect();
		virtual void  getMessage();
		void  gestClient();

		virtual bool gestServerBase();
		virtual void  gestServer();

		//Uint8	searchClient(const char *name_client);
		//void sendAll(Uint8  ID_Message,  Uint8  *message=NULL,  int  len=0);
		//bool  setupHost();




		ClientSocket 		*getFreeSlot(); // adds client
		bool 				freeSlot(ClientSocket * sockClient); // removes client...

		virtual bool 		gestMessage(SOCKET in_socket, uint8_t *buffer, uint32_t len)=0;


		void closeSocket(SOCKET sock);
		//int socketAdd(void *sock);
		//int socketDel(void *sock);
		SOCKET socketAccept();
		bool socketReady(SOCKET sock);




	public:

		static int getMsg(SOCKET sock,uint8_t  *buf);
		static int putMsg(SOCKET sock,uint8_t  *buf,  uint32_t  len);


		//bool          IsServer();

		void  unload();

		CServer();

		bool  setup(int  port, const char *name_client="Server");  //  Reads  configuration  of  machine  &  init  sdl_net...
		void setTimeout( int seconds);
		//void  setupAsClient(  const char *ip, int _src_port, int _dst_port, const char *name_client="Client");

		//bool  DisconnectedCable();
		//void  WaitToDisconnect();
		//bool  ChangeIpReceived();

		bool  isConnected();
		virtual void  connect();
		virtual void  disconnect();
		void  Reconnection();

		void resume();
		void pause();


		ClientSocket  	clientSocket[MAX_SOCKETS];
		int 			freeSocket[MAX_SOCKETS];
		int        		n_freeSockets; // 1-n_freeSlot => TOTAL ACTIVE CLIENTS!!!


		void  PrintStatus();
		virtual ~CServer();
	};



};






