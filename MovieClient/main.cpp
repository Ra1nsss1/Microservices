#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#pragma comment(lib, "Ws2_32.lib")
static std::string encodeUrl(const std::string& text) {
    const char* hex = "0123456789ABCDEF";
    std::string result;
    for (unsigned char c : text) {
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') || c == '-' || c == '_') result += static_cast<char>(c);
        else { result += '%'; result += hex[c >> 4]; result += hex[c & 15]; }
    }
    return result;
}

int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    WSADATA data{};
    if (WSAStartup(MAKEWORD(2, 2), &data) != 0) return 1;
    while (true) {
        std::cout << "\nВведіть назву фільму або 0 для виходу: ";
        std::string title;
        if (!std::getline(std::cin, title) || title == "0") break;
        if (title.find_first_not_of(" \t") == std::string::npos) { std::cout << "Назва не може бути порожньою.\n"; continue; }
        SOCKET socketHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_port = htons(8085);
        address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        if (socketHandle == INVALID_SOCKET || connect(socketHandle, reinterpret_cast<sockaddr*>(&address), sizeof(address)) == SOCKET_ERROR) {
            std::cout << "Сервіс недоступний. Спочатку запустіть MovieService.\n";
            if (socketHandle != INVALID_SOCKET) closesocket(socketHandle);
            continue;
        }
        std::string request = "GET /api/movies?title=" + encodeUrl(title) + " HTTP/1.1\r\nHost: 127.0.0.1\r\nConnection: close\r\n\r\n";
        send(socketHandle, request.data(), static_cast<int>(request.size()), 0);
        std::string response;
        char buffer[4096];
        int count;
        while ((count = recv(socketHandle, buffer, sizeof(buffer), 0)) > 0) response.append(buffer, count);
        closesocket(socketHandle);
        size_t split = response.find("\r\n\r\n");
        if (split == std::string::npos) { std::cout << "Помилка відповіді сервісу.\n"; continue; }
        // показуємо дані, які повернув сервіс
        std::cout << response.substr(split + 4) << '\n';
    }
    WSACleanup();
    return 0;
}
