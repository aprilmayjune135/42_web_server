#include "ConfigServer.hpp"
#include "settings.hpp"
#include <iostream>
#include <iomanip>
// TODO: Remove port 80 somewhere when process is done
ConfigServer::ConfigServer()
{}

void	ConfigServer::addServerName(std::string name)
{
	this->_server_name.push_back(name);
}

void	ConfigServer::addClientBodySize(size_t client_body_size)
{
	this->_client_body_size = client_body_size;
}

void	ConfigServer::addAllowedMethods(std::string method)
{
	this->_locations[_locations.size() - 1].addAllowedMethods(method);
}

void	ConfigServer::addAutoIndex(int status)
{
	this->_locations[_locations.size() - 1].addAutoIndex(status);
}

void	ConfigServer::addErrorPage(int page_number, std::string path)
{
	_error_pages.push_back(std::pair<int, std::string>(page_number, path));
}

void	ConfigServer::addLocation(ConfigLocation location)
{
	this->_locations.push_back(location);
}

void ConfigServer::addCgi(std::string extention, std::string path)
{
	_locations[_locations.size() - 1].addCgi(extention, path);
}
void	ConfigServer::addAddress(std::string host, int port)
{
	_address.push_back(std::make_pair(host, port));
}

void	ConfigServer::addReturn(int code, std::string path)
{
	_locations[_locations.size() - 1].addReturn(code, path);
}

void	ConfigServer::addRoot(std::string root)
{
	_locations[_locations.size() - 1].addRoot(root);
}

void	ConfigServer::addIndex(std::string index)
{
	_locations[_locations.size() - 1].addIndex(index);
}

void	ConfigServer::addLocationFlag(location_flag flag)
{
	_locations[_locations.size() - 1].addLocationFlag(flag);
}

int	ConfigServer::emptyAddress()
{
	return _ports.empty();
}

// Getters
std::vector<int> ConfigServer::getPorts()
{
	return _ports;
}

ConfigServer::const_iterator ConfigServer::begin() const
{
	return (this->_ports.begin());
}

ConfigServer::const_iterator ConfigServer::end() const
{
	return (this->_ports.end());
}

std::map<std::pair<std::string, int>, std::vector<ConfigServer::server_pointer> > ConfigServer::getAddressMap()
{
	initAddressMap();

	return this->_address_map;
}

// Utility
void	ConfigServer::initAddressMap()
{
	initServerBlock();
	for (size_t i = 0; i < _address.size(); i++)
	{
		_address_map.insert(std::make_pair(_address[i], _server_block));
	}
}

void	ConfigServer::initServerBlock()
{
	SmartPointer<ServerBlock> tmp = SmartPointer<ServerBlock>(new ServerBlock);
	tmp->_client_body_size = _client_body_size;
	tmp->_server_names = _server_name;
	tmp->_error_pages = _error_pages;
	for (size_t i = 0; i < _locations.size(); i++)
	{
		tmp->_locations.push_back(_locations[i].getLocationBlock());
	}
	_server_block.push_back(tmp);
}
