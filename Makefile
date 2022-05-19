
# # # # # # # # # # # # # # # # # # # # # # # # # # # # 

# $Author:     Tadeas Kachyna <xkachy00@stud.fit.vutbr.cz>
# $Date:       $2022-03-10

# # # # # # # # # # # # # # # # # # # # # # # # # # # # 

C=gcc 
CFLAGS= -std=gnu99 -pedantic -Wall -Wextra -Werror

all:
	$(C) $(CFLAGS) hinfosvc.c -o hinfosvc
clean:
	rm -f hinfosvc hinfosvc.o