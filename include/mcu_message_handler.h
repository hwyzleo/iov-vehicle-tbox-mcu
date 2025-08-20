//
// Created by hwyz_leo on 2025/8/11.
//

#ifndef MCUSERVICE_MCU_MESSAGE_HANDLER_H
#define MCUSERVICE_MCU_MESSAGE_HANDLER_H
#include <memory>

#include "mcu_message.pb.h"

class McuMessageHandler {
public:
    /**
     * 处理MCU消息
     * @param message MCU消息
     */
    virtual void handle(const std::shared_ptr<tbox::mcu::v1::McuMessage> *message) = 0;

    virtual ~McuMessageHandler() = default;
};

#endif //MCUSERVICE_MCU_MESSAGE_HANDLER_H
