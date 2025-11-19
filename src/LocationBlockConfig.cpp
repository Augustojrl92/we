/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   LocationBlockConfig.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fvizcaya <fvizcaya@student.42madrid.com>   #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-11-09 21:21:09 by fvizcaya          #+#    #+#             */
/*   Updated: 2025-11-09 21:21:09 by fvizcaya         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <LocationBlockConfig.hpp>

LocationBlock::LocationBlock() { }
LocationBlock::~LocationBlock() { }

LocationBlock& LocationBlock::operator=(const LocationBlock& otherLocationBlock)
{
	if (this != &otherLocationBlock) {
		ServerBlock::operator=(otherLocationBlock);
		locationPath = otherLocationBlock.locationPath;
	}
	return (*this);
}

void LocationBlock::setLocationPath(std::string _locationPath)
{
	locationPath = _locationPath;
}

std::string LocationBlock::getLocationPath(void) const
{
	return (locationPath);
}