/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmorenil <fmorenil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 16:51:28 by aurodrig          #+#    #+#             */
/*   Updated: 2025/10/16 22:06:40 by fmorenil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/config.hpp"
#include <sstream>
#include <iostream>
#include <fstream>
#include "../includes/utils.hpp"

Config::Config() : listeningPort(8080), documentRoot("./www"), indexPath("index.html") {}

Config::~Config() {}

void Config::setNumServerBlocks(int numBlocks)
{
    numServerBlocks = numBlocks;
}

int Config::getNumServerBlocks(void) const
{
    return (numServerBlocks);
}

void    Config::setListeningPort(int _listeningPort)
{
    listeningPort = _listeningPort;
}

int     Config::getListeningPort(void) const
{
    return (listeningPort);
}

void    Config::setDocumentRoot(std::string _documentRoot)
{
    documentRoot = _documentRoot;
}

std::string    Config::getDocumentRoot(void) const
{
    return (documentRoot);
}

void    Config::setIndexPath(std::string _indexPath)
{
    indexPath = _indexPath;
}

std::string    Config::getIndexPath(void) const
{
    return (indexPath);
}

void    Config::setErrorPage(int errorCode, std::string path)
{
    errorPageMap[errorCode] = path;
}

std::map<int, std::string>  Config::getErrorPageMap(void) const
{
    return (errorPageMap);
}

void    Config::setServerName(std::vector<std::string> _serverName)
{
    for (size_t i = 0; i < _serverName.size(); i++) {
        serverName.push_back(_serverName[i]);
    }
}

std::vector<std::string>    Config::getServerName(void) const
{
    return (serverName);
}

void Config::setAllowMethods(std::vector<std::string> _allowMethods)
{
    for (size_t i = 0; i < _allowMethods.size(); i++) {
        allowMethods.push_back(_allowMethods[i]);
    }
}

std::vector<std::string>    Config::getAllowMethods(void) const
{
    return (allowMethods);
}

void Config::setClientMaxBodySize(std::string _clientMaxBodySize)
{
    clientMaxBodySize = _clientMaxBodySize;
}

std::string Config::getClientMaxBodySize(void) const
{
    return (clientMaxBodySize);
}

void Config::setAutoIndex(bool _autoIndex)
{
    autoIndex = _autoIndex;
}

bool Config::getAutoIndex(void) const
{
    return (autoIndex);
}

void Config::setCgiPass(std::string _cgiPass)
{
    cgiPass = _cgiPass;
}

std::string Config::getCgiPass(void) const
{
    return (cgiPass);
}    

void Config::setUploadEnable(bool _uploadEnable)
{
    uploadEnable = _uploadEnable;
}

bool Config::getUploadEnable(void) const
{
    return (uploadEnable);
}

void Config::setUploadStore(std::string _uploadStore)
{
    uploadStore = _uploadStore;
}

std::string Config::getUploadStore(void) const
{
    return (uploadStore);
}

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


void Config::printConfig(void) const
{
    std::cout << "====================================="  << std::endl;
    std::cout << listeningPort  << std::endl;
    std::cout << documentRoot  << std::endl;
    std::cout << indexPath  << std::endl;
    std::cout << clientMaxBodySize  << std::endl;
    std::cout << (autoIndex ? "Autoindex: on" : "Autoindex: off") << std::endl;
    std::cout << cgiPass  << std::endl;
    std::cout << (uploadEnable ? "Upload: on" : "Upload: off") << std::endl;
    std::cout << uploadStore  << std::endl;
    for (std::map<int, std::string>::const_iterator it = errorPageMap.begin(); it != errorPageMap.end(); it++) {
        std::cout << "Error " << it->first  << " --> " << it->second << std::endl;
    }
    for (size_t i = 0; i < serverName.size(); i++) {
        std::cout << serverName[i] << " ";
    }
    std::cout << std::endl;
    for (size_t i = 0; i < allowMethods.size(); i++) {
        std::cout << allowMethods[i] << " ";
    }
    std::cout << std::endl;
}
