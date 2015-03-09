/*
 * session.cpp
 *
 *  Created on: Mar 4, 2015
 *      Author: simonenkos
 */

#include <common/session.hpp>

#define BUFFER_SIZE 1500

session::session(lesen::socket_ptr socket_ptr,
                 unsigned read_timeout) : socket_ptr_(socket_ptr)
                                        , read_timeout_(read_timeout)
                                        , is_stopped_(true)
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
 * Method allows to start data transfer.
 */
void session::start(bool rx_first)
{
   assert(socket_ptr_);
   is_stopped_ = false;

   // Setting up deadline timer. Read operation can be blocked for a long time
   // because there will be no incoming data.

   read_timer_ptr_ = std::make_shared<boost::asio::deadline_timer>(socket_ptr_->get_io_service());
   read_timer_ptr_->async_wait(std::bind(&session::read_expiry, this));

   rx_first ? do_read() : do_write();
};

/**
 * Method allows to update socket with a new one and start data transfer.
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

void session::stop()
{
   if (!is_stopped_)
   {
      is_stopped_ = true;

      if (socket_ptr_ && socket_ptr_->is_open())
      {
         socket_ptr_->close();
      }
      if (read_timer_ptr_) read_timer_ptr_->cancel();
   }
};

void session::do_read()
{
   if (is_stopped_) return;

   socket_ptr_->async_read_some
   (
         boost::asio::buffer(rx_buffer, BUFFER_SIZE),
         [this](const boost::system::error_code & error, std::size_t length)
         {
            if (error != boost::asio::error::operation_aborted)
            {
               read_timer_ptr_->cancel();
            }
            if (!is_stopped_ && !error)
            {
               if (put(rx_buffer, length) > 0) do_write();
            }
         }
   );

   read_timer_ptr_->expires_from_now(boost::posix_time::milliseconds(read_timeout_));
};

void session::do_write()
{
   if (is_stopped_) return;

   int written_size = get(tx_buffer, BUFFER_SIZE);

   if (written_size > 0)
   {
      socket_ptr_->async_write_some
      (
            boost::asio::buffer(tx_buffer, written_size),
            [this](const boost::system::error_code & error, std::size_t length)
            {
               if (!is_stopped_ && !error)
               {
                  do_read();
               }
               else stop();
            }
      );
   }
   else do_read();
};

void session::read_expiry()
{
   if (is_stopped_) return;

   // Reactivate timer with current handler.
   read_timer_ptr_->async_wait(std::bind(&session::read_expiry, this));

   // Check if timer expires correctly, cause a new asynchronous operation may move the deadline.
   if (read_timer_ptr_->expires_at() <= boost::asio::deadline_timer::traits_type::now())
   {
      if (socket_ptr_->is_open())
      {
         socket_ptr_->cancel();
         do_write();
      }
   }
};
