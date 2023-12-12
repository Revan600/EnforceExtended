void SocketExample()
{
    //socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)
    auto sock = NET_Socket(2, 1, 6);

    while (NET_Connect(sock, "127.0.0.1", 8899) != 0)
    {
        ;
    }

    // some data
    int data[64];

    for (auto i = 0; i < 64; i++)
    {
        data[i] = i;
    }

    NET_Send(sock, data, 64, 0);

    NET_CloseSocket(sock);
}