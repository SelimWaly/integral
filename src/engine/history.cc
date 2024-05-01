#include "history.h"

#include "search.h"

const int kHistoryGravity = 16384;
const int kHistoryScale = 300;
const int kHistoryOffset = 300;
const int kHistoryMaxBonus = 2000;

int history_bonus(int depth) {
  return std::min(kHistoryScale * depth - kHistoryOffset, kHistoryMaxBonus);
}

int scale_bonus(int score, int bonus) {
  return bonus - score * std::abs(bonus) / kHistoryGravity;
}

inline int move_index(Move move) {
  return move.get_from() * Square::kSquareCount + move.get_to();
}

inline int move_index(PieceType piece, Move move) {
  return piece * kFromToCombinations + move.get_from() * Square::kSquareCount + move.get_to();
}

MoveHistory::MoveHistory(const BoardState &state)
    : state_(state), killer_moves_({}), cont_history_({}), butterfly_history_({}) {}

int MoveHistory::get_history_score(Move move, Color turn) noexcept {
  return butterfly_history_[turn][move_index(move)];
}

int MoveHistory::get_cont_history_score(Move move, int plies_ago, SearchStack *stack) noexcept {
  const auto piece = state_.get_piece_type(move.get_from());
  return stack->ply >= plies_ago
             ? cont_history_[stack->ply - plies_ago][piece][move.get_to()]
             : 0;
}

std::array<Move, 2> &MoveHistory::get_killers(int ply) {
  assert(ply >= 0 && ply < kMaxPlyFromRoot);
  return killer_moves_[ply];
}

void MoveHistory::update_killer_move(Move move, int ply) {
  if (move == killer_moves_[ply][0]) return;

  killer_moves_[ply][1] = killer_moves_[ply][0];
  killer_moves_[ply][0] = move;
}

void MoveHistory::update_cont_history(Move best_move, List<Move, kMaxMoves> &bad_quiets, int depth, SearchStack *stack) {
  const auto update_cont_hist_entry = [&stack, this](int plies_ago, Move move, int bonus) {
    const auto piece = state_.get_piece_type(move.get_from());
    if (stack->ply >= plies_ago && stack->behind(plies_ago)->best_move) {
      short &score = cont_history_[stack->ply - plies_ago][piece][move.get_to()];
      score += scale_bonus(score, bonus);
    }
  };

  const int bonus = history_bonus(depth);
  update_cont_hist_entry(1, best_move, bonus);
  update_cont_hist_entry(2, best_move, bonus);
  update_cont_hist_entry(4, best_move, bonus);

  // lower the score of the quiet moves that failed to raise alpha
  // a good side effect of this is that moves that caused a beta cutoff earlier and were awarded a bonus but no longer
  // cause a beta cutoff are eventually "discarded"
  const int penalty = -bonus;
  for (int i = 0; i < bad_quiets.size(); i++) {
    const auto &bad_quiet = bad_quiets[i];

    // apply a linear dampening to the bonus (penalty here) as the depth increases
    update_cont_hist_entry(1, bad_quiet, penalty);
    update_cont_hist_entry(2, bad_quiet, penalty);
    update_cont_hist_entry(4, bad_quiet, penalty);
  }
}

void MoveHistory::update_history(Move best_move, List<Move, kMaxMoves> &bad_quiets, Color turn, int depth) {
  const int bonus = history_bonus(depth);

  // apply a linear dampening to the bonus as the depth increases
  short &score = butterfly_history_[turn][move_index(best_move)];
  score += scale_bonus(score, bonus);

  // lower the score of the quiet moves that failed to raise alpha
  // a good side effect of this is that moves that caused a beta cutoff earlier and were awarded a bonus but no longer
  // cause a beta cutoff are eventually "discarded"
  const int penalty = -bonus;
  for (int i = 0; i < bad_quiets.size(); i++) {
    const auto &bad_quiet = bad_quiets[i];

    // apply a linear dampening to the bonus (penalty here) as the depth increases
    short &bad_quiet_score = butterfly_history_[turn][move_index(bad_quiet)];
    bad_quiet_score += scale_bonus(bad_quiet_score, penalty);
  }
}

void MoveHistory::decay() {
  for (auto &killers : killer_moves_) {
    killers.fill(Move::null_move());
  }
}

void MoveHistory::clear() {
  for (auto &scores : butterfly_history_) {
    scores.fill(0);
  }
  for (auto &killers : killer_moves_) {
    killers.fill(Move::null_move());
  }
  for (auto &counters : cont_history_) {
    for (auto &pieces : counters) {
      pieces.fill(0);
    }
  }
}

void MoveHistory::clear_killers(int ply) {
  killer_moves_[ply].fill(Move::null_move());
}