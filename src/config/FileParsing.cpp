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

#include <FileParsing.hpp>

configFileParser::configFileParser()
{
	currentState = FILE_PARSER_STATE_INIT;
	serverConfig = nullptr;
	numServerBlocks = 0;
	numLocationBlocks = 0;	
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

void configFileParser::setConfigPointer(Config* _serverConfig)
{
	serverConfig = _serverConfig;
}

void configFileParser::setFilePath(const std::string& _filePath)
{
	filePath = _filePath;
}

bool	configFileParser::parseFile(void)
{
	std::string 				currentLine, previousLine;

	configFile.open(filePath.c_str());
	if (!configFile.is_open()) {
		setCurrentState(FILE_PARSER_STATE_ERROR);
		std::string errMsg = "Could not open file ";
		errMsg.append(filePath);
		return(handleError(errMsg));
	
	}
	int serverBlocksCount = 0;
	std::vector<int> locationBlocksCounts;
	configFileError_t configFileSanity = configSanityCheckAndBlocksCount(configFile, serverBlocksCount, locationBlocksCounts);
	if (configFileSanity != FILE_ERROR_OK) {
		setCurrentState(FILE_PARSER_STATE_ERROR);
		std::string errMsg;
		switch (configFileSanity) {
			case FILE_ERROR_NO_SERVER_BLOCKS:
				errMsg = "At least one server block must be present in configuration file " + filePath;
				return (handleError(errMsg));
				break;
			case FILE_ERROR_NESTED_SERVER_BLOCKS:
				errMsg = "Nested seerver blocks are not allowed in configuration file  " + filePath;
				return (handleError(errMsg));
				break;
			case FILE_ERROR_NESTED_LOCATION_BLOCKS:
				errMsg = "Nested location blocks are not allowed in configuration file " + filePath;
				return (handleError(errMsg));
				break;
			case FILE_ERROR_TOO_MANY_SERVER_BLOCKS:
				serverBlocksCount = MAX_SERVER_BLOCKS;
				ERR_PRINT("Warning: Sever blocks exceed maximum allowed. Truncated to " << MAX_SERVER_BLOCKS);
				break;
			case FILE_ERROR_TOO_MANY_LOCATION_BLOCKS:
				ERR_PRINT("Warning: Location blocks exceed maximum allowed. Truncated to " << MAX_LOCATION_BLOCKS_PER_SERVER);
				break;
			default:
				errMsg = "Unknown error in configuration file ";
				break;
		}
		errMsg.append(filePath);
		return(handleError(errMsg));
	}
	numServerBlocks = serverBlocksCount;
	// Resize vector for server blocks.
	serverConfig->getServerBlocks().resize(numServerBlocks);
	// Resize vector for location blocks inside each server block.
	for (int i = 0; i < numServerBlocks; i++) {
		serverConfig->getServerBlockIndex(i).setLocationBlocksSize(locationBlocksCounts[i]);
	}
	configFile.clear();
	configFile.seekg(0, std::ios::beg);
	
	std::string currentLine, previousLine, token;
	int currentServerIndex = 0;
	int currentLocationIndex = 0;
	configFileContext_t previousContext = FILE_CONTEXT_MAIN_CONTEXT;
	 enum blockType {
			NONE,
			SERVER,
			LOCATION
	} currentBlock = NONE;

	while (std::getline(configFile, currentLine)) {
		if (getCurrentState() == FILE_PARSER_STATE_ERROR) {
			std::string errMsg = "Error in configuration file " + previousLine;
			return(handleError(errMsg));
		}
		std::istringstream iss(currentLine);
		while (iss >> token) {
			previousContext = getCurrentContext();
			handleContext(token);
            if (token == "server" && previousContext == FILE_CONTEXT_MAIN_CONTEXT) {
				currentBlock = SERVER;
				currentLocationIndex = 0;
            }
			else if (token == "location" && previousContext == FILE_CONTEXT_SERVER) {
				currentBlock = LOCATION;
				std::string _locationPath;
				if (!(iss >> _locationPath)) {
					std::string errMsg = "Error in configuration file. Location must be followed by path: " + currentLine + " File: " + filePath;
					return handleError(errMsg);
				}
				serverConfig->getServerBlockIndex(currentServerIndex).setLocationBlockPathIndex(currentLocationIndex, _locationPath);
				currentLocationIndex++;
			}
			else if (token == "}") {
				if (currentBlock == LOCATION) {
					currentBlock = SERVER;
				}
				else if (currentBlock == SERVER) {
					currentBlock = NONE;
					currentServerIndex++;
				}
			}
			else {
				// Call tokenizer corresponding to the current context
				if (getCurrentContext() == FILE_CONTEXT_SERVER && currentBlock == SERVER) {
					tokenizeLineServerBlock(currentLine, serverConfig->getServerBlockIndex(currentServerIndex));
				}
				else if (getCurrentContext() == FILE_CONTEXT_LOCATION && currentBlock == LOCATION) {
					tokenizeLineLocationBlock(currentLine, serverConfig->getServerBlockIndex(currentServerIndex).getLocationBlocksIndex(currentLocationIndex - 1));
				}
			}
        }
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

void configFileParser::tokenizeLineServerBlock(const std::string& line, ServerBlock& currentServerBlock)
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
				currentServerBlock.setListeningPort(stringToInt(tokenVector[1]));
				break;
			case FILE_KEYWORD_IFACE:
				currentServerBlock.setIfaceAddress(tokenVector[1]);
				break;
			case FILE_KEYWORD_SERVER_NAME:
				currentServerBlock.setServerName(getSubVector(tokenVector, 1, tokenVector.size() - 1));
				break;
			case FILE_KEYWORD_ROOT:
				currentServerBlock.setDocumentRoot(tokenVector[1]);
				break;
			case FILE_KEYWORD_INDEX:
				currentServerBlock.setIndexPath(tokenVector[1]);
				break;
			case FILE_KEYWORD_ERROR_PAGE:
				for (size_t i = 1; i < tokenVector.size(); i++) {
					currentServerBlock.setErrorPage(stringToInt(tokenVector[i]), tokenVector[tokenVector.size() - 2]);
				}
				break;
			case FILE_KEYWORD_ALLOW_METHODS:
				currentServerBlock.setAllowMethods(getSubVector(tokenVector, 1, tokenVector.size() - 1));
				break;
			case FILE_KEYWORD_CLIENT_MAX_BODY_SIZE:
				currentServerBlock.setClientMaxBodySize(tokenVector[1]);
				break;
			case FILE_KEYWORD_AUTO_INDEX:
				if (tokenVector[1] != "on" && tokenVector[1] != "off") {
					setCurrentState(FILE_PARSER_STATE_ERROR);
					break;
				}
				currentServerBlock.setAutoIndex(tokenVector[1] == "on");
				break;
			case FILE_KEYWORD_CGI_PASS:
				currentServerBlock.setCgiPass(tokenVector[1]);
				break;
			case FILE_KEYWORD_UPLOAD_ENABLE:
				if (tokenVector[1] != "on" && tokenVector[1] != "off") {
					setCurrentState(FILE_PARSER_STATE_ERROR);
					break;
				}
					currentServerBlock.setUploadEnable(tokenVector[1] == "on");
				break;
			case FILE_KEYWORD_UPLOAD_STORE:
				currentServerBlock.setUploadStore(tokenVector[1]);
				break;
			case FILE_KEYWORD_HTTP:
				break;
			default:
				setCurrentState(FILE_PARSER_STATE_ERROR);
				break;
		}
	}
}

void configFileParser::tokenizeLineLocationBlock(const std::string& line, LocationBlock& currentLocationBlock)
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
			case FILE_KEYWORD_ROOT:
				currentLocationBlock.setDocumentRoot(tokenVector[1]);
				break;
			case FILE_KEYWORD_INDEX:
				currentLocationBlock.setIndexPath(tokenVector[1]);
				break;
			case FILE_KEYWORD_ERROR_PAGE:
				for (size_t i = 1; i < tokenVector.size(); i++) {
					currentLocationBlock.setErrorPage(stringToInt(tokenVector[i]), tokenVector[tokenVector.size() - 2]);
				}
				break;
			case FILE_KEYWORD_ALLOW_METHODS:
				currentLocationBlock.setAllowMethods(getSubVector(tokenVector, 1, tokenVector.size() - 1));
				break;
			case FILE_KEYWORD_CLIENT_MAX_BODY_SIZE:
				currentLocationBlock.setClientMaxBodySize(tokenVector[1]);
				break;
			case FILE_KEYWORD_AUTO_INDEX:
				if (tokenVector[1] != "on" && tokenVector[1] != "off") {
					setCurrentState(FILE_PARSER_STATE_ERROR);
					break;
				}
				currentLocationBlock.setAutoIndex(tokenVector[1] == "on");
				break;
			case FILE_KEYWORD_CGI_PASS:
				currentLocationBlock.setCgiPass(tokenVector[1]);
				break;
			case FILE_KEYWORD_UPLOAD_ENABLE:
				if (tokenVector[1] != "on" && tokenVector[1] != "off") {
					setCurrentState(FILE_PARSER_STATE_ERROR);
					break;
				}
					currentLocationBlock.setUploadEnable(tokenVector[1] == "on");
				break;
			case FILE_KEYWORD_UPLOAD_STORE:
				currentLocationBlock.setUploadStore(tokenVector[1]);
				break;
			case FILE_KEYWORD_HTTP:
				break;
			default:
				setCurrentState(FILE_PARSER_STATE_ERROR);
				break;
		}
	}
}


void configFileParser::handleContext(const std::string &token)
{
	configFileToken_t tokenType = getTokenType(token);
	tokenVector.push_back(tokenType);

	switch(tokenType) {
		case FILE_TOKEN_CLOSE:
			if (!contextStack.empty()) {
				contextStack.pop();
				if (!contextStack.empty()) {
					setCurrentContext(contextStack.top());
				}
				else {
					setCurrentContext(FILE_CONTEXT_MAIN_CONTEXT);
					ERR_PRINT("Unexpected '}'");
					currentState = FILE_PARSER_STATE_ERROR;
				}
			}
			break;
		case FILE_TOKEN_KEYWORD:
			currentState = FILE_PARSER_STATE_CONTEXT;
			if (token == "server") {
				setCurrentContext(FILE_CONTEXT_SERVER);
				contextStack.push(FILE_CONTEXT_SERVER);
			}
			else if (token == "location") {
				setCurrentContext(FILE_CONTEXT_LOCATION);
				contextStack.push(FILE_CONTEXT_LOCATION);
			}
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

const Config& configFileParser::getConfig(int serverBlockIndex) const {
    return (serverConfig[serverBlockIndex]);
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
	if (token == "iface") {
		return (FILE_KEYWORD_IFACE);
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

configFileError_t configFileParser::configSanityCheckAndBlocksCount(std::istream &fileStream, int &serverCount, std::vector<int> &locationCounts) {

    enum blockType {
		NONE,
		SERVER,
		LOCATION
	} currentBlock = NONE;

    int currentServerIndex = -1;

    std::string currentLine;
    while (std::getline(fileStream, currentLine)) {
        std::istringstream iss(currentLine);
        std::string token;
        while (iss >> token) {
            if (token == "server") {
                if (currentBlock == SERVER) {
                    return FILE_ERROR_NESTED_SERVER_BLOCKS;
                }
                currentBlock = SERVER;
                serverCount++;
                if (serverCount > MAX_SERVER_BLOCKS) {
                    return FILE_ERROR_TOO_MANY_SERVER_BLOCKS;
                }
                locationCounts.push_back(0);
                currentServerIndex++;
            } else if (token == "location") {
                if (currentBlock == LOCATION) {
                    return FILE_ERROR_NESTED_LOCATION_BLOCKS;
                }
                currentBlock = LOCATION;
                if (currentServerIndex >= 0) {
                    locationCounts[currentServerIndex]++;
                    if (locationCounts[currentServerIndex] > MAX_LOCATION_BLOCKS_PER_SERVER) {
                        return FILE_ERROR_TOO_MANY_LOCATION_BLOCKS;
                    }
                }
            } else if (token == "}") {
                if (currentBlock == LOCATION) {
                    currentBlock = SERVER;
                } else if (currentBlock == SERVER) {
                    currentBlock = NONE;
                }
            }
        }
    }
	if (serverCount == 0) {
		return FILE_ERROR_NO_SERVER_BLOCKS;
	}
    return FILE_ERROR_OK;
}

bool configFileParser::handleError(std::string& errorMsg)
{
	configFile.close();
	ERR_PRINT(errorMsg);
	serverConfig = NULL;
	return false;
}

int32_t	configFileParser::getServerBlocks(void)
{
	return (numServerBlocks);
}

