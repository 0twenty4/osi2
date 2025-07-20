#include <ws2tcpip.h>
#include <iostream>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")

std::string sum(std::string summands) {
	std::istringstream sstream(summands);
	double a, b;
	sstream >> a >> b;
	return std::to_string(a + b);
}

int start_server() {
	PCSTR server_ip = "192.168.0.100";
	int port = 2011;
	int clients_count = 2;
    int recv_size = 2;
    int send_size = 1;
	int ret_val;

	WSAData wsa_data;
	if (WSAStartup(MAKEWORD(2, 2, ), &wsa_data) != NO_ERROR) {
		std::cout << "WSAStartup failed" << std::endl;
		return 0;
	}

	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

	std::cout << "Creating the server socket" << std::endl;

	SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (server_socket == INVALID_SOCKET) {
		std::cout << "Socket creation failed with error: " << WSAGetLastError();
		WSACleanup();
		return 0;
	}

	std::cout << "Binding the server socket" << std::endl;

	ret_val = bind(server_socket, (sockaddr*)&server_addr, sizeof(server_addr));

	if (ret_val == SOCKET_ERROR) {
		std::cout << "Binding server socket failed with error: " << WSAGetLastError();
		closesocket(server_socket);
		WSACleanup();
		return 0;
	}

	SOCKADDR_IN client_addr;
	int client_addr_size = sizeof(client_addr);

	while (true) {
		std::cout << "Listening to the server socket" << std::endl;

		ret_val = listen(server_socket, clients_count);

		if (ret_val == SOCKET_ERROR) {
			std::cout << "Listening to the server socket failed with error: " << WSAGetLastError();
			closesocket(server_socket);
			WSACleanup();
			return 0;
		}

		std::cout << "Accepting the client socket" << std::endl;

		SOCKET client_socket = accept(server_socket, (sockaddr*)&client_addr, &client_addr_size);
		if (client_socket == INVALID_SOCKET) {
			std::cout << "Accepting failed with error: " << WSAGetLastError();
			closesocket(server_socket);
			WSACleanup();
			return 0;
		}

		std::cout << "Receiving client data" << std::endl;

		std::string summands;
		summands.resize(1024);

		ret_val = recv(client_socket, (char*)summands.data(), summands.size(), 0);

		if (ret_val == SOCKET_ERROR) {
			std::cout << "Receiving failed with error: " << WSAGetLastError();
			closesocket(client_socket);
			closesocket(server_socket);
			WSACleanup();
			return 0;
		}

		if (summands.data()[0]=='s') {
			std::cout << "Shutting down the server" << std::endl;
			closesocket(client_socket);
			closesocket(server_socket);
			WSACleanup();
			return 1;
		}

		std::cout << "Sending results" << std::endl;

		std::string res;

		res = sum(summands);

		ret_val = send(client_socket, res.data(), res.size(), 0);

		if (ret_val == SOCKET_ERROR) {
			std::cout << "Sending failed with error: " << WSAGetLastError();
			closesocket(client_socket);
			closesocket(server_socket);
			WSACleanup();
			return 0;
		}
		closesocket(client_socket);
	}
}


int main() {
    start_server();
}