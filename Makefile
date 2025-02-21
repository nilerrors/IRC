# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: bel-bouc <bel-bouc@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/07/14 18:38:18 by bel-bouc          #+#    #+#              #
#    Updated: 2024/07/14 20:49:58 by bel-bouc         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ircserv

# FLAGS = -Wextra -Wall -Werror -std=c++98
FLAGS = -std=c++98

SRC_MAIN        := $(wildcard *.cpp)
SRC_CLASSES     := $(wildcard classes/*.cpp)

OBJ_MAIN        := obj/main.o
OBJ_CLASSES     := $(patsubst classes/%, obj/%,\
				$(patsubst %.cpp, %.o,\
				$(patsubst %.cpp, %.o, $(SRC_CLASSES))))

$(NAME): $(OBJ_MAIN) $(OBJ_CLASSES) 
	c++  -o $@ $^ 
	
obj/%.o: **/%.cpp
	c++ $(FLAGS) -g -o $@ -c $< 

$(OBJ_MAIN) : $(SRC_MAIN) 
	c++ $(FLAGS) -c -g  $(SRC_MAIN) -o obj/main.o 

all: $(NAME)

clean:
	rm -f  $(OBJ_MAIN) $(OBJ_CLASSES) 

fclean:
	rm -f  $(OBJ_MAIN) $(OBJ_CLASSES)  $(NAME)

re: fclean all

.PHONY : all clean fclean re 