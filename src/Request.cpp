/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmorenil <fmorenil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/24 18:47:07 by aurodrig          #+#    #+#             */
/*   Updated: 2025/10/20 16:13:52 by fmorenil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Request.hpp"
#include "../includes/utils.hpp"

Request::Request() {}

void Request::parse(const std::string &raw_request) {
    std::istringstream stream(raw_request);
    std::string line;

    // ---- Primera línea: "GET /index.html HTTP/1.1"
    if (std::getline(stream, line) && !line.empty()) {
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        parseRequestLine(line);
    }

    // ---- Headers
    while (std::getline(stream, line)) {
        if (line == "\r" || line == "" || line == "\n") break;
        if (!line.empty() && line[line.size() - 1] == '\r')
            line.erase(line.size() - 1);
        parseHeaderLine(line);
    }



    // ---- Body (si hay)
    std::string bodyContent;
    while (std::getline(stream, line)) {
        bodyContent += line + "\n";
    }
    body = bodyContent;
}

void Request::parseRequestLine(const std::string &line) {
    std::istringstream iss(line);
    iss >> method >> path >> version;
}

void Request::parseHeaderLine(const std::string &line) {
    size_t pos = line.find(':');
    if (pos == std::string::npos)
        return;

    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 1);

    // eliminar espacios o tabuladores al inicio del valor
    while (!value.empty() && (value[0] == ' ' || value[0] == '\t'))
        value.erase(0, 1);

    // eliminar \r o \n al final del valor
    while (!value.empty() &&
           (value[value.size() - 1] == '\r' || value[value.size() - 1] == '\n'))
        value.erase(value.size() - 1);

    // convertir clave a minúsculas para evitar diferencias
    headers[toLower(key)] = value;
}

std::string Request::getMethod() const {
    return (this->method);
}