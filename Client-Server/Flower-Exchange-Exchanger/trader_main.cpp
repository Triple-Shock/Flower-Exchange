// client.cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <tchar.h>

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers eg: winsock 1
#include <winsock2.h> // Improvements to the Windows Sockets API done by microsoft
#include <Ws2tcpip.h> // For InetPton function
#pragma comment(lib, "ws2_32.lib") // Linker directive to link with ws2_32.lib winsock2 library

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

// CSV support functions
std::string readCSVFile(const std::string& filePath);
void writeCsvToFile(const std::string& csvString, const std::string& filename);

int main() {

    // SOCKET STUFF
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return EXIT_FAILURE;
    }



    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    InetPton(AF_INET, _T("127.0.0.1"), &serverAddr.sin_addr.s_addr); // _T() is used to convert the string literal to a TCHAR 



    while (true) {
        // Ask user to input CSV file name
        std::string csvFilePath;
        std::cout << "Enter CSV file name (or type 'exit' to quit): ";
        std::getline(std::cin, csvFilePath);

        SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Error creating socket\n";
            WSACleanup();
            return EXIT_FAILURE;
        }

        if (connect(clientSocket, reinterpret_cast<SOCKADDR*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR) {
            std::cerr << "Error connecting to server\n";
            closesocket(clientSocket);
            WSACleanup();
            return EXIT_FAILURE;
        }

        if (csvFilePath == "exit") {
            break;  // Exit the loop if the user types 'exit'
        }

        // Read CSV file
        std::string csvData = readCSVFile(csvFilePath);

        // Send size of CSV first
        // Send CSV size to server
        uint32_t csvSize = static_cast<uint32_t>(csvData.size());
        send(clientSocket, reinterpret_cast<const char*>(&csvSize), sizeof(csvSize), 0);


        // Send CSV to server
        size_t totalSent = 0;
        while (totalSent < csvData.size()) {
            //std::cout << "Sent " << totalSent << " bytes of " << csvData.size() << " bytes\n";
            size_t remainingData = csvData.size() - totalSent;
            size_t chunkSize = BUFFER_SIZE < remainingData ? BUFFER_SIZE : remainingData;

            int bytesSent = send(clientSocket, csvData.c_str() + totalSent, chunkSize, 0);

            if (bytesSent <= 0) {
                // Handle error or connection closed
                std::cerr << "Error sending CSV to server\n";
                break;
            }

            totalSent += static_cast<size_t>(bytesSent);
        }
        /*std::string eof = "\r\n\r\n";
        send(clientSocket, eof.c_str(), eof.size(), 0);*/
        std::cout << "Sent CSV to server\n";

        // Receive processed CSV from server
        uint32_t rcvCsvSize = 0;
        recv(clientSocket, reinterpret_cast<char*>(&rcvCsvSize), sizeof(rcvCsvSize), 0);

        std::string receivedCSV;
        receivedCSV.resize(rcvCsvSize);
        size_t totalReceived = 0;
        while (totalReceived < rcvCsvSize) {
            int bytesReceived = recv(clientSocket, &receivedCSV[totalReceived], rcvCsvSize - totalReceived, 0);

            if (bytesReceived <= 0) {
                // Either an error occurred or the client closed the connection
                std::cerr << "Error receiving processed CSV from server\n";
                break;
            }

            totalReceived += static_cast<size_t>(bytesReceived);
        }



        char buffer[BUFFER_SIZE] = { 0 };
        recv(clientSocket, buffer, BUFFER_SIZE, 0);
        std::string processedCSV(buffer);
        std::cout << "Received processed CSV from server:\n" << receivedCSV << "\n";

        // Write processed CSV to file
        std::string processedCSVFilePath = "execution_report_" + csvFilePath;
        writeCsvToFile(receivedCSV, processedCSVFilePath);
        closesocket(clientSocket);

    }

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

void writeCsvToFile(const std::string& csvString, const std::string& filename)
{
    std::ofstream outputFile(filename, std::ios::out | std::ios::app);

    if (!outputFile.is_open())
    {
        std::cerr << "Error opening file: " << filename << "-> Creating new file with same filename" << std::endl;

        // Try creating a new file
        outputFile.open(filename, std::ios::out);
        if (!outputFile.is_open())
        {
            std::cerr << "Error creating file: " << filename << std::endl;
            return;
        }
    }

    outputFile << csvString;

    outputFile.close();

    std::cout << "CSV data has been written to: " << filename << std::endl;
}
