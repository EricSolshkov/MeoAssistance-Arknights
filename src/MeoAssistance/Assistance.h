#pragma once

#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <unordered_map>

#include "AbstractTask.h"
#include "AsstDef.h"
#include "AsstMsg.h"
#include "AsstInfrastDef.h"

namespace cv
{
    class Mat;
}

namespace asst
{
    class Controller;
    class Identify;

    class Assistance
    {
    public:
        Assistance(AsstCallback callback = nullptr, void* callback_arg = nullptr);
        ~Assistance();

        // 根据配置文件，决定捕获模拟器、USB 还是远程设备
        bool catch_default();
        // 捕获模拟器
        bool catch_emulator(const std::string& emulator_name = std::string());
        // 捕获自定义设备
        bool catch_custom();
        // 不实际进行捕获，调试用接口
        bool catch_fake();

        // 添加刷理智任务
        bool append_sanity(bool only_append = true);
        // 添加领取日常任务奖励任务
        bool append_receive_award(bool only_append = true);
        // 添加访问好友基建任务
        bool append_visit(bool with_shopping, bool only_append = true);

        // 添加公开招募操作任务
        bool append_recruiting(const std::vector<int>& required_level, bool set_time, bool only_append = true);
        // 添加基建换班任务任务
        bool append_infrast_shift(infrast::WorkMode work_mode, const std::vector<std::string>& order, UsesOfDrones uses, double dorm_threshold, bool only_append = true);

        // 添加流程任务，应该是private的，调试用临时放到public
        bool append_process_task(const std::string& task, int retry_times = ProcessTaskRetryTimesDefault, std::string task_chain = std::string(), bool only_append = true);

#ifdef LOG_TRACE
        // 调试用
        bool append_debug_task();
#endif

        // 开始执行任务队列
        bool start(bool block = true);
        // 停止任务队列并清空
        bool stop(bool block = true);

        bool set_param(const std::string& type, const std::string& param, const std::string& value);

        static constexpr int ProcessTaskRetryTimesDefault = 20;
        static constexpr int OpenRecruitTaskRetyrTimesDefault = 5;

    private:
        void working_proc();
        void msg_proc();
        static void task_callback(AsstMsg msg, const json::value& detail, void* custom_arg);

        void append_match_task(const std::string& task_chain, const std::vector<std::string>& tasks, int retry_times = ProcessTaskRetryTimesDefault, bool front = false);
        void append_task(const json::value& detail, bool front = false);
        void append_callback(AsstMsg msg, json::value detail);
        void clear_exec_times();
        json::value organize_stage_drop(const json::value& rec); // 整理关卡掉落的材料信息

        bool m_inited = false;

        bool m_thread_exit = false;
        std::deque<std::shared_ptr<AbstractTask>> m_tasks_deque;
        AsstCallback m_callback = nullptr;
        void* m_callback_arg = nullptr;

        bool m_thread_idle = true;
        std::thread m_working_thread;
        std::mutex m_mutex;
        std::condition_variable m_condvar;

        std::thread m_msg_thread;
        std::queue<std::pair<AsstMsg, json::value>> m_msg_queue;
        std::mutex m_msg_mutex;
        std::condition_variable m_msg_condvar;
    };
}
