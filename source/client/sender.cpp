/*
 * sender.cpp
 *
 *  Created on: Mar 8, 2015
 *      Author: simonenkos
 */

#include <iostream>

#include <boost/regex.hpp>
#include <boost/format.hpp>

#include <common/message.hpp>
#include <client/sender.hpp>

sender::sender(const std::string src_file,
               const std::string dst_file) : session(nullptr)
                                           , src_file_(src_file)
                                           , dst_file_(dst_file)
                                           , transfer_state_(INACTIVE)
{ };

sender::~sender()
{
   file_stream_.close();
};

bool sender::start(lesen::io_service_ptr io_service_ptr,
                   const std::string & host,
                   const std::string & port)
{
   file_stream_.open(src_file_);

   if (!file_stream_.is_open())
   {
      std::cout << "Cannot open file: " << src_file_ << std::endl;
      return false;
   }

   resolver_ptr_ = std::make_shared<boost::asio::ip::tcp::resolver>(* io_service_ptr);

   if (resolve(host, port))
   {
      transfer_state_ = CONTROL_REQUEST;

      connect();
      return true;
   }

   return false;
};

bool sender::resolve(const std::string & host, const std::string & port)
{
   try
   {
      connection_ = resolver_ptr_->resolve(boost::asio::ip::tcp::resolver::query(host, port));
   }
   catch (boost::system::error_code & error)
   {
      std::cout << "Can't resolve '" << host << ":" << port << "' address!" << std::endl;
      return false;
   }

   return true;
};

void sender::connect()
{
   auto socket_ptr = std::make_shared<boost::asio::ip::tcp::socket>(resolver_ptr_->get_io_service());

   boost::asio::async_connect
   (
         * socket_ptr, connection_,
         [this, socket_ptr](const boost::system::error_code & error, boost::asio::ip::tcp::resolver::iterator iter)
         {
            if (!error)
            {
               connection_ = iter;
               session::start(socket_ptr, false);
            }
            else std::cout << "Can't connect to server!" << std::endl;
         }
   );
};

int sender::put(unsigned char * data_ptr, unsigned size)
{
   if (transfer_state_ == CONTROL_RESPONSE)
   {
      boost::cmatch parameters_match;

      const char * data_beg_ptr = (const char *) data_ptr;
      const char * data_end_ptr = (const char *) data_ptr + size;

      bool regex_result = boost::regex_search(data_beg_ptr,
                                              data_end_ptr,
                                              parameters_match,
                                              boost::regex(RESPONSE_MESSAGE_REGEX));

      if (!regex_result || (parameters_match.size() == RESPONSE_MESSAGE_PARAMETERS))
      {
         std::cout << "Receive invalid data from server!" << std::endl;
         return -1;
      }

      if (!resolve(connection_->host_name(), parameters_match[1])) return -1;

      transfer_state_ = DATA;
      connect();
      return 0;
   }
   return (transfer_state_ == INACTIVE) ? 0 : size;
};

int sender::get(unsigned char * data_ptr, unsigned size)
{
   switch (transfer_state_)
   {
      case CONTROL_REQUEST:
      {
         struct stat64 file_status;

         if (stat64(src_file_.c_str(), &file_status) < 0)
         {
            std::cout << "Can't get statistics for file: " << src_file_ << std::endl;
            return -1;
         }

         auto message = (boost::format(REQUEST_MESSAGE_FORMAT) % dst_file_ % file_status.st_size).str();
         auto message_size = message.size();

         memset(data_ptr, 0, size);
         memcpy(data_ptr, message.data(), message_size);

         transfer_state_ = CONTROL_RESPONSE;
         return message_size;
      }
      break;

      case DATA:
      {
         if (!file_stream_.eof())
         {
            static int counter = 0;

            counter += size;
            printf("%d/r", counter);
            fflush(stdout);

            return file_stream_.readsome((char *) data_ptr, size);
         }
         else transfer_state_ = INACTIVE;
      }
      break;

      default: break;
   }
   return 0;
};
