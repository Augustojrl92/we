/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmorenil <fmorenil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 16:54:56 by aurodrig          #+#    #+#             */
/*   Updated: 2025/10/16 22:03:26 by fmorenil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <map>
#include <vector>

class Config {
    private:
        int                         listeningPort;
        std::string                 documentRoot;
        std::string                 indexPath;
        std::vector<std::string>    serverName;
        std::vector<std::string>    allowMethods;
        std::map<int, std::string>  errorPageMap;
        std::string                 clientMaxBodySize;
        std::string                 cgiPass;
        bool                        autoIndex;
        bool                        uploadEnable;
        std::string                 uploadStore;
        int                         numServerBlocks;

    public:
        
        Config();
        ~Config();

        void                        setNumServerBlocks(int numBlocks);
        int                         getNumServerBlocks(void) const;
        void                        setListeningPort(int _listeningPort);
        int                         getListeningPort(void) const;
        void                        setDocumentRoot(std::string _documentRoot);
        std::string                 getDocumentRoot(void) const;
        void                        setIndexPath(std::string _indexPath);
        std::string                 getIndexPath(void) const;
        void                        setErrorPage(int errorCode, std::string path);
        std::map<int, std::string>  getErrorPageMap(void) const;
        void                        setServerName(std::vector<std::string> _serverName);
        std::vector<std::string>    getServerName(void) const;
        void                        setAllowMethods(std::vector<std::string> _allowMethods);
        std::vector<std::string>    getAllowMethods(void) const;
        void                        setClientMaxBodySize(std::string _clientMaxBodySize);
        std::string                 getClientMaxBodySize(void) const;
        void                        setAutoIndex(bool _autoIndex);
        bool                        getAutoIndex(void) const;
        void                        setCgiPass(std::string _cgiPass);
        std::string                 getCgiPass(void) const;
        void                        setUploadEnable(bool _uploadEnable);
        bool                        getUploadEnable(void) const;
        void                        setUploadStore(std::string _uploadStore);
        std::string                 getUploadStore(void) const;
        bool                        load(const std::string& path);
        void                        printConfig(void) const;
};

#endif
