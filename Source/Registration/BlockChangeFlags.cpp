#include <Core/Common.h>

#include "BlockChangeFlags.h"


BlockChangeFlags::BlockChangeFlags(const Vec3i& block_count)
{
    _flags.resize(2 * (block_count.x + 1) * 2 * (block_count.y + 1) * 2 * (block_count.z + 1));
    std::fill(_flags.begin(), _flags.end(), uint8_t(1));
}
bool BlockChangeFlags::is_block_set(const Vec3i& block_p, bool shift) const
{
    Vec3i sub_block_offset = shift ? Vec3i(0, 0, 0) : Vec3i(1, 1, 1);

    for (int z = 0; z < 2; ++z) 
    {
        for (int y = 0; y < 2; ++y) 
        {
            for (int x = 0; x < 2; ++x) 
            {
                Vec3i sb = 2 * block_p + sub_block_offset + Vec3i(x, y, z);
                if (flag(sb) != 0)
                    return true;
            }
        }
    }
    return false;
}
void BlockChangeFlags::set_block(const Vec3i& block_p, bool changed, bool shift)
{
    // We keep track of the changed flags for both shifted and non-shifted blocks simultaneously.
    // The mask looks like this: <b7> ... <b2> <ShiftFlag> <NonShiftFlag>
    // If any of the flags are set, this indicates the block is changed

    Vec3i sub_block_offset = shift ? Vec3i(0, 0, 0) : Vec3i(1, 1, 1);

    uint8_t this_mask = shift ? 0x2 : 0x1;
    uint8_t opposite_mask = shift ? 0x1 : 0x2;
    for (int i = 0; i < 8; ++i) 
    {
        int x = (i % 2) % 2;
        int y = (i / 2) % 2;
        int z = i / 4;

        Vec3i sb = 2 * block_p + sub_block_offset + Vec3i(x, y, z);
        set(sb, (flag(sb)  & opposite_mask) | ((changed ? this_mask : 0) & this_mask));
    }
}
uint8_t BlockChangeFlags::flag(const Vec3i& subblock_p) const
{
    int i = subblock_p.z * _block_count.x * _block_count.y + subblock_p.y * _block_count.x + subblock_p.x;
    if (i > 0 && i < int(_flags.size()))
        return _flags[subblock_p.z * _block_count.x * _block_count.y + subblock_p.y * _block_count.x + subblock_p.x];
    return 0;
}
void BlockChangeFlags::set(const Vec3i& subblock_p, uint8_t flags)
{
    int i = subblock_p.z * _block_count.x * _block_count.y + subblock_p.y * _block_count.x + subblock_p.x;
    if (i > 0 && i < int(_flags.size()))
        _flags[subblock_p.z * _block_count.x * _block_count.y + subblock_p.y * _block_count.x + subblock_p.x] = flags;
}
