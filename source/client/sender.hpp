/*
 * sender.hpp
 *
 *  Created on: Mar 8, 2015
 *      Author: simonenkos
 */

#ifndef __LESEN__CLIENT__SENDER__
#define __LESEN__CLIENT__SENDER__

#include <fstream>

#include <common/session.hpp>

/**
 * Class implements client's logic for transferring a file to
 * a remote host.
 */
class sender : protected session
{
   std::string src_file_;
   std::string dst_file_;

   std::ifstream file_stream_;

   lesen::resolver_ptr resolver_ptr_;
   lesen::resolver_it  connection_;

   enum
   {
      INACTIVE,
      CONTROL_REQUEST,
      CONTROL_RESPONSE,
      DATA,
   }
   transfer_state_;

public:
   sender(const std::string src_file,
          const std::string dst_file);
   ~sender();

   sender(const sender &  other) = delete;
   sender(const sender && other) = delete;

   sender & operator=(const sender &  other) = delete;
   sender & operator=(const sender && other) = delete;

   bool start(lesen::io_service_ptr io_service_ptr,
              const std::string & host,
              const std::string & port);

private:
   bool resolve(const std::string & host,
                const std::string & port);
   void connect();

   int put(unsigned char * data_ptr, unsigned size);
   int get(unsigned char * data_ptr, unsigned size);
};

#endif /* __LESEN__CLIENT__SENDER__ */
