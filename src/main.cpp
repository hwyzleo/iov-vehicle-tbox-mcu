//
// Created by hwyz_leo on 2025/8/7.
//
#include <iostream>

#include "application.h"
#include "spdlog/spdlog.h"

#include "mcu_api.h"
#include "mqtt_client.h"
#include "mcu_client.h"

class MainApplication : public hwyz::Application {
protected:
    bool initialize() override {
        if (!init_mcu_communication_module()) {
            spdlog::error("初始化MCU通信模块失败");
            return false;
        }
        if (!MqttClient::get_instance().load_config(getConfig())) {
            spdlog::error("加载MQTT客户端配置信息失败");
            return false;
        }
        return true;
    }

    void cleanup() override {
        McuClient::get_instance().stop();
        MqttClient::get_instance().stop();
    }

    int execute() override {
        McuClient::get_instance().start();
        MqttClient::get_instance().start();
        spdlog::info("主函数运行");
        return 0;
    }
};

APPLICATION_ENTRY(MainApplication)