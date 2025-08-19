//
// Created by hwyz_leo on 2025/8/11.
//

#ifndef MCUSERVICE_MCU_RSMS_HANDLER_H
#define MCUSERVICE_MCU_RSMS_HANDLER_H

#include "mcu_message_handler.h"

/**
 * MCU国标消息处理
 */
class McuRsmsHandler : public McuMessageHandler {
public:
    /**
     * 析构虚函数
     */
    ~McuRsmsHandler() override;

    /**
     * 防止对象被复制
     */
    McuRsmsHandler(const McuRsmsHandler &) = delete;

    /**
     * 防止对象被赋值
     * @return
     */
    McuRsmsHandler &operator=(const McuRsmsHandler &) = delete;

    /**
     * 获取单例
     * @return 单例
     */
    static McuRsmsHandler &get_instance();

public:
    /**
     * 处理MCU消息
     * @param message MCU消息
     */
    void handle(const std::shared_ptr<tbox::mcu::v1::McuMessage> *message) override;

private:
    McuRsmsHandler();
};

#endif //MCUSERVICE_MCU_RSMS_HANDLER_H
