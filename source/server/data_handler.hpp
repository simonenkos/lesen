/*
 * data_data_handler.hpp
 *
 *  Created on: Mar 4, 2015
 *      Author: simonenkos
 */

#ifndef __LESEN__SERVER__DATA_HANDLER__
#define __LESEN__SERVER__DATA_HANDLER__

#include <thread>
#include <fstream>

#include <common/session.hpp>

/**
 * This class holds a separate thread and implements both acceptor and session
 * interfaces to receive connection request and data of a file from client.
 */
class data_handler : protected acceptor
                   , protected session
                   , public std::thread
{
   lesen::io_service_ptr io_service_ptr_;
   std::atomic<bool> transfer_is_over_;
   unsigned long long file_size_;
   std::ofstream file_stream_;

public:
   data_handler(lesen::io_service_ptr io_service_ptr,
                const boost::asio::ip::tcp::endpoint & endpoint,
                const std::string & output_file_name,
                unsigned long long file_size);
   ~data_handler();

   data_handler(const data_handler &  other) = delete;
   data_handler(const data_handler && other) = delete;

   data_handler & operator=(const data_handler &  other) = delete;
   data_handler & operator=(const data_handler && other) = delete;

   bool is_transfer_over() const;

private:
   int put(unsigned char * data_ptr, unsigned size);
   int get(unsigned char * data_ptr, unsigned size);

   bool on_accept(lesen::socket_ptr socket_ptr);

   void process();
};

typedef std::shared_ptr<data_handler> data_handler_ptr;

#endif /* __LESEN__SERVER__DATA_HANDLER__ */
