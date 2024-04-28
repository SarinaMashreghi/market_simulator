cmake_minimum_required(VERSION 3.22)

project(market_simulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BINARY_DIR "${CMAKE_SOURCE_DIR}/build")

add_executable(market_simulator
	main.cpp

	# cpp files
	src/tools.cpp
	src/stochastic.cpp
	src/exchange.cpp
	src/client.cpp
	src/option_pricer.cpp

	# header files
	include/tools.h 
	include/stochastic.h
	include/exchange.h 
	include/client.h
	include/option_pricer.h
)
