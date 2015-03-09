/*
 * session.hpp
 *
 *  Created on: Mar 4, 2015
 *      Author: simonenkos
 */

#ifndef __LESEN__COMMON__SESSION__
#define __LESEN__COMMON__SESSION__

#include <common/acceptor.hpp>

/**
 * This class holds a socket and provides simple interface to transfer data to a some
 * remote host. When data is received, a 'put' method will be called. When the socket
 * is ready to send data, data will be requested from user through a 'get' method.
 */
class session
{
   lesen::socket_ptr socket_ptr_;
   lesen::timer_ptr  read_timer_ptr_;

   unsigned char * tx_buffer;
   unsigned char * rx_buffer;

   unsigned read_timeout_;
   bool is_stopped_;

public:
   session(lesen::socket_ptr socket_ptr,
           unsigned read_timeout);
   virtual ~session();

   session(const session &  other) = delete;
   session(const session && other) = delete;

   session& operator=(const session &  other) = delete;
   session& operator=(const session && other) = delete;

   void start(bool rx_first = true);
   void start(lesen::socket_ptr socket_ptr, bool rx_first = true);
   void stop();

protected:

   /**
    * Method should return value less than zero if an error has occurred during data
    * processing. Otherwise, it should return the amount of data, that was
    * successfully processed.
    */
   virtual int put(unsigned char * data_ptr, unsigned size) = 0;

   /**
    * Method should return amount of data that has been stored into a buffer. Otherwise,
    * the method should return value less or equal to zero.
    */
   virtual int get(unsigned char * data_ptr, unsigned size) = 0;

private:
   void do_read();
   void do_write();
   void read_expiry();
};

#endif /* __LESEN__COMMON__SESSION__ */
