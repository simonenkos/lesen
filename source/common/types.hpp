/*
 * types.hpp
 *
 *  Created on: Mar 6, 2015
 *      Author: simonenkos
 */

#ifndef __LESEN__COMMON__TYPES__
#define __LESEN__COMMON__TYPES__

#include <boost/asio.hpp>

namespace lesen
{

// Shortcuts for frequently used types.

typedef std::shared_ptr<boost::asio::io_service>        io_service_ptr;
typedef std::shared_ptr<boost::asio::deadline_timer>    timer_ptr;
typedef std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_ptr;
typedef std::shared_ptr<boost::asio::ip::tcp::resolver> resolver_ptr;
typedef std::shared_ptr<boost::asio::ip::tcp::socket>   socket_ptr;

typedef boost::asio::ip::tcp::resolver::iterator        resolver_it;

} /* namespace lesen */

#endif /* __LESEN__COMMON__TYPES__ */
