#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <winsock2.h>

#include "Order.h"
#include "Trader.h"
#include "Exchange.h"
#include "OrderBook.h"

#pragma comment(lib, "ws2_32.lib")

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

std::string readCSVFile(const std::string& filePath);

int main(int argc, char* argv[]) {


    std::string filename = "instruments.csv"; // Default filename

    // Check if a filename is provided as a command-line argument
    if (argc == 2) {
        filename = argv[1];
    }

    Trader trader;

    // SOCKET STUFF

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return EXIT_FAILURE;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Error creating socket\n";
        WSACleanup();
        return EXIT_FAILURE;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Error binding socket\n";
        closesocket(serverSocket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        std::cerr << "Error listening on socket\n";
        closesocket(serverSocket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    std::cout << "Server listening on port " << PORT << "...\n";

    SOCKET clientSocket;
    sockaddr_in clientAddr;
    int addrLen = sizeof(clientAddr);

    while (true) {
        // Accept connection from client
        std::cout << "Waiting for client to connect..." << std::endl;
        clientSocket = accept(serverSocket, reinterpret_cast<SOCKADDR*>(&clientAddr), &addrLen);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Error accepting connection\n";
            closesocket(serverSocket);
            WSACleanup();
            return EXIT_FAILURE;
        }
        std::cout << "Client connected\n";

        uint32_t csvSize = 0;
        recv(clientSocket, reinterpret_cast<char*>(&csvSize), sizeof(csvSize), 0);

        std::string receivedCSV;
        receivedCSV.resize(csvSize);
        int totalReceived = 0;
        while (totalReceived < csvSize) {
            int bytesReceived = recv(clientSocket, &receivedCSV[totalReceived], csvSize - totalReceived, 0);

            if (bytesReceived <= 0) {
                // Either an error occurred or the client closed the connection
                std::cerr << "Error receiving CSV from client\n";
                break;
            }

            totalReceived += bytesReceived;
        }

        std::cout << "Received CSV from client:\n" << std::endl;

        // Initialise order books
        OrderBook roseOrderBook("Rose");
        OrderBook lavenderOrderBook("Lavender");
        OrderBook lotusOrderBook("Lotus");
        OrderBook tulipOrderBook("Tulip");
        OrderBook orchidOrderBook("Orchid");

        // Initialise output file
        std::ofstream file(OUTFILE, std::ofstream::trunc);
        if (file.is_open())
        {
            file << "Execution_Rep.csv" << std::endl;
            file << "Order ID,Client Order ID,Instrument,Side,Exec Status,Quantity,Price,Reason,Transaction Time" << std::endl;
        }
        else
            return 0;

        file.close();

        // Vectorisation
        auto start = std::chrono::high_resolution_clock::now();
        std::vector<Order> orders = trader.readOrders(receivedCSV);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "Time taken to create orders vector: " << duration.count() << " microseconds" << std::endl;

        // Trade

        // loop through orders
        start = std::chrono::high_resolution_clock::now();
        for (Order& order : orders) {
            // std::cout << ":::::::: " << std::endl;
            if (order.is_valid()) {
                if (order.instrument == "Rose")
                    roseOrderBook.trade(order);
                else if (order.instrument == "Lavender")
                    lavenderOrderBook.trade(order);
                else if (order.instrument == "Lotus")
                    lotusOrderBook.trade(order);
                else if (order.instrument == "Tulip")
                    tulipOrderBook.trade(order);
                else if (order.instrument == "Orchid")
                    orchidOrderBook.trade(order);
            }
            else {
                // std::cout << "Invalid order: " << order.client_order_id << std::endl;
                order.write_csv();
            }

        }
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "Time taken to process all orders: " << duration.count() << " microseconds" << std::endl;

        // WITHOUT CREATING VECTOR 

        //start = std::chrono::high_resolution_clock::now();

        //// Parse CSV
        //// std::vector<Order> orders;
        //orders.clear();

        //// Create a stringstream from the CSV string
        //std::istringstream csvStream(receivedCSV);

        //std::string line;
        //std::getline(csvStream, line); // Skip the header line
        //std::getline(csvStream, line); // Skip the header line 2
        //int i = 1; // keep order count
        //while (std::getline(csvStream, line)) {
        //    Order order = trader.parseOrder(line, i);
        //    if (order.is_valid()) {
        //        if (order.instrument == "Rose")
        //            roseOrderBook.trade(order);
        //        else if (order.instrument == "Lavender")
        //            lavenderOrderBook.trade(order);
        //        else if (order.instrument == "Lotus")
        //            lotusOrderBook.trade(order);
        //        else if (order.instrument == "Tulip")
        //            tulipOrderBook.trade(order);
        //        else if (order.instrument == "Orchid")
        //            orchidOrderBook.trade(order);
        //    }
        //    else {
        //        // std::cout << "Invalid order: " << order.client_order_id << std::endl;
        //        order.write_csv();
        //    }
        //    //orders.push_back(order);
        //    i++;
        //}
        //end = std::chrono::high_resolution_clock::now();
        //duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        //std::cout << "Time taken to process orders CSV: " << duration.count() << " microseconds" << std::endl;

        // WITHOUT CREATING VECTOR - END

        // Send execution report back to client
        std::string outCsvData = readCSVFile(OUTFILE);
        uint32_t outCsvSize = static_cast<uint32_t>(outCsvData.size());
        send(clientSocket, reinterpret_cast<const char*>(&outCsvSize), sizeof(outCsvSize), 0);

        // Send CSV to server
        size_t totalSent = 0;
        while (totalSent < outCsvData.size()) {
            //std::cout << "Sent " << totalSent << " bytes of " << outCsvData.size() << " bytes\n";
            size_t remainingData = outCsvData.size() - totalSent;
            size_t chunkSize = BUFFER_SIZE < remainingData ? BUFFER_SIZE : remainingData;

            int bytesSent = send(clientSocket, outCsvData.c_str() + totalSent, chunkSize, 0);

            if (bytesSent <= 0) {
                // Handle error or connection closed
                std::cerr << "Error sending CSV to server\n";
                break;
            }

            totalSent += static_cast<size_t>(bytesSent);
        }
        /*std::string eof = "\r\n\r\n";
        send(clientSocket, eof.c_str(), eof.size(), 0);*/
        std::cout << "Sent CSV to client\n\n" << "...........\n\n\n";


        //send(clientSocket, outCsvData.c_str(), outCsvData.size(), 0);
        //std::cout << "Processed CSV sent back to client" << std::endl;

        // clear output file
        file.open(OUTFILE, std::ofstream::trunc);
        if (file.is_open())
        {
            file << "Execution_Rep.csv" << std::endl;
            file << "Order ID,Client Order ID,Instrument,Side,Exec Status,Quantity,Price,Reason,Transaction Time" << std::endl;
        }
        else
            return 0;

        // remove order books
        roseOrderBook.~OrderBook();
        lavenderOrderBook.~OrderBook();
        lotusOrderBook.~OrderBook();
        tulipOrderBook.~OrderBook();
        orchidOrderBook.~OrderBook();

        // clear orders vector
        orders.clear();

        // Close client socket
        closesocket(clientSocket);
    }

    closesocket(serverSocket);
    WSACleanup();


    return 0;
}

std::string readCSVFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filePath << "\n";
        exit(EXIT_FAILURE);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}
