#ifndef NETWORK_COMMUNICATION_HPP
#define NETWORK_COMMUNICATION_HPP

#include <boost/asio.hpp>
#include <iostream>
#include <vector>
#include <cstdlib> 
#include <string>
#include <sstream>
#include <iomanip>
#include <zlib.h>
#include <nlohmann/json.hpp>
#include <boost/thread.hpp>

#include "ParticleSimulation.hpp"

using boost::asio::ip::tcp;
namespace asio = boost::asio;
using boost::system::error_code;
using json = nlohmann::json;

std::vector<char> prepareMessageForJavaUTF(const std::string& message);
std::string decompressGzip(const std::vector<char>& compressedData);

void sendExplorerToServer(asio::ip::tcp::socket& socket, SimulationPanel& SimPanel);

uint64_t ntohll(uint64_t value);
long long getTimeDifference(uint64_t javaTimeMillis);

#endif // NETWORK_COMMUNICATION_HPP
