#ifndef INTEGRAL_UCI_H_
#define INTEGRAL_UCI_H_

#include <any>
#include <utility>

#include "board.h"
#include "fen.h"

class Search;

namespace uci {

const std::string kEngineName = "Integral";
const std::string kEngineVersion = "0.1";
const std::string kEngineAuthor = "Aron Petkovski";

enum class OptionType {
  kCheck,
  kSpin,
  kString
};

class Option {
 public:
  Option() {}

  explicit Option(OptionType type, std::any value, int min = 0, int max = 0)
      : type_(type), default_(std::move(value)), min_(min), max_(max) {
    if (type_ != OptionType::kSpin) {
      // Non spin types should not have a min/max
      assert(min_ == 0 && max_ == 0);
    }
  }

  void SetValue(const std::any &value) {
    if (type_ == OptionType::kSpin) {
      assert(value >= min && value <= max);
    }
    value_ = value;
  }

 private:
  OptionType type_;
  std::any value_, default_;
  // Should only be used with spin types
  int min_, max_;
};

void Position(Board &board, std::stringstream &input_stream);

void Go(Board &board, Search &search, std::stringstream &input_stream);

void Test(Board &board, std::stringstream &input_stream);

void AcceptCommands(int arg_count, char **args);

}  // namespace uci

#endif  // INTEGRAL_UCI_H_