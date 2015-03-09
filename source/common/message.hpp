/*
 * message.hpp
 *
 *  Created on: Mar 8, 2015
 *      Author: simonenkos
 */

#ifndef __LESEN__COMMON__MESSAGE__
#define __LESEN__COMMON__MESSAGE__

// A list of templates for different requests, responses or errors messages.

extern const char *   REQUEST_MESSAGE_REGEX;
extern const char *   REQUEST_MESSAGE_FORMAT;
extern const unsigned REQUEST_MESSAGE_PARAMETERS;

extern const char *   RESPONSE_MESSAGE_REGEX;
extern const char *   RESPONSE_MESSAGE_FORMAT;
extern const unsigned RESPONSE_MESSAGE_PARAMETERS;

extern const char *   ERROR_MESSAGE_FORMAT;
extern const char *   ERROR_BUSY;

#endif /* __LESEN__COMMON__MESSAGE__ */
