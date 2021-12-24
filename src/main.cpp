#include "client.h"

#include "util/io.h"

int
main() noexcept {
    Flusher f1(serr);
    Flusher f0(sout);

    return client();
}
