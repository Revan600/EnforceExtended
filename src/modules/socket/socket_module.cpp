#ifdef WIN32
#include <event_bus.h>
#include <enf/types.h>
#include <script_registrator_collection.h>

#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")

class socket_registrator : public ext_script_registrator_base {
public:
    socket_registrator() {
        register_function("NET_Socket", net_socket);
        register_function("NET_Connect", net_connect);
        register_function("NET_Send", net_send);
        register_function("NET_Receive", net_receive);
        register_function("NET_Shutdown", net_shutdown);
        register_function("NET_CloseSocket", net_close_socket);
    }

private:
    static uint64_t net_socket(enf_int af, enf_int type, enf_int protocol) {
        return socket(af, type, protocol);
    }

    static enf_int net_connect(uint64_t socket, enf_string ip, enf_int port) {
        sockaddr_in sa{};
        sa.sin_family = AF_INET;
        inet_pton(AF_INET, ip, &(sa.sin_addr));
        sa.sin_port = htons(port);

        return connect(socket, (struct sockaddr*) &sa, sizeof(sa));
    }

    static enf_int net_send(uint64_t socket, enf_int_static_array buffer, enf_int bufferSize, enf_int flags) {
        static thread_local std::vector<char> conversionBuffer; // fucking enforce doesn't have bytes

        conversionBuffer.clear();

        for (int i = 0; i < bufferSize; ++i) {
            conversionBuffer.push_back(static_cast<char>(buffer[i]));
        }

        return send(socket, conversionBuffer.data(), bufferSize, flags);
    }

    static enf_int net_receive(uint64_t socket, enf_int_static_array buffer, enf_int bufferSize, enf_int flags) {
        static thread_local std::vector<char> conversionBuffer; // fucking enforce doesn't have bytes

        conversionBuffer.clear();

        if (conversionBuffer.size() < bufferSize)
            conversionBuffer.resize(bufferSize);

        auto result = recv(socket, conversionBuffer.data(), bufferSize, flags);

        if (result == 0 || result == SOCKET_ERROR) {
            return result;
        }

        for (int i = 0; i < bufferSize; ++i) {
            buffer[i] = static_cast<enf_int>(conversionBuffer[i]);
        }

        return result;
    }

    static enf_int net_shutdown(uint64_t socket, enf_int how) {
        return shutdown(socket, how);
    }

    static enf_int net_close_socket(uint64_t socket) {
        return closesocket(socket);
    }
};

static init_func fn([]() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    script_registrator_collection::add<socket_registrator>();
}, "socket", {"scripting"});
#endif