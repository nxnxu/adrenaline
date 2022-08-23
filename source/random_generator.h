#ifndef RANDOM_GENERATOR_H
#define RANDOM_GENERATOR_H

#include <random>

#include "context.h"

class RandomGenerator {
   public:
    std::random_device m_rd{};
    std::seed_seq m_seed{m_rd(), m_rd(), m_rd(), m_rd(), m_rd()};
    std::mt19937 m_gen{m_seed};
    RandomGenerator() = default;
    void tick(Context const& context) noexcept;
};

extern RandomGenerator random_generator;
#endif  // RANDOM_GENERATOR_H
