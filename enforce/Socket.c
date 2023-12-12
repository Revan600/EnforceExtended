typedef int[] SocketHandle;

static proto native SocketHandle NET_Socket(int af, int type, int protocol);
static proto native int NET_Connect(SocketHandle socket, string ip, int port);
static proto native int NET_Send(SocketHandle socket, int[] buffer, int bufferSize, int flags);
static proto native int NET_Receive(SocketHandle socket, int[] buffer, int bufferSize, int flags);
static proto native int NET_Shutdown(SocketHandle socket, int how);
static proto native int NET_CloseSocket(SocketHandle socket);