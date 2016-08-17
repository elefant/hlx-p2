#include "common.h"
#include <string.h>
#include <vector>
#include <string>

void log(const char* aModule,
         const char* aFormat,
         ...)
{
  va_list ap;
  va_start( ap, aFormat );
  va_end( ap );

  std::string msg;
  std::vector<char> vbuf( 1024 );
  while( true ) {
    int needed = vsnprintf( &vbuf[0], vbuf.size(), aFormat, ap );
    if (needed <= (int)vbuf.size() && needed >= 0) {
      msg = std::string( &vbuf[0], (size_t)needed );
      break;
    }
    vbuf.resize( vbuf.size() * 2 ); // weird
  }

  printf("[%s] %s", aModule, msg.c_str());
}

std::string vecToString(const std::vector<int>& aVector)
{
  std::string s;
  for (auto v : aVector) {
    char buf[100];
    snprintf(buf, sizeof(buf), "%d", v);
    s += buf;
    s += " ";
  }
  return s;
}