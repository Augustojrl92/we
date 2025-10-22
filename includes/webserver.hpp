 #ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "../include/fileParsing.hpp"
#include "Client.hpp"
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <poll.h>
#include "config.hpp"
#include "Server.hpp"


// 🔹 Macros de debug y error
#if 1
#define ERR_PRINT(msg) std::cerr << "[ERROR] " << __FILE__ << ":" << __LINE__ \
    << " (" << __func__ << ") - " << msg << std::endl;
#else
#define ERR_PRINT(...)
#endif

#if 0
#define DBG_PRINT(msg) std::cout << "[DEBUG] " << __FILE__ << ":" << __LINE__ \
    << " (" << __func__ << ") - " << msg << std::endl;
#else
#define DBG_PRINT(...)
#endif

#define INF_PRINT(msg) std::cout << "[INFO] " << __FILE__ << ":" << __LINE__ \
    << " (" << __func__ << ") - " << msg << std::endl;

#define VER_PRINT(msg) std::cout << msg << std::endl;


class WebServer {
    private:
        Config				    config;
        std::vector<pollfd>	    fds;
        std::map<int, Client*>  clients;
        Server                  *server; 

    public:
        WebServer();
        ~WebServer();

        bool 			loadConfig(const std::string& path);
        const Config&	getConfig() const;
        void 			setConfig(const Config& cfg);

        void handleNewConnection();
        void handleClientData(int client_fd);
        void handleClientSend(int client_fd);
        void cleanupClient(int client_fd);
        void updatePollEvents(int client_fd, short events);
        void processClientRequest(int client_fd);

        void init(int port);
        void run();
};

#endif
