#include <boost/asio.hpp>
#include <iostream>
#include <string>

using namespace boost::asio;

void sendCommand(serial_port& serial, const std::string& command) {
    // Send the command to the Arduino
    write(serial, buffer(command + "\n"));

    // Prepare a streambuf for reading the response
    streambuf responseBuffer;
    read_until(serial, responseBuffer, "\n"); // Wait for newline-terminated response

    // Convert the response into a string
    std::istream responseStream(&responseBuffer);
    std::string response;
    std::getline(responseStream, response);

    // Print the response
    std::cout << "Arduino says: " << response << std::endl;
}

int main() {
    io_context io;
    serial_port serial(io, "COM6");
    serial.set_option(serial_port_base::baud_rate(9600));
    serial.set_option(serial_port_base::character_size(8));
    serial.set_option(serial_port_base::parity(serial_port_base::parity::none));
    serial.set_option(serial_port_base::stop_bits(serial_port_base::stop_bits::one));
    serial.set_option(serial_port_base::flow_control(serial_port_base::flow_control::none));

    std::cout << "Connected to Arduino." << std::endl;

    std::string command;
    while (true) {
        std::cout << "Enter command (SWEEP, POS:<angle>, q to quit): ";
        std::cin >> command;

        if (command == "q") break;

        if (command == "SWEEP") {
            sendCommand(serial, "<cmd:SWEEP>");
        } else if (command.rfind("POS:", 0) == 0) { // Check if command starts with "POS:"
            sendCommand(serial, "<cmd:POS:" + command.substr(4) + ">");
        } else {
            std::cout << "Unknown command!" << std::endl;
        }
    }

    return 0;
}
