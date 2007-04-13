#include "afinet.h"
#include "messages.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

static void
afinet_set_port(GSockAddr *addr, gint port, gchar *service, gchar *proto)
{
  if (proto)
    {
      struct servent *se;
      
      se = getservbyname(service, proto);
      if (se)
        {
          port = ntohs(se->s_port);
        }
      else
        {
          msg_error("Error finding port number, falling back to default",
                    evt_tag_printf("service", "%s/%s", proto, service),
                    NULL);
          return;
        }
    }
    
  ((struct sockaddr_in *) &addr->sa)->sin_port = htons(port);
  
}

static void
afinet_set_ip(GSockAddr *addr, gchar *ip)
{
  if (!inet_aton(ip, &((struct sockaddr_in *) &addr->sa)->sin_addr))
    { 
      struct hostent *he;
      
      he = gethostbyname(ip);
      if (he)
        {
          ((struct sockaddr_in *) &addr->sa)->sin_addr = *(struct in_addr *) he->h_addr;
        }
    }
}

void 
afinet_sd_set_localport(LogDriver *s, gint port, gchar *service, gchar *proto)
{
  AFSocketSourceDriver *self = (AFSocketSourceDriver *) s;
  
  afinet_set_port(self->bind_addr, port, service, proto);
}

void 
afinet_sd_set_localip(LogDriver *s, gchar *ip)
{
  AFSocketSourceDriver *self = (AFSocketSourceDriver *) s;
  
  afinet_set_ip(self->bind_addr, ip);
}

LogDriver *
afinet_sd_new(gchar *ip, gint port, guint flags)
{
  AFInetSourceDriver *self = g_new0(AFInetSourceDriver, 1);
  
  afsocket_sd_init_instance(&self->super, flags);
  self->super.flags |= AFSOCKET_KEEP_ALIVE | AFSOCKET_LISTENER_KEEP_ALIVE;
  self->super.bind_addr = g_sockaddr_inet_new(ip, port);
  if (flags & AFSOCKET_DGRAM)
    self->super.flags |= AFSOCKET_PROTO_RFC3164;
  return &self->super.super;
}

/* afinet destination */

void 
afinet_dd_set_localport(LogDriver *s, gint port, gchar *service, gchar *proto)
{
  AFInetDestDriver *self = (AFInetDestDriver *) s;
  
  afinet_set_port(self->super.bind_addr, port, service, proto);
}

void 
afinet_dd_set_destport(LogDriver *s, gint port, gchar *service, gchar *proto)
{
  AFInetDestDriver *self = (AFInetDestDriver *) s;
  
  afinet_set_port(self->super.dest_addr, port, service, proto);
}

void 
afinet_dd_set_localip(LogDriver *s, gchar *ip)
{
  AFInetDestDriver *self = (AFInetDestDriver *) s;
  
  afinet_set_ip(self->super.bind_addr, ip);
}

void 
afinet_dd_set_sync_freq(LogDriver *self, gint sync_freq)
{
}

LogDriver *
afinet_dd_new(gchar *ip, gint port, guint flags)
{
  AFInetDestDriver *self = g_new0(AFInetDestDriver, 1);
  
  afsocket_dd_init_instance(&self->super, flags);
  self->super.bind_addr = g_sockaddr_inet_new("0.0.0.0", 0);
  self->super.dest_addr = g_sockaddr_inet_new(ip, port);
  if (flags & AFSOCKET_DGRAM)
    self->super.flags |= AFSOCKET_PROTO_RFC3164;
  return &self->super.super;
}
