
#include <streambuf>
#include <string_view>

namespace dasi::util {

//----------------------------------------------------------------------------------------------------------------------

/// @note A filtering streambuf, to add a specified indent

class IndentStream : public std::streambuf {

public: // methods

    IndentStream(std::ostream& dest, std::string_view indent, bool initialIndent=false) :
        dest_(dest.rdbuf()),
        indent_(indent),
        stream_(dest),
        newline_(initialIndent) {
        stream_.rdbuf(this);
    }

    ~IndentStream() override {
        stream_.rdbuf(dest_);
    }

private: // methods

    int overflow(int ch) override {
        if (newline_ && ch != '\n') {
            dest_->sputn(indent_.c_str(), indent_.size());
        }
        newline_ = (ch == '\n');
        return dest_->sputc(ch);
    }


private: // members

    std::streambuf* dest_;
    std::string indent_;
    std::ostream& stream_;
    bool newline_;
};

//----------------------------------------------------------------------------------------------------------------------

} // namespace dasi
