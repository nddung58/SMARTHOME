#ifndef NDD_STD_TYPES_H_
#define NDD_STD_TYPES_H_

#include "string.h"
#include "stdio.h"

typedef enum
{
    ON_HOST_LIVING_ROOM,
    ON_BOARD_BED_ROOM,
    NODE_MAX
} nodes_t;

typedef enum
{
    LDR = 0x01,
    PIR = 0x02,
    MQ2 = 0x03,
    HUMI = 0x04,
    TEMP = 0x05,
    RAIN = 0x06,
    LED = 0x11,
    FAN = 0x12,
    BUZZER = 0x13,
    AWNINGS = 0x14,
    DOOR = 0x15,
    MODE = 0x20,
    UNKNOWN = 0xFF,
    DEVICE_MAX = 0xFF
} id_end_device_t;

typedef enum
{
    AUTO_MODE = 0,
    MANUAL_MODE = 1
} app_mode_t;

static const char *node_name[] = {
    [ON_HOST_LIVING_ROOM] = "living",
    [ON_BOARD_BED_ROOM] = "bed",
};

static const char *device_name[] = {
    [LDR] = "ldr",
    [PIR] = "pir",
    [MQ2] = "mq2",
    [HUMI] = "humi",
    [TEMP] = "temp",
    [RAIN] = "rain",
    [LED] = "led",
    [FAN] = "fan",
    [BUZZER] = "buzzer",
    [AWNINGS] = "awnings",
    [DOOR] = "door",
    [MODE] = "mode",
    [UNKNOWN] = "unknown",
};

static inline char *get_key_topic(nodes_t node, id_end_device_t dev)
{
    if (node >= NODE_MAX || dev >= DEVICE_MAX)
        return NULL;

    const char *n = node_name[node];
    const char *d = (dev < DEVICE_MAX) ? device_name[dev] : NULL;

    if (!n || !d)
        return NULL;

    static _Thread_local char key[32];
    snprintf(key, sizeof(key), "%s_%s", n, d);
    return key;
}

typedef enum
{
    TOPIC_UP,
    TOPIC_CMD,
    TOPIC_INVALID
} topic_type_t;

typedef struct
{
    nodes_t node;
    id_end_device_t dev;
    topic_type_t type;
} topic_info_t;

static inline nodes_t get_node_from_name(const char *name)
{
    if (!name)
        return NODE_MAX;
    for (int i = 0; i < NODE_MAX; i++)
    {
        if (node_name[i] && strcmp(name, node_name[i]) == 0)
            return (nodes_t)i;
    }
    return NODE_MAX;
}

static inline id_end_device_t get_device_from_name(const char *name)
{
    if (!name)
        return UNKNOWN;
    for (int i = 0; i < (int)DEVICE_MAX; i++)
    {
        if (device_name[i] != NULL && strcmp(name, device_name[i]) == 0)
            return (id_end_device_t)i;
    }
    return UNKNOWN;
}

static inline topic_info_t parse_topic(const char *topic)
{
    topic_info_t info = {.node = NODE_MAX, .dev = UNKNOWN, .type = TOPIC_INVALID};
    if (!topic)
        return info;

    char buf[64] = {0};
    strncpy(buf, topic, sizeof(buf) - 1);

    char *node_str = strtok(buf, "_");
    char *dev_str = strtok(NULL, "_");
    char *cmd_str = strtok(NULL, "_");

    if (node_str && dev_str)
    {
        info.node = get_node_from_name(node_str);
        info.dev = get_device_from_name(dev_str);
        info.type = (cmd_str && strcmp(cmd_str, "cmd") == 0) ? TOPIC_CMD : TOPIC_UP;
    }

    return info;
}

#define TOPIC_CMD(node, dev) node "_" dev "_cmd"

#endif