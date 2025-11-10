/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fmorenil <fmorenil@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 16:54:56 by aurodrig          #+#    #+#             */
/*   Updated: 2025/10/28 20:20:44 by fmorenil         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <map>
#include <vector>
#include <ServerBlockConfig.hpp>

class Config {
    private:
        int             				numServerBlocks;
		int								numListenBlocks;
        std::vector<ServerBlock>		serverBlocks;

    public:
        
        Config();
        ~Config();

        void						setNumServerBlocks(int numBlocks);
        int             			getNumServerBlocks(void) const;
		void						setNumListenBlocks(int numBlocks);
		int							getNumListenBlocks(void) const;
		void            			setServerBlocks(std::vector<ServerBlock> _serverBlocks);
		std::vector<ServerBlock>&	getServerBlocks(void);
		ServerBlock&				getServerBlockIndex(int index);
		const ServerBlock&			getServerBlockIndex(int index) const;


        bool            load(const std::string& path);
        void            printConfig(void) const;
};

#endif
