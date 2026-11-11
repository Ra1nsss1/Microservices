#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "MovieApi.h"
#include <iostream>
#include <string>
#pragma comment(lib, "Ws2_32.lib")

int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    WSADATA data{};
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0) { std::cerr << "Не вдалося запустити мережу.\n"; return 1; }
    SOCKET server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == INVALID_SOCKET) { WSACleanup(); return 1; }
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    address.sin_port = htons(8085);
    if (bind(server, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == SOCKET_ERROR || listen(server, 5) == SOCKET_ERROR) {
        std::cerr << "Порт 8085 зайнятий або недоступний.\n";
        closesocket(server); WSACleanup(); return 1;
    }
    std::cout << "MovieService працює на http://127.0.0.1:8085\nЗакрийте вікно, щоб зупинити сервер.\n";
    MovieApi api;
    while (true) {
        SOCKET client = accept(server, nullptr, nullptr);
        if (client == INVALID_SOCKET) break;
        char buffer[4096]{};
        int count = recv(client, buffer, sizeof(buffer), 0);
        if (count > 0) {
            std::string response = api.handle(std::string(buffer, count));
            size_t sent = 0;
            while (sent < response.size()) {
                int n = send(client, response.data() + sent, static_cast<int>(response.size() - sent), 0);
                if (n <= 0) break;
                sent += static_cast<size_t>(n);
            }
        }
        closesocket(client);
    }
    closesocket(server);
    WSACleanup();
    return 0;
}
