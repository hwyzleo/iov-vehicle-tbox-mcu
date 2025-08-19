//
// Created by hwyz_leo on 2025/8/7.
//
#include <cstddef>

#include "utils.h"
#include "spdlog/spdlog.h"

#include "mcu_client.h"
#include "mcu_api.h"
#include "mcu_rsms_handler.h"

McuClient::~McuClient() = default;

McuClient::McuClient() = default;

McuClient &McuClient::get_instance() {
    static McuClient instance;
    return instance;
}

static bool receive_mcu_message_callback(void *obj, std::shared_ptr<tbox::mcu::v1::McuMessage> *message) {
    McuClient::get_instance().on_receive_mcu_message(obj, message);
    return true;
}

bool McuClient::init() {
    spdlog::info("MCU客户端初始化");
    if (!register_mcu_message_callback(NULL, receive_mcu_message_callback)) {
        spdlog::error("注册MCU消息失败");
        return false;
    }
    registerHandler();
    std::string new_vin;
    if (!get_vin(new_vin)) {
        spdlog::error("获取车架号失败");
        return false;
    }
    std::string old_vin = hwyz::Utils::global_read_string(hwyz::global_key_t::VIN);
    if (old_vin.empty() || old_vin != new_vin) {
        if (!old_vin.empty() && old_vin != new_vin) {
            spdlog::warn("车架号发生变更[{}->{}]", old_vin, new_vin);
        }
        hwyz::Utils::global_write_string(hwyz::global_key_t::VIN, new_vin);
    }
    std::string new_iccid;
    if (!get_current_iccid(new_iccid)) {
        spdlog::error("获取当前ICCID失败");
        return false;
    }
    std::string old_iccid = hwyz::Utils::global_read_string(hwyz::global_key_t::CURRENT_ICCID);
    if (old_iccid.empty() || old_iccid != new_iccid) {
        hwyz::Utils::global_write_string(hwyz::global_key_t::CURRENT_ICCID, new_iccid);
    }
    std::string new_battery_pack_sn;
    if (!get_battery_pack_sn(new_battery_pack_sn)) {
        spdlog::error("获取电池包序列号失败");
        return false;
    }
    std::string old_battery_pack_sn = hwyz::Utils::global_read_string(hwyz::global_key_t::BATTERY_PACK_SN);
    if (old_battery_pack_sn.empty() || old_battery_pack_sn != new_battery_pack_sn) {
        if (!old_battery_pack_sn.empty() && old_battery_pack_sn != new_battery_pack_sn) {
            spdlog::warn("电池包序列号发生变更[{}->{}]", old_battery_pack_sn, new_battery_pack_sn);
        }
        hwyz::Utils::global_write_string(hwyz::global_key_t::BATTERY_PACK_SN, new_battery_pack_sn);
    }
    std::string new_tbox_sn;
    if (!get_tbox_sn(new_tbox_sn)) {
        spdlog::error("获取TBox序列号失败");
        return false;
    }
    std::string old_tbox_sn = hwyz::Utils::global_read_string(hwyz::global_key_t::TBOX_SN);
    if (old_tbox_sn.empty() || old_tbox_sn != new_tbox_sn) {
        if (!old_tbox_sn.empty() && old_tbox_sn != new_tbox_sn) {
            spdlog::warn("TBox序列号发生变更[{}->{}]", old_tbox_sn, new_tbox_sn);
        }
        hwyz::Utils::global_write_string(hwyz::global_key_t::TBOX_SN, new_tbox_sn);
    }
    std::string new_tbox_mcu_version;
    if (!get_tbox_mcu_version(new_tbox_mcu_version)) {
        spdlog::error("获取TBox MCU版本失败");
        return false;
    }
    std::string old_tbox_mcu_version = hwyz::Utils::global_read_string(hwyz::global_key_t::TBOX_MCU_VERSION);
    if (old_tbox_mcu_version.empty() || old_tbox_mcu_version != new_tbox_mcu_version) {
        hwyz::Utils::global_write_string(hwyz::global_key_t::TBOX_MCU_VERSION, new_tbox_mcu_version);
    }
    return true;
}

void McuClient::registerHandler() {
    message_handler_[1] = &McuRsmsHandler::get_instance();
}

bool McuClient::start() {
    spdlog::info("MCU客户端启动");
    if (!init()) {
        spdlog::error("MCU客户端初始化失败");
        return false;
    }
    return true;
}

void McuClient::stop() {

}

void McuClient::on_receive_mcu_message(void *obj, std::shared_ptr<tbox::mcu::v1::McuMessage> *message) {
    spdlog::debug("收到MCU消息[类型:{}]", (*message)->message_type());
    message_handler_[(*message)->message_type()]->handle(message);
}



