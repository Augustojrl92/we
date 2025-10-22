/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aurodrig <aurodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/10 20:06:31 by aurodrig          #+#    #+#             */
/*   Updated: 2025/10/10 20:07:18 by aurodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <netinet/in.h>
#include <string>
#include <iostream>
#include <unistd.h>   // close()

class Server {
private:
    int         listen_fd;   // socket de escucha
    sockaddr_in address;     // dirección local
    int         port;        // puerto del servidor

public:
    // Constructor / Destructor
    explicit Server(int port);
    ~Server();

    // Getters
    int  getFd() const;
    int  getPort() const;

    // Operaciones
    int  acceptClient() const; // Aceptar un nuevo cliente
};

#endif
