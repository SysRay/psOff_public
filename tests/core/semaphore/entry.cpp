
#include "core/kernel/semaphore.h"
#include "modules_include/common.h"

#include <fff/fff.h>
#include <future>
#include <gtest/gtest.h>

DEFINE_FFF_GLOBALS;

namespace pthread {
FAKE_VALUE_FUNC(int, getThreadId);
}

TEST(core_semaphore, init) {
  RESET_FAKE(pthread::getThreadId);
  pthread::getThreadId_fake.return_val = 4;

  constexpr int initCount = 1;
  constexpr int maxCount  = 10;

  {
    auto sem = createSemaphore_fifo("test", initCount, maxCount);
    EXPECT_TRUE(sem);

    EXPECT_EQ(sem->getSignalCounter(), initCount);
  }

  {
    auto sem = createSemaphore_fifo(nullptr, initCount, maxCount);
    EXPECT_TRUE(sem);

    EXPECT_EQ(sem->getSignalCounter(), initCount);
  }
}

TEST(core_semaphore, polling_1) {
  RESET_FAKE(pthread::getThreadId);
  pthread::getThreadId_fake.return_val = 4;

  constexpr int initCount = 1;
  constexpr int maxCount  = 10;

  auto sem = createSemaphore_fifo("test", initCount, maxCount);

  auto fut = std::async(std::launch::async, [&sem] {
    {
      auto res = sem->poll(2);
      EXPECT_EQ(res, getErr(ErrCode::_EAGAIN));
    }
    {
      auto res = sem->poll(1);
      EXPECT_EQ(res, Ok);

      EXPECT_EQ(sem->getSignalCounter(), 0);
    }
    {
      auto res = sem->poll(2);
      EXPECT_EQ(res, getErr(ErrCode::_EAGAIN));
    }
    {
      auto resSignal = sem->signal(2);
      EXPECT_EQ(resSignal, Ok);

      auto res = sem->poll(2);
      EXPECT_EQ(res, Ok);

      EXPECT_EQ(sem->getSignalCounter(), 0);
    }

    {
      auto resSignal = sem->signal(4);
      EXPECT_EQ(resSignal, Ok);

      auto res1 = sem->poll(2);
      EXPECT_EQ(res1, Ok);
      EXPECT_EQ(sem->getSignalCounter(), 2);

      auto res2 = sem->poll(2);
      EXPECT_EQ(res2, Ok);
      EXPECT_EQ(sem->getSignalCounter(), 0);
    }
  });

  auto res = fut.wait_for(std::chrono::milliseconds(10));
  EXPECT_NE(res, std::future_status::timeout);
}

TEST(core_semaphore, signal_1) {
  RESET_FAKE(pthread::getThreadId);
  pthread::getThreadId_fake.return_val = 4;

  constexpr int initCount = 1;
  constexpr int maxCount  = 10;

  auto sem = createSemaphore_fifo("test", initCount, maxCount);

  { // Check wait instant release
    auto resSignal = sem->signal(1);
    EXPECT_EQ(resSignal, Ok);
    EXPECT_EQ(sem->getSignalCounter(), 2);

    auto fut = std::async(std::launch::async, [&sem] {
      {
        auto res = sem->wait(2, nullptr); // wait forever
        EXPECT_EQ(res, Ok);
      }
    });

    auto res = fut.wait_for(std::chrono::milliseconds(10));
    EXPECT_NE(res, std::future_status::timeout);
  }

  EXPECT_EQ(sem->getSignalCounter(), 0);

  { // Check timeout
    auto resSignal = sem->signal(1);
    EXPECT_EQ(resSignal, Ok);
    EXPECT_EQ(sem->getSignalCounter(), 1);

    auto fut = std::async(std::launch::async, [&sem] {
      {
        uint32_t micros = 1;
        auto     res    = sem->wait(2, &micros); // wait timeout
        EXPECT_EQ(res, getErr(ErrCode::_ETIMEDOUT));
      }
    });

    auto res = fut.wait_for(std::chrono::milliseconds(10));
    EXPECT_NE(res, std::future_status::timeout);
  }
  EXPECT_EQ(sem->getSignalCounter(), 1);

  { // Check wait signal afterwards
    auto fut = std::async(std::launch::async, [&sem] {
      {
        auto res = sem->wait(2, nullptr); // wait forever
        EXPECT_EQ(res, Ok);
      }
    });
    std::this_thread::sleep_for(std::chrono::microseconds(100)); // this or mock condition var

    auto resSignal = sem->signal(1);
    EXPECT_EQ(resSignal, Ok);
    EXPECT_EQ(sem->getSignalCounter(), 2);

    auto res = fut.wait_for(std::chrono::milliseconds(10));
    EXPECT_NE(res, std::future_status::timeout);
  }
}

TEST(core_semaphore, signal_2) {
  RESET_FAKE(pthread::getThreadId);

  pthread::getThreadId_fake.custom_fake = []() {
    static int counter = 0;
    return ++counter;
  };

  constexpr int initCount = 0;
  constexpr int maxCount  = 10;

  auto sem = createSemaphore_fifo("test", initCount, maxCount);
  EXPECT_EQ(sem->getSignalCounter(), 0);

  size_t countItems = 0;
  { // Check wait signal afterwards (sequenze release)
    auto fut1 = std::async(std::launch::async, [&sem, &countItems] {
      {
        auto res = sem->wait(2, nullptr); // wait forever
        ++countItems;
        EXPECT_EQ(res, Ok);
      }
    });
    auto fut2 = std::async(std::launch::async, [&sem, &countItems] {
      {
        auto res = sem->wait(2, nullptr); // wait forever
        ++countItems;
        EXPECT_EQ(res, Ok);
      }
    });
    std::this_thread::sleep_for(std::chrono::microseconds(100)); // this or mock condition var

    {
      auto resSignal = sem->signal(2);
      EXPECT_EQ(resSignal, Ok);
    }

    auto res1 = fut1.wait_for(std::chrono::microseconds(100));
    EXPECT_NE(res1, std::future_status::timeout);
    EXPECT_EQ(sem->getSignalCounter(), 0);
    EXPECT_EQ(countItems, 1);

    {
      auto resSignal = sem->signal(2);
      EXPECT_EQ(resSignal, Ok);
    }

    auto res2 = fut2.wait_for(std::chrono::microseconds(100));
    EXPECT_NE(res2, std::future_status::timeout);
    EXPECT_EQ(countItems, 2);
  }

  { // Check wait signal afterwards (direct release)
    auto fut1 = std::async(std::launch::async, [&sem] {
      {
        auto res = sem->wait(2, nullptr); // wait forever
        EXPECT_EQ(res, Ok);
      }
    });
    auto fut2 = std::async(std::launch::async, [&sem] {
      {
        auto res = sem->wait(2, nullptr); // wait forever
        EXPECT_EQ(res, Ok);
      }
    });
    std::this_thread::sleep_for(std::chrono::microseconds(100)); // this or mock condition var

    {
      auto resSignal = sem->signal(4);
      EXPECT_EQ(resSignal, Ok);
    }

    auto res1 = fut1.wait_for(std::chrono::microseconds(100));
    EXPECT_NE(res1, std::future_status::timeout);

    auto res2 = fut2.wait_for(std::chrono::microseconds(100));
    EXPECT_NE(res2, std::future_status::timeout);
    EXPECT_EQ(sem->getSignalCounter(), 0);
  }

  sem.reset();
}

TEST(core_semaphore, signal_exit) {
  RESET_FAKE(pthread::getThreadId);

  pthread::getThreadId_fake.custom_fake = []() {
    static int counter = 0;
    return ++counter;
  };

  constexpr int initCount = 0;
  constexpr int maxCount  = 10;

  auto sem = createSemaphore_fifo("test", initCount, maxCount);
  EXPECT_EQ(sem->getSignalCounter(), 0);

  { // Check wait signal afterwards and exit release
    auto fut1 = std::async(std::launch::async, [&sem] {
      {
        auto res = sem->wait(2, nullptr); // wait forever
        EXPECT_EQ(res, Ok);
      }
    });
    auto fut2 = std::async(std::launch::async, [&sem] {
      {
        auto res = sem->wait(2, nullptr); // wait forever
        EXPECT_EQ(res, getErr(ErrCode::_ECANCELED));
      }
    });
    std::this_thread::sleep_for(std::chrono::microseconds(100)); // this or mock condition var

    auto resSignal = sem->signal(2);
    EXPECT_EQ(resSignal, Ok);

    auto res1 = fut1.wait_for(std::chrono::microseconds(100));
    EXPECT_NE(res1, std::future_status::timeout);
    EXPECT_EQ(sem->getSignalCounter(), 0);

    auto res2 = fut2.wait_for(std::chrono::microseconds(100));
    EXPECT_EQ(res2, std::future_status::timeout);
    sem.reset();
  }
}