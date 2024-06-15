#pragma once

#include <iostream>
#include <memory>
#include <sys/socket.h>
#include <unistd.h>

class SocketDeleter {
public:
    void operator()(int* sockfd) const {
        if (sockfd && *sockfd != -1) {
            std::cout << "Closing socket: " << *sockfd << std::endl;
            close(*sockfd);
            delete sockfd;
        }
    }
};