#include <iostream>
#include <fstream>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>
#include <regex>
#include <future>

void reverse_dns_lookup(const std::string& ip) {
    struct sockaddr_in sa;
    char host[NI_MAXHOST];
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = inet_addr(ip.c_str());
    int res = getnameinfo((struct sockaddr*)&sa, sizeof(sa), host, NI_MAXHOST, NULL, 0, 0);
    if (res == 0) {
        std::regex ip_regex(R"((\d{1,3}\.){3}\d{1,3}$)");
        std::string hostname(host);
        if (!std::regex_match(hostname, ip_regex)) {
            std::cout << ip << " -> " << host << std::endl;
        }
    }
}

void process_subnet(const std::string& subnet) {
    std::string ip, mask;
    std::istringstream iss(subnet);
    if (std::getline(iss, ip, '/') && std::getline(iss, mask)) {
        int mask_bits = std::stoi(mask);
        struct in_addr addr;
        if (inet_pton(AF_INET, ip.c_str(), &addr) != 1) {
            std::cerr << "Error: invalid IP address" << std::endl;
            return;
        }
        uint32_t ip_value = ntohl(addr.s_addr);
        uint32_t mask_value = (mask_bits == 0) ? 0 : (~0 << (32 - mask_bits));
        uint32_t network_value = ip_value & mask_value;
        uint32_t broadcast_value = network_value | ~mask_value;
        for (uint32_t i = network_value + 1; i < broadcast_value; i++) {
            struct in_addr addr = { htonl(i) };
            reverse_dns_lookup(inet_ntoa(addr));
        }
    } else {
        struct in_addr addr;
        if (inet_pton(AF_INET, subnet.c_str(), &addr) != 1) {
            std::cerr << "Error: invalid IP address" << std::endl;
            return;
        }
        reverse_dns_lookup(subnet);
    }
}


void process_file(const std::string& filename, size_t num_threads) {
    std::ifstream file(filename.c_str());
    if (!file.is_open()) {
        std::cerr << "Error: could not open file " << filename << std::endl;
        return;
    }
    std::string line;
    std::vector<std::future<void>> futures;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string ip_address, subnet;
        if (std::getline(iss, ip_address, '/') && std::getline(iss, subnet)) {
            process_subnet(ip_address + "/" + subnet);
        } else {
            futures.emplace_back(std::async(std::launch::async, reverse_dns_lookup, ip_address));
        }
    }
    file.close();

    for (auto& future : futures) {
        future.wait();
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3 || (std::string(argv[1]) != "-s" && std::string(argv[1]) != "-f")) {
        std::cerr << "Usage: " << argv[0] << " [-s subnet / single ip | -f filename]" << std::endl;
        return 1;
    }
    if (std::string(argv[1]) == "-s") {
        process_subnet(argv[2]);
    } else if (std::string(argv[1]) == "-f") {
        process_file(argv[2],4);
    }
    return 0;
}
