#ifndef ETH_CLIENT_HPP
#define ETH_CLIENT_HPP

#include <array>
#include <cstdint>

bool recordWipeOnChain(
    const std::array<uint8_t,32>& certHash,
    const std::array<uint8_t,32>& deviceHash,
    uint8_t wipeMethod
);

#endif
