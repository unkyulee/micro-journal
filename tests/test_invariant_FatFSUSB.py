import pytest
import ctypes
import struct


# Simulate the sector buffer and memcpy operation as described in the vulnerable code
SECTOR_SIZE = 512  # Standard sector size in bytes


class SectorBuffer:
    """Simulates the _sectBuff with bounds checking enforcement."""
    
    def __init__(self, size=SECTOR_SIZE):
        self.size = size
        self.buffer = bytearray(size)
    
    def safe_read(self, offset, bufsize):
        """Simulate memcpy(buffer, _sectBuff + offset, bufsize) with bounds check."""
        if offset < 0:
            raise ValueError("Negative offset")
        if bufsize < 0:
            raise ValueError("Negative bufsize")
        if offset + bufsize > self.size:
            raise OverflowError(
                f"Out-of-bounds read: offset={offset} + bufsize={bufsize} = "
                f"{offset + bufsize} exceeds buffer size {self.size}"
            )
        return bytes(self.buffer[offset:offset + bufsize])
    
    def safe_write(self, offset, data, bufsize):
        """Simulate memcpy(_sectBuff + offset, buffer, bufsize) with bounds check."""
        if offset < 0:
            raise ValueError("Negative offset")
        if bufsize < 0:
            raise ValueError("Negative bufsize")
        if offset + bufsize > self.size:
            raise OverflowError(
                f"Out-of-bounds write: offset={offset} + bufsize={bufsize} = "
                f"{offset + bufsize} exceeds buffer size {self.size}"
            )
        self.buffer[offset:offset + bufsize] = data[:bufsize]


def validate_scsi_bounds(offset, bufsize, sector_size=SECTOR_SIZE):
    """
    Security invariant: offset + bufsize must never exceed sector_size.
    Returns True if safe, False if it would cause out-of-bounds access.
    """
    if offset < 0 or bufsize < 0:
        return False
    return (offset + bufsize) <= sector_size


# Adversarial payloads: (offset, bufsize) tuples representing crafted SCSI commands
@pytest.mark.parametrize("payload", [
    # --- Clearly out-of-bounds cases ---
    {"offset": 0, "bufsize": 513},           # bufsize exceeds sector size
    {"offset": 1, "bufsize": 512},           # offset + bufsize = 513 > 512
    {"offset": 512, "bufsize": 1},           # offset at boundary + any data
    {"offset": 511, "bufsize": 2},           # one byte over
    {"offset": 256, "bufsize": 257},         # middle + overflow
    {"offset": 0, "bufsize": 1024},          # double sector size
    {"offset": 0, "bufsize": 0xFFFF},        # large bufsize
    {"offset": 0xFFFF, "bufsize": 1},        # large offset
    {"offset": 0xFFFF, "bufsize": 0xFFFF},   # both large
    {"offset": 512, "bufsize": 512},         # both at boundary
    {"offset": 513, "bufsize": 0},           # offset beyond buffer
    {"offset": 0, "bufsize": 2**31 - 1},     # INT_MAX bufsize
    {"offset": 2**31 - 1, "bufsize": 1},     # INT_MAX offset
    {"offset": 2**32 - 1, "bufsize": 1},     # UINT_MAX offset
    {"offset": 0, "bufsize": 2**32 - 1},     # UINT_MAX bufsize
    {"offset": 2**16, "bufsize": 2**16},     # 64KB each
    {"offset": 1, "bufsize": 2**31},         # near-max bufsize with offset
    {"offset": 255, "bufsize": 258},         # subtle overflow
    {"offset": 500, "bufsize": 13},          # 500+13=513 > 512
    {"offset": 400, "bufsize": 113},         # 400+113=513 > 512
    # --- Integer overflow / wraparound attempts ---
    {"offset": 2**32 - 10, "bufsize": 20},  # wraps around in 32-bit
    {"offset": 2**64 - 1, "bufsize": 1},    # 64-bit max
    {"offset": 0, "bufsize": 2**64 - 1},    # 64-bit max bufsize
    # --- Negative values (should be rejected) ---
    {"offset": -1, "bufsize": 512},
    {"offset": 0, "bufsize": -1},
    {"offset": -512, "bufsize": 512},
    {"offset": -1, "bufsize": -1},
])
def test_sector_buffer_bounds_invariant(payload):
    """
    Invariant: Any memcpy operation on the sector buffer (_sectBuff) using
    offset and bufsize derived from SCSI commands MUST NOT access memory
    beyond the allocated buffer size. The sum (offset + bufsize) must always
    be <= SECTOR_SIZE, and both values must be non-negative.
    
    This guards against out-of-bounds memory access via crafted SCSI READ/WRITE
    commands from a malicious USB host.
    """
    offset = payload["offset"]
    bufsize = payload["bufsize"]
    
    # The security invariant: bounds must be validated before any memcpy
    is_safe = validate_scsi_bounds(offset, bufsize, SECTOR_SIZE)
    
    # All payloads in this test are adversarial — they MUST be rejected
    assert not is_safe, (
        f"SECURITY VIOLATION: offset={offset}, bufsize={bufsize} was incorrectly "
        f"considered safe. offset + bufsize = {offset + bufsize} must not exceed "
        f"sector size {SECTOR_SIZE}. This would allow out-of-bounds memory access."
    )
    
    # Additionally verify that attempting the operation raises an error
    sect_buf = SectorBuffer(SECTOR_SIZE)
    
    with pytest.raises((OverflowError, ValueError)):
        sect_buf.safe_read(offset, bufsize)
    
    with pytest.raises((OverflowError, ValueError)):
        fake_data = b'\xAA' * min(abs(bufsize) if bufsize > 0 else 1, 1024)
        sect_buf.safe_write(offset, fake_data, bufsize)


@pytest.mark.parametrize("payload", [
    # --- Valid/safe cases that MUST be allowed ---
    {"offset": 0, "bufsize": 512},
    {"offset": 0, "bufsize": 0},
    {"offset": 0, "bufsize": 1},
    {"offset": 511, "bufsize": 1},
    {"offset": 256, "bufsize": 256},
    {"offset": 100, "bufsize": 100},
    {"offset": 512, "bufsize": 0},
    {"offset": 0, "bufsize": 511},
])
def test_sector_buffer_valid_access_allowed(payload):
    """
    Invariant: Legitimate SCSI commands with valid offset and bufsize values
    that do not exceed the sector buffer boundary MUST be allowed to proceed
    without error. The security check must not block valid operations.
    """
    offset = payload["offset"]
    bufsize = payload["bufsize"]
    
    is_safe = validate_scsi_bounds(offset, bufsize, SECTOR_SIZE)
    
    assert is_safe, (
        f"REGRESSION: offset={offset}, bufsize={bufsize} was incorrectly rejected. "
        f"offset + bufsize = {offset + bufsize} is within sector size {SECTOR_SIZE}."
    )
    
    sect_buf = SectorBuffer(SECTOR_SIZE)
    
    # These should not raise
    try:
        sect_buf.safe_read(offset, bufsize)
        if bufsize > 0:
            fake_data = b'\x00' * bufsize
            sect_buf.safe_write(offset, fake_data, bufsize)
    except (OverflowError, ValueError) as e:
        pytest.fail(
            f"Valid access was incorrectly blocked: offset={offset}, "
            f"bufsize={bufsize}. Error: {e}"
        )


@pytest.mark.parametrize("offset,bufsize", [
    (0, SECTOR_SIZE + 1),
    (1, SECTOR_SIZE),
    (SECTOR_SIZE // 2, SECTOR_SIZE // 2 + 1),
])
def test_integer_overflow_protection(offset, bufsize):
    """
    Invariant: The bounds check must correctly detect overflow conditions
    even when individual values of offset or bufsize appear reasonable
    but their sum exceeds the buffer boundary.
    """
    # Verify the sum actually overflows the buffer
    assert offset + bufsize > SECTOR_SIZE, (
        f"Test setup error: {offset} + {bufsize} = {offset + bufsize} "
        f"should exceed {SECTOR_SIZE}"
    )
    
    is_safe = validate_scsi_bounds(offset, bufsize, SECTOR_SIZE)
    assert not is_safe, (
        f"SECURITY VIOLATION: Combined offset={offset} + bufsize={bufsize} = "
        f"{offset + bufsize} exceeds sector size {SECTOR_SIZE} but was not detected."
    )