#define __APICALL_EXTERN
#include "export.h"
#undef __APICALL_EXTERN

#include "../handler.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/bind/bind.hpp>
#include <boost/interprocess/ipc/message_queue.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/signals2.hpp>
#include <iostream>
#include <thread>

namespace {

constexpr std::string_view PIPENAME         = "psoff-system_circle";
constexpr size_t           MAX_MESSAGE_SIZE = 500;

void onEvent(events::system_circle::IEventHandler* handler, char const* data, uint32_t size);

struct PImplReader {
  boost::interprocess::message_queue mq;

  boost::signals2::signal<void(char const*, uint32_t)> signal;

  std::thread reader;

  PImplReader(): mq(boost::interprocess::open_only, PIPENAME.data()) {
    reader = std::thread([this]() {
      printf("reading circle...\n");

      std::vector<char> payload(MAX_MESSAGE_SIZE);
      while (true) {
        size_t   recv = 0;
        uint32_t prio = 0;

        // Read Header

        try {
          mq.receive(payload.data(), payload.size(), recv, prio);
        } catch (boost::interprocess::interprocess_exception& ex) {
          std::cout << "Interprocess exception: " << ex.what() << std::endl;
          // You can access the error code as well
          std::cout << "Error code: " << ex.get_error_code() << std::endl;
          break;
        } catch (...) {
          printf("receive error\n");
          break;
        }
        if (recv == 0) continue;
        // -

        signal(payload.data(), recv);
      }
    });
  }

  ~PImplReader() { reader.join(); }
};

struct PImplWriter {
  boost::interprocess::message_queue mq;

  PImplWriter(): mq(boost::interprocess::open_or_create, PIPENAME.data(), 10, MAX_MESSAGE_SIZE) {}
};

auto getReader() {
  static PImplReader inst;
  return &inst;
}

auto getWriter() {
  static PImplWriter inst;
  return &inst;
}

void onEvent(events::system_circle::IEventHandler* handler, char const* data, uint32_t size) {

  boost::iostreams::array_source source(data, size);

  boost::iostreams::stream<boost::iostreams::array_source> stream(source);

  boost::archive::binary_iarchive ia(stream);

  int32_t eventId;
  ia >> eventId;

  printf("received event: %d %u\n", eventId, size);

  switch (eventId) {
    case 0: {
      events::system_circle::LoadArgs obj;

      ia >> obj;
      handler->onEventLoadExec(obj);

    } break;
    case 1: {
      events::system_circle::SetArg obj;

      ia >> obj;
      handler->onEventSetArguments(obj);
    } break;
  }
}
} // namespace

namespace events::system_circle::core {

void registerSelf(events::system_circle::IEventHandler* obj) {
  using namespace boost::placeholders;
  getReader()->signal.connect(boost::bind(onEvent, obj, _1, _2));
}

void postEvent(std::string const& stream) {
  assert(stream.size() <= MAX_MESSAGE_SIZE);
  try {
    getWriter()->mq.send(stream.data(), stream.size(), 0);
  } catch (...) {
    printf("postEventError\n");
  }
}

void initChild() {
  boost::interprocess::message_queue::remove(PIPENAME.data());
  (void)getWriter();
}
} // namespace events::system_circle::core
