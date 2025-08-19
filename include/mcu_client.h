//
// Created by hwyz_leo on 2025/8/7.
//

#ifndef MCUSERVICE_MCU_CLIENT_H
#define MCUSERVICE_MCU_CLIENT_H
#include "mcu_message.pb.h"
#include "mcu_message_handler.h"

class McuClient {
public:
    /**
     * 析构虚函数
     */
    ~McuClient();

    /**
     * 防止对象被复制
     */
    McuClient(const McuClient &) = delete;

    /**
     * 防止对象被赋值
     * @return
     */
    McuClient &operator=(const McuClient &) = delete;

    /**
     * 获取单例
     * @return 单例
     */
    static McuClient &get_instance();

public:
    /**
     * 初始化
     * @return 是否成功
     */
    bool init();

    /**
     * 启动
     * @return 启动是否成功
     */
    bool start();

    /**
     * 停止
     */
    void stop();

    /**
     * 接收到MCU消息的回调函数
     * @param obj 自定义对象
     * @param message MCU消息
     * @return 接收到消息是否成功
     */
    void on_receive_mcu_message(void *obj, std::shared_ptr<tbox::mcu::v1::McuMessage> *message);

private:
    // 消息处理器
    std::unordered_map<uint32_t, McuMessageHandler*> message_handler_;

private:

    McuClient();

    /**
     * 注册消息处理器
     */
    void registerHandler();

};
#endif //MCUSERVICE_MCU_CLIENT_H
