cmake_minimum_required(VERSION 3.28)
project(os_idz4)

set(CMAKE_CXX_STANDARD 20)

add_executable(server
        out/Out.cpp
        out/StreamOut.cpp
        out/UdpOut.cpp
        server/server.cpp
        server/main.cpp
        server/handler.cpp
        parse_short.cpp
        inet_utils.cpp
)

add_executable(producer
        clients/producer.cpp
        parse_short.cpp
        inet_utils.cpp
        out/Out.cpp
        out/StreamOut.cpp
        out/UdpOut.cpp
)
add_executable(smoker
        clients/smoker.cpp
        parse_short.cpp
        inet_utils.cpp
        out/Out.cpp
        out/StreamOut.cpp
        out/UdpOut.cpp
)
add_executable(logger
        clients/logger.cpp
        parse_short.cpp
        inet_utils.cpp
)
