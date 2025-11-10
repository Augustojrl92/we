/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlockConfig.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fvizcaya <fvizcaya@student.42madrid.com>   #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-11-06 21:49:44 by fvizcaya          #+#    #+#             */
/*   Updated: 2025-11-06 21:49:44 by fvizcaya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ServerBlockConfig.hpp>

ServerBlock::ServerBlock() : listeningPort(8080), documentRoot("./www"), indexPath("index.html"), autoIndex(false), uploadEnable(false) { }

ServerBlock::~ServerBlock() { }

ServerBlock& ServerBlock::operator=(const ServerBlock& otherServerBlock)
{
	if (this != &otherServerBlock) {
		serverName = otherServerBlock.serverName;
		listeningPort = otherServerBlock.listeningPort;
		ifaceAddress = otherServerBlock.ifaceAddress;
		documentRoot = otherServerBlock.documentRoot;
		indexPath = otherServerBlock.indexPath;
		allowMethods = otherServerBlock.allowMethods;
		errorPageMap = otherServerBlock.errorPageMap;
		clientMaxBodySize = otherServerBlock.clientMaxBodySize;
		cgiPass = otherServerBlock.cgiPass;
		autoIndex = otherServerBlock.autoIndex;
		uploadEnable = otherServerBlock.uploadEnable;
		uploadStore = otherServerBlock.uploadStore;
		locationBlocks = otherServerBlock.locationBlocks;
		numLocationBlocks = otherServerBlock.numLocationBlocks;
	}
	return (*this);
}

void    ServerBlock::setServerName(std::vector<std::string> _serverName)
{
    for (size_t i = 0; i < _serverName.size(); i++) {
        serverName.push_back(_serverName[i]);
    }
}

std::vector<std::string>    ServerBlock::getServerName(void) const
{
    return (serverName);
}

void    ServerBlock::setListeningPort(int _listeningPort)
{
    listeningPort = _listeningPort;
}

int     ServerBlock::getListeningPort(void) const
{
    return (listeningPort);
}

void	ServerBlock::setIfaceAddress(std::string _ifaceAddress)
{
	ifaceAddress = _ifaceAddress;
}

std::string	ServerBlock::getIfaceAddress(void) const
{
	return (ifaceAddress);
}

void    ServerBlock::setDocumentRoot(std::string _documentRoot)
{
    documentRoot = _documentRoot;
}

std::string    ServerBlock::getDocumentRoot(void) const
{
    return (documentRoot);
}

void    ServerBlock::setIndexPath(std::string _indexPath)
{
    indexPath = _indexPath;
}

std::string    ServerBlock::getIndexPath(void) const
{
    return (indexPath);
}

void    ServerBlock::setErrorPage(int errorCode, std::string path)
{
    errorPageMap[errorCode] = path;
}

std::map<int, std::string>  ServerBlock::getErrorPageMap(void) const
{
    return (errorPageMap);
}

void ServerBlock::setAllowMethods(std::vector<std::string> _allowMethods)
{
    for (size_t i = 0; i < _allowMethods.size(); i++) {
        allowMethods.push_back(_allowMethods[i]);
    }
}

std::vector<std::string>    ServerBlock::getAllowMethods(void) const
{
    return (allowMethods);
}

void ServerBlock::setClientMaxBodySize(std::string _clientMaxBodySize)
{
    clientMaxBodySize = _clientMaxBodySize;
}

std::string ServerBlock::getClientMaxBodySize(void) const
{
    return (clientMaxBodySize);
}

void ServerBlock::setAutoIndex(bool _autoIndex)
{
    autoIndex = _autoIndex;
}

bool ServerBlock::getAutoIndex(void) const
{
    return (autoIndex);
}

void ServerBlock::setUploadEnable(bool _uploadEnable)
{
    uploadEnable = _uploadEnable;
}

bool ServerBlock::getUploadEnable(void) const
{
    return (uploadEnable);
}

void ServerBlock::setUploadStore(std::string _uploadStore)
{
    uploadStore = _uploadStore;
}

std::string ServerBlock::getUploadStore(void) const
{
    return (uploadStore);
}

void ServerBlock::setCgiPass(std::string _cgiPass)
{
    cgiPass = _cgiPass;
}

std::string ServerBlock::getCgiPass(void) const
{
    return (cgiPass);
}

void ServerBlock::setLocationBlocks(std::vector<LocationBlock> _locationBlocks)
{
	for (int i = 0; i < (int)_locationBlocks.size(); i++) {
		locationBlocks.push_back(_locationBlocks[i]);
	}
}

const std::vector<LocationBlock>&	ServerBlock::getLocationBlocks(void) const
{
	return (locationBlocks);
}

LocationBlock& ServerBlock::getLocationBlocksIndex(int index)
{
	
	if (index >= locationBlocks.size()) {
        throw std::out_of_range("Invalid location index");
    }

	return (locationBlocks[index]);
}

const LocationBlock& ServerBlock::getLocationBlocksIndex(int index) const
{
	
	if (index >= locationBlocks.size()) {
		throw std::out_of_range("Invalid location index");
	}

	return (locationBlocks[index]);
}

void ServerBlock::setNumLocationBlocks(int numBlocks)
{
	numLocationBlocks = numBlocks;
}

int ServerBlock::getNumLocationBlocks(void) const
{
	return (numLocationBlocks);
}

void ServerBlock::setLocationBlocksSize(int size)
{
	locationBlocks.resize(size);
}

void ServerBlock::setLocationBlockPathIndex(int index, std::string path)
{
	locationBlocks[index].setLocationPath(path);
}