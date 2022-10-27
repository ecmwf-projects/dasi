
/// @author Simon Smart
/// @date   Oct 2022

#pragma once

#include "dasi/api/Key.h"

#include "eckit/filesystem/URI.h"

#include <iterator>


namespace dasi {

//-------------------------------------------------------------------------------------------------

struct DataLocation {
    eckit::URI uri;
    eckit::Offset offset;
    eckit::Length length;
};

//-------------------------------------------------------------------------------------------------

struct ListElement {
    Key key;
    DataLocation location;
    time_t timestamp;

private: // members

    friend std::ostream& operator<<(std::ostream& s, const ListElement& elem) {
        elem.print(s);
        return s;
    };

    void print(std::ostream& s) const;
};

//-------------------------------------------------------------------------------------------------

// TODO: Template this for other API calls

class ListGeneratorImpl;

class ListGeneratorIterator : std::iterator<std::input_iterator_tag,
                                        const ListElement,
                                        std::ptrdiff_t,
                                        const ListElement*,
                                        const ListElement> {

public: // methods
    explicit ListGeneratorIterator(ListGeneratorImpl& generator);

    ListGeneratorIterator& operator++();
    const ListElement& operator*() const;

    [[ nodiscard ]]
    bool done() const;

private: // members
    ListGeneratorImpl& generator_;
};


class GeneratorIteratorSentinel {
    template <typename Lhs>
    friend bool operator!=(const Lhs& lhs, const GeneratorIteratorSentinel& rhs) { return !lhs.done(); }
    template <typename Rhs>
    friend bool operator!=(const GeneratorIteratorSentinel& lhs, const Rhs& rhs) { return !rhs.done(); }
};


//-------------------------------------------------------------------------------------------------

class ListGenerator {

public: // methods

    explicit ListGenerator(std::unique_ptr<ListGeneratorImpl>&& impl);
    ~ListGenerator();

    ListGeneratorIterator begin() { return ListGeneratorIterator{*impl_}; }
    static GeneratorIteratorSentinel end() { return {}; }

private: // members
    std::unique_ptr<ListGeneratorImpl> impl_;
};

//-------------------------------------------------------------------------------------------------

} // namespace dasi

