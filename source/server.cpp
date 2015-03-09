/*
 * server.cpp
 *
 *  Created on: Mar 4, 2015
 *      Author: simonenkos
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include <iostream>
#include <thread>

#include <common/logger.hpp>
#include <server/manager.hpp>

#define LOG_FILE_PATH "/var/log/lesen.log"
#define WORKING_DIR   "/"

bool is_stop_requested = false;

int main(int argc, char ** argv)
{
   // Check programm arguments.

   if (argc != 3)
   {
      std::cout << "Invalid arguments! Usage: lesen-server <port> <max_connections>" << std::endl;
      return EXIT_FAILURE;
   }

   int port = std::atoi(argv[1]);
   int conn = std::atoi(argv[2]);

   // Fork to make a daemon process.

   pid_t sid, pid = fork();

   if (pid < 0)
   {
      std::cout << "Cannot create subprocess: " << strerror(errno) << std::endl;
      return EXIT_FAILURE;
   }
   else if (pid > 0)
   {
      return EXIT_SUCCESS;
   }

   // Initialize logger, which will be used to log some important events.

   if (!logger::initialize(LOG_FILE_PATH))
   {
      std::cout << "Cannot open log at: " << LOG_FILE_PATH << std::endl;
      return EXIT_FAILURE;
   }

   // Finishing the creation of the daemon process.

   umask(0);

   if ((sid = getsid(pid)) < 0)
   {
      LOG("Can't get session identifier!");
      return EXIT_FAILURE;
   }

   if (chdir(WORKING_DIR) < 0)
   {
      LOG("Can't change working directory to: " << WORKING_DIR);
      return EXIT_FAILURE;
   }

   close(STDIN_FILENO);
   close(STDOUT_FILENO);
   close(STDERR_FILENO);

   // Register of some signal handlers to be able correctly finish up
   // the work of the daemon.

   auto daemon_closer = [](int signal) {
      LOG("Closed by '" << strsignal(signal) << "' signal");
      is_stop_requested = true;
   };

   signal(SIGINT,  daemon_closer);
   signal(SIGQUIT, daemon_closer);
   signal(SIGHUP,  daemon_closer);
   signal(SIGTERM, daemon_closer);

   // Creating main entities and starting polling cycle.

   auto io_service_ptr = std::make_shared<boost::asio::io_service>();
   auto endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port);

   auto manager_ptr = std::make_shared<manager>(io_service_ptr,
                                                endpoint,
                                                conn);
   manager_ptr->accept(false);

   while (!is_stop_requested)
   {
      try
      {
         io_service_ptr->poll();
         manager_ptr->clean_up_finished();
      }
      catch (boost::system::error_code & error)
      {
         LOG("An error was appeared during main cycle: " << error.message());
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
   }
   io_service_ptr->stop();
   io_service_ptr.reset();

   return EXIT_SUCCESS;
}
