#include "events/template/events.h"
#include "events/template/handler.h"

#include <gmock/gmock-function-mocker.h>
#include <gtest/gtest.h>

using ::testing::_;
using ::testing::SaveArg;

class TestHandler: private events::video::IEventHandler {
  public:
  TestHandler() = default;

  MOCK_METHOD(void, onEventFlip, (), (override));
  MOCK_METHOD(void, onEventVBlank, (events::video::VBlankData const&), (override));
};

TEST(eventsystem, basic) {
  TestHandler testHandler;

  EXPECT_CALL(testHandler, onEventFlip);
  events::video::postEventFlip();

  events::video::VBlankData vblankData;
  EXPECT_CALL(testHandler, onEventVBlank(_)).WillOnce(SaveArg<0>(&vblankData));
  events::video::postEventVBlank({.time = 4});
  EXPECT_EQ(vblankData.time, 4);
}