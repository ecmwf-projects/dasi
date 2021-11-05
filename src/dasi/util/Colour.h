#pragma once

#include <iosfwd>


namespace dasi {

//----------------------------------------------------------------------------------------------------------------------

class Colour {
public:
    static std::ostream& on(std::ostream&);
    static std::ostream& off(std::ostream&);

    static std::ostream& reset(std::ostream&);

    static std::ostream& bold(std::ostream&);
    static std::ostream& underline(std::ostream&);
    static std::ostream& blink(std::ostream&);
    static std::ostream& reverse(std::ostream&);
    static std::ostream& hidden(std::ostream&);

    static std::ostream& black(std::ostream&);
    static std::ostream& red(std::ostream&);
    static std::ostream& green(std::ostream&);
    static std::ostream& yellow(std::ostream&);
    static std::ostream& magenta(std::ostream&);
    static std::ostream& blue(std::ostream&);
    static std::ostream& cyan(std::ostream&);
    static std::ostream& white(std::ostream&);

    static std::ostream& blackBackground(std::ostream&);
    static std::ostream& redBackground(std::ostream&);
    static std::ostream& greenBackground(std::ostream&);
    static std::ostream& yellowBackground(std::ostream&);
    static std::ostream& magentaBackground(std::ostream&);
    static std::ostream& blueBackground(std::ostream&);
    static std::ostream& cyanBackground(std::ostream&);
    static std::ostream& whiteBackground(std::ostream&);
};

//----------------------------------------------------------------------------------------------------------------------

}  // namespace dasi
