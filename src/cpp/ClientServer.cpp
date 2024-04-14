#include "ClientServer.hpp"

#include <boost/asio.hpp>
#include <iostream>
#include <vector>
#include <cstdlib> 
#include <string>
#include <sstream>
#include <iomanip>
#include <zlib.h>
#include <fstream>
#include <chrono>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/system/error_code.hpp>

#include "ParticleSimulation.hpp"

using boost::asio::ip::tcp;
namespace asio = boost::asio;
using boost::system::error_code;
namespace fs = boost::filesystem;
using json = nlohmann::json;

std::vector<char> prepareMessageForJavaUTF(const std::string& message) {
    unsigned short len = htons(static_cast<unsigned short>(message.length()));

    std::vector<char> formattedMessage;
    formattedMessage.resize(2 + message.length());

    std::memcpy(formattedMessage.data(), &len, 2); 
    std::memcpy(formattedMessage.data() + 2, message.c_str(), message.length()); 

    return formattedMessage;
}

std::string decompressGzip(const std::vector<char>& compressedData) {
    z_stream strm = {};
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = compressedData.size();
    strm.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(compressedData.data()));

    if (inflateInit2(&strm, 16 + MAX_WBITS) != Z_OK) {
        throw std::runtime_error("inflateInit2 failed");
    }

    int ret;
    char outbuffer[4096];
    std::string decompressedData;

    do {
        strm.avail_out = sizeof(outbuffer);
        strm.next_out = reinterpret_cast<Bytef*>(outbuffer);
        ret = inflate(&strm, Z_NO_FLUSH);
        assert(ret != Z_STREAM_ERROR);  
        switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                inflateEnd(&strm);
                throw std::runtime_error("inflate failed");
        }
        int have = sizeof(outbuffer) - strm.avail_out;
        decompressedData.append(outbuffer, have);
    } while (strm.avail_out == 0);

    inflateEnd(&strm);

    return decompressedData;
}

void sendExplorerToServer(asio::ip::tcp::socket& socket, SimulationPanel& SimPanel) {
    std::shared_ptr<Explorer> explorer = SimPanel.getExplorer();
    if (explorer) {
        //std::cout << "ExpExists " << std::endl;
        double x_coords = explorer->getXCoord();
        double y_coords = explorer->getYCoord();
        std::string message = "ExplorerCoordinates " + std::to_string(x_coords) + " " + std::to_string(y_coords);
        auto formattedMessage = prepareMessageForJavaUTF(message);
        size_t bytesWritten = asio::write(socket, asio::buffer(formattedMessage));
        if (bytesWritten == formattedMessage.size()) {
            // std::cout << "Successfully wrote " << bytesWritten << " bytes to the socket." << std::endl;
        } else {
            std::cerr << "Error writing to socket. Expected " << formattedMessage.size()
                      << " bytes but wrote " << bytesWritten << " bytes." << std::endl;
        }
    }
}

uint64_t ntohll(uint64_t value) {
    static const int num = 1;
    if (*(char *)&num == 1) {
        return ((uint64_t)ntohl(value & 0xFFFFFFFF) << 32) | ntohl(value >> 32);
    } else {
        return value;
    }
}

long long getTimeDifference(uint64_t javaTimeMillis) {
    uint64_t correctedJavaTimeMillis = ntohll(javaTimeMillis);

    auto now = std::chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    uint64_t currentTimeMillis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();

    long long timeDifference = currentTimeMillis - correctedJavaTimeMillis;

    return timeDifference;
}

int main() {
    std::cout << "Current Path: " << fs::current_path() << std::endl;
    
    fs::path currentPath = fs::current_path();
    fs::path configPath = currentPath.parent_path().parent_path() / "config.json";

    std::ifstream configFile(configPath.string());
    json configJson;

    if (configFile.is_open()) {
        configFile >> configJson;
        configFile.close();
    } else {
        std::cerr << "Could not open config file: " << configPath.string() << " - " << std::strerror(errno) << std::endl;
        return 1;
    }

    std::string server_ip = configJson.value("ip", "127.0.0.1"); 
    std::string server_port = configJson.value("port", "1234");
    std::cout << "Configured to connect to server at " << server_ip << ":" << server_port << std::endl;

    asio::io_context io_context;
    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve(server_ip, server_port);
    tcp::socket socket(io_context);
    boost::system::error_code ec;
    connect(socket, endpoints, ec);

    ParticleSimulation simulation;

    if (ec) {
        std::cerr << "Failed to connect to server: " << ec.message() << std::endl;
        return 1;
    }
    std::cout << "Connected to server." << std::endl;

    boost::thread simThread([&simulation](){
        simulation.run();
        std::cout << "Close 2" << std::endl;
    });

    boost::thread simUpdateThread([&simulation](){
        simulation.updateSimulationLoop();
        std::cout << "Close 1" << std::endl;
    });


    boost::thread explorerThread([&socket, &simulation](){
        while (simulation.getIsRunning()) {
            if (simulation.getSimulationPanel().getExplorer() != nullptr && simulation.getSimulationPanel().getExplorer()->getMove()){
                sendExplorerToServer(socket, simulation.getSimulationPanel());
                simulation.getSimulationPanel().getExplorer()->revertMove();
            }
            boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
        }
        std::cout << "Close 3" << std::endl;
    });

    std::cout << "Starting to read from server." << std::endl;

    try {
        boost::system::error_code ec;
        size_t len;
        //socket.non_blocking(true, ec);

        while (simulation.getIsRunning()) {
            asio::streambuf buffer;
            std::istream input_stream(&buffer);

            boost::asio::socket_base::bytes_readable command(true);
            socket.io_control(command);

            len = socket.read_some(buffer.prepare(2), ec); 

            if (ec == boost::asio::error::would_block) {
                // std::cerr << "Nothing! " << std::endl;
                
            } else if (ec) {
                std::cerr << "Read error: " << ec.message() << std::endl;
                break;

            } else {
                buffer.commit(len); 

                std::vector<unsigned char> lengthBytes(2);
                input_stream.read(reinterpret_cast<char*>(lengthBytes.data()), 2);
                unsigned int length = (lengthBytes[0] << 8) | lengthBytes[1]; 
                std::string dataType;
                dataType.resize(length);
                len = asio::read(socket, buffer, asio::transfer_exactly(length));
                input_stream.read(&dataType[0], length);
                dataType.erase(std::remove(dataType.begin(), dataType.end(), '\0'), dataType.end());

                std::cout << "Data Type: " << dataType << std::endl;

                uint64_t serverTime;
                asio::read(socket, asio::buffer(&serverTime, sizeof(serverTime)));
                
                std::cout << "Received server time: " << serverTime << std::endl;

                long elapsedTime = getTimeDifference(serverTime);
                std::cout << "Elapsed Time: " << elapsedTime << " milliseconds" << std::endl;

                len = asio::read(socket, buffer, asio::transfer_exactly(4));
                std::vector<unsigned char> jsonLengthBytes(4);
                input_stream.read(reinterpret_cast<char*>(jsonLengthBytes.data()), 4);
                unsigned int jsonLength = (jsonLengthBytes[0] << 24) |
                                        (jsonLengthBytes[1] << 16) |
                                        (jsonLengthBytes[2] << 8)  |
                                        jsonLengthBytes[3];

                std::vector<char> jsonData(jsonLength);
                len = asio::read(socket, buffer, asio::transfer_exactly(jsonLength));
                input_stream.read(jsonData.data(), jsonLength);

                try {
                    std::string decompressedJson = decompressGzip(jsonData);
                    auto jsonParsed = json::parse(decompressedJson);
                    std::cout << "JSON Data: " << jsonParsed.dump(4) << std::endl;
                    
                    if ("ID" == dataType){
                        simulation.setID(jsonParsed);
                    } else if ("Particles" == dataType){
                        simulation.addParticle(jsonParsed, elapsedTime);
                    } else if ("Explorers" == dataType){
                        simulation.addOtherExplorer(jsonParsed);
                    } else if ("Remove" == dataType){
                        simulation.removeExplorer(jsonParsed);
                    } 

                } catch (const std::exception& e) {
                    std::cerr << "Error handling JSON data: " << e.what() << std::endl;
                }
            } 
        }
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    } 

    std::cout << "Close 4" << std::endl;
    simulation.setIsRunning();

    simUpdateThread.join();
    simThread.join();
    explorerThread.join();

    return 0;
}
