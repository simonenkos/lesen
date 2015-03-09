/*
 * message.cpp
 *
 *  Created on: Mar 8, 2015
 *      Author: simonenkos
 */

#include <common/message.hpp>

const char *   REQUEST_MESSAGE_REGEX      = "file=(\\/.*)&size=(\\d+)";
const char *   REQUEST_MESSAGE_FORMAT     = "file=%s&size=%s";
const unsigned REQUEST_MESSAGE_PARAMETERS = 2;

const char *   RESPONSE_MESSAGE_REGEX      = "port=(\\d+)";
const char *   RESPONSE_MESSAGE_FORMAT     = "port=%d";
const unsigned RESPONSE_MESSAGE_PARAMETERS = 1;

const char *   ERROR_MESSAGE_FORMAT = "error=\"%s\"";
const char *   ERROR_BUSY           = "busy";
