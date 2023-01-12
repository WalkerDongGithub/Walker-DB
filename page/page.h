#ifndef PAGE_H
#define PAGE_H

#include "../definition.h"


class Page {

public:
    Page() {
        ResetMemory();
    }

    inline Ptr GetData() { return data; }

    inline const Ptr GetConstData() const { return (const Ptr)data; }

private:

    inline void ResetMemory() {
        memset(data, 0xff, PAGE_SIZE);
    }

    // members
    Byte data[PAGE_SIZE]; // actual data

};

#endif