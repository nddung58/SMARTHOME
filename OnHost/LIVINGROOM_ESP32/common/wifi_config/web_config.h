#ifndef __WEB_CONFIG_H
#define __WEB_CONFIG_H

#include <stdint.h>

typedef void (*http_post_handle_t) (char *data, int len);

void start_webserver(void);
void stop_webserver(void);
void http_post_set_callback(void *cb);


#endif // __WEB_CONFIG_H