/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fileParsing.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fvizcaya <fvizcaya@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/15 14:58:08 by fvizcaya          #+#    #+#             */
/*   Updated: 2025/10/16 20:01:51 by fvizcaya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/fileParsing.hpp"

configFileParser::configFileParser(std::string& filePath)
{
	currentState = FILE_PARSER_STATE_INIT;
	this->filePath = filePath;
}

configFileParser::~configFileParser()
{
    if (configFile.is_open())
        configFile.close();
}

void configFileParser::setCurrentState(configFileParserState_t _currentState)
{
	currentState = _currentState;
}

configFileParserState_t	configFileParser::getCurrentState(void)
{
	return (currentState);
}


// void configFileParser::parseFile(const std::string& filePath)
bool	configFileParser::parseFile()
{
	std::string 				currentLine, previousLine;

	configFile.open(filePath.c_str());
	if (!configFile.is_open()) {
		setCurrentState(FILE_PARSER_STATE_ERROR);
		std::string errMsg = "Could not open file ";
		errMsg.append(filePath);
		handleError(errMsg);
	}
	numServerBlocks = countServerBlocks();
	serverConfig.setNumServerBlocks(numServerBlocks);
	configFile.clear();
	configFile.seekg(0, std::ios::beg);
	if (!configFile.is_open()) {
		std::string errMsg;
		errMsg.append(filePath);
		handleError(errMsg);
	}
	while (std::getline(configFile, currentLine)) {
		if (getCurrentState() == FILE_PARSER_STATE_ERROR) {
			std::string errMsg = "Error in configuration file: ";
			errMsg.append(previousLine);
			handleError(errMsg);
			break;
		}
		tokenizeLine(currentLine);
		previousLine = currentLine;
	}
	return (getCurrentState() != FILE_PARSER_STATE_ERROR);
}
/*
	try {	
		while (std::getline(configFile, currentLine)) {
			tokenizeLine(currentLine);
			if (currentState == FILE_PARSER_STATE_ERROR) {
				ERR_PRINT("Error parsing configuration file.");
				configFile.close();
				return (false);
			}
		}
		configFile.close();
		currentState = FILE_PARSER_STATE_SUCCESS;
		serverConfig.printConfig();
		return (true);
	} catch (const std::exception& e) {
		ERR_PRINT("Error parsing file: " << e.what());
		configFile.close();
		currentState = FILE_PARSER_STATE_ERROR;
		return (false);
	}
  */

static bool checkTokenVector(std::vector<std::string>& tokenVector)
{
	std::string aux;
	size_t		index;
	
	if (tokenVector.back() == ";")
		return (true);

	index = tokenVector[tokenVector.size() - 1].find(';');
	if (index != std::string::npos) {
		tokenVector[tokenVector.size() - 1] = tokenVector[tokenVector.size() - 1].substr(0, index);
		tokenVector.push_back(";");
		return (true);
	}
	return (false);
}

void configFileParser::tokenizeLine(const std::string& line)
{
	std::istringstream 			iss(line);
	std::string 				token, auxPath;
	std::vector<std::string> 	tokenVector;

	while (iss >> token) {
		if (token[0] == '#') {
			currentState = FILE_PARSER_STATE_ERROR;
			break;
		}
		tokenVector.push_back(token);
		handleContext(token);
	}
	
	if (tokenVector.size() >= 2 && checkTokenVector(tokenVector)) {
		currentKeyword = getKeywordType(tokenVector[0]);
		switch(currentKeyword) {
			case FILE_KEYWORD_LISTEN:
				if (getCurrentContext() == FILE_CONTEXT_SERVER) {
					serverConfig.setListeningPort(stringToInt(tokenVector[1]));
				}
				break;
			case FILE_KEYWORD_ROOT:
				if (getCurrentContext() == FILE_CONTEXT_LOCATION) {
					serverConfig.setDocumentRoot(tokenVector[1]);
				}
				break;
			case FILE_KEYWORD_INDEX:
				if (getCurrentContext() == FILE_CONTEXT_LOCATION) {
					serverConfig.setIndexPath(tokenVector[1]);
				}
				break;
			case FILE_KEYWORD_ERROR_PAGE:
				if (getCurrentContext() == FILE_CONTEXT_SERVER) {
					for (size_t i = 1; i < tokenVector.size(); i++) {
						serverConfig.setErrorPage(stringToInt(tokenVector[i]), tokenVector[tokenVector.size() - 2]);
					}
				}
				break;
			case FILE_KEYWORD_SERVER_NAME:
				if (getCurrentContext() == FILE_CONTEXT_SERVER) {
					serverConfig.setServerName(getSubVector(tokenVector, 1, tokenVector.size() - 1));
				}
				break;
			case FILE_KEYWORD_ALLOW_METHODS:
				if (getCurrentContext() == FILE_CONTEXT_SERVER || getCurrentContext() == FILE_CONTEXT_LOCATION) {
					serverConfig.setAllowMethods(getSubVector(tokenVector, 1, tokenVector.size() - 1));
				}
				break;
			case FILE_KEYWORD_CLIENT_MAX_BODY_SIZE:
				if (getCurrentContext() == FILE_CONTEXT_SERVER || getCurrentContext() == FILE_CONTEXT_LOCATION) {
					serverConfig.setClientMaxBodySize(tokenVector[1]);
				}
				break;
			case FILE_KEYWORD_AUTO_INDEX:
				if (getCurrentContext() == FILE_CONTEXT_SERVER || getCurrentContext() == FILE_CONTEXT_LOCATION) {
					if (tokenVector[1] != "on" && tokenVector[1] != "off") {
						setCurrentState(FILE_PARSER_STATE_ERROR);
						break;
					}
					serverConfig.setAutoIndex(tokenVector[1] == "on");
				}
				break;
			case FILE_KEYWORD_CGI_PASS:
				if (getCurrentContext() == FILE_CONTEXT_SERVER || getCurrentContext() == FILE_CONTEXT_LOCATION) {
					serverConfig.setCgiPass(tokenVector[1]);
				}
				break;
			case FILE_KEYWORD_UPLOAD_ENABLE:
				if (getCurrentContext() == FILE_CONTEXT_SERVER || getCurrentContext() == FILE_CONTEXT_LOCATION) {
					if (tokenVector[1] != "on" && tokenVector[1] != "off") {
						setCurrentState(FILE_PARSER_STATE_ERROR);
						break;
					}
					serverConfig.setUploadEnable(tokenVector[1] == "on");
				}
				break;
			case FILE_KEYWORD_UPLOAD_STORE:
				if (getCurrentContext() == FILE_CONTEXT_SERVER || getCurrentContext() == FILE_CONTEXT_LOCATION) {
					serverConfig.setUploadStore(tokenVector[1]);
				}
				break;
			case FILE_KEYWORD_HTTP:
				break;
			default:
				setCurrentState(FILE_PARSER_STATE_ERROR);
				break;
		}
	}
}

void configFileParser::handleContext(const std::string& token)
{
	configFileToken_t tokenType = getTokenType(token);
	tokenVector.push_back(tokenType);

	switch(tokenType) {
		case FILE_TOKEN_CLOSE:
			if (!contextStack.empty())
				contextStack.pop();
			else {
				ERR_PRINT("Unexpected '}'");
				currentState = FILE_PARSER_STATE_ERROR;
			}
		case FILE_TOKEN_KEYWORD:
			currentState = FILE_PARSER_STATE_CONTEXT;
			if (token == "http")
				setCurrentContext(FILE_CONTEXT_HTTP);
			else if (token == "server")
				setCurrentContext(FILE_CONTEXT_SERVER);
			else if (token == "location")
				setCurrentContext(FILE_CONTEXT_LOCATION);
			break;
		case FILE_TOKEN_OPEN:
		case FILE_TOKEN_SEMICOLON:
			break;
		default:
			break;
	}
	DBG_PRINT(getCurrentContext());
}

void configFileParser::setCurrentContext(configFileContext_t context)
{
	contextStack.push(context);
}

configFileContext_t configFileParser::getCurrentContext(void)
{
	if (!contextStack.empty())
		return (contextStack.top());
	return (FILE_CONTEXT_MAIN_CONTEXT);
}

const Config& configFileParser::getConfig() const {
    return (serverConfig);
}

configFileToken_t configFileParser::getTokenType(const std::string& token)
{
	if (token == "{")
		return (FILE_TOKEN_OPEN);
	else if (token == "}")
		return (FILE_TOKEN_CLOSE);
	else if (token == ";")
		return (FILE_TOKEN_SEMICOLON);
	else if (token == "#")
		return (FILE_TOKEN_SHARP);
	else {
		currentKeyword = getKeywordType(token);
		
	}
	return (FILE_TOKEN_KEYWORD);
}

configFileKeyword_t configFileParser::getKeywordType(const std::string& token)
{
	if (token == "listen") {
		return (FILE_KEYWORD_LISTEN);
	}
	else if (token == "root") {
		return (FILE_KEYWORD_ROOT);
	}
	else if (token == "index") {
		return (FILE_KEYWORD_INDEX);
	}
	else if (token == "error_page") {
		return (FILE_KEYWORD_ERROR_PAGE);
	}
	else if (token == "server_name") {
		return (FILE_KEYWORD_SERVER_NAME);
	}
	else if (token == "allow_methods") {
		return (FILE_KEYWORD_ALLOW_METHODS);
	}
	else if (token == "http") {
		return (FILE_KEYWORD_HTTP);
	}
	else if (token == "client_max_body_size") {
		return (FILE_KEYWORD_CLIENT_MAX_BODY_SIZE);
	}
	else if (token == "auto_index") {
		return (FILE_KEYWORD_AUTO_INDEX);
	}
	else if (token == "cgi_pass") {
		return (FILE_KEYWORD_CGI_PASS);
	}
	else if (token == "upload_enable") {
		return (FILE_KEYWORD_UPLOAD_ENABLE);
	}
	else if (token == "upload_store") {
		return (FILE_KEYWORD_UPLOAD_STORE);
	}
	return (FILE_KEYWORD_UNKNOWN);
}

int32_t configFileParser::countServerBlocks(void)
{
	std::string 		currentLine; 
    std::string 		token;
    int32_t 			count = 0;

	while (std::getline(configFile, currentLine)) {
		std::istringstream	iss(currentLine);
		while (iss >> token) {
			if (token == "server")
			{
				count++;
			}
		}
	}
    return count;
}

void configFileParser::handleError(std::string& errorMsg)
{
	configFile.close();
	ERR_PRINT(errorMsg);
	exit(0);
}

