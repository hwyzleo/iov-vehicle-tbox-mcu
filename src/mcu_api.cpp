//
// Created by hwyz_leo on 2025/8/18.
//
#include <thread>
#include <chrono>
#include <atomic>
#include <random>

#include "spdlog/spdlog.h"
#include "mcu_message.pb.h"
#include "rsms_data_v1.pb.h"

#include "mcu_api.h"

struct McuCallbackEntry {
    void *obj;
    receive_mcu_message_callback_t callback;
};

static std::vector<McuCallbackEntry> registered_callbacks;
static std::mutex callback_mutex;
static std::atomic<bool> timer_running{false};
static std::thread timer_thread;

// 封装国标数据
std::string package_rsms_data() {
    thread_local static std::random_device rd;
    thread_local static std::mt19937 gen(rd());
    thread_local static std::uniform_int_distribution<int> speed_dist(0, 1000);
    thread_local static std::uniform_int_distribution<int> total_voltage(3878, 3898);
    thread_local static std::uniform_int_distribution<int> total_current(950, 1000);
    thread_local static std::uniform_int_distribution<int> accelerator_pedal_position(0, 100);
    thread_local static std::uniform_int_distribution<int> brake_pedal_position(0, 100);
    thread_local static std::uniform_int_distribution<int> drive_motor_controller_temperature(80, 90);
    thread_local static std::uniform_int_distribution<int> drive_motor_temperature(110, 115);
    thread_local static std::uniform_int_distribution<int> drive_motor_speed(24000, 26000);
    thread_local static std::uniform_int_distribution<int> drive_motor_torque(19900, 20000);
    thread_local static std::uniform_int_distribution<int> drive_motor_current(19950, 20000);
    thread_local static std::uniform_int_distribution<int> longitude(103000000, 121000000);
    thread_local static std::uniform_int_distribution<int> latitude(22000000, 39000000);
    thread_local static std::uniform_int_distribution<int> cell_voltage(4040, 4066);
    thread_local static std::uniform_int_distribution<int> cell_temperature(76, 78);

    tbox::mcu::rsms::v1::RsmsData rsms_data;

    tbox::mcu::rsms::v1::VehicleData *vehicle_data = rsms_data.mutable_vehicle_data();
    int accelerator_pedal_position_value = accelerator_pedal_position(gen);
    vehicle_data->set_vehicle_state(1); // 默认启动状态才传国标
    vehicle_data->set_charging_state(3); // 默认未充电
    vehicle_data->set_running_mode(1); // 默认纯电
    vehicle_data->set_speed(speed_dist(gen)); // 0～1000随机值
    vehicle_data->set_total_odometer(100); // 先默认100
    vehicle_data->set_total_voltage(total_voltage(gen)); // 3878~3898随机值
    vehicle_data->set_total_current(total_current(gen)); // 950~1000随机值
    vehicle_data->set_soc(100); // 先默认100
    vehicle_data->set_dcdc_state(1); // 默认工作状态
    if (accelerator_pedal_position_value > 0) {
        vehicle_data->set_driving(true);
        vehicle_data->set_braking(false);
        vehicle_data->set_brake_pedal_position(0);
    } else {
        vehicle_data->set_driving(false);
        int brake_pedal_position_value = brake_pedal_position(gen);
        if (brake_pedal_position_value > 0) {
            vehicle_data->set_braking(true);
        } else {
            vehicle_data->set_braking(false);
        }
        vehicle_data->set_brake_pedal_position(brake_pedal_position_value);
    }
    vehicle_data->set_gear(14); // 默认D挡
    vehicle_data->set_insulation_resistance(4333); // 默认4333
    vehicle_data->set_accelerator_pedal_position(accelerator_pedal_position_value); // 0～100随机值

    tbox::mcu::rsms::v1::DriveMotor *drive_motor = rsms_data.mutable_drive_motor();
    drive_motor->set_drive_motor_count(2); // 默认2个电机
    auto single_drive_motor1 = drive_motor->add_drive_motor_list();
    single_drive_motor1->set_sn(1); // 第1个电机
    single_drive_motor1->set_state(1); // 默认耗电状态
    single_drive_motor1->set_controller_temperature(drive_motor_controller_temperature(gen)); // 默认电机控制器温度范围40~50随机
    single_drive_motor1->set_speed(drive_motor_speed(gen)); // 默认电机转速范围4000~6000随机
    single_drive_motor1->set_torque(drive_motor_torque(gen)); // 默认电机扭矩范围-10~0随机
    single_drive_motor1->set_temperature(drive_motor_temperature(gen)); // 默认电机温度范围70~75随机
    single_drive_motor1->set_controller_input_voltage(3890); // 默认电压389V
    single_drive_motor1->set_controller_dc_bus_current(drive_motor_current(gen)); // 默认电流-5~0A随机
    auto single_drive_motor2 = drive_motor->add_drive_motor_list();
    single_drive_motor2->set_sn(2); // 第1个电机
    single_drive_motor2->set_state(1); // 默认耗电状态
    single_drive_motor2->set_controller_temperature(drive_motor_controller_temperature(gen)); // 默认电机控制器温度范围40~50随机
    single_drive_motor2->set_speed(drive_motor_speed(gen)); // 默认电机转速范围4000~6000随机
    single_drive_motor2->set_torque(drive_motor_torque(gen)); // 默认电机扭矩范围-10~0随机
    single_drive_motor2->set_temperature(drive_motor_temperature(gen)); // 默认电机温度范围70~75随机
    single_drive_motor2->set_controller_input_voltage(3890); // 默认电压389V
    single_drive_motor2->set_controller_dc_bus_current(drive_motor_current(gen)); // 默认电流-5~0A随机

    tbox::mcu::rsms::v1::Position *position = rsms_data.mutable_position();
    position->set_position_valid(true); // 默认位置有效
    position->set_south_latitude(false); // 默认北纬
    position->set_west_longitude(false); // 默认东经
    position->set_longitude(longitude(gen)); // 默认经度随机
    position->set_latitude(latitude(gen)); // 默认纬度随机

    tbox::mcu::rsms::v1::BatteryVoltage *battery_voltage = rsms_data.mutable_battery_voltage();
    battery_voltage->set_battery_count(1);
    auto single_battery_voltage = battery_voltage->add_battery_voltage_list();
    single_battery_voltage->set_sn(1);
    single_battery_voltage->set_voltage(3893);
    single_battery_voltage->set_current(19974);
    single_battery_voltage->set_cell_count(16);
    single_battery_voltage->set_frame_start_cell_sn(1);
    single_battery_voltage->set_frame_cell_count(16);
    int max_voltage_cell_no = 0;
    int min_voltage_cell_no = 0;
    int cell_max_voltage = 0;
    int cell_min_voltage = 0;
    for (int i = 0; i < 16; i++) {
        int tmp_voltage = cell_voltage(gen);
        single_battery_voltage->add_cell_voltage_list(tmp_voltage);
        if (tmp_voltage > cell_max_voltage) {
            max_voltage_cell_no = i + 1;
            cell_max_voltage = tmp_voltage;
        }
        if (cell_min_voltage == 0 || tmp_voltage < cell_min_voltage) {
            min_voltage_cell_no = i + 1;
            cell_min_voltage = tmp_voltage;
        }
    }

    tbox::mcu::rsms::v1::BatteryTemperature *battery_temperature = rsms_data.mutable_battery_temperature();
    battery_temperature->set_battery_count(1);
    auto single_battery_temperature = battery_temperature->add_battery_temperature_list();
    single_battery_temperature->set_sn(1);
    single_battery_temperature->set_probe_count(15);
    int max_temperature_probe_no = 0;
    int min_temperature_probe_no = 0;
    int max_temperature = 0;
    int min_temperature = 0;
    for (int i = 0; i < 15; i++) {
        int tmp_temperature = cell_temperature(gen);
        single_battery_temperature->add_temperatures(tmp_temperature);
        if (tmp_temperature > max_temperature) {
            max_temperature_probe_no = i + 1;
            max_temperature = tmp_temperature;
        }
        if (min_temperature == 0 || tmp_temperature < min_temperature) {
            min_temperature_probe_no = i + 1;
            min_temperature = tmp_temperature;
        }
    }

    tbox::mcu::rsms::v1::Extremum *extremum = rsms_data.mutable_extremum();
    extremum->set_max_voltage_battery_device_no(1);
    extremum->set_max_voltage_cell_no(max_voltage_cell_no);
    extremum->set_cell_max_voltage(cell_max_voltage);
    extremum->set_min_voltage_battery_device_no(1);
    extremum->set_min_voltage_cell_no(min_voltage_cell_no);
    extremum->set_cell_min_voltage(cell_min_voltage);
    extremum->set_max_temperature_device_no(1);
    extremum->set_max_temperature_probe_no(max_temperature_probe_no);
    extremum->set_max_temperature(max_temperature);
    extremum->set_min_temperature_device_no(1);
    extremum->set_min_temperature_probe_no(min_temperature_probe_no);
    extremum->set_min_temperature(min_temperature);

    tbox::mcu::rsms::v1::Alarm *alarm = rsms_data.mutable_alarm();
    alarm->set_max_alarm_level(0);
    alarm->set_alarm_flag(0);
    alarm->set_battery_fault_count(0);
    alarm->set_drive_motor_fault_count(0);
    alarm->set_engine_fault_count(0);
    alarm->set_other_fault_count(0);

    return rsms_data.SerializeAsString();
}

// 定频推送国标数据
void timer_rsms() {
    using namespace std::chrono;

    while (timer_running) {
        std::this_thread::sleep_for(seconds(1));
        std::lock_guard<std::mutex> lock(callback_mutex);
        if (!registered_callbacks.empty()) {
            auto message = std::make_shared<tbox::mcu::v1::McuMessage>();
            message->set_message_type(MCU_MESSAGE_TYPE_RSMS);
            message->set_message_data(package_rsms_data());

            // 创建消息指针
            auto *message_ptr = new std::shared_ptr<tbox::mcu::v1::McuMessage>(message);

            // 调用所有注册的回调函数
            for (auto it = registered_callbacks.begin(); it != registered_callbacks.end();) {
                bool result = it->callback(it->obj, message_ptr);
                if (!result) {
                    spdlog::warn("MCU消息回调函数执行失败，移除回调");
                    it = registered_callbacks.erase(it);
                } else {
                    ++it;
                }
            }

            delete message_ptr;
        }
    }
}

void start_timer_rsms() {
    if (!timer_running) {
        timer_running = true;
        timer_thread = std::thread(timer_rsms);
        spdlog::info("消息回调被注册，开始定频模拟消息");
    }
}

void stop_timer_rsms() {
    if (timer_running) {
        timer_running = false;
        if (timer_thread.joinable()) {
            timer_thread.join();
        }
        spdlog::info("MCU定时任务已停止");
    }
}

bool init_mcu_communication_module() {
    spdlog::info("模拟初始化MCU通讯模块");
    return true;
}

bool get_vin(std::string &vin) {
    spdlog::info("模拟返回车架号");
    vin = "HWYZTEST000000001";
    return true;
}

bool get_current_iccid(std::string &iccid) {
    spdlog::info("模拟返回当前ICCID");
    iccid = "89860923790000000001";
    return true;
}

bool get_battery_pack_sn(std::string &battery_pack_sn) {
    spdlog::info("模拟返回电池包序列号");
    battery_pack_sn = "19000000XXYY000001";
    return true;
}

bool get_tbox_sn(std::string &tbox_sn) {
    spdlog::info("模拟返回TBox序列号");
    tbox_sn = "18100000XXYY000001";
    return true;
}

bool get_tbox_mcu_version(std::string &version) {
    spdlog::info("模拟返回TBox MCU版本");
    version = "v1.0.0";
    return true;
}

bool register_mcu_message_callback(void *obj, receive_mcu_message_callback_t callback) {
    spdlog::info("模拟注册MCU消息");
    if (callback == nullptr) {
        return false;
    }
    std::lock_guard<std::mutex> lock(callback_mutex);
    registered_callbacks.push_back({obj, callback});
    spdlog::info("注册MCU消息回调函数，当前共有{}个回调", registered_callbacks.size());
    if (!registered_callbacks.empty()) {
        start_timer_rsms();
    }
    return true;
}