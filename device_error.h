#pragma once
#include <string>
#include <stdio.h>

class device_error {
private:
	std::string message;
public:
	device_error(std::string msg) {
		this->message = msg;
	}

	void print_error() {
        printf("Error: %s\n", message.c_str());
	}

	std::string get_error_string() {
		return message;
	}

    char const *get_error_c_string() {
		return message.c_str();
	}
};
