
#ifndef simplelib_color_hpp
#define simplelib_color_hpp

#include <string>

namespace SimpleLib {

    struct Account { enum Type { Current, Savings }; };
    std::string accountTypeToString(Account::Type type);

    class Color {
    public:
        virtual std::string name() const = 0;
        virtual ~Color() {}
    };

    class Red : public Color {
    public:
        virtual std::string name() const;
    };

    class Blue : public Color {
    public:
        virtual std::string name() const;
    };

    std::string getColorName(const Color &color);
};

#endif

