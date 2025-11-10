/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmorenil <fmorenil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 16:51:28 by aurodrig          #+#    #+#             */
/*   Updated: 2025/10/28 20:21:11 by fmorenil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Config.hpp>
#include <utils.hpp>

#include <sstream>
#include <iostream>
#include <fstream>

Config::Config() { }

Config::~Config() { }

void Config::setNumServerBlocks(int _numServerBlocks)
{
    numServerBlocks = _numServerBlocks;
}

int Config::getNumServerBlocks(void) const
{
    return (numServerBlocks);
}

void Config::setNumListenBlocks(int _numListenBlocks)
{
    numListenBlocks = _numListenBlocks;
}

int Config::getNumListenBlocks(void) const
{
	return (numListenBlocks);
}

void Config::setServerBlocks(std::vector<ServerBlock> _serverBlocks)
{
	for (int i = 0; i < numServerBlocks; i++) {
		serverBlocks.push_back(_serverBlocks[i]);
	}
}

std::vector<ServerBlock>& Config::getServerBlocks(void)
{
	return (serverBlocks);
}
ServerBlock& Config::getServerBlockIndex(int index)
{
	if (index >= numServerBlocks) {
		throw std::out_of_range("Invalid server block index");
	}
	return (serverBlocks[index]);
}

const ServerBlock& Config::getServerBlockIndex(int index) const
{
	if (index >= numServerBlocks) {
		throw std::out_of_range("Invalid server block index");
	}
	return (serverBlocks[index]);
}

void Config::printConfig(void) const
{
	for (int i = 0; i < numServerBlocks; i++) {
    	std::cout << "====================================="  << std::endl;
    	std::cout << serverBlocks[i].getListeningPort()  << std::endl;
    	std::cout <<  serverBlocks[i].getDocumentRoot()  << std::endl;
    	std::cout <<  serverBlocks[i].getIndexPath()  << std::endl;
    	std::cout << serverBlocks[i].getClientMaxBodySize()  << std::endl;
    	std::cout << (serverBlocks[i].getAutoIndex() ? "Autoindex: on" : "Autoindex: off") << std::endl;
    	std::cout << serverBlocks[i].getCgiPass()  << std::endl;
    	std::cout << (serverBlocks[i].getUploadEnable() ? "Upload: on" : "Upload: off") << std::endl;
    	std::cout << serverBlocks[i].getUploadStore()  << std::endl;
    	for (std::map<int, std::string>::const_iterator it = serverBlocks[i].getErrorPageMap().begin(); it != serverBlocks[i].getErrorPageMap.end(); it++) {
        	std::cout << "Error " << it->first  << " --> " << it->second << std::endl;
    	}
    	for (size_t i = 0; i < serverBlocks[i].getServerName().size(); i++) {
        	std::cout << serverBlocks[i].getServerName()[i] << " ";
    	}
    	std::cout << std::endl;
    	for (size_t i = 0; i < serverBlocks[i].getAllowMethods().size(); i++) {
        	std::cout << serverBlocks[i].getAllowMethods()[i] << " ";
    	}
    	std::cout << std::endl;
		for (size_t j = 0; j < serverBlocks[i].getNumLocationBlocks(); j++) {
			std::cout << "  --- Location Block " << j + 1 << " ---" << std::endl;
			std::cout << "  Path: " << getServerBlockIndex(i).getLocationBlocksIndex(j).getLocationPath() << std::endl;
			std::cout << "  Path: " << getServerBlockIndex(i).getLocationBlocksIndex(j).getClientMaxBodySize() << std::endl;
			std::cout << "  Path: " << getServerBlockIndex(i).getLocationBlocksIndex(j).getCgiPass() << std::endl;
			std::cout << "  Root: " << getServerBlockIndex(i).getLocationBlocksIndex(j).getDocumentRoot() << std::endl;
			std::cout << "  Autoindex: " << (getServerBlockIndex(i).getLocationBlocksIndex(j).getAutoIndex() ? "on" : "off") << std::endl;
			std::cout << "  Upload: " << (getServerBlockIndex(i).getLocationBlocksIndex(j).getUploadEnable() ? "on" : "off") << std::endl;
			std::cout << "  Upload Store: " << getServerBlockIndex(i).getLocationBlocksIndex(j).getUploadStore() << std::endl;
		}
	}
}

/*
bool Config::load(const std::string& path) {
    // Leer todo el archivo de una vez de forma no-bloqueante
    std::string file_content = readFileNonBlocking(path);
    if (file_content.empty()) {
        std::cerr << " No se pudo abrir el archivo de configuración: " << path << std::endl;
        return false;
    }

    // Procesar línea por línea desde el contenido en memoria
    std::istringstream file_stream(file_content);
    std::string line;
    while (std::getline(file_stream, line)) {
        std::istringstream iss(line);
        std::string key;
        if (!(iss >> key)) continue;

        if (key == "listen") {
            iss >> listeningPort;
        } else if (key == "root") {
            iss >> documentRoot;
        } else if (key == "index") {
            iss >> indexPath;
        } else if (key == "error_page") {
            int code;
            std::string file_path;
            iss >> code >> file_path;
            errorPageMap[code] = file_path;
        }
    }
    return true;
}
*/

