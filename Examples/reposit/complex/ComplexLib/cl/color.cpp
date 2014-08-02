
#include <cl/color.hpp>

std::string ComplexLib::Red::name() const {
    return "Red";
}

std::string ComplexLib::Blue::name() const {
    return "Blue";
}

std::string ComplexLib::getColorName(boost::shared_ptr<Color> color) {
    return color->name();
}

std::string ComplexLib::accountTypeToString(Account::Type type) {
    switch (type) {
      case Account::Current:
        return "Current";
      case Account::Savings:
        return "Savings";
      default:
        return "Unrecognized account type";
    }
}

