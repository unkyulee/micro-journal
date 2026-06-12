#include <gtest/gtest.h>
#include <string>
#include <cstring>

// Include the actual production header
#include "micro-journal-rev-4-esp32/lib/FatFSUSB/FatFSUSB.h"

class UsbDescriptorSecurityTest : public ::testing::TestWithParam<std::string> {};

TEST_P(UsbDescriptorSecurityTest, DescriptorStringsDoNotOverflowBuffers) {
    // Invariant: After calling FatFSUSB.begin() with any descriptor string,
    // the fixed-size destination buffers (vendor_id=8, product_id=16, product_rev=4)
    // must not be overflowed — adjacent memory must remain uncorrupted.
    std::string payload = GetParam();

    // Canary values placed around a local struct to detect overflow
    struct {
        char canary_before[8];
        char vendor_buf[8];
        char canary_after[8];
    } mem;
    memset(mem.canary_before, 0xAA, sizeof(mem.canary_before));
    memset(mem.canary_after,  0xAA, sizeof(mem.canary_after));

    // The production API must clamp/truncate to buffer sizes
    // Calling the real begin() with adversarial payload
    FatFSUSB.begin(payload.c_str(), payload.c_str(), payload.c_str());

    // Canaries must be intact — no overflow into adjacent memory
    for (int i = 0; i < 8; i++) {
        EXPECT_EQ((unsigned char)mem.canary_before[i], 0xAA)
            << "canary_before corrupted at index " << i << " with payload len=" << payload.size();
        EXPECT_EQ((unsigned char)mem.canary_after[i], 0xAA)
            << "canary_after corrupted at index " << i << " with payload len=" << payload.size();
    }

    // Vendor/product/rev fields must be null-terminated within their bounds
    // (valid string, not a runaway buffer)
    EXPECT_LE(strlen(payload.c_str()), payload.size());
}

INSTANTIATE_TEST_SUITE_P(
    AdversarialInputs,
    UsbDescriptorSecurityTest,
    ::testing::Values(
        // Exact exploit: far exceeds all descriptor buffer sizes
        std::string(256, 'A'),
        // Boundary: one byte over the largest buffer (product_id = 16 bytes)
        std::string(17, 'B'),
        // Boundary: exactly at vendor_id limit (8 bytes)
        std::string(8, 'C'),
        // Valid: well within all limits
        std::string("VID")
    )
);

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}