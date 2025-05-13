#include "pch.h"
#include "QueueManager.h"
#include "SessionManager.hpp"

void QueueManager::push(const Task& task)
{
    {
        std::lock_guard<std::mutex> lock(TaskMutex);
        TaskQueue.push(task);
        std::cout << "[QueueManager::push] task.message -> " << task.message << std::endl;
    }
    TaskCV.notify_one();
}

void QueueManager::run()
{
    while (true) {
        std::unique_lock<std::mutex> lock(TaskMutex);
        TaskCV.wait(lock, [this]() { return !TaskQueue.empty(); });

        auto task = TaskQueue.front();
        TaskQueue.pop();
        lock.unlock();

        std::cout << "[QueueManager::run] TaskQueue 에 작업이 있습니다. 작업 진행 -> " << task.message << std::endl;
        process(task);
    }
}

void QueueManager::process(Task& task)
{
    auto& session = task.session;
    std::string msg = task.message;
    std::istringstream iss(msg);
    
    std::string ID;
    iss >> ID;

    if (ID == "CHAT")
    {
        std::string CHATID, CHATMSG;
        iss >> CHATID;
        if (session->get_chat_id() == "") { 
            session->set_chat_id(CHATID); 
            std::shared_ptr<std::string> msg = std::make_shared<std::string>("CHAT OK\n");
            session->push_WriteQueue(msg);
            return;
        }
        else {
            std::getline(iss, CHATMSG);
            std::shared_ptr<std::string> shared_msg = std::make_shared<std::string>(session->get_chat_id() + " " + CHATMSG + "\n");
            SessionManager::GetInstance().BroadCast(shared_msg);
        }
    }
    else if (ID == "MOVE")
    {
        std::string DIR;
        iss >> DIR;
        // TEST
        std::shared_ptr<std::string> shared_msg = std::make_shared<std::string>("MOVE " + session->get_chat_id() + " " + DIR + "\n");
        session->push_WriteQueue(shared_msg);
    }

    //session->push_WriteQueue(msg);
    std::cout << "[QueueManager::process] Task 작업 완료 -> " << msg << std::endl;
}
