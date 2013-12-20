#pragma once

#include "typedefs.h"
#include "str.h"
#include "error.h"

using str     = strref;
using bytebuf = buf;
using strbuf  = buf;
using byteref = bufref;

extern char __thread thread_local_buf[1024*8];