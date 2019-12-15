#pragma once


#define  MAX_LENGTH_MESSAGE 16384
#define  MAX_SOCKETS					100
namespace zetnet{




	class  CServer
	{
	public:

		typedef  struct  {
			SOCKET socket;
			int    idxClient;
			bool    header_sent; // for streaming purposes!

		}  ClientSocket;

		ClientSocket  	clientSocket[MAX_SOCKETS];
		int 			freeSocket[MAX_SOCKETS];
		int        		n_freeSockets; // 1-n_freeSlot => TOTAL ACTIVE CLIENTS!!!


		static int getMsg(SOCKET sock,uint8_t  *buf);
		static int putMsg(SOCKET sock,uint8_t  *buf,  uint32_t  len);
		static void mainLoop(CServer *);

		CServer();

		bool  setup(int  port, const char *name_client="Server");  //  Reads  configuration  of  machine  &  init  sdl_net...
		void setTimeout( int seconds);

		bool  isConnected();
		virtual void  connect();
		virtual void  disconnect();
		void  reconnection();

		void resume();
		void pause();


		void  printStatus();
		void  unload();
		virtual ~CServer();
	protected:



		bool configured;
		char  *message;
		const char *host;
		uint32_t  ipaddr;
		uint8_t  buffer[MAX_LENGTH_MESSAGE];
		int	      src_port,dst_port;

		bool  	connected
		,reconnection_request
		,disconnect_request
		,is_streaming_server;


		std::string  value_variable_host;

		unsigned  initialized;

		void setTimeDelay(unsigned long delay);

		void  internal_disconnect();
		bool  internal_connect();
		virtual void  getMessage();
		void  gestClient();

		virtual bool gestServerBase();
		virtual void  gestServer();

		ClientSocket 		*getFreeSlot(); // adds client
		bool 				freeSlot(ClientSocket * sockClient); // removes client...

		virtual bool 		gestMessage(SOCKET in_socket, uint8_t *buffer, uint32_t len)=0;


		void closeSocket(SOCKET sock);
		SOCKET socketAccept();
		bool socketReady(SOCKET sock);

	private:
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

		static const char * getErrorSockOpt();

		void  update();  //  Receive  messages,  gest  &  send...

	};
};






