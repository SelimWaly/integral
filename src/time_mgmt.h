#ifndef INTEGRAL_TIME_MGMT_H_
#define INTEGRAL_TIME_MGMT_H_

#include "types.h"
#include "board.h"

#include <array>
#include <chrono>
#include <thread>
#include <condition_variable>

class TimeManagement {
 public:
  struct Config {
    int depth{};
    int move_time{};
    std::array<int, 2> time{};
    std::array<int, 2> increment{};
  };

  explicit TimeManagement(const Config &config, Board &board);

  const Config &get_config();

  void start();

  void stop();

  void update_nodes_searched();

  void update_node_spent_table(const Move &move, long long prev_nodes_searched);

  [[nodiscard]] bool soft_times_up(const Move &pv_move);

  [[nodiscard]] bool times_up() const;

  [[nodiscard]] long long nodes_per_second() const;

  [[nodiscard]] long long get_nodes_searched() const;

  [[nodiscard]] long long get_move_time() const;

  [[nodiscard]] long long time_elapsed() const;

  [[nodiscard]] long long calculate_hard_limit();

  [[nodiscard]] long long calculate_soft_limit(const Move &pv_move);

 private:
  const Config &config_;
  Board &board_;
  std::chrono::steady_clock::time_point start_time_, end_time_;
  long long current_move_time_;
  long long nodes_searched_;
  std::atomic<bool> times_up_;
  std::atomic<bool> worker_processed_;
  std::array<long long, 4096> node_spent_table_;
  std::mutex mutex_;
  std::condition_variable times_up_cv_;
  std::thread worker;
};

#endif // INTEGRAL_TIME_MGMT_H_