#ifndef _SPEED_CORE_H_
#define _SPEED_CORE_H_

#include <ruby.h>
#include <vector>
#include "pagespeed/rules/rule_provider.h"
#include "pagespeed/core/serializer.h"
#include "pagespeed/core/formatter.h"
#include "pagespeed/core/rule.h"
#include "pagespeed/core/dom.h"
#include "pagespeed/core/result_provider.h"
#include "pagespeed/core/pagespeed_input.h"
#include "pagespeed/core/resource.h"
#include "pagespeed/core/engine.h"
#include "pagespeed/formatters/formatter_util.h"
#include "pagespeed/formatters/text_formatter.h"

#ifdef __cplusplus
#define exportable extern "C"
#else
#define exportable
#endif

#define RBCALL(f) ((VALUE (*)(...))f)

#ifndef RSTRING_LEN
  #define RSTRING_LEN(x) RSTRING(x)->len
#endif

#ifndef RSTRING_PTR
  #define RSTRING_PTR(x) RSTRING(x)->ptr
#endif

#ifndef SP_STR
  #define SP_STR(s) std::string(RSTRING_PTR(s),RSTRING_LEN(s))
#endif

#endif
