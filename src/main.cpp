/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmorenil <fmorenil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/27 14:02:14 by fvizcaya          #+#    #+#             */
/*   Updated: 2025/10/20 15:48:31 by fmorenil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserver.hpp"
#include "../includes/CGIHandler.hpp"

int main(int argc, char **argv) {

    std::string         config_path;
    WebServer           server;
    
    if (argc > 1)
        config_path = argv[1];
    else
        config_path = "config/default.conf";
    
    try {
        configFileParser parser(config_path);
        if (!parser.parseFile()) {
            ERR_PRINT("Error parsing configuration file.");
            return (1);
        }
        server.setConfig(parser.getConfig());
    } catch (const std::exception& e) {
        ERR_PRINT("❌ Exception: " + std::string(e.what()));
        return (1);
    }
    
    
    // Usar puerto del config 
    server.init(server.getConfig().getListeningPort());
    server.run();

    return (0);
}
