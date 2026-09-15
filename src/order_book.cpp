#include "order_book.hpp"
#include <algorithm>
#include <stdexcept>

bool OrderBook::submit(Order order) {
    trades_.clear();
    if (order.id == 0 || order.quantity == 0) return false;
    if (orders_.contains(order.id)) return false;
    order.remaining = order.quantity;
    order.sequence = sequence_++;

    if (order.type == OrderType::Market) {
        if (order.side == Side::Buy) match_buy(order);
        else match_sell(order);
        return true;
    }

    if (order.price_ticks <= 0) return false;
    if (order.side == Side::Buy) match_buy(order);
    else match_sell(order);
    if (order.remaining > 0) rest(order);
    return true;
}

void OrderBook::rest(Order order) {
    orders_.emplace(order.id, order);
    if (order.side == Side::Buy) {
        auto &q = bids_[order.price_ticks];
        q.push_back(order.id);
        positions_[order.id] = std::prev(q.end());
    } else {
        auto &q = asks_[order.price_ticks];
        q.push_back(order.id);
        positions_[order.id] = std::prev(q.end());
    }
}

void OrderBook::erase_if_filled(uint64_t id) {
    auto it = orders_.find(id);
    if (it != orders_.end() && it->second.remaining == 0) {
        positions_.erase(id);
        orders_.erase(it);
    }
}

void OrderBook::match_buy(Order& incoming) {
    while (incoming.remaining > 0 && !asks_.empty()) {
        auto level_it = asks_.begin();
        const auto price = level_it->first;
        if (incoming.type == OrderType::Limit && incoming.price_ticks < price) break;

        auto &q = level_it->second;
        while (incoming.remaining > 0 && !q.empty()) {
            const uint64_t resting_id = q.front();
            auto rit = orders_.find(resting_id);
            if (rit == orders_.end()) { q.pop_front(); continue; }

            auto &resting = rit->second;
            const uint64_t qty = std::min(incoming.remaining, resting.remaining);
            incoming.remaining -= qty;
            resting.remaining -= qty;
            trades_.push_back({incoming.id, resting.id, price, qty});
            if (resting.remaining == 0) {
                positions_.erase(resting.id);
                q.pop_front();
                orders_.erase(rit);
            }
        }
        if (q.empty()) asks_.erase(level_it);
    }
}

void OrderBook::match_sell(Order& incoming) {
    while (incoming.remaining > 0 && !bids_.empty()) {
        auto level_it = bids_.begin();
        const auto price = level_it->first;
        if (incoming.type == OrderType::Limit && incoming.price_ticks > price) break;

        auto &q = level_it->second;
        while (incoming.remaining > 0 && !q.empty()) {
            const uint64_t resting_id = q.front();
            auto rit = orders_.find(resting_id);
            if (rit == orders_.end()) { q.pop_front(); continue; }

            auto &resting = rit->second;
            const uint64_t qty = std::min(incoming.remaining, resting.remaining);
            incoming.remaining -= qty;
            resting.remaining -= qty;
            trades_.push_back({resting.id, incoming.id, price, qty});
            if (resting.remaining == 0) {
                positions_.erase(resting.id);
                q.pop_front();
                orders_.erase(rit);
            }
        }
        if (q.empty()) bids_.erase(level_it);
    }
}

bool OrderBook::cancel(uint64_t order_id) {
    auto it = orders_.find(order_id);
    if (it == orders_.end()) return false;

    const auto side = it->second.side;
    const auto price = it->second.price_ticks;
    auto pos = positions_.find(order_id);
    if (pos != positions_.end()) {
        if (side == Side::Buy) {
            auto lit = bids_.find(price);
            if (lit != bids_.end()) {
                lit->second.erase(pos->second);
                if (lit->second.empty()) bids_.erase(lit);
            }
        } else {
            auto lit = asks_.find(price);
            if (lit != asks_.end()) {
                lit->second.erase(pos->second);
                if (lit->second.empty()) asks_.erase(lit);
            }
        }
        positions_.erase(pos);
    }
    orders_.erase(it);
    return true;
}

std::optional<int64_t> OrderBook::best_bid() const {
    if (bids_.empty()) return std::nullopt;
    return bids_.begin()->first;
}
std::optional<int64_t> OrderBook::best_ask() const {
    if (asks_.empty()) return std::nullopt;
    return asks_.begin()->first;
}

std::vector<BookLevel> OrderBook::bids(size_t depth) const {
    std::vector<BookLevel> out;
    for (auto &[p, q] : bids_) {
        if (out.size() >= depth) break;
        uint64_t total = 0;
        for (auto id : q) total += orders_.at(id).remaining;
        out.push_back({p, total, q.size()});
    }
    return out;
}
std::vector<BookLevel> OrderBook::asks(size_t depth) const {
    std::vector<BookLevel> out;
    for (auto &[p, q] : asks_) {
        if (out.size() >= depth) break;
        uint64_t total = 0;
        for (auto id : q) total += orders_.at(id).remaining;
        out.push_back({p, total, q.size()});
    }
    return out;
}
