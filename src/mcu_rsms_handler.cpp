//
// Created by hwyz_leo on 2025/8/13.
//
#include "spdlog/spdlog.h"

#include "mcu_rsms_handler.h"
#include "rsms_data_v1.pb.h"
#include "mqtt_client.h"

McuRsmsHandler::McuRsmsHandler() {
}

McuRsmsHandler::~McuRsmsHandler() {

}

McuRsmsHandler &McuRsmsHandler::get_instance() {
    static McuRsmsHandler instance;
    return instance;
}

void McuRsmsHandler::handle(const std::shared_ptr<tbox::mcu::v1::McuMessage> *message) {
    if (!message || !(*message)) {
        spdlog::error("MCU消息异常");
        return;
    }

    auto msg = *message;

    // 检查是否有数据
    if (msg->message_data().empty()) {
        spdlog::error("MCU消息为空");
        return;
    }

    // 反序列化
    tbox::mcu::rsms::v1::RsmsData rsms_data;
    if (!rsms_data.ParseFromString(msg->message_data())) {
        spdlog::error("解析国标数据失败");
        return;
    }

    int mid = 0;
    std::string serialized_data = rsms_data.SerializeAsString();
    MqttClient::get_instance().publish(mid,
                                       "RSMS/MCU_DATA",
                                       serialized_data.data(),
                                       static_cast<int>(serialized_data.size()),
                                       1);
}