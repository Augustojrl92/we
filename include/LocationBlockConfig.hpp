/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationBlockConfig.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fvizcaya <fvizcaya@student.42madrid.com>   #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-11-06 21:49:56 by fvizcaya          #+#    #+#             */
/*   Updated: 2025-11-06 21:49:56 by fvizcaya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef _LOCATIONBLOCKCONFIG_H_
#define _LOCATIONBLOCKCONFIG_H_

#include <ServerBlockConfig.hpp>

class LocationBlock : public ServerBlock {
	
	private:
		std::string					locationPath;

	public:
		LocationBlock();
		~LocationBlock();
		LocationBlock& operator=(const LocationBlock& otherLocationBlock);

		void						setLocationPath(std::string _locationPath);
		std::string					getLocationPath(void) const;
};

#endif