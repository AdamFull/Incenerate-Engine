#include "SparseVoxelOctree.h"

using namespace engine;

template <typename Int, std::enable_if_t<(std::is_integral_v<Int>), int> = 0>
constexpr Int div_ceil(Int x, Int y)
{
    return 1 + ((x - 1) / y);
}

constexpr uint64_t dupl_bits_naive(uint64_t input, size_t bits)
{
    if (bits == 0) {
        return 0;
    }
    const auto lim = div_ceil<size_t>(64, bits);

    uint64_t result = 0;
    for (size_t i = 0, b_out = 0; i < lim; ++i) {
        for (size_t j = 0; j < bits; ++j, ++b_out) {
            result |= static_cast<uint64_t>((input >> i) & 1) << b_out;
        }
    }

    return result;
}

constexpr uint64_t ileave_zeros_naive(uint32_t input, size_t bits)
{
    const auto lim = std::min<size_t>(32, div_ceil<size_t>(64, bits + 1));

    uint64_t result = 0;
    for (size_t i = 0, b_out = 0; i < lim; ++i) {
        result |= static_cast<uint64_t>(input & 1) << b_out;
        input >>= 1;
        b_out += bits + 1;
    }

    return result;
}

template <size_t BITS>
constexpr uint64_t ileave_zeros(uint32_t input)
{
    if constexpr (BITS == 0) {
        return input;
    }
    else {
        constexpr uint64_t MASKS[] = {
            dupl_bits_naive(ileave_zeros_naive(uint32_t(-1), BITS), 1),
            dupl_bits_naive(ileave_zeros_naive(uint32_t(-1), BITS), 2),
            dupl_bits_naive(ileave_zeros_naive(uint32_t(-1), BITS), 4),
            dupl_bits_naive(ileave_zeros_naive(uint32_t(-1), BITS), 8),
            dupl_bits_naive(ileave_zeros_naive(uint32_t(-1), BITS), 16),
        };

        uint64_t n = input;
        for (int i = 4; i != -1; --i) {
            const auto shift = BITS * (1 << i);
            n |= n << shift;
            n &= MASKS[i];
        }

        return n;
    }
}

constexpr uint64_t ileave3(uint32_t x, uint32_t y, uint32_t z)
{
    return (ileave_zeros<2>(x) << 2) | (ileave_zeros<2>(y) << 1) | ileave_zeros<2>(z);
}

CSparseVoxelOctree::CSparseVoxelOctree()
{
	root = std::make_unique<FOctreeBranch>();
}

void CSparseVoxelOctree::insert(const octreevec_t& pos, uint32_t color)
{
    ensureSpace(pos);
    auto octreeNodeIndex = indexOf(pos);
    insert(octreeNodeIndex, color);
}

octreevec_t CSparseVoxelOctree::minIncl() const
{
    return octreevec_t(-(1ull << depth));
}

octreevec_t CSparseVoxelOctree::maxIncl() const
{
    return octreevec_t((1ull << depth) - 1ull);
}

octreevec_t CSparseVoxelOctree::minExcl() const
{
    return octreevec_t(-(1ull << depth) - 1ull);
}

octreevec_t CSparseVoxelOctree::maxExcl() const
{
    return octreevec_t(1ull << depth);
}

uint32_t& CSparseVoxelOctree::operator[](const octreevec_t& pos)
{
    ensureSpace(pos);
    auto octreeNodeIndex = indexOf(pos);
    return findOrCreate(octreeNodeIndex);
}

uint32_t& CSparseVoxelOctree::at(const octreevec_t& pos)
{
    auto lim = boundsTest(pos);
    assert(lim == 0);
    auto octreeNodeIndex = indexOf(pos);
    auto* result = find(octreeNodeIndex);
    assert(result != nullptr);
    return *result;
}

const uint32_t& CSparseVoxelOctree::at(const octreevec_t& pos) const
{
    auto lim = boundsTest(pos);
    assert(lim == 0);
    auto octreeNodeIndex = indexOf(pos);
    auto* result = find(octreeNodeIndex);
    assert(result != nullptr);
    return *result;
}

uint32_t& CSparseVoxelOctree::findOrCreate(uint64_t octreeNodeIndex)
{
    FOctreeNode* node = root.get();
    for (size_t s = depth * 3; s != size_t(-3); s -= 3) 
    {
        uint32_t octDigit = (octreeNodeIndex >> s) & 0b111;
        if (s != 0) 
        {
            auto* branch = static_cast<FOctreeBranch*>(node);
            if (branch->children[octDigit] != nullptr) 
                node = branch->children[octDigit].get();
            else 
            {
                auto* child = s == 3 ? static_cast<FOctreeNode*>(new FOctreeLeaf)
                    : static_cast<FOctreeNode*>(new FOctreeBranch);
                node = child;
                branch->children[octDigit] = std::unique_ptr<FOctreeNode>{ child };
            }
        }
        else 
        {
            auto* leaf = static_cast<FOctreeLeaf*>(node);
            return leaf->data[octDigit];
        }
    }
    assert(false && "Unreacheble diapasone.");
}

uint32_t* CSparseVoxelOctree::find(uint64_t octreeNodeIndex) const
{
    FOctreeNode* node = root.get();
    for (size_t s = depth * 3; s != size_t(-3); s -= 3) 
    {
        uint32_t octDigit = (octreeNodeIndex >> s) & 0b111;
        if (s != 0) 
        {
            auto* branch = static_cast<FOctreeBranch*>(node);
            if (branch->children[octDigit] == nullptr) 
            {
                return nullptr;
            }
            else 
            {
                node = branch->children[octDigit].get();
            }
        }
        else 
        {
            auto* leaf = static_cast<FOctreeLeaf*>(node);
            return &leaf->data[octDigit];
        }
    }
    assert(false && "Unreacheble diapasone.");
}

uint64_t CSparseVoxelOctree::indexOf(const octreevec_t& pos) const
{
    auto uPos = octreevec_t(pos - minIncl());
    return ileave3(uPos[0], uPos[1], uPos[2]);
}

void CSparseVoxelOctree::ensureSpace(const octreevec_t& pos)
{
    if (uint32_t lim = boundsTest(pos); lim != 0)
        grow(lim);
}

void CSparseVoxelOctree::insert(uint64_t octreeNodeIndex, uint32_t color)
{
    findOrCreate(octreeNodeIndex) = color;
}

void CSparseVoxelOctree::grow(uint32_t lim)
{
	for (size_t size = 1ull << depth; size <= lim; depth <<= 1ull, size = 1ull << depth)
		growOnce();
}

void CSparseVoxelOctree::growOnce()
{
    for (size_t i = 0; i < 8; ++i) 
    {
        if (root->children[i] == nullptr)
            continue;

        auto parent = std::make_unique<FOctreeBranch>();
        parent->children[~i & 0b111] = std::move(root->children[i]);
        root->children[i] = std::move(parent);
    }
}

uint32_t CSparseVoxelOctree::boundsTest(const octreevec_t& v) const
{
    constexpr auto absForBoundsTest = [](int32_t x) -> uint32_t 
    {
        return static_cast<uint32_t>(x < 0 ? -x - 1 : x);
    };

    static_assert (absForBoundsTest(-5) == 4);
    uint32_t max = absForBoundsTest(v[0]) | absForBoundsTest(v[1]) | absForBoundsTest(v[2]);
    return max >= (1u << depth) ? max : 0;
}