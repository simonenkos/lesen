/*
 * flow_handler.hpp
 *
 *  Created on: Mar 8, 2015
 *      Author: simonenkos
 */

#ifndef __LESEN__SERVER__FLOW_HANDLER__
#define __LESEN__SERVER__FLOW_HANDLER__

#include <queue>

#include <common/session.hpp>
#include <server/manager_interface.hpp>

/**
 * This class receive requests from clients for starting new procedure
 * of data transferring.
 */
class flow_handler : protected session
{
   manager_interface_ptr manager_interface_ptr_;
   std::queue<std::string> output_message_queue_;

public:
   flow_handler(manager_interface_ptr interface_ptr,
                lesen::socket_ptr socket_ptr);

   flow_handler(const flow_handler &  other) = delete;
   flow_handler(const flow_handler && other) = delete;

   flow_handler & operator=(const flow_handler &  other) = delete;
   flow_handler & operator=(const flow_handler && other) = delete;

private:
   int put(unsigned char * data_ptr, unsigned size);
   int get(unsigned char * data_ptr, unsigned size);
};

typedef std::shared_ptr<flow_handler> flow_handler_ptr;

#endif /* __LESEN__SERVER__FLOW_HANDLER__ */
