
#include "BlockChangeFlags.h"
#include "GraphCut.h"

template<typename TImage>
BlockedGraphCutOptimizer<TImage>::BlockedGraphCutOptimizer() : 
_epsilon(0.5),
_energy(0.05)
{
}
template<typename TImage>
BlockedGraphCutOptimizer<TImage>::~BlockedGraphCutOptimizer()
{
}
template<typename TImage>
void BlockedGraphCutOptimizer<TImage>::execute(
    const Image* fixed, 
    const Image* moving, 
    int pair_count,
    const ImageUInt8& constraint_mask,
    const ImageVec3d& constraint_values,
    ImageVec3d& def)
{
    PYTHON_STDOUT("Performing registration.\n");

    _energy.set_images(fixed, moving, pair_count);

    Vec3d moving_spacing = moving[0].spacing();
    Vec3d moving_spacing_inv = Vec3d(1.0 / moving_spacing.x, 1.0 / moving_spacing.y, 1.0 / moving_spacing.z);

    int ndims = def.ndims();
    Vec3i dims = def.size();

    Vec3i block_dims = Vec3i(12, 12, 12);
    if (block_dims.x == 0)
        block_dims.x = dims.x;
    if (block_dims.y == 0)
        block_dims.y = dims.y;
    if (block_dims.z == 0)
        block_dims.z = dims.z;

    Vec3i block_count(
        dims.x / block_dims.x,
        dims.y / block_dims.y,
        dims.z / block_dims.z
        );

    // Rest
    Vec3i block_rest = Vec3i(
        dims.x % block_dims.x,
        dims.y % block_dims.y,
        dims.z % block_dims.z
        );

    block_count.x += (block_rest.x > 0 ? 1 : 0);
    block_count.y += (block_rest.y > 0 ? 1 : 0);
    block_count.z += (block_rest.z > 0 ? 1 : 0);

    if (ndims == 2)
    {
        _neighbors[0] = Vec3i(1, 0, 0);
        _neighbors[1] = Vec3i(-1, 0, 0);
        _neighbors[2] = Vec3i(0, 1, 0);
        _neighbors[3] = Vec3i(0, -1, 0);
    }
    else if (ndims == 3)
    {
        _neighbors[0] = Vec3i(1, 0, 0);
        _neighbors[1] = Vec3i(-1, 0, 0);
        _neighbors[2] = Vec3i(0, 1, 0);
        _neighbors[3] = Vec3i(0, -1, 0);
        _neighbors[4] = Vec3i(0, 0, 1);
        _neighbors[5] = Vec3i(0, 0, -1);
    }

    PYTHON_STDOUT("Volume size: %d, %d, %d\n", dims.x, dims.y, dims.z);
    PYTHON_STDOUT("Block count: %d, %d, %d\n", block_count.x, block_count.y, block_count.z);
    PYTHON_STDOUT("Block size: %d, %d, %d\n", block_dims.x, block_dims.y, block_dims.z);
    PYTHON_STDOUT("Block rest: %d, %d, %d\n", block_rest.x, block_rest.y, block_rest.z);

    BlockChangeFlags change_flags(block_count); 

    if (constraint_mask && constraint_values)
    {
        // Set constraint values
        for (int z = 0; z < dims.z; ++z)
        {
            for (int y = 0; y < dims.y; ++y)
            {
                for (int x = 0; x < dims.x; ++x)
                {
                    if (constraint_mask(x, y, z) > 0) 
                    {
                        def(x, y, z) = constraint_values(x, y, z);
                    }
                }
            }
        }
    }

    bool done = false;
    while (!done)
    {
        done = true;

        for (int use_shift = 0; use_shift < 2; ++use_shift)
        {
            if (use_shift == 1 && (block_count.x * block_count.y * block_count.z) <= 1)
                continue;

            Vec3i block_offset(0, 0, 0);
            if (use_shift == 1) 
            {
                block_offset.x = (block_dims.x / 2);
                block_offset.y = (block_dims.y / 2);
                block_offset.z = (block_dims.z / 2);
            }

            for (int black_or_red = 0; black_or_red < 2; black_or_red++)
            {
                Vec3i real_block_count = Vec3i(
                    block_count.x + (use_shift == 1 ? 1 : 0),
                    block_count.y + (use_shift == 1 ? 1 : 0),
                    block_count.z + (use_shift == 1 ? 1 : 0));

                int num_blocks = real_block_count.x * real_block_count.y * real_block_count.z;

#pragma omp parallel for
                for (int block_idx = 0; block_idx < num_blocks; ++block_idx)
                {
                    int block_x = (block_idx % real_block_count.x) % real_block_count.y;
                    int block_y = (block_idx / real_block_count.x) % real_block_count.y;
                    int block_z = block_idx / (real_block_count.x*real_block_count.y);

                    int off = (block_z) % 2;
                    off = (block_y + off) % 2;
                    off = (block_x + off) % 2;

                    if (off != black_or_red)
                    {
                        continue;
                    }

                    Vec3i block_p(block_x, block_y, block_z);

                    bool need_update = change_flags.is_block_set(block_p, use_shift == 1);
                    int n_count = ndims == 3 ? 6 : 4;
                    for (int n = 0; n < n_count; ++n)
                    {
                        need_update = need_update || change_flags.is_block_set(block_p + _neighbors[n], use_shift == 1);
                    }

                    if (!need_update)
                    {
                        continue;
                    }

                    bool block_changed = false;
                    for (int n = 0; n < n_count; ++n)
                    {
                        Vec3d delta = _epsilon * Vec3d(
                            moving_spacing_inv.x * _neighbors[n].x,
                            moving_spacing_inv.y * _neighbors[n].y,
                            moving_spacing_inv.z * _neighbors[n].z
                            );

                        block_changed |= do_block(
                            block_p,
                            block_dims,
                            block_offset,
                            delta,
                            constraint_mask,
                            def);

                    }
                    change_flags.set_block(block_p, block_changed, use_shift == 1);
                    done = done && !block_changed;
                }
            }
        }
    }

    //cout << "Energy: " << calculate_energy(l, deformation) << endl;

}

template<typename TImage>
bool BlockedGraphCutOptimizer<TImage>::do_block(
    const Vec3i& block_p,
    const Vec3i& block_dims,
    const Vec3i& block_offset,
    const Vec3d& delta,
    const ImageUInt8& constraint_mask,
    ImageVec3d& def)
{
    Vec3i dims = def.size();

    GraphCut<double> graph(block_dims);

    double cost = 0;
    {
        for (int sub_z = 0; sub_z < block_dims.z; sub_z++)
        {
            for (int sub_y = 0; sub_y < block_dims.y; sub_y++)
            {
                for (int sub_x = 0; sub_x < block_dims.x; sub_x++)
                {
                    // Global coordinates
                    int gx = block_p.x * block_dims.x - block_offset.x + sub_x;
                    int gy = block_p.y * block_dims.y - block_offset.y + sub_y;
                    int gz = block_p.z * block_dims.z - block_offset.z + sub_z;

                    // Skip voxels outside volume
                    if (gx < 0 || gx >= dims.x ||
                        gy < 0 || gy >= dims.y ||
                        gz < 0 || gz >= dims.z)
                    {
                        graph.add_term1(sub_x, sub_y, sub_z, 0, 0);
                        continue;
                    }

                    Vec3i p(gx, gy, gz);
                    Vec3d def1 = def(p);

                    double f0;
                    double f1;

                    if (constraint_mask(p) > 0)
                    {
                        // Constrained values should not move
                        f0 = 0;
                        f1 = 1000000;
                    }
                    else
                    {
                        f0 = _energy.unary_term(p, def1);
                        f1 = _energy.unary_term(p, def1 + delta);
                    }

                    // Block borders (excl image borders) (T-weights with binary term for neighboring voxels)

                    if (sub_x == 0 && gx != 0)
                    {
                        Vec3i step(-1, 0, 0);
                        Vec3d& def2 = def(gx - 1, gy, gz);
                        f0 += _energy.binary_term(def1, def2, step);
                        f1 += _energy.binary_term(def1 + delta, def2, step);
                    }
                    else if (sub_x == block_dims.x - 1 && gx < dims.x - 1)
                    {
                        Vec3i step(1, 0, 0);
                        Vec3d& def2 = def(gx + 1, gy, gz);
                        f0 += _energy.binary_term(def1, def2, step);
                        f1 += _energy.binary_term(def1 + delta, def2, step);
                    }

                    if (sub_y == 0 && gy != 0)
                    {
                        Vec3i step(0, -1, 0);
                        Vec3d& def2 = def(gx, gy - 1, gz);
                        f0 += _energy.binary_term(def1, def2, step);
                        f1 += _energy.binary_term(def1 + delta, def2, step);
                    }
                    else if (sub_y == block_dims.y - 1 && gy < dims.y - 1)
                    {
                        Vec3i step(0, 1, 0);
                        Vec3d& def2 = def(gx, gy + 1, gz);
                        f0 += _energy.binary_term(def1, def2, step);
                        f1 += _energy.binary_term(def1 + delta, def2, step);
                    }

                    if (sub_z == 0 && gz != 0)
                    {
                        Vec3i step(0, 0, -1);
                        Vec3d& def2 = def(gx, gy, gz - 1);
                        f0 += _energy.binary_term(def1, def2, step);
                        f1 += _energy.binary_term(def1 + delta, def2, step);
                    }
                    else if (sub_z == block_dims.z - 1 && gz < dims.z - 1)
                    {
                        Vec3i step(0, 0, 1);
                        Vec3d& def2 = def(gx, gy, gz + 1);
                        f0 += _energy.binary_term(def1, def2, step);
                        f1 += _energy.binary_term(def1 + delta, def2, step);
                    }

                    graph.add_term1(sub_x, sub_y, sub_z, f0, f1);

                    cost += f0;

                    if (sub_x + 1 < block_dims.x && gx + 1 < dims.x)
                    {
                        Vec3i step(1, 0, 0);
                        Vec3d& def2 = def(p + step);
                        double f_same = _energy.binary_term(def1, def2, step);
                        double f01 = _energy.binary_term(def1, def2 + delta, step);
                        double f10 = _energy.binary_term(def1 + delta, def2, step);

                        graph.add_term2(
                            sub_x, sub_y, sub_z,
                            sub_x + 1, sub_y, sub_z, 
                            f_same, f01, f10, f_same);

                        cost += f_same;
                    }
                    if (sub_y + 1 < block_dims.y && gy + 1 < dims.y)
                    {
                        Vec3i step(0, 1, 0);
                        Vec3d& def2 = def(p + step);
                        double f_same = _energy.binary_term(def1, def2, step);
                        double f01 = _energy.binary_term(def1, def2 + delta, step);
                        double f10 = _energy.binary_term(def1 + delta, def2, step);

                        graph.add_term2(
                            sub_x, sub_y, sub_z,
                            sub_x, sub_y + 1, sub_z,
                            f_same, f01, f10, f_same);

                        cost += f_same;
                    }
                    if (sub_z + 1 < block_dims.z && gz + 1 < dims.z)
                    {
                        Vec3i step(0, 0, 1);
                        Vec3d& def2 = def(p + step);
                        double f_same = _energy.binary_term(def1, def2, step);
                        double f01 = _energy.binary_term(def1, def2 + delta, step);
                        double f10 = _energy.binary_term(def1 + delta, def2, step);

                        graph.add_term2(
                            sub_x, sub_y, sub_z,
                            sub_x, sub_y, sub_z + 1,
                            f_same, f01, f10, f_same);

                        cost += f_same;
                    }
                }
            }
        }
    }

    double current_emin = graph.minimize();

    bool changed_flag = false;
    if (current_emin + 0.00001 < cost) //Accept solution
    {
        for (int sub_z = 0; sub_z < block_dims.z; sub_z++)
        {
            for (int sub_y = 0; sub_y < block_dims.y; sub_y++)
            {
                for (int sub_x = 0; sub_x < block_dims.x; sub_x++)
                {
                    // Global coordinates
                    int gx = block_p.x * block_dims.x - block_offset.x + sub_x;
                    int gy = block_p.y * block_dims.y - block_offset.y + sub_y;
                    int gz = block_p.z * block_dims.z - block_offset.z + sub_z;

                    // Skip voxels outside volume
                    if (gx < 0 || gx >= dims.x ||
                        gy < 0 || gy >= dims.y ||
                        gz < 0 || gz >= dims.z)
                    {
                        continue;
                    }

                    Vec3i p(gx, gy, gz);
                    if (constraint_mask(p) > 0) {
                        continue;
                    }

                    if (graph.get_var(sub_x, sub_y, sub_z) == 1)
                    {
                        def(gx, gy, gz) += delta;
                        changed_flag = true;
                    }
                }
            }
        }
    }

    return changed_flag;
}
