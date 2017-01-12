#ifndef __REGISTRATION_BLOCK_CHANGE_FLAGS_H__
#define __REGISTRATION_BLOCK_CHANGE_FLAGS_H__

#include <Core/Image/Vec3.h>

class BlockChangeFlags
{
public:
    BlockChangeFlags(const Vec3i& block_count);

    bool is_block_set(const Vec3i& block_p, bool shift) const;
    void set_block(const Vec3i& block_p, bool changed, bool shift);

private:
    uint8_t flag(const Vec3i& subblock_p) const;
    void set(const Vec3i& subblock_p, uint8_t flags);

    Vec3i _block_count;
    std::vector<uint8_t> _flags;
};



#endif // __REGISTRATION_BLOCK_CHANGE_FLAGS_H__
