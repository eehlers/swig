
#include "color.hpp"

std::string SimpleLib::Red::name() const {
    return "Red";
}

std::string SimpleLib::Blue::name() const {
    return "Blue";
}

std::string SimpleLib::getColorName(boost::shared_ptr<Color> color) {
    return color->name();
}

std::string SimpleLib::accountTypeToString(Account::Type type) {
    switch (type) {
      case Account::Current:
        return "Current";
      case Account::Savings:
        return "Savings";
      default:
        return "Unrecognized account type";
    }
}

