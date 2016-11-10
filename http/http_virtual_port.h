#ifndef _HTTP_VIRTUAL_PORT_H_
#define _HTTP_VIRTUAL_PORT_H_
#include "buffer.h"
#include "base.h"
#include "http_mod_connect.h"
#include "http_request.h"

void virtual_port_match(http_conf *g, http_connect_t *con);

#endif
