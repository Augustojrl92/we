/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmorenil <fmorenil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/02 16:00:43 by fmorenil          #+#    #+#             */
/*   Updated: 2025/10/16 17:50:24 by fmorenil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <map>
#include <sstream>
#include <vector>

// Ya lo tienes
std::string getContentType(const std::string& path);

// Nuevas funciones
std::string toLower(const std::string &str);
std::string decodeChunkedBody(const std::string &chunked);

void parseRequest(const std::string& raw,
                  std::string& method,
                  std::string& path,
                  std::map<std::string, std::string>& headers,
                  std::string& body);

int stringToInt(const std::string& str);
std::vector<std::string> getSubVector(std::vector<std::string>& vec, size_t start, size_t end);

// Funciones de I/O no-bloqueante
std::string readFileNonBlocking(const std::string& filepath);
bool writeFileNonBlocking(const std::string& filepath, const std::string& content);
bool fileExistsNonBlocking(const std::string& filepath);

#endif

