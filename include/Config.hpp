/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Config.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aurodrig <aurodrig@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/23 16:54:56 by aurodrig          #+#    #+#             */
/*   Updated: 2025/11/14 08:51:30 by aurodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <ServerBlockConfig.hpp>

class Config {
    private:
        int             				numServerBlocks;
        std::vector<ServerBlock>		serverBlocks;

    public:
        
        Config();
        ~Config();

        void						setNumServerBlocks(int numBlocks);
        int             			getNumServerBlocks(void) const;
		void            			setServerBlocks(std::vector<ServerBlock> _serverBlocks);
		std::vector<ServerBlock>&	getServerBlocks(void);
		ServerBlock&				getServerBlockIndex(int index);
		const ServerBlock&			getServerBlockIndex(int index) const;
        void            printConfig(void) const;
};

#endif
