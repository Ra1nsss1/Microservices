#include "MovieApi.h"
#include <sstream>
#include <vector>
static std::string escapeJson(const std::string& text) {
    std::string output;
    for (unsigned char c : text) {
        if (c == '"' || c == '\\') { output += '\\'; output += static_cast<char>(c); }
        else if (c == '\n') output += "\\n";
        else if (c >= 32) output += static_cast<char>(c);
    }
    return output;
}

static int hexValue(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return -1;
}

static std::string decodeUrl(const std::string& text) {
    std::string output;
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '%' && i + 2 < text.size()) {
            int a = hexValue(text[i + 1]), b = hexValue(text[i + 2]);
            if (a >= 0 && b >= 0) { output += static_cast<char>(a * 16 + b); i += 2; continue; }
        }
        output += text[i] == '+' ? ' ' : text[i];
    }
    return output;
}

std::string MovieApi::handle(const std::string& request) const {
    const std::string prefix = "GET /api/movies?title=";
    if (request.rfind(prefix, 0) != 0) return "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json; charset=utf-8\r\nConnection: close\r\n\r\n{\"error\":\"Неправильний запит\"}";
    size_t end = request.find(' ', prefix.size());
    if (end == std::string::npos) return "HTTP/1.1 400 Bad Request\r\nConnection: close\r\n\r\n";
    std::string title = decodeUrl(request.substr(prefix.size(), end - prefix.size()));
    if (title.empty()) return "HTTP/1.1 400 Bad Request\r\nContent-Type: application/json; charset=utf-8\r\nConnection: close\r\n\r\n{\"error\":\"Введіть назву фільму\"}";
    std::vector<Movie> found = catalog.find(title);
    if (found.empty()) return "HTTP/1.1 404 Not Found\r\nContent-Type: application/json; charset=utf-8\r\nConnection: close\r\n\r\n{\"error\":\"Фільмів не знайдено\"}";
    std::ostringstream body;
    body << "{\"movies\":[";
    for (size_t i = 0; i < found.size(); ++i) {
        if (i) body << ',';
        const Movie& m = found[i];
        body << "{\"title\":\"" << escapeJson(m.title) << "\",\"year\":" << m.year
             << ",\"genre\":\"" << escapeJson(m.genre) << "\",\"description\":\""
             << escapeJson(m.description) << "\",\"rating\":" << m.rating << '}';
    }
    body << "]}";
    return "HTTP/1.1 200 OK\r\nContent-Type: application/json; charset=utf-8\r\nConnection: close\r\n\r\n" + body.str();
}
