/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aurodrig <aurodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/10 20:07:44 by aurodrig          #+#    #+#             */
/*   Updated: 2025/10/11 01:09:14 by aurodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserver.hpp"
#include <cstring>    // memset
#include <cstdlib>    // exit
#include <cstdio>     // perror
#include <arpa/inet.h>
#include "../includes/Server.hpp"

Server::Server(int port) : port(port) {
    // 1. Crear socket
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // 2. Reusar dirección
    int opt = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // 3. Configurar dirección
    std::memset(&address, 0, sizeof(address));
    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port        = htons(port);

    // 4. Bind
    if (bind(listen_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // 5. Listen
    if (listen(listen_fd, 10) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "[Server] Listening on port " << port << std::endl;
}

Server::~Server() {
    if (listen_fd != -1)
        close(listen_fd);
}

int Server::getFd() const { return listen_fd; }
int Server::getPort() const { return port; }

int Server::acceptClient() const {
    int client_fd = accept(listen_fd, NULL, NULL);
    if (client_fd < 0)
        perror("accept");
    return client_fd;
}
