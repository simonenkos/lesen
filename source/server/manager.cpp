/*
 * manager.cpp
 *
 *  Created on: Mar 4, 2015
 *      Author: simonenkos
 */

#include <algorithm>

#include <boost/format.hpp>

#include <common/logger.hpp>
#include <server/manager.hpp>

manager::manager(lesen::io_service_ptr io_service_ptr,
                 const boost::asio::ip::tcp::endpoint & endpoint,
                 unsigned short int max_connections) : acceptor(io_service_ptr, endpoint)
                                                     , first_service_port_(endpoint.port() + 1)
{
   LOG("Run file server at port: " << endpoint.port());
   LOG("Available service ports range: " << first_service_port_ << " - " << first_service_port_ + max_connections);

   flow_handlers_list_.resize(max_connections);
   data_handlers_list_.resize(max_connections);
};

manager::~manager()
{
   for (auto & data_handler_ptr : data_handlers_list_)
   {
      if (data_handler_ptr && data_handler_ptr->joinable()) data_handler_ptr->join();
   }
};

/**
 * Method allows to check which handlers were stopped and remove them from list.
 */
void manager::clean_up_finished()
{
   for (unsigned index = 0; index < data_handlers_list_.size(); index++)
   {
      auto & data_handler_ptr = data_handlers_list_[index];
      auto & flow_handler_ptr = flow_handlers_list_[index];

      if (data_handler_ptr && data_handler_ptr->is_transfer_over())
      {
         if (data_handler_ptr->joinable()) data_handler_ptr->join();

         data_handler_ptr.reset();
         flow_handler_ptr.reset();
      }
   }
};

bool manager::on_accept(lesen::socket_ptr socket_ptr)
{
   auto peer_info = (boost::format("%s:%d") % socket_ptr->remote_endpoint().address()
                                            % socket_ptr->remote_endpoint().port()).str();

   auto it = std::find_if(flow_handlers_list_.begin(),
                          flow_handlers_list_.end(),
                          [](flow_handler_ptr ptr) { return !ptr; });

   if (flow_handlers_list_.end() == it)
   {
      LOG("Cannot accept new connection from " << peer_info << " : No free handlers!");
      return false;
   }
   else LOG("Accept new connection from " << peer_info);

   auto manager_interface_ptr = std::dynamic_pointer_cast<manager_interface>(shared_from_this());

   (* it) = std::make_shared<flow_handler>(manager_interface_ptr, socket_ptr);

   return true;
};

int manager::new_data_flow(const std::string & file, unsigned long long size)
{
   auto it = std::find_if(data_handlers_list_.begin(),
                          data_handlers_list_.end(),
                          [](data_handler_ptr ptr) { return !ptr; });

   if (data_handlers_list_.end() == it)
   {
      LOG("No free handlers to receive data for file: " << file);
      return -1;
   }
   else LOG("Transferring " << size << " bytes of data to file: " << file);

   auto service_port = first_service_port_ + std::distance(data_handlers_list_.begin(), it);
   auto endpoint = boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), service_port);
   auto io_service_ptr = std::make_shared<boost::asio::io_service>();

   (* it) = std::make_shared<data_handler>(io_service_ptr, endpoint, file, size);

   return service_port;
};
