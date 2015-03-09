/*
 * session.cpp
 *
 *  Created on: Mar 4, 2015
 *      Author: simonenkos
 */

#include <common/session.hpp>

#define BUFFER_SIZE 1500

session::session(lesen::socket_ptr socket_ptr) : socket_ptr_(socket_ptr)
{
   tx_buffer = new unsigned char [BUFFER_SIZE];
   rx_buffer = new unsigned char [BUFFER_SIZE];

   assert(tx_buffer && rx_buffer);
};

session::~session()
{
   delete [] tx_buffer;
   delete [] rx_buffer;
};

/**
 * To start data transfer.
 */
void session::start(bool rx_first)
{
   assert(socket_ptr_);
   rx_first ? do_read() : do_write();
};

/**
 * To update socket with a new one and start data transfer.
 */
void session::start(lesen::socket_ptr socket_ptr, bool rx_first)
{
   if (socket_ptr_)
   {
      socket_ptr_->close();
      socket_ptr_.reset();
   }
   socket_ptr_ = socket_ptr;

   start(rx_first);
};

void session::do_read()
{
   socket_ptr_->async_read_some
   (
         boost::asio::buffer(rx_buffer, BUFFER_SIZE),
         [this](const boost::system::error_code & error, std::size_t length)
         {
            if (!error)
            {
               if (put(rx_buffer, length) > 0) do_write();
            }
            else socket_ptr_->cancel();
         }
   );
};

void session::do_write()
{
   int written_size = get(tx_buffer, BUFFER_SIZE);

   if (written_size > 0)
   {
      socket_ptr_->async_write_some
      (
            boost::asio::buffer(tx_buffer, written_size),
            [this](const boost::system::error_code & error, std::size_t length)
            {
               if (!error)
               {
                  do_read();
               }
               else socket_ptr_->cancel();
            }
      );
   }
   else do_read();
};
