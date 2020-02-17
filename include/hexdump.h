
#ifndef HEXDUMP_H
#define HEXDUMP_H

#include <iostream>

std::ostream& hex_dump(std::ostream& os, const void *buffer,
                       std::size_t bufsize, bool showPrintableChars = true);


#endif // hexdump_h
