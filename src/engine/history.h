#ifndef INTEGRAL_HISTORY_H
#define INTEGRAL_HISTORY_H

#include <array>

#include "../chess/move_gen.h"

class SearchStack;

constexpr int kFromToCombinations = Square::kSquareCount * Square::kSquareCount;

using KillerMoves = std::array<std::array<Move, 2>, kMaxPlyFromRoot>;
using ButterflyHistory = std::array<std::array<short, kFromToCombinations>, 2>;
using ContinuationHistory = std::array<std::array<std::array<std::array<short, 64>, 12>, 64>, 12>;

class MoveHistory {
 public:
  explicit MoveHistory(const BoardState &state);

  int get_history_score(Move move, Color turn) noexcept;

  int get_cont_history_score(Move move, int plies_ago, SearchStack *stack) noexcept;

  std::array<Move, 2> &get_killers(int ply);

  void update_killer_move(Move move, int ply);

  void update_cont_history(Move best_move, List<Move, kMaxMoves> &bad_quiets, int depth, SearchStack *stack);

  void update_history(Move best_move, List<Move, kMaxMoves> &bad_quiets, Color turn, int depth);

  void clear();

  void clear_killers(int ply);

 private:
  const BoardState &state_;
  KillerMoves killer_moves_;
  ButterflyHistory butterfly_history_;
  ContinuationHistory cont_history_;
};

#endif  // INTEGRAL_HISTORY_H