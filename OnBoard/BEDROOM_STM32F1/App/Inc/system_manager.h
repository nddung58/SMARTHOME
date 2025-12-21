#ifndef __SYSTEM_MANAGER_H
#define __SYSTEM_MANAGER_H

#include <stdint.h>

/**
 * Khởi tạo các thiết bị và cảm biến.
 * Bao gồm: DHT11, LDR, MQ2, LED RGB, Motor, Siren,...
 */
void device_init(void);

/**
 * Cập nhật dữ liệu từ các cảm biến và thiết bị.
 * Gửi lên hệ thống nếu có thay đổi.
 */
void DeviceManager_UpdateData(void);

#endif // system_manager.h
