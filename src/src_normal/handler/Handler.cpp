#include "Handler.hpp"
#include "settings.hpp"
#include "fd/Client.hpp"
#include "fd/File.hpp"
#include <fcntl.h>
#include <cstdlib>
#include <sstream>
#include <poll.h>
#include <sys/socket.h>


Handler::Handler(): _file(NULL) {}

//TODO: Make recv work with multiple iterations, so each iter can loop over request
int	Handler::parseRequest(Client* client, int fd)
{
	//TODO: CHECK MAXLEN
	_request.resize(BUFFER_SIZE, '\0');
	ssize_t ret = recv(fd, &_request[0], BUFFER_SIZE, 0);
	if (ret == ERR)
	{
		perror("Recv");
		return (ERR);
	}
	else if (ret == 0)
	{
		client->flag = AFdInfo::TO_ERASE;
		return ERR;
	}
	_request.resize(ret);
	printf("Request size: %lu, Bytes read: %ld\n", _request.size(), ret);

	//TODO: check for continue reading and change the return value
	if(_request_parser.parseHeader(_request) == RequestParser::BAD_REQUEST)
	{
		printf("OMG!!!!\n");		
		return (ERR);
	}

	_request_parser.print();
	generateAbsoluteTarget();
	printf("Absolute target: %s\n", _absolute_target.c_str());
	return OK;

}

void	Handler::generateAbsoluteTarget()
{
	//TODO: resort to the correct Pathname based on default path from config (add Client* client)
	if (_request_parser.getTargetResource() == "/")
	{
		_absolute_target =  "./page_sample/index.html";
	}
	else
	{
		_absolute_target =  "./page_sample" + _request_parser.getTargetResource();
	}
}

//TODO: create response with error status
int Handler::executeMethod(Client* client, FdTable & fd_table)
{
	// TODO: add error handling from Parser (Status != 200)

	if (buildFile(client, fd_table) == ERR)
	{
		return ERR;
	}

	switch (_request_parser.getMethod())
	{
		case RequestParser::GET:
			methodGet(client, fd_table);
			break;
		case RequestParser::POST:
			methodPost(client, fd_table);
			break;
		case RequestParser::DELETE:
			methodDelete(client, fd_table);
			break; 
		default:
			break;
	}

	resetBuffer(); //for _request
	return OK;
}

int	Handler::buildFile(Client* client, FdTable & fd_table)
{
	previewMethod();
	if (createFile(client) == ERR
		|| insertFile(fd_table) == ERR)
	{
		return ERR;
	}
	return OK;
}

//TODO: check if O_TRUNC is correct
void Handler::previewMethod()
{
	switch (_request_parser.getMethod())
	{
		case RequestParser::GET:
			_oflag = O_RDONLY;
			_file_event = AFdInfo::READING;
			break;
		case RequestParser::POST:
			_oflag = O_CREAT | O_RDWR | O_TRUNC;
			_file_event = AFdInfo::WRITING;
			break;
		case RequestParser::DELETE:
			_oflag = O_RDONLY;
			_file_event = AFdInfo::READING;
			break; 
		default:
			break;
	}
}

int	Handler::createFile(Client *client)
{

	int	file_fd = open(_absolute_target.c_str(), _oflag, 0644);
	if (file_fd == ERR)
	{
		// TODO: add error handling (i.e. no file find)
		perror("open in method");
		return ERR;
	}

	printf(BLUE_BOLD "Open File:" RESET_COLOR " [%d]\n", file_fd);

	_file = new File(client, file_fd);

	return OK;

}

int	Handler::insertFile(FdTable & fd_table)
{
	int	file_index = fd_table.size();
	if (fd_table.insertFd(_file) == ERR)
	{
		return ERR;
	}
	_file->updateEvents(_file_event, fd_table);

	return OK;
}

//TODO: retain information about the next request if present
void	Handler::resetBuffer()
{
	_request.clear();
}

int	Handler::methodGet(Client* client, FdTable & fd_table)
{
	return OK;
}

int	Handler::methodPost(Client* client, FdTable & fd_table)
{
	_file->setContent(_request_parser.getMessageBody());

	return OK;
}

int	Handler::methodDelete(Client* client, FdTable & fd_table)
{

	if (remove(_request_parser.getTargetResource().c_str()) == ERR)
	{
		perror("remove in methodDelete");
		return ERR;
	}
	printf(BLUE_BOLD "Delete File:" RESET_COLOR " [%s]\n", _request_parser.getTargetResource().c_str());

	return OK;
}

int	Handler::sendResponse(int fd)
{
	generateResponse();

	_file->flag = AFdInfo::TO_ERASE;
	_file = NULL;

	if (send(fd, _response.c_str(), _response.size(), 0) == ERR)
	{
		perror("send");
		return ERR;
	}

	return OK;
}

void	Handler::generateHeaderString()
{
	for (header_iterator i = _header_fields.begin(); i != _header_fields.end(); ++i)
	{
		_header_string += (i->first + ": " + i->second + NEWLINE);
	}
}

void	Handler::generateResponse()
{
	if (_file)
	{
		_message_body = _file->getContent();
	}
	_http_version = "HTTP/1.1";
	_status_code = "200";
	_status_phrase = "OK";
	_header_fields["Host"] = "localhost";
	_header_fields["Content-Length"] = ft_itoa(_message_body.size());

	generateHeaderString();
	
	_response = _http_version + ' '
				+ _status_code + ' '
				+ _status_phrase + NEWLINE
				+ _header_string + NEWLINE
				+ _message_body;
}

std::string	Handler::ft_itoa(int i) const
{
	std::stringstream	ss;
	ss << i;
	return ss.str();
}