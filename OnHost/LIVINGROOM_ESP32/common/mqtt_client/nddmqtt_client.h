#ifndef COMMON_NDD_MQTT_CLIENT_H__
#define COMMON_NDD_MQTT_CLIENT_H__

typedef void (*mqtt_handle_t)(char *topic, int topic_len, char *data, int len);

void mqtt_init(const char *topics[], int topic_count);
void mqtt_start(void);
void mqtt_set_callback(void *cb);
void mqtt_pub(const char *topic, const char *data, int len);

#endif // COMMON_NDD_MQTT_CLIENT_H__
