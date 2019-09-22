#
# cristian Wed Dec 29 11:09:35 2004
# make file for testing all game stuff
#

SHELL = /bin/sh
DIR=/transferServer
PROG = $(DIR)/transferServer/server




OBJECTS = $(DIR)/locks/criticalSectionsFunctions.o\
          $(DIR)/threads/threadFunctions.o\
		  $(DIR)/utils/utilsFunctions.o\
		  $(DIR)/utils/log.o\
		  $(DIR)/sockets/packet/packet.o\
		  $(DIR)/sockets/socket/csocket.o\
		  $(DIR)/client/client.o\
		  $(DIR)/sockets/server/socketServer.o\
		  $(DIR)/transferServer/transferServer.o

HEADERS = $(DIR)/definitions/errorsDefinitions.h\
          $(DIR)/utils/utilsFunctions.h\
		  $(DIR)/utils/log.h\
          $(DIR)/locks/criticalSectionsFunctions.h\
          $(DIR)/threads/threadFunctions.h\
		  $(DIR)/sockets/packet/packet.h\
		  $(DIR)/sockets/socket/csocket.h\
		  $(DIR)/client/client.h\
		  $(DIR)/sockets/socketsDefinitions.h\
		  $(DIR)/sockets/server/socketServer.h
          
		  
CC	= g++

#rutas de los include del projecto
SSL_INCLUDE=/ssl/openssl.0.9.8/include/
CPLUS_INCLUDE_PATH =$(DIR)/client:$(DIR)/definitions/:$(DIR)/locks/:$(DIR)/threads/:$(DIR)/utils/:$(DIR)/sockets/client/:$(DIR)/sockets/server/:$(DIR)/sockets/socket/:$(DIR)/sockets/packet/:$(DIR)/sockets/
export CPLUS_INCLUDE_PATH

#flags del compilador
#CFLAGS = -DLINUX=1 -D_GNU_SOURCE -D_REENTRANT -DINCL_STACK_CRAWL -O3 -g
CFLAGS = -DLINUX=1 -D_REENTRANT -DINCL_STACK_CRAWL -O3 -g -Wall -Wno-deprecated

#para link
#LDFLAGS = -L/usr/local/lib -L/usr/lib -L/usr/local/ssl/lib -L$(PPLIB)
LDFLAGS = -L/usr/local/lib -L/usr/lib -L/ssl/openssl.0.9.8/lib

#libs necesarios para compilar
LIBS = -lpthread -lssl

.cpp.o: $(SOURCES)
	$(CC) -I$(SSL_INCLUDE) $(CPPFLAGS) $(CFLAGS) -c $< -o $*.o


#genera el ejecutable
PROG: $(OBJECTS)
	  $(CC) $(LDFLAGS) $(LIBS) -o $(PROG) $(OBJECTS)

all: Makefile $(PROG)
.PHONY: all

##########################
# Compilation rules.
#
# Take the shortcut and make all object files depend on all header files.
#
$(OBJECTS): $(HEADERS)
