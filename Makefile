all: xml2json

xml2json: xml2json.cpp
	g++ -g -o $@ $^ -lxmlparser -ljsoncpp

clean:
	@rm xml2json
