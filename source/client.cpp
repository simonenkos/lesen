/*
 * client.cpp
 *
 *  Created on: Mar 4, 2015
 *      Author: simonenkos
 */

#include <stdlib.h>
#include <iostream>

#include <client/sender.hpp>

int main(int argc, char ** argv)
{
   if (argc != 5)
   {
      std::cout << "Invalid arguments! Usage: lesen-client <server_host> <server_port> <src_file> <dst_file>" << std::endl;
      return EXIT_FAILURE;
   }

   std::string host = argv[1];
   std::string port = argv[2];

   std::string src_file = argv[3];
   std::string dst_file = argv[4];

   auto io_server_ptr = std::make_shared<boost::asio::io_service>();
   auto sender_ptr = std::make_shared<sender>(src_file, dst_file);

   if (!sender_ptr->start(io_server_ptr, host, port)) return EXIT_FAILURE;

   try
   {
      io_server_ptr->run();
   }
   catch (boost::system::error_code & error)
   {
      std::cout << "An error was appeared during the data transfer: " << error.message() << std::endl;
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
};
