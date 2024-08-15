cc		= c++

SRC	=  client.cpp server.cpp channels.cpp 

FLAGS = -Wall -Wextra -Werror 

fileo	= $(SRC:.cpp=.o)

Name	= ircserv

all : $(Name)

$(Name) : $(fileo)
		$(cc) $(FLAGS) $(fileo) -o $@

%.o	:  %.cpp Bureaucrat.hpp
		$(cc) $(FLAGS) $(file) -c $<

clean :
		rm -rf $(fileo)

fclean : clean
		rm -rf $(Name)

re	: fclean all
	
.Phony : clean fclean re