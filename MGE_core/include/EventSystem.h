#pragma once

#include <optional>
#include <memory>
#include <thread>
#include <deque>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <functional>

#include "GlobalFunctions.h"

constexpr uint64_t SECTION_WIDTH = 10000;

enum EventSystemType : uint64_t
{
    unknown = 0,
    gui_Section = SECTION_WIDTH,
    lastReserved = (SECTION_WIDTH * 2) - 1
};

class BaseEvent
{
public:

    [[nodiscard]] virtual uint64_t getType() const = 0;
    [[nodiscard]] uint64_t getSection() const
    {
        return static_cast<uint64_t>(getType()) / SECTION_WIDTH;
    }

    virtual ~BaseEvent() = default;
};

using Callback = std::function<void(const BaseEvent&)>;

struct LifetimeToken
{
    uint64_t type = EventSystemType::unknown;
    std::weak_ptr<void> m_observerLifetime;

    [[nodiscard]] bool IsAlive() const
    {
        return !m_observerLifetime.expired();
    }
};

struct ObserverLifetime
{
    struct AliveTag {};
    std::shared_ptr<void> alive = std::make_shared<AliveTag>();
};

struct Subscription
{
public:
    Subscription(const LifetimeToken& token, Callback&& function)
        : m_token(token), m_EventSystemCallback(std::move(function)) {
    }

    LifetimeToken m_token;
    Callback m_EventSystemCallback;
};

template<typename T>
LifetimeToken MakeObserverToken(uint64_t type, const ObserverLifetime& lifetime)
{
    return { type, lifetime.alive };
}

class UdmEventSystem
{
public:
    UdmEventSystem()
    {
        workerThread = std::thread([this] {eventSystemGarbageCollector(); });
    }

    [[nodiscard]] std::unique_ptr<BaseEvent> pullEvent()
    {
        std::lock_guard<std::mutex> lock(eventSystemMutex);
        if (eventQueue.empty())
            return nullptr;
        std::unique_ptr<BaseEvent> event = std::move(eventQueue.front());
        eventQueue.pop_front();
        emptyEventQue.store(eventQueue.empty());
        return event;
    }

    void pushEvent(std::unique_ptr<BaseEvent> event)
    {
        _ASSERT(event);
        if (!event)
            return;

        std::lock_guard<std::mutex> lock(eventSystemMutex);
        eventQueue.push_back(std::move(event));
        emptyEventQue.store(eventQueue.empty());

		cv.notify_one();
    }

    [[nodiscard]] bool isEmpty()
    {
		// Performance critical function - should be called frequently in the main app loop
        // use atomic variable to avoid locking mutex every time in release configuration
#ifdef _DEBUG
        std::lock_guard<std::mutex> lock(eventSystemMutex);
		_ASSERT(emptyEventQue.load() == eventQueue.empty());
#endif
		return emptyEventQue.load();
    }

    void addEventSystemCallback(const LifetimeToken& token, Callback cb)
    {
        //WARNING: could be run in child thread!!
        std::lock_guard lock(subscriptionMutex);
        subscriptions[token.type].push_back(std::make_shared<Subscription>(token, std::move(cb)));
    }

    std::vector<std::shared_ptr<Subscription>> getCopyOfEventSystemCallbacks(const BaseEvent& e)
    {
        MAIN_THREAD_GUARD;
        if (!mgeCore::isMainThread()) // Cannot copy EventSystemCallbacks from a worker thread because the GUI element could be destroyed during the EventSystemCallback.
            return {};

        std::lock_guard lock(subscriptionMutex);
        auto it = subscriptions.find(e.getType());
        if (it != subscriptions.end())
            return { it->second };
        return {};
    }

    void publishEvent(const BaseEvent& e)
    {
        MAIN_THREAD_GUARD;
        if (!mgeCore::isMainThread()) // Cannot use EventSystemCallbacks from a worker thread because the GUI element could be destroyed during the EventSystemCallback.
            return;

        auto EventSystemCallbacks = getCopyOfEventSystemCallbacks(e);
        for (auto& sub : EventSystemCallbacks)
        {
            if (sub->m_token.IsAlive())
            {
                try
                {
                    sub->m_EventSystemCallback(e);
                }
#ifdef _DEBUG
                catch (const std::exception& e)
                {
                    _ASSERT(false);
                    [[maybe_unused]] auto err = e.what();
                }
#endif
                catch (...)
                {
                    _ASSERT(false);
                    // not used now....
                }
            }
            else
            {
                cv.notify_one();
                needClean.store(true); //for auto GC in non-blocking thread
            }
        }
    }

    void removeInvalidCallbacks()
    {
        _ASSERT(!mgeCore::isMainThread());
        std::lock_guard lock(subscriptionMutex);
        for (auto it = subscriptions.begin(); it != subscriptions.end(); )
        {
            auto& EventSystemCallbacks = it->second;

            EventSystemCallbacks.erase(std::remove_if(EventSystemCallbacks.begin(), EventSystemCallbacks.end(),
                [](const auto& sub)
                {
                    return sub->m_token.m_observerLifetime.expired();
                }), EventSystemCallbacks.end());

            if (EventSystemCallbacks.empty())
                it = subscriptions.erase(it);
            else
                ++it;
        }
    }

    [[nodiscard]] bool canBeTerminated() const
    {
        return canBeterminated.load();
	}

    UdmEventSystem(UdmEventSystem&) = delete;
    UdmEventSystem(UdmEventSystem&&) = delete;
    UdmEventSystem operator=(UdmEventSystem&) = delete;
    UdmEventSystem operator=(UdmEventSystem&&) = delete;

    ~UdmEventSystem()
    {
        running.store(false);
        cv.notify_one();
        if (workerThread.joinable())
            workerThread.join();
    }

private:
	std::atomic<bool> emptyEventQue{ true };
    std::thread workerThread;
    std::mutex eventSystemMutex;
    std::deque<std::unique_ptr<BaseEvent>> eventQueue;
    std::mutex subscriptionMutex;
    std::unordered_map<uint64_t, std::vector<std::shared_ptr<Subscription>>> subscriptions;
    std::atomic<bool> running{ true };
    std::atomic<bool> canBeterminated{ false };
    std::atomic<bool> needClean = false; //ensure auto cleaning (GC)

    std::condition_variable cv;
    std::mutex waitMutex;
    
    void eventSystemGarbageCollector()
    {
        _ASSERT(!mgeCore::isMainThread());

        std::unique_lock lock(waitMutex);

        while (running.load())
        {
            cv.wait(lock, [this]
                { return !running || needClean; });

            if (!running)
                break;

            lock.unlock();

            if (needClean.exchange(false)) //garbage collector
                removeInvalidCallbacks();

            lock.lock();
        }
        canBeterminated.store(true);
    }
};

inline UdmEventSystem& getEventSystem()
{
    static UdmEventSystem es;
    return es;
}

class ObserverTokens
{
public:
    ObserverTokens() = default;

    template<typename Event, typename F>
    void addToken(uint64_t type, F&& f)
    {
        Callback cb =
            [func = std::forward<F>(f)](const BaseEvent& e)
            {
                func(static_cast<const Event&>(e));
            };

        auto lifetime = std::make_shared<ObserverLifetime>();
        LifetimeToken observer{ type, lifetime->alive };

        getEventSystem().addEventSystemCallback(observer, std::move(cb));
        m_lifetimes.push_back(std::move(lifetime));
    }

    void removeToken()
    {
        _ASSERT(false);
        //Todo:
        //not yet...
    }

private:
    //todo: add observerUniqueId to distinguish lifetimes in case an observer is removed from the vector
    std::vector<std::shared_ptr<ObserverLifetime>> m_lifetimes;
};