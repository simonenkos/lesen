/*
 * acceptor.hpp
 *
 *  Created on: Mar 7, 2015
 *      Author: simonenkos
 */

#ifndef __LESEN__COMMON__ACCEPTOR__
#define __LESEN__COMMON__ACCEPTOR__

#include <common/types.hpp>

/**
 * This class keeps boost's acceptor for holding new connections. For each
 * new connection, method 'on_accept' will be called and a socket, that
 * points to remote host, will be passed as an argument at this call.
 */
class acceptor
{
   lesen::acceptor_ptr acceptor_ptr_;

public:
   acceptor(lesen::io_service_ptr io_service_ptr,
            const boost::asio::ip::tcp::endpoint & endpoint);
   virtual ~acceptor();

   acceptor(const acceptor &  other) = delete;
   acceptor(const acceptor && other) = delete;

   acceptor & operator=(const acceptor &  other) = delete;
   acceptor & operator=(const acceptor && other) = delete;

   void accept(bool only_once = false);

protected:

   /**
    * Method should return 'true' if new connection was accepted.
    * Otherwise it should return 'false'.
    */
   virtual bool on_accept(lesen::socket_ptr socket_ptr) = 0;
};

#endif /* __LESEN__COMMON__ACCEPTOR__ */
