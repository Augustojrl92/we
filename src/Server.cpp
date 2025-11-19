/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aurodrig <aurodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/10 20:07:44 by aurodrig          #+#    #+#             */
/*   Updated: 2025/11/07 12:19:27 by aurodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Webserver.hpp>
#include <cstring>    // memset
#include <cstdlib>    // exit
#include <cstdio>     // perror
#include <arpa/inet.h>
#include <Server.hpp>
#include <fcntl.h>  // para fcntl()

Server::Server(int port) : port(port) {
    DBG_PRINT("Creating Server object on port " << port);
    // 1. Crear socket
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0){
        ERR_PRINT("Failed to create socket (socket() failed)");
        return;
    }

    // 2. Reusar dirección
    int opt = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        ERR_PRINT("Failed to set SO_REUSEADDR option");
        close(listen_fd);
        listen_fd = -1;
        return;
    }

    // 3. Configurar dirección
    std::memset(&address, 0, sizeof(address));
    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port        = htons(port);

    // 4. Bind
    if (bind(listen_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        ERR_PRINT("bind() failed on port " << port);
        close(listen_fd);
        listen_fd = -1;
        return;
    }

    // 5. Listen
    if (listen(listen_fd, 10) < 0) {
        ERR_PRINT("listen() failed on port " << port);
        close(listen_fd);
        listen_fd = -1;
        return;
    }
        // 🔹 Poner el nuevo socket en modo no bloqueante (REVISAR CON FRANK Y FER SI ESTO ES NESESARIO Y PORQUE )
    if (fcntl(listen_fd, F_SETFL, O_NONBLOCK) < 0)
    {
        ERR_PRINT("Failed to set non-blocking mode on listen_fd");
        close(listen_fd);
        listen_fd = -1;
        return;
    }

    //INF_PRINT("Server listening on port " << port);
}

    Server::~Server() {
        if (listen_fd != -1)
            close(listen_fd);
            INF_PRINT("Server socket closed successfully");
    }

    int Server::getFd() const { return listen_fd; }
    int Server::getPort() const { return port; }

    int Server::acceptClient() const
    {
        int client_fd = accept(listen_fd, NULL, NULL);
        if (client_fd < 0){
            ERR_PRINT("accept() failed while trying to accept a new client");
            return -1;
        }

          // 🔹 Poner el nuevo socket en modo no bloqueante (REVISAR CON FRANK Y FER SI ESTO ES NESESARIO Y PORQUE )
        // if (fcntl(client_fd, F_SETFL, O_NONBLOCK) < 0)
        // {
        //     ERR_PRINT("Failed to set non-blocking mode on client socket");
        //     close(client_fd);
        //     return -1;
        // }
        DBG_PRINT("New client accepted: fd " << client_fd);
        return client_fd;
    }
