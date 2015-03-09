/*
 * manager_interface.hpp
 *
 *  Created on: Mar 8, 2015
 *      Author: simonenkos
 */

#ifndef __LESEN__SERVER__MANAGER_INTERFACE__
#define __LESEN__SERVER__MANAGER_INTERFACE__

#include <string>
#include <memory>

struct manager_interface
{
   virtual ~manager_interface()
   { };

   /**
    * Method should starts new data transferring process to receive
    * contents for a file.
    */
   virtual int new_data_flow(const std::string & file, unsigned long long size) = 0;
};

typedef std::shared_ptr<manager_interface> manager_interface_ptr;

#endif /* __LESEN__SERVER__MANAGER_INTERFACE__ */
