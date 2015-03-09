/*
 * data_data_handler.cpp
 *
 *  Created on: Mar 4, 2015
 *      Author: simonenkos
 */

#include <common/logger.hpp>
#include <server/data_handler.hpp>

#define MAX_EXPIRIES_COUNT 10
#define READ_TIMEOUT       1000 // ms

data_handler::data_handler(lesen::io_service_ptr io_service_ptr,
                           const boost::asio::ip::tcp::endpoint & endpoint,
                           const std::string & output_file_name,
                           unsigned long long file_size) : acceptor(io_service_ptr, endpoint)
                                                         , session(nullptr, READ_TIMEOUT)
                                                         , std::thread(&data_handler::process, this)
                                                         , io_service_ptr_(io_service_ptr)
                                                         , transfer_is_over_(false)
                                                         , file_size_(file_size)
{
   file_stream_.open(output_file_name);

   assert(file_stream_.is_open());
};

data_handler::~data_handler()
{
   file_stream_.close();
};

/**
 * Method check status of data transferring operation using thread-safe flag.
 */
bool data_handler::is_transfer_over() const
{
   return transfer_is_over_;
};

int data_handler::put(unsigned char * data_ptr, unsigned size)
{
   unsigned writing_size = file_size_ < size
                         ? file_size_ : size;

   file_stream_.write((const char *)data_ptr, writing_size);
   file_size_ -= writing_size;

   if (file_size_ <= 0)
   {
      stop();
      io_service_ptr_->stop();
      return 0;
   }
   return size;
};

int data_handler::get(unsigned char * data_ptr, unsigned size)
{
   return 0;
};

bool data_handler::on_accept(lesen::socket_ptr socket_ptr)
{
   start(socket_ptr);
   return true;
};

void data_handler::process()
{
   accept(true);
   io_service_ptr_->run();
   transfer_is_over_ = true;
};
