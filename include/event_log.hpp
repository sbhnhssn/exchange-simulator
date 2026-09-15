#pragma once
#include "order_book.hpp"
#include <cstdint>
#include <string>
#include <vector>

struct Event {
    enum class Kind : uint8_t { Submit = 1, Cancel = 2 };
    Kind kind{};
    Order order{};
    uint64_t cancel_id{};
};

class EventLog {
public:
    explicit EventLog(std::string path);
    void append_submit(const Order& order);
    void append_cancel(uint64_t id);
    std::vector<Event> replay() const;
private:
    std::string path_;
    void append(const std::vector<uint8_t>& bytes);
};
