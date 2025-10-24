// /* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserver.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aurodrig <aurodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/28 15:46:08 by fmorenil          #+#    #+#             */
/*   Updated: 2025/09/23 20:35:54 by aurodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserver.hpp"
#include "../includes/utils.hpp"
#include "../includes/Request.hpp"
#include "../includes/Response.hpp"
#include "../includes/CGIHandler.hpp"

#include <cstring> // memset
#include <cstdlib> // exit, EXIT_FAILURE
#include <cstdio>  // perror
#include <sstream>
#include <vector>
#include <fstream>
#include "../includes/config.hpp"
#include "../includes/UploadHandler.hpp"


bool WebServer::loadConfig(const std::string &path)
{
    return config.load(path);
}

void WebServer::setConfig(const Config &newConfig)
{
    config = newConfig;
}

const Config &WebServer::getConfig() const
{
    return config;
}

WebServer::WebServer() : server(NULL) {}

WebServer::~WebServer() {
    for (std::map<int, Client*>::iterator it = clients.begin(); 
         it != clients.end(); ++it) {
        delete it->second;
    }
    clients.clear();
    
    if (server) {
        delete server;
    }
}

void WebServer::init(int port) {
    // 🔹 1. Crear el objeto Server (socket + bind + listen)
    server = new Server(port);

    // 🔹 2. Añadir el FD del servidor al vector de poll
    pollfd server_poll;
    server_poll.fd = server->getFd();
    server_poll.events = POLLIN;
    fds.push_back(server_poll);

    INF_PRINT("WebServer initialized on port " << port);
}


void WebServer::run() {
    INF_PRINT("Server running on port " << server->getPort());

    while (true) {
        int activity = poll(fds.data(), fds.size(), -1);
        if (activity < 0) {
            ERR_PRINT("poll failed");
            continue;
        } else if (activity == 0) {
            // Timeout (won't happen with -1, but good practice)
            continue;
        }

        // Process events in reverse order to avoid problems when removing elements
        for (int i = fds.size() - 1; i >= 0; i--) {
            // Check for errors or hangup first
            if (fds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) {
                if (fds[i].fd != server->getFd()) {
                    // Client connection has an error
                    DBG_PRINT("Client connection error/hangup: fd=" << fds[i].fd);
                    cleanupClient(fds[i].fd);
                }
                continue;
            }
            
            if (fds[i].revents & POLLIN) {
                if (fds[i].fd == server->getFd()) {
                    // New connection
                    handleNewConnection();
                } else {
                    // Data from existing client
                    handleClientData(fds[i].fd);
                }
            } else if (fds[i].revents & POLLOUT) {
                // Client ready to send response
                handleClientSend(fds[i].fd);
            }
        }

        // Procesar clientes que están listos para generar respuesta
        for (std::map<int, Client*>::iterator it = clients.begin(); 
             it != clients.end(); ++it) {
            if (it->second->isReady()) {
                processClientRequest(it->first);
            }
        }
    }
}

void WebServer::handleNewConnection()
{
    int client_fd = server->acceptClient();
    if (client_fd < 0) {
        ERR_PRINT("Failed to accept new client");
        return;
    }

    // Crear nuevo cliente
    Client *new_client = new Client(client_fd);
    clients[client_fd] = new_client;

    // Añadir a polling
    pollfd client_poll = {client_fd, POLLIN, 0};
    fds.push_back(client_poll);

    DBG_PRINT("New client connected: fd " << client_fd);
}

void WebServer::handleClientData(int client_fd)
{
    Client *client = clients[client_fd];
    if (!client)
        return;

    if (!client->receiveData())
    {
        // Error o desconexión
        DBG_PRINT("Client disconnected or error: fd=" << client_fd);
        cleanupClient(client_fd);
        return;
    }

    if (client->isRequestComplete())
    {
        updatePollEvents(client_fd, POLLOUT);
    }
}

void WebServer::handleClientSend(int client_fd)
{
    Client *client = clients[client_fd];
    if (!client)
        return;

    if (!client->sendData())
    {
        // Error de envío
        ERR_PRINT("Send error for client: fd=" << client_fd);
        cleanupClient(client_fd);
        return;
    }

    if (client->isResponseComplete())
    {
        DBG_PRINT("Response sent completely: fd=" << client_fd);
        cleanupClient(client_fd);
    }
}

void WebServer::cleanupClient(int client_fd)
{
    std::map<int, Client *>::iterator it = clients.find(client_fd);
    if (it != clients.end())
    {
        delete it->second; // Esto cerrará el fd automáticamente
        clients.erase(it);
    }

    for (size_t i = 0; i < fds.size(); i++)
    {
        if (fds[i].fd == client_fd)
        {
            fds.erase(fds.begin() + i);
            break;
        }
    }

    DBG_PRINT("Client cleaned up: fd=" << client_fd);
}

void WebServer::updatePollEvents(int client_fd, short events)
{
    for (size_t i = 0; i < fds.size(); i++)
    {
        if (fds[i].fd == client_fd)
        {
            fds[i].events = events;
            break;
        }
    }
}

void WebServer::processClientRequest(int client_fd) {
    Client* client = clients[client_fd];
    if (!client) return;
    
    std::string request_str = client->getRequest();
    DBG_PRINT("Processing request for fd=" << client_fd);
    
    // Parse request using Request class
    Request req;
    req.parse(request_str);
    
    // Handle chunked encoding if present
    if (req.headers.count("transfer-encoding") &&
        req.headers["transfer-encoding"] == "chunked") {
        DBG_PRINT("Decoding chunked body...");
        req.body = decodeChunkedBody(req.body);
    }
    
    // Validate request
    if (req.method.empty() || req.path.empty()) {
        Response res;
        res.status_code = 400;
        res.body = "<h1>400 Bad Request</h1>";
        res.headers["Content-Type"] = "text/html";
        std::ostringstream oss;
        oss << res.body.size();
        res.headers["Content-Length"] = oss.str();
        client->setResponse(res.toString());
        updatePollEvents(client_fd, POLLOUT);
        return;
    }
    
    DBG_PRINT("Request: " << req.method << " " << req.path);

    // Construct file path (separar query string si existe)
    std::string clean_path = req.path;
    size_t query_pos = clean_path.find('?');
    if (query_pos != std::string::npos) {
        clean_path = clean_path.substr(0, query_pos);
    }
    
    std::string file_path;
    if (clean_path == "/")
        file_path = config.getDocumentRoot() + "/" + config.getIndexPath();
    else
        file_path = config.getDocumentRoot() + clean_path;
    
    Response res;
    
    // Handle different HTTP methods
    if (req.method == "GET") {
        // Verificar si es un script CGI
        if (CGIHandler::isCGIScript(file_path)) {
            CGIHandler cgi(req, file_path);
            std::string cgi_output = cgi.execute();
            
            if (!cgi_output.empty()) {
                // Parsear output CGI para separar headers del body
                size_t header_end = cgi_output.find("\r\n\r\n");
                if (header_end == std::string::npos) {
                    header_end = cgi_output.find("\n\n");
                    if (header_end != std::string::npos) {
                        header_end += 2;
                    }
                } else {
                    header_end += 4;
                }
                
                if (header_end != std::string::npos) {
                    std::string cgi_headers = cgi_output.substr(0, header_end);
                    res.body = cgi_output.substr(header_end);                    // Parsear headers CGI
                    std::istringstream header_stream(cgi_headers);
                    std::string line;
                    while (std::getline(header_stream, line)) {
                        size_t colon_pos = line.find(':');
                        if (colon_pos != std::string::npos) {
                            std::string key = line.substr(0, colon_pos);
                            std::string value = line.substr(colon_pos + 1);
                            // Remover espacios
                            while (!value.empty() && value[0] == ' ') value.erase(0, 1);
                            while (!value.empty() && (value[value.length()-1] == '\r' || value[value.length()-1] == '\n')) value.erase(value.length()-1);
                            res.headers[key] = value;
                        }
                    }
                } else {
                    res.body = cgi_output;
                }
                res.status_code = 200;
            } else {
                res.status_code = 500;
                res.body = "<h1>500 Internal Server Error - CGI Failed</h1>";
            }
        } else {
            // Archivo estático normal
            std::string file_content = readFileNonBlocking(file_path);
            if (!file_content.empty()) {
                res.body = file_content;
                res.status_code = 200;
            } else {
                res.status_code = 404;
                std::string err_content = readFileNonBlocking("www/404-maracas.html");
                if (!err_content.empty()) {
                    res.body = err_content;
                } else {
                    res.body = "<h1>404 Not Found</h1>";
                }
            }
        }
    }
    else if (req.method == "POST") {
    // Verificar si es un script CGI
    if (CGIHandler::isCGIScript(file_path)) {
        CGIHandler cgi(req, file_path);
        std::string cgi_output = cgi.execute();

        if (!cgi_output.empty()) {
            size_t header_end = cgi_output.find("\r\n\r\n");
            if (header_end == std::string::npos) {
                header_end = cgi_output.find("\n\n");
                if (header_end != std::string::npos)
                    header_end += 2;
            } else {
                header_end += 4;
            }

            if (header_end != std::string::npos) {
                std::string cgi_headers = cgi_output.substr(0, header_end);
                res.body = cgi_output.substr(header_end);

                std::istringstream header_stream(cgi_headers);
                std::string line;
                while (std::getline(header_stream, line)) {
                    size_t colon_pos = line.find(':');
                    if (colon_pos != std::string::npos) {
                        std::string key = line.substr(0, colon_pos);
                        std::string value = line.substr(colon_pos + 1);
                        while (!value.empty() && value[0] == ' ') value.erase(0, 1);
                        while (!value.empty() && 
                              (value[value.length()-1] == '\r' || value[value.length()-1] == '\n'))
                            value.erase(value.length()-1);
                        res.headers[key] = value;
                    }
                }
            } else {
                res.body = cgi_output;
            }
            res.status_code = 200;
        } else {
            res.status_code = 500;
            res.body = "<h1>500 Internal Server Error - CGI Failed</h1>";
        }
    }
    // 📦 Soporte para uploads (multipart/form-data)
else if (UploadHandler::isUploadRequest(req)) {
    UploadHandler uploader(req, config);
    res = uploader.handle();
}
    // 🧾 POST normal (sin multipart)
    else if (writeFileNonBlocking(file_path, req.body)) {
        res.body = "<h1>File created with POST</h1>";
        res.status_code = 201;
    } else {
        res.status_code = 500;
        res.body = "<h1>500 Internal Server Error</h1>";
    }
}
    else if (req.method == "DELETE") {
        // Verificar que el archivo existe antes de eliminarlo
        if (fileExistsNonBlocking(file_path) && remove(file_path.c_str()) == 0) {
            res.status_code = 200;
            res.body = "<h1>File deleted with DELETE</h1>";
        } else {
            res.status_code = 404;
            res.body = "<h1>404 Not Found</h1>";
        }
    }
    else {
        res.status_code = 405;
        res.body = "<h1>405 Method Not Allowed</h1>";
    }
    
//     // Set response headers
//     std::ostringstream oss;
//     oss << res.body.size();
//     res.headers["Content-Length"] = oss.str();
    
//     // Solo establecer Content-Type si no fue establecido por CGI
//     if (res.headers.find("Content-Type") == res.headers.end()) {
//         res.headers["Content-Type"] = getContentType(file_path);

//     }
    
//     // Send response
//     client->setResponse(res.toString());
//     updatePollEvents(client_fd, POLLOUT);
// }

// Set response headers
std::ostringstream oss;
oss << res.body.size();
res.headers["Content-Length"] = oss.str();

// ✅ Forzar Content-Type correcto en respuestas HTML
if (res.headers.find("Content-Type") == res.headers.end()) {
    // Si el cuerpo contiene HTML, forzamos text/html
    if (res.body.find("<html") != std::string::npos || 
        res.body.find("<h1>") != std::string::npos ||
        res.body.find("<!DOCTYPE html>") != std::string::npos ||
        res.body.find("<p>") != std::string::npos) {
        res.headers["Content-Type"] = "text/html; charset=UTF-8";
    }
    else {
        // Intentar detectar tipo desde extensión
        std::string guessedType = getContentType(file_path);
        // Evitar tipo binario genérico
        if (guessedType == "application/octet-stream")
            res.headers["Content-Type"] = "text/html; charset=UTF-8";
        else
            res.headers["Content-Type"] = guessedType;
    }
}

// Send response
client->setResponse(res.toString());
updatePollEvents(client_fd, POLLOUT);
}

