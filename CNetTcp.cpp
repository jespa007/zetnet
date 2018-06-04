#include <net/CNetTcp.h>


const char * SERVER_FULL     = "FULL";






int  CNetTcp::getMsg(void *sock, Uint8  *buf){
	if(type_protocol==CNET_UDP_PROTOCOL){
		return UDP_getMsg((UDPsocket)sock,buf);
	}else {
		return TCP_getMsg((TCPsocket)sock, buf);
	}
	return 0;
}

int  CNetTcp::putMsg(void *sock,Uint8  *buf,  Uint32  len){
	if(type_protocol==CNET_UDP_PROTOCOL){
		return UDP_putMsg((UDPsocket)sock,buf,len);
	}else{
		return TCP_putMsg((TCPsocket)sock, buf,len);
	}
	return 0;
}

void CNetTcp::socketClose(void *sock){
	if(type_protocol==CNET_UDP_PROTOCOL) {
		SDLNet_UDP_Close((UDPsocket)sock);
	}else{
		SDLNet_TCP_Close((TCPsocket)sock);
	}
}

int CNetTcp::socketAdd(void *sock){
	if(type_protocol==CNET_UDP_PROTOCOL){
		return SDLNet_UDP_AddSocket(socketSet,(UDPsocket)sock);
	}else{
		return SDLNet_TCP_AddSocket(socketSet, (TCPsocket)sock);
	}
	return 0;
}

int CNetTcp::socketDel(void *sock){
	if(type_protocol==CNET_UDP_PROTOCOL){
		return SDLNet_UDP_DelSocket(socketSet,(UDPsocket)sock);
	}
	else{
		return SDLNet_TCP_DelSocket(socketSet, (TCPsocket)sock);
	}
	return 0;
}

void * CNetTcp::socketAccept(void * sock){
	if(type_protocol==CNET_TCP_PROTOCOL){
		return SDLNet_TCP_Accept((TCPsocket)sock);
	}
	return 0;
}

int CNetTcp::socketReady(void *sock){
	return SDLNet_SocketReady((SDLNet_GenericSocket)sock);
}

//--------------------------------------------------------------------
//  PUBLIC
CNetTcp::CNetTcp()
{
	src_port=0;
	dst_port=0;
	host=NULL;//"127.0.0.1";
	RequestToDisConnect=false;
	timeout=1000;
	I_Am_Server  =  false;
	initialized  =  false;
	Want_reconnection = false;
	IsStreamingServer=false;

	memset(clientSocket,0,sizeof(clientSocket));

	for(int i=0; i < MAX_CLIENTS; i++){
		clientSocket[i].idxClient=i;
		clientSocket[i].header_sent=false;
		freeSocket[i]=i;
	}

	n_freeSockets=MAX_CLIENTS;

	message=NULL;
	//n_clients  =  0;
	socketSet  =  NULL;
	socket  = NULL;

	connected  =  false;
	RequestToConnect  =  false;
	configured = false;
	type_protocol = CNET_TCP_PROTOCOL;
	attributes = 0;

	udp_packet=SDLNet_AllocPacket(MAX_SIZE_UDP_PACKET);

	TimeToReconnect = 0;
	TimerPolling = 0;
	TimerActivityNet=0;
}

bool CNetTcp::setupHost()
{
	// kill thread if is active...
	unLoad();

	if(I_Am_Server && type_protocol == CNET_TCP_PROTOCOL)
	{

		if(SDLNet_ResolveHost(&ip,NULL,dst_port)==-1)
		{
			print_error_cr("SDLNet_ResolveHost:  %s",SDLNet_GetError());
			return false;
		}
	}
	else  //  Is  a  client  machine....
	{
		//  open  the  server  socket
		//  Resolve  the  argument  into  an  IPaddress  type
		if(SDLNet_ResolveHost(&ip,ValueVariableHost.c_str(),dst_port)==-1) {
			print_error_cr("SDLNet_ResolveHost:  %s",SDLNet_GetError());
			return false;
		}
	}

	//  perform  a  byte  endianess  correction  for  the  next  print_info_cr
	ipaddr=SDL_SwapBE32(ip.host);
	host=(char  *)SDLNet_ResolveIP(&ip);
	timeout  =  5000;

	print_info_cr("Setup host %s (%d.%d.%d.%d:%i) by %s as %s",
			host,
			ipaddr>>24,
			(ipaddr>>16)&0xff,
			(ipaddr>>8)&0xff,
			ipaddr&0xff,
			dst_port,
			type_protocol==CNET_UDP_PROTOCOL?"UDP":"TCP",
					I_Am_Server?"Server":"Client");

	udp_packet->channel = -1;
	udp_packet->address = ip;

	CThread::start();

	return true;
}
//---------------------------------------------------------------------------------------------------------------------------
bool  CNetTcp::internal_connect()
{
	if(type_protocol==CNET_UDP_PROTOCOL) return UDP_GetConnection();

	if(TCP_GetConnection()){
		if(this->I_Am_Server){ // add server soket ...
			socketAdd(socket);
		}
		return true;
	}

	return false;
}
//---------------------------------------------------------------------------------------------------------------------------
bool CNetTcp::IsServer()
{
	return I_Am_Server;
}
//---------------------------------------------------------------------------------------------------------------------------
void  CNetTcp::setupAsServer(  int _src_port, int _dst_port, const char *name_server,int protocol)  //  Reads  configuration  of  machine  &  init  sdl_net...
{
	I_Am_Server = true;

	src_port = _src_port;
	dst_port = _dst_port;
	if(protocol == CNET_UDP_PROTOCOL)
		ValueVariableHost = "127.0.0.1";
	else
		ValueVariableHost = "0.0.0.0";
	//NameVariable = name_server;

	type_protocol = protocol;

	// init common variables ...
	if(!createSocketSet()){
		return;
	}

	// setupHost();
	setupHost();

	configured = true;
}

//---------------------------------------------------------------------------------------------------------------------------
void  CNetTcp::setupAsClient( const char *_ip, int _src_port, int _dst_port, const char *name_client,unsigned int _attributes)  //  Reads  configuration  of  machine  &  init  sdl_net...
{
	attributes = _attributes;

	if((attributes & CNET_UDP_PROTOCOL) == CNET_UDP_PROTOCOL) type_protocol = CNET_UDP_PROTOCOL;
	else if((attributes & CNET_TCP_PROTOCOL) == CNET_TCP_PROTOCOL) type_protocol = CNET_TCP_PROTOCOL;
	else {
		print_error_cr("No protocol established!");
		return;
	}

	// Client function...
	I_Am_Server = false;

	src_port = _src_port;
	dst_port = _dst_port;

	ValueVariableHost = _ip;

	// init common variables ...
	if(!createSocketSet()){
		return;
	}

	// setup host information (if server)
	setupHost();

	//print_info_cr("Loading  net  ok (%s)....",(type_protocol == CNET_TCP_PROTOCOL)?"TCP":(type_protocol == CNET_UDP_PROTOCOL)?"UDP":"??");

	configured = true;
}
//---------------------------------------------------------------
void CNetTcp::WaitToDisconnect() {
	while(connected)
	{
		print_info_cr("Wait to disconnect!");
	}
}
//---------------------------------------------------------------
bool  CNetTcp::isConnected() {
	return  connected;
}
//--------------------------------------------------------------------
bool  		CNetTcp::sendMessageToServer(Uint8  *data,  unsigned  len) {
	int len_sended;

	if(socket == NULL) {
		print_error_cr("Cannot send message because, client not connected to server!");
		return false;
	}

	len_sended  =  putMsg(socket,data,len);

	if(len_sended  !=  (int)len) {
		print_error_cr("Error  to  to  send  message  (expected %i  bytes and %i was sent)!",len,len_sended);
		return  false;
	}

	return true;
}

void CNetTcp::resume(){
	connect();
}
void CNetTcp::pause(){
	disconnect();
}
//--------------------------------------------------------------------
//  create  a  socket  socketSet  that  has  the  server  socket  and  all  the  client  sockets
bool CNetTcp::createSocketSet()
{
	if(socketSet!=NULL)  //  free-it  ?
	{
		print_error_cr("Socket socketSet already created!");
		return false;
	}
	// Allocate for server + clients...
	if(type_protocol == CNET_TCP_PROTOCOL)
		socketSet=SDLNet_AllocSocketSet(MAX_SOCKETS);
	else // allocate only for server...
		socketSet=SDLNet_AllocSocketSet(1);

	if(socketSet==NULL)
	{
		print_error_cr("SDLNet_AllocSocketSet:  %s",  SDLNet_GetError());
		return  false;  //most  of  the  time  this  is  a  major  error,  but  do  what  you  want.
	}
	return true;
}
//--------------------------------------------------------------------
tClientSocket * CNetTcp::getFreeSlot(){

	tClientSocket *cs=NULL;

	if(n_freeSockets > 0){

		if(freeSocket[n_freeSockets-1] != SOCKET_CLIENT_NOT_AVAILABLE){
			cs = &clientSocket[freeSocket[n_freeSockets-1]];
			freeSocket[n_freeSockets-1]=SOCKET_CLIENT_NOT_AVAILABLE;
			n_freeSockets--;
		}else{
			print_error_cr("internal error!");
			return NULL;
		}
	}
	else{
		print_error_cr("no space left!!");
	}
	return cs;
}

bool CNetTcp::freeSlot(tClientSocket *clientSock){

	if(n_freeSockets < MAX_CLIENTS){

		if(clientSock->socket != NULL){

			freeSocket[n_freeSockets]=clientSock->idxClient;

			socketDel(clientSock->socket);
			socketClose(clientSock->socket);
			clientSock->socket = NULL;
			clientSock->header_sent = false;


			n_freeSockets++;

			return true;
		}
		else{
			print_error_cr("Cannot free because -SOCKET IS NOT AVAILABLE-");
		}
	}
	else{
		print_error_cr("Cannot free because -MAX_CLIENTS REACHED-");
	}
	return false;
}

//---------------------------------------------------------------------------------------------------------------
bool CNetTcp::gestServerBase()
{
	//char buffer[100];
	int  numready;

	if(socketSet == NULL){
		return false;
	}

	numready=SDLNet_CheckSockets(socketSet,timeout);

	if(numready==-1) {
		print_error_cr("SDLNet_CheckSockets:  %s",SDLNet_GetError());
		return false;
	}
	//if(numready > 0) {
	if(socketReady(socket)) {
		tClientSocket *c = getFreeSlot();

#if __DEBUG__
		printf("Adding new client\n");
#endif

		if(c==NULL){ // no space left ... reject client ...
			print_error_cr("*** Maximum client count reached - rejecting client connection ***");
			// Accept the client connection to clear it from the incoming connections list
			void * tempSock = socketAccept(socket);

			putMsg(tempSock, (Uint8 *)SERVER_FULL, strlen(SERVER_FULL) + 1);

			// Shutdown, disconnect, and close the socket to the client
			socketClose(tempSock);
			return false;
		}
		// accept socket client (you're ready to get / send messages from/to this socket)...
		c->socket =socketAccept(socket);
		socketAdd(c->socket);
	}
	return true;
}

void CNetTcp::gestServer()
{
	if(!gestServerBase())
	{
		return;
	}

	for (int clientNumber = 0; clientNumber < MAX_CLIENTS; clientNumber++)  {
		// If the socket is ready (i.e. it has data we can read)... (SDLNet_SocketReady returns non-zero if there is activity on the socket, and zero if there is no activity)
		if(clientSocket[clientNumber].socket != NULL){
			int clientSocketActivity = socketReady(clientSocket[clientNumber].socket);
			//cout << "Just checked client number " << clientNumber << " and received activity status is: " << clientSocketActivity << endl;
			// If there is any activity on the client socket...
			if (clientSocketActivity != 0)
			{
				if(IsStreamingServer){ // only sends messages to client
					if(!gestMessage(clientSocket[clientNumber].socket,buffer, sizeof(buffer))){
#if __DEBUG__
						printf("gestMessage:Erasing client %i (Stream)\n",clientNumber);
#endif
						freeSlot(&clientSocket[clientNumber]);
					}
				}
				else{

					if(getMsg(clientSocket[clientNumber].socket,  (Uint8  *)buffer))  //  message read ...
					{
						if(!gestMessage(clientSocket[clientNumber].socket,buffer, sizeof(buffer))){
	#if __DEBUG__
							printf("gestMessage:Erasing client %i (gestMessage)\n",clientNumber);
	#endif
							freeSlot(&clientSocket[clientNumber]);
						}
					}else{ // remove that socket because there's no activity...
	#if __DEBUG__
						printf("gestMessage:Erasing client %i (getMessage)\n",clientNumber);
	#endif
						freeSlot(&clientSocket[clientNumber]);
					}
				}

			} // End of if client socket is active check
		}
	}// End of server socket check sockets loop
}
//------------------------------------------------------------------------------------------------
void  CNetTcp::internal_disconnect()
{
	if( connected)
	{
		connected  =  false;

		if(I_Am_Server)
		{
			// remove all clients boot (only for TCP protocol)
			for(int i = 0; i < MAX_CLIENTS; i++){
				if(clientSocket[i].socket!=NULL){
					socketClose(clientSocket[i].socket);
					clientSocket[i].socket=NULL;
					clientSocket[i].header_sent=false;
				}
			}

			for(int i=0; i < MAX_CLIENTS; i++){
				freeSocket[i]=i;
			}

			n_freeSockets=MAX_CLIENTS;

			if(socket != NULL){
				// close and free socket server...
				socketDel(socket);
				socketClose(socket);
			}

			socket = NULL;
			print_info_cr("Disconnect server!");
		}
		else
			print_info_cr("Disconnect client!");
	}
}
//------------------------------------------------------------------------------------------------------------------------
//char  *str  =  NULL;
void CNetTcp::unLoad()
{
	//disconnect();
	//WaitToDisconnect();
	CThread::stop();
	internal_disconnect();
	//SDL_Delay(500);
}

CNetTcp::~CNetTcp() {
	unLoad();
	if(udp_packet){
		SDLNet_FreePacket(udp_packet);
		udp_packet=NULL;
	}
}
//---------------------------------------------------------------------------------------------------------v
void  CNetTcp::getMessage()
{
	if(!connected)  return;

	if(I_Am_Server)
	{
		gestServer();
	}
	else  //  Client...
	{
		if(type_protocol==CNET_UDP_PROTOCOL)
			UDP_GestClient();
		else
			TCP_GestClient();
	}
}
//--------------------------------------------------------------------
void  CNetTcp::connect()
{
	RequestToConnect  =  true;
}
//--------------------------------------------------------------------
void  CNetTcp::disconnect()
{
	RequestToDisConnect  =  true;
}
//--------------------------------------------------------------------
void  CNetTcp::Reconnection() {
	Want_reconnection = true;
}
//--------------------------------------------------------------------
void  CNetTcp::mainLoopThread()  //  Receive  messages,  gest  &  send...
{
	TimerPolling = SDL_GetTicks()+1000;

	if(RequestToConnect)// && !Want_reconnection)
	{
		if(!connected)
		{
			if(TimeToReconnect < SDL_GetTicks())
			{
				if(!internal_connect())
				{
					TimeToReconnect = SDL_GetTicks() + TIME_TO_RECONNECT;
					print_error_cr("  Not  connected!  -->  try  connection  to  3s");
				}
				else{
					RequestToConnect=false;
				}
			}
			SDL_Delay(100);
		}
		//  else  is  connected...
	}
	else
	{
		if(RequestToDisConnect) //Want_reconnection)
		{
			RequestToDisConnect = false;
			internal_disconnect();
		}
	}
	getMessage();  //  For  server  update  connections  &  get  messages  from  clients...
}


//------------------------------------------------------------------------------------------------------------
void  CNetTcp::TCP_GetIpAddressFromSocket(TCPsocket  sock,  char  *buffer)
{
	Uint32  IPAddress;

	if(sock)
	{
		IPaddress  *remoteIP  =  SDLNet_TCP_GetPeerAddress(sock);

		IPAddress=SDL_SwapBE32(remoteIP->host);

		//  output  the  IP  address  nicely
		sprintf(buffer,"%d.%d.%d.%d",
				(IPAddress)>>24,
				((IPAddress)>>16)&0xff,
				((IPAddress)>>8)&0xff,
				(IPAddress)&0xff);
	}
}

//---------------------------------------------------------------------------------------------------------------------------------------

//  receive  a  buffer  from  a  TCP  socket  with  error  checking
//  this  function  handles  the  memory,  so  it  can't  use  any  []  arrays
//  returns  0  on  any  errors,  or  a  valid  char*  on  success
int  CNetTcp::TCP_getMsg(TCPsocket  sock,  Uint8  *buf)
{
//#define MAXLEN 1024
	int result;
	//char msg[MAXLEN];

	result = SDLNet_TCP_Recv(sock,buf,MAX_LENGTH_MESSAGE);
	if(result <= 0) {
		// TCP Connection is broken. (because of error or closure)
		//SDLNet_TCP_Close(sock);
		//exit(1);
		//print_error_cr("Cannot get message: %s!",SDL_GetError());
		return 0;
	}
	else {
		if(result < (MAX_LENGTH_MESSAGE-1)){
			buf[result] = 0;

		}
		else {
			print_error_cr("Max message reached %i<!",result,MAX_LENGTH_MESSAGE);
			return 0;
		}
		//printf("Received: \"%s\"\n",buf);
	}


	return  result;
}

//  send  a  CString  buffer  over  a  TCP  socket  with  error  checking
//  returns  0  on  any  errors,  length  sent  on  success
int  CNetTcp::TCP_putMsg(TCPsocket  sock,  Uint8  *buf,  Uint32  len) {
	Uint32  result=0;



	if(!len) {
		print_error_cr("0 bytes to send or buffer is NULL!");
		return 0;
	}

	//  send  the  buffer,  with  the  NULL  as  well
	result=SDLNet_TCP_Send(sock,buf,len);

	if(result<len) {
		print_error_cr("SDLNet_TCP_Send (%i<%i):  %s",result,len,  SDLNet_GetError());
		return(0);
	}
	return(result);
}
//---------------------------------------------------------------------------------------------------------------------------------------
bool  CNetTcp::TCP_GetConnection() // Try connect to server...
{
	if(connected)   {
		return  true;
	}
	// Disconnect from net....
	internal_disconnect();

	socket=SDLNet_TCP_Open(&ip);
	if(!socket) {
		print_error_cr("%s %d.%d.%d.%d:%i : %s",
			host,
			ipaddr>>24,
			(ipaddr>>16)&0xff,
			(ipaddr>>8)&0xff,
			ipaddr&0xff,
			dst_port,
			SDLNet_GetError());
		return  false;
	}
	//  New  set
	/*socketSet=create_sockset();
	if(!socketSet) {
		print_error_cr("SDLNet_AllocSocketSet:  %s",  SDLNet_GetError());
		//  exit(EXIT_FAILURE);  //most  of  the  time  this  is  a  major  error,  but  do  what  you  want.
		return  false;
	}*/

	connected = true;

	//print_info_cr("Connection  OK!");
	print_info_cr("Established connection the TCP socket at port %i", dst_port);

	return  connected;
}

//---------------------------------------------------------------------------------------------------------------------------------------
void  CNetTcp::TCP_GestClient()
{
	int  numready=0;

	if(socketSet == NULL) return;

	numready=SDLNet_CheckSockets(socketSet,  timeout);

	if(numready==-1)  //  Not  connected!
	{
		print_error_cr("SDLNet_CheckSockets:  %s",SDLNet_GetError());
		connected  =  false;
		return;
	}

	//  check  to  see  if  the  server  sent  us  data
	if(SDLNet_SocketReady(socket)  &&  numready) // recived messaje from server...
	{
		//print_info_cr("Incoming server msg! %i %i",type_protocol,CNetTcp::CNET_TCP_PROTOCOL);
		//  if  numready  is  >  0  (Server  send  data!)...
		//  getMsg  is  in  tcputil.h,  it  gets  a  CString  from  the  socket
		//  with  a  bunch  of  error  handling
		if(!TCP_getMsg((TCPsocket)socket,(Uint8  *)buffer))
		{
			/*SomeBadMessage = true;
			actual_trie_bad_message--;

			if(!actual_trie_bad_message)
			{
				actual_trie_bad_message = MAX_TRIES_BAD_MESSAGE;
				//connected  =  false;
				Want_reconnection = true;
				print_error_cr("Recibido 3 veces el mensafe mal!");
			}*/
		}
		/*else // Recibido mensaje servidor!
		{
			if(WantToClearBadMessageFlag)
			{
				SomeBadMessage = false;
				WantToClearBadMessageFlag = false;
			}

			actual_trie_bad_message = MAX_TRIES_BAD_MESSAGE;
			TimerActivityNet = SDL_GetTicks()+TIME_ACTIVITY_NET;
			awaiting_ack  =  false;

			//  post  it  to  the  screen

			gestMessage(NULL,buffer, sizeof(buffer));
		}*/
	}
	/*else  //  Is  the  time  for  send  info  to  server!
	{
		while(n_messages  >  0)
		{
			n_messages--;
			print_info_cr("Sending  a  message  %i  to  server!",msg[n_messages].ID_Message);
			sendMessageInternal(socket,msg[n_messages].ID_Message,  msg[n_messages].data,  msg[n_messages].len);
		}

		if(TimerActivityNet<SDL_GetTicks()  &&  connected) // No activity net ----> Fail!
		{
			//disconnected_cable  =  true;
		}


	}*/
}

//--------------------------------------------------------------------
void  CNetTcp::PrintStatus()
{
}
