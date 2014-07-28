
#include "color.hpp"

std::string FullLib::Red::name() const {
    return "Red";
}

std::string FullLib::Blue::name() const {
    return "Blue";
}

std::string FullLib::getColorName(boost::shared_ptr<Color> color) {
    return color->name();
}

std::string FullLib::accountTypeToString(Account::Type type) {
    switch (type) {
      case Account::Current:
        return "Current";
      case Account::Savings:
        return "Savings";
      default:
        return "Unrecognized account type";
    }
}

