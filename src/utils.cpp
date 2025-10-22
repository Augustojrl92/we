/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmorenil <fmorenil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 16:05:40 by fmorenil          #+#    #+#             */
/*   Updated: 2025/10/16 20:09:53 by fmorenil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/utils.hpp"
#include "../includes/webserver.hpp"
#include <algorithm>
#include <sstream>
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
// Removed errno.h - not allowed to use errno after read/write operations

// Detecta Content-Type según extensión del archivo
std::string getContentType(const std::string& path) {
    if (path.find(".html") != std::string::npos) return "text/html";
    if (path.find(".css")  != std::string::npos) return "text/css";
    if (path.find(".js")   != std::string::npos) return "application/javascript";
    if (path.find(".png")  != std::string::npos) return "image/png";
    if (path.find(".jpg")  != std::string::npos || path.find(".jpeg") != std::string::npos) return "image/jpeg";
    if (path.find(".gif")  != std::string::npos) return "image/gif";
    if (path.find(".txt")  != std::string::npos) return "text/plain";
    return "application/octet-stream";
}

// Convierte string a minúsculas
std::string toLower(const std::string &str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

int stringToInt(const std::string& str) {
    std::stringstream ss(str);
    int value = 0;
    ss >> value;
    return value;
}

std::vector<std::string> getSubVector(std::vector<std::string>& vec, size_t start, size_t end) {
    if (start >= vec.size() || end > vec.size() || start >= end) {
        return std::vector<std::string>();
    }
    return std::vector<std::string>(vec.begin() + start, vec.begin() + end);
}


std::string decodeChunkedBody(const std::string &chunked) {
    std::string decoded;
    std::string normalized = chunked;
    size_t pos = 0;

    // Normalizar saltos de línea (asegura que todos los saltos sean \r\n)
    for (size_t i = 0; i < normalized.size(); ++i) {
        if (normalized[i] == '\n' && (i == 0 || normalized[i - 1] != '\r')) {
            normalized.insert(i, "\r");
            ++i;
        }
    }

    // Leer cada chunk hasta que el tamaño sea 0 (fin del body)
    while (true) {
        size_t crlf = normalized.find("\r\n", pos);
        if (crlf == std::string::npos)
            break;

        std::string size_str = normalized.substr(pos, crlf - pos);
        int chunk_size = std::strtol(size_str.c_str(), NULL, 16);

        if (chunk_size == 0)
            break;

        pos = crlf + 2;
        if (pos + chunk_size > normalized.size())
            break;

        decoded += normalized.substr(pos, chunk_size);
        pos += chunk_size + 2; // saltar el \r\n final del chunk
    }

    return decoded;
}

// Leer archivo de forma no-bloqueante
std::string readFileNonBlocking(const std::string& filepath) {
    std::string	content;
    char		buffer[4096];
    ssize_t		bytes_read;
	
    int fd = open(filepath.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd == -1) {
        return "";
    }
    
    
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        content.append(buffer, bytes_read);
    }
    
    // For regular files, read() returning -1 means end of file or error
    // Since we don't use poll() for regular files, we should check if we got all data
    // bytes_read == 0 means EOF, bytes_read == -1 means error or would block
    if (bytes_read == -1) {
        close(fd);
        return "";  // Return empty string on any read error
    }
    
    close(fd);
    return (content);
}

// Escribir archivo de forma no-bloqueante
bool writeFileNonBlocking(const std::string& filepath, const std::string& content) {
	const char*	data = content.c_str();
	size_t		total_size = content.size();
	size_t		written = 0;
	
    int fd = open(filepath.c_str(), O_WRONLY | O_CREAT | O_TRUNC | O_NONBLOCK, 0644);
    if (fd == -1) {
        return false;
    }
    
    while (written < total_size) {
        ssize_t bytes_written = write(fd, data + written, total_size - written);
        if (bytes_written == -1) {
            // For regular files, we could use poll() but it's not required
            // A write error to a regular file is likely a real error
            close(fd);
            return false;
        }
        written += bytes_written;
    }
    
    close(fd);
    return (true);
}

// Verificar si archivo existe de forma no-bloqueante
bool fileExistsNonBlocking(const std::string& filepath) {
    int fd = open(filepath.c_str(), O_RDONLY | O_NONBLOCK);
    if (fd == -1) {
        return false;
    }
	
    close(fd);
    return (true);
}



