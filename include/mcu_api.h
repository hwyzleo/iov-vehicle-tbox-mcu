//
// Created by hwyz_leo on 2025/8/7.
//

#ifndef MCUSERVICE_MCU_API_H
#define MCUSERVICE_MCU_API_H

#include <string>
#include <optional>

#include "mcu_message.pb.h"

// MCU消息类型枚举
enum mcu_message_type_t {
    MCU_MESSAGE_TYPE_RSMS = 1,
};

/**
 * 初始化MCU通信模块
 * @return 是否成功
 */
bool init_mcu_communication_module();

/**
 * 获取车架号
 * @param vin 车架号
 * @return 是否成功
 */
bool get_vin(std::string &vin);

/**
 * 获取TBox序列号
 * @param tbox_sn TBox序列号
 * @return 是否成功
 */
bool get_tbox_sn(std::string &tbox_sn);

/**
 * 获取MCU版本
 * @param version MCU版本
 * @return 是否成功
 */
bool get_mcu_version(std::string &version);

/**
 * 接收MCU消息回调
 * @param obj 自定义对象
 * @param message MCU消息
 * @return 是否成功
 */
using receive_mcu_message_callback_t = bool (*)(void *obj, std::shared_ptr<tbox::mcu::v1::McuMessage> *message);

/**
 * 注册接收MCU消息回调
 * @param obj 自定义对象
 * @param callback 回调函数
 * @return 是否成功
 */
bool register_mcu_message_callback(void *obj, receive_mcu_message_callback_t callback);

/**
 * 发送MCU消息
 * @param message MCU消息
 * @return 是否成功
 */
bool send_mcu_message(std::shared_ptr<tbox::mcu::v1::McuMessage> *message);

#endif //MCUSERVICE_MCU_API_H
