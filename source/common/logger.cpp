/*
 * logger.cpp
 *
 *  Created on: Mar 6, 2015
 *      Author: simonenkos
 */

#include <common/logger.hpp>

std::shared_ptr<std::ofstream> logger::log_ptr_ = 0;
