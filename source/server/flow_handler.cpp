/*
 * flow_handler.cpp
 *
 *  Created on: Mar 8, 2015
 *      Author: simonenkos
 */

#include <boost/regex.hpp>
#include <boost/format.hpp>

#include <common/logger.hpp>
#include <common/message.hpp>
#include <server/flow_handler.hpp>

flow_handler::flow_handler(manager_interface_ptr interface_ptr,
                           lesen::socket_ptr socket_ptr) : session(socket_ptr)
                                                         , manager_interface_ptr_(interface_ptr)
{
   assert(manager_interface_ptr_);
   start();
};

int flow_handler::put(unsigned char * data_ptr, unsigned size)
{
   boost::cmatch parameters_match;

   const char * data_beg_ptr = (const char *) data_ptr;
   const char * data_end_ptr = (const char *) data_ptr + size;

   if (!boost::regex_search(data_beg_ptr, data_end_ptr, parameters_match, boost::regex(REQUEST_MESSAGE_REGEX)))
   {
      LOG("Receive invalid data: " << std::string((char *) data_ptr, size));
   }
   else if ((parameters_match.size() - 1) == REQUEST_MESSAGE_PARAMETERS)
   {
      auto file = parameters_match[1];
      auto size = std::stoll(parameters_match[2]);
      auto port = manager_interface_ptr_->new_data_flow(file, size);
      auto answer = (port > 0) ? (boost::format(RESPONSE_MESSAGE_FORMAT) % port).str()
                               : (boost::format(ERROR_MESSAGE_FORMAT) % ERROR_BUSY).str();
      output_message_queue_.push(answer);

      if (port > 0) return size;
   }
   return 0;
};

int flow_handler::get(unsigned char * data_ptr, unsigned size)
{
   if (!output_message_queue_.empty())
   {
      auto answer = output_message_queue_.front();
      auto answer_size = answer.size();

      if (answer_size > size)
      {
         LOG("Not enough buffer space to process a response to a client!");
         return -1;
      }

      memset(data_ptr, 0, size);
      memcpy(data_ptr, answer.data(), answer_size);
      return answer_size;
   }
   return 0;
};
