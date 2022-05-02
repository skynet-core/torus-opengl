#include "catch_main.h"

#include <iostream>
#include <filesystem>
#include <vector>
#include <cstdlib>
#include <cstring>
#include "Utils.h"

const char* alphabet = "0123456789";

void writefunc(char* buffer, size_t size) {
	for (size_t i = 0; i < size - 1; i++)
	{
		buffer[i] = alphabet[i % 10];
	}
	buffer[size - 1] = 0;
}


TEST_CASE("Test buffer writes", "[buffer]") {
	std::vector<char> buf(10);
	writefunc(&buf[0], buf.size());
	REQUIRE("012345678" == std::string{&buf[0]});
	std::fill(buf.begin(), buf.end(), 0);
	writefunc(&buf[0], buf.size());
	REQUIRE("012345678" == std::string{ &buf[0] });
	//	writefunc(buf.data(), buf.size());
	//	REQUIRE(true);
}