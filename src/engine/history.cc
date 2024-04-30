#include "history.h"

MoveHistory::MoveHistory(const BoardState &state)
    : state_(state), killer_moves_({}), counter_moves_({}), butterfly_history_({}) {}

const int &MoveHistory::get_history_score(Move move, Color turn) noexcept {
  return butterfly_history_[turn][move.get_from()][move.get_to()];
}

std::array<Move, 2> &MoveHistory::get_killers(int ply) {
  assert(ply >= 0 && ply < kMaxPlyFromRoot);
  return killer_moves_[ply];
}

Move &MoveHistory::get_counter(Move move) {
  const auto to = move.get_to();
  return counter_moves_[state_.get_piece_type(to)][to];
}

void MoveHistory::update_killer_move(Move move, int ply) {
  if (move == killer_moves_[ply][0]) return;

  killer_moves_[ply][1] = killer_moves_[ply][0];
  killer_moves_[ply][0] = move;
}

void MoveHistory::update_counter_move(Move prev_move, Move counter) {
  if (prev_move != Move::null_move()) {
    const auto to = prev_move.get_to();
    counter_moves_[state_.get_piece_type(to)][to] = counter;
  }
}

const int kHistoryGravity = 16384;
const int kHistoryScale = 300;
const int kHistoryOffset = 300;
const int kHistoryMaxBonus = 2000;

int history_bonus(int depth) {
  return std::min(kHistoryScale * depth - kHistoryOffset, kHistoryMaxBonus);
}

void MoveHistory::update_history(Move move, List<Move, kMaxMoves> &bad_quiets, Color turn, int depth) {
  auto &move_history_score = butterfly_history_[turn][move.get_from()][move.get_to()];

  // apply a linear dampening to the bonus as the depth increases
  const int bonus = history_bonus(depth);
  const int scaled_bonus = bonus - move_history_score * std::abs(bonus) / kHistoryGravity;
  move_history_score += scaled_bonus;

  // lower the score of the quiet moves that failed to raise alpha
  // a good side effect of this is that moves that caused a beta cutoff earlier and were awarded a bonus but no longer
  // cause a beta cutoff are eventually "discarded"
  penalize_history(bad_quiets, turn, depth);
}

void MoveHistory::penalize_history(List<Move, kMaxMoves> &moves, Color turn, int depth) {
  const int penalty = -history_bonus(depth);
  for (int i = 0; i < moves.size(); i++) {
    const auto &move = moves[i];
    auto &move_history_score = butterfly_history_[turn][move.get_from()][move.get_to()];

    // apply a linear dampening to the bonus (penalty here) as the depth increases
    const int scaled_penalty = penalty - move_history_score * std::abs(penalty) / kHistoryGravity;
    move_history_score += scaled_penalty;
  }
}

void MoveHistory::decay() {
  for (auto &killers : killer_moves_) {
    killers.fill(Move::null_move());
  }
  for (auto &counters : counter_moves_) {
    counters.fill(Move::null_move());
  }
}

void MoveHistory::clear() {
  for (auto &sides : butterfly_history_) {
    for (auto &move_scores : sides) {
      move_scores.fill(0);
    }
  }
  for (auto &killers : killer_moves_) {
    killers.fill(Move::null_move());
  }
  for (auto &counters : counter_moves_) {
    counters.fill(Move::null_move());
  }
}

void MoveHistory::clear_killers(int ply) {
  killer_moves_[ply].fill(Move::null_move());
}