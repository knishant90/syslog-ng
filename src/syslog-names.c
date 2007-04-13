#include "syslog-names.h"
#include "syslog-ng.h"
#include <string.h>

struct sl_name sl_levels[] = {
  {"alert", LOG_ALERT},
  {"crit", LOG_CRIT},
  {"debug", LOG_DEBUG},
  {"emerg", LOG_EMERG},
  {"err", LOG_ERR},
  {"error", LOG_ERR},		/* DEPRECATED */
  {"info", LOG_INFO},
  {"notice", LOG_NOTICE},
  {"panic", LOG_EMERG},		/* DEPRECATED */
  {"warning", LOG_WARNING},
  {"warn", LOG_WARNING},	/* DEPRECATED */
  {NULL, -1}
};

struct sl_name sl_facilities[] = {
  {"auth", LOG_AUTH},
#ifdef LOG_AUTHPRIV
  {"authpriv", LOG_AUTHPRIV},
#endif
  {"cron", LOG_CRON},
  {"daemon", LOG_DAEMON},
#ifdef LOG_FTP
  {"ftp", LOG_FTP},
#endif
  {"kern", LOG_KERN},
  {"lpr", LOG_LPR},
  {"mail", LOG_MAIL},
  {"news", LOG_NEWS},
  {"security", LOG_AUTH},	/* DEPRECATED */
  {"syslog", LOG_SYSLOG},
  {"user", LOG_USER},
  {"uucp", LOG_UUCP},
  {"local0", LOG_LOCAL0},
  {"local1", LOG_LOCAL1},
  {"local2", LOG_LOCAL2},
  {"local3", LOG_LOCAL3},
  {"local4", LOG_LOCAL4},
  {"local5", LOG_LOCAL5},
  {"local6", LOG_LOCAL6},
  {"local7", LOG_LOCAL7},
  {NULL, -1}
};

int
syslog_lookup_name(const char *name, struct sl_name names[])
{
  int i;

  for (i = 0; names[i].name; i++)
    {
      if (strcasecmp(name, names[i].name) == 0)
	{
	  return i;
	}
    }
  return -1;
}

char *
syslog_lookup_value(int value, struct sl_name names[])
{
  int i;

  for (i = 0; names[i].name; i++)
    {
      if (names[i].value == value)
	{
	  return names[i].name;
	}
    }
  return NULL;
}

guint32
syslog_make_range(guint32 r1, guint32 r2)
{
  guint32 x;

  if (r1 > r2)
    {
      x = r2;
      r2 = r1;
      r1 = x;
    }
  return ((1 << (r2 + 1)) - 1) & ~((1 << r1) - 1);
}
