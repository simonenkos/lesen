/*
 * acceptor.cpp
 *
 *  Created on: Mar 7, 2015
 *      Author: simonenkos
 */

#include <common/acceptor.hpp>

acceptor::acceptor(lesen::io_service_ptr io_service_ptr,
                   const boost::asio::ip::tcp::endpoint & endpoint)
{
   acceptor_ptr_ = std::make_shared<boost::asio::ip::tcp::acceptor>(* io_service_ptr, endpoint);
};

acceptor::~acceptor()
{ };

void acceptor::accept(bool only_once)
{
   auto socket_ptr = std::make_shared<boost::asio::ip::tcp::socket>(acceptor_ptr_->get_io_service());

   acceptor_ptr_->async_accept
   (
         * socket_ptr,
         [this, socket_ptr, only_once](const boost::system::error_code & error)
         {
            if (!acceptor_ptr_->is_open()) return;

            if (!error)
            {
               if (!on_accept(socket_ptr))
               {
                  socket_ptr->close();
                  return;
               }
            }
            if (!only_once) accept(only_once);
         }
   );
};
