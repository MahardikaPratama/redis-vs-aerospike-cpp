CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
INCLUDES = -I./src -I/usr/local/include
LIBS = -lcryptopp -laerospike -lssl -lcrypto -lpthread -lz -lm

SRCDIR = src
SOURCES = $(SRCDIR)/app.cpp \
          $(SRCDIR)/operations/operations.cpp \
          $(SRCDIR)/database/aerospike_connection.cpp \
          $(SRCDIR)/data_types/book_serialization.cpp \
          $(SRCDIR)/utils/resource_monitor.cpp
TARGET = aerospike-app

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SOURCES) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
