/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmorenil <fmorenil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/20 15:57:19 by fmorenil          #+#    #+#             */
/*   Updated: 2025/10/21 19:15:18 by fmorenil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/CGIHandler.hpp"
#include "../includes/Request.hpp"
#include "../includes/webserver.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <signal.h>
#include <cctype>
#include <cstring>

CGIHandler::CGIHandler(const Request& _request, const std::string& _scriptPath) : request(_request), scriptPath(_scriptPath) {
    setupEnvironment();
}

CGIHandler::~CGIHandler() {}

void CGIHandler::setupEnvironment() {
    // Variables de entorno estándar CGI
    _env["REQUEST_METHOD"] = request.getMethod();
    _env["SCRIPT_NAME"] = scriptPath;
    _env["SCRIPT_FILENAME"] = scriptPath;  // CRÍTICO para PHP-CGI
    _env["PATH_INFO"] = request.path;
    _env["QUERY_STRING"] = "";
    
    // Extraer query string si existe
    size_t queryPos = request.path.find('?');
    if (queryPos != std::string::npos) {
        _env["QUERY_STRING"] = request.path.substr(queryPos + 1);
        _env["PATH_INFO"] = request.path.substr(0, queryPos);
    }
    
    _env["SERVER_PROTOCOL"] = "HTTP/1.1";
    _env["GATEWAY_INTERFACE"] = "CGI/1.1";
    _env["SERVER_SOFTWARE"] = "webserv/1.0";
    _env["REQUEST_URI"] = request.path;
    _env["DOCUMENT_ROOT"] = "/home/fmorenil/webserv/www";
    
    // Headers del request
    if (request.headers.find("Content-Type") != request.headers.end()) {
        _env["CONTENT_TYPE"] = request.headers.at("Content-Type");
    }
    
    if (request.headers.find("Content-Length") != request.headers.end()) {
        _env["CONTENT_LENGTH"] = request.headers.at("Content-Length");
    } else if (!request.body.empty()) {
        std::ostringstream oss;
        oss << request.body.length();
        _env["CONTENT_LENGTH"] = oss.str();
    }
    
    // Variables adicionales de headers HTTP
    for (std::map<std::string, std::string>::const_iterator it = request.headers.begin(); 
         it != request.headers.end(); ++it) {
        std::string headerName = "HTTP_" + it->first;
        // Convertir a mayúsculas y reemplazar - por _
        for (size_t i = 0; i < headerName.length(); i++) {
            if (headerName[i] == '-')
                headerName[i] = '_';
            else
                headerName[i] = std::toupper(headerName[i]);
        }
        _env[headerName] = it->second;
    }
    _env["REDIRECT_STATUS"] = "200";

}

void CGIHandler::setupPipes(int pipeFd[2]) {
    int flags;

    // Hacer el pipe de lectura no-bloqueante
    flags = fcntl(pipeFd[0], F_GETFL);
    fcntl(pipeFd[0], F_SETFL, flags | O_NONBLOCK);
}

pid_t CGIHandler::forkAndExec(int pipeFd[2]) {
    pid_t pid = fork();
    
    if (pid == -1) {
        ERR_PRINT("Fork failed: " + std::string(strerror(errno)));
        return -1;
    }
    
    if (pid == 0) {
        // Proceso hijo
        close(pipeFd[0]); // Cerrar extremo de lectura
        
        // Redirigir stdout al pipe
        dup2(pipeFd[1], STDOUT_FILENO);
        close(pipeFd[1]);
        
        // Si es POST, preparar stdin con el body
            // Si es POST, conectar el body al stdin del CGI
    if (request.getMethod() == "POST") {
        int stdinPipe[2];
        if (pipe(stdinPipe) == -1) {
            ERR_PRINT("Failed to create stdin pipe: " + std::string(strerror(errno)));
            exit(1);
        }

        pid_t writer_pid = fork();
        if (writer_pid == -1) {
            ERR_PRINT("Fork for POST writer failed: " + std::string(strerror(errno)));
            exit(1);
        }

        if (writer_pid == 0) {
            // Proceso hijo escritor: envía el body
            close(stdinPipe[0]);
            if (!request.body.empty()) {
                write(stdinPipe[1], request.body.c_str(), request.body.length());
            }
            close(stdinPipe[1]);
            exit(0);
        }

        // Proceso CGI: leerá el body desde stdin
        close(stdinPipe[1]);
        dup2(stdinPipe[0], STDIN_FILENO);
        close(stdinPipe[0]);
    }

        
        // Construir argv y envp
        std::vector<char*> argv = buildArgv();
        std::vector<char*> envp = buildEnvp();
        

        // Ejecutar el script
        execve(argv[0], &argv[0], &envp[0]);
        
        // Si llegamos aquí, execve falló
        // Escribir un mensaje de error y cerrar descriptores
        const char* error_msg = "CGI execution failed\n";
        write(STDERR_FILENO, error_msg, strlen(error_msg));
        
        // Cerrar todos los descriptores y terminar usando abort (o loop infinito como último recurso)
        while(1) {
            // Loop infinito para evitar que el proceso hijo continúe
            // El proceso padre lo matará con el timeout
        }
    }
    
    return (pid);
}

std::vector<char*> CGIHandler::buildEnvp() const {
    static char envBuffers[50][512];  // Máximo 50 variables de entorno de 512 chars cada una
    static int envCount = 0;
    std::vector<char*> envp;
    
    envCount = 0;  // Reset counter
    
    for (std::map<std::string, std::string>::const_iterator it = _env.begin();
         it != _env.end() && envCount < 50; ++it) {
        std::string envVar = it->first + "=" + it->second;
        strncpy(envBuffers[envCount], envVar.c_str(), sizeof(envBuffers[envCount]) - 1);
        envBuffers[envCount][sizeof(envBuffers[envCount]) - 1] = '\0';
        envp.push_back(envBuffers[envCount]);
        envCount++;
    }
    
    envp.push_back(NULL);
    return envp;
}

std::vector<char*> CGIHandler::buildArgv() const {
    // Usamos buffers estáticos grandes para evitar problemas de memoria
    static char interpreterBuffer[256];
    static char scriptBuffer[256];
    std::vector<char*> argv;
    
    std::string interpreter = getInterpreter(scriptPath);
    if (!interpreter.empty()) {
        strncpy(interpreterBuffer, interpreter.c_str(), sizeof(interpreterBuffer) - 1);
        interpreterBuffer[sizeof(interpreterBuffer) - 1] = '\0';
        argv.push_back(interpreterBuffer);
    }
    
    strncpy(scriptBuffer, scriptPath.c_str(), sizeof(scriptBuffer) - 1);
    scriptBuffer[sizeof(scriptBuffer) - 1] = '\0';
    argv.push_back(scriptBuffer);
    
    argv.push_back(NULL);
    return argv;
}

std::string CGIHandler::execute() {
    int pipeFd[2];
    
    if (pipe(pipeFd) == -1) {
        ERR_PRINT("Pipe creation failed: " + std::string(strerror(errno)));
        return "";
    }

    setupPipes(pipeFd);
    pid_t childPid = forkAndExec(pipeFd);
    
    if (childPid == -1) {
        close(pipeFd[0]);
        close(pipeFd[1]);
        return "";
    }
    
    // Proceso padre
    close(pipeFd[1]); // Cerrar extremo de escritura
    
    std::string result;
    char buffer[4096];
    ssize_t bytesRead;
    
    // Usar poll para timeout del CGI
    struct pollfd pfd;
    pfd.fd = pipeFd[0];
    pfd.events = POLLIN;
    
    int timeout = 30000; // 30 segundos timeout
    int pollResult = poll(&pfd, 1, timeout);
    
    if (pollResult > 0 && (pfd.revents & POLLIN)) {
        // Leer output del CGI
        while ((bytesRead = read(pipeFd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytesRead] = '\0';
            result += buffer;
            
            // Verificar si hay más datos disponibles
            pollResult = poll(&pfd, 1, 0);
            if (pollResult <= 0) break;
        }
    } else if (pollResult == 0) {
        // Timeout - matar el proceso hijo
        kill(childPid, SIGKILL);
        ERR_PRINT("CGI script timeout");
    }
    
    close(pipeFd[0]);
    
    // Esperar a que termine el proceso hijo
    int status;
    waitpid(childPid, &status, 0);
    
    return (result);
}

bool CGIHandler::isCGIScript(const std::string& path) {
    // Verificar extensiones CGI comunes
    size_t dotPos = path.find_last_of('.');
    if (dotPos == std::string::npos) {
        return false;
    }
    
    std::string extension = path.substr(dotPos);
    return (extension == ".php" || extension == ".py" || extension == ".pl" || 
            extension == ".sh" || extension == ".cgi");
}

std::string CGIHandler::getInterpreter(const std::string& path) {
    size_t dotPos = path.find_last_of('.');
    if (dotPos == std::string::npos) {
        return "";
    }
    
    std::string extension = path.substr(dotPos);
    
    if (extension == ".php") {
        return "/usr/bin/php-cgi";
    } else if (extension == ".py") {
        return "/usr/bin/python3";
    } else if (extension == ".pl") {
        return "/usr/bin/perl";
    } else if (extension == ".sh" || extension == ".cgi") {
        return "/bin/bash";
    }
    
    return "";
}