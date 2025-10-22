# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: fmorenil <fmorenil@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/27 15:55:23 by fvizcaya          #+#    #+#              #
#    Updated: 2025/10/14 19:44:16 by fmorenil         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -Iincludes

SRC = src/main.cpp
SRC += src/webserver.cpp 
SRC += src/utils.cpp 
SRC += src/config.cpp 
SRC += src/Request.cpp 
SRC += src/Response.cpp
SRC += src/Client.cpp
SRC += src/CGIHandler.cpp
SRC += src/config/fileParsing.cpp
SRC += src/Server.cpp
#SRC += src/test_cgi.cpp

OBJ = $(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(NAME) $(OBJ)

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re