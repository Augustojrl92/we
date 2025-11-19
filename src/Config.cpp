/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aurodrig <aurodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 16:51:28 by aurodrig          #+#    #+#             */
/*   Updated: 2025/11/14 08:51:50 by aurodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Config.hpp>
#include <utils.hpp>

#include <sstream>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <LocationBlockConfig.hpp>

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
    	std::cout << getServerBlockIndex(i).getListeningPort()  << std::endl;
    	std::cout <<  getServerBlockIndex(i).getDocumentRoot()  << std::endl;
    	std::cout <<  getServerBlockIndex(i).getIndexPath()  << std::endl;
    	std::cout << getServerBlockIndex(i).getClientMaxBodySize()  << std::endl;
    	std::cout << (getServerBlockIndex(i).getAutoIndex() ? "Autoindex: on" : "Autoindex: off") << std::endl;
    	std::cout << getServerBlockIndex(i).getCgiPass()  << std::endl;
    	std::cout << (getServerBlockIndex(i).getUploadEnable() ? "Upload: on" : "Upload: off") << std::endl;
    	std::cout << getServerBlockIndex(i).getUploadStore()  << std::endl;
		const std::map<int, std::string>& errorPages = getServerBlockIndex(i).getErrorPageMap();
		for (std::map<int, std::string>::const_iterator it = errorPages.begin(); it != errorPages.end(); ++it) {
	    	std::cout << "Error " << it->first  << " --> " << it->second << std::endl;
		}
    	for (size_t j = 0; j < getServerBlockIndex(i).getServerName().size(); j++) {
        	std::cout << getServerBlockIndex(i).getServerName()[j] << " ";
    	}
    	std::cout << std::endl;
    	for (size_t j = 0; j < getServerBlockIndex(i).getAllowMethods().size(); j++) {
        	std::cout << getServerBlockIndex(i).getAllowMethods()[j] << " ";
    	}
    	std::cout << std::endl;
		for (int j = 0; j < getServerBlockIndex(i).getNumLocationBlocks(); j++) {
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
