CXXFLAGS = -O3 -g0 -march=native
LDFLAGS = $(CXXFLAGS)

dnsseed: dns.o bitcoin.o netbase.o protocol.o db.o main.o util.o logging.o
	g++ -pthread $(LDFLAGS) -o dnsseed dns.o bitcoin.o netbase.o protocol.o db.o main.o util.o logging.o -lcrypto

%.o: %.cpp bitcoin.h netbase.h protocol.h db.h serialize.h uint256.h util.h logging.h
	g++ -std=c++11 -pthread $(CXXFLAGS) -Wall -Wno-unused -Wno-sign-compare -Wno-reorder -Wno-comment -c -o $@ $<

dns.o: dns.c
	gcc -pthread -std=c99 $(CXXFLAGS) dns.c -Wall -c -o dns.o

%.o: %.cpp

clean:
	$(RM) dnsseed dns.o bitcoin.o netbase.o protocol.o db.o main.o util.o logging.o
