/*
 * manager.hpp
 *
 *  Created on: Mar 4, 2015
 *      Author: simonenkos
 */

#ifndef __LESEN__SERVER__MANAGER__
#define __LESEN__SERVER__MANAGER__

#include <vector>

#include <server/flow_handler.hpp>
#include <server/data_handler.hpp>
#include <server/manager_interface.hpp>

/**
 * Main server's class that manages different control flows and data flows using
 * 'flow_handler' and 'data_handler'. It accepts new connections and starts
 * receiving requests from clients. According to requests it starts new process
 * for data transfer in a separate thread on a new socket.
 */
class manager : public manager_interface
              , public acceptor
              , public std::enable_shared_from_this<manager>
{
   unsigned short int first_service_port_;

   std::vector<flow_handler_ptr> flow_handlers_list_;
   std::vector<data_handler_ptr> data_handlers_list_;

public:
   manager(lesen::io_service_ptr io_service_ptr,
           const boost::asio::ip::tcp::endpoint & endpoint,
           unsigned short int max_connections);
   ~manager();

   manager(const manager &  other) = delete;
   manager(const manager && other) = delete;

   manager & operator=(const manager &  other) = delete;
   manager & operator=(const manager && other) = delete;

   void clean_up_finished();

private:
   bool on_accept(lesen::socket_ptr socket_ptr);
   int new_data_flow(const std::string & file, unsigned long long size);
};

#endif /* __LESEN__SERVER__MANAGER__ */
