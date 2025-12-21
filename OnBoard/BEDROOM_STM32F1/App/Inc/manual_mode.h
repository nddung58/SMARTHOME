#ifndef __MANUAL_H__
#define __MANUAL_H__

/**
 * @brief Xử lý các gói tin từ UART queue khi đang ở chế độ Manual.
 *
 * Hàm này nên được gọi trong vòng lặp chính.
 * Nó sẽ kiểm tra chế độ hiện tại và chỉ xử lý nếu đang ở Manual.
 */
void Manual_Process(void);

#endif /* __MANUAL_H__ */
