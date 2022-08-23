#ifndef ADRENALINE_PARTICLE_TICK_FUNCTIONS_H
#define ADRENALINE_PARTICLE_TICK_FUNCTIONS_H

#include "ParticleManager.h"
#include "context.h"

void PARTICLE_TICK_NORMAL_WHITE(Context const &context,
                                Particle &particle) noexcept;
void PARTICLE_TICK_NORMAL_FIRE(Context const &context,
                               Particle &particle) noexcept;
void PARTICLE_TICK_NORMAL_RED(Context const &context,
                              Particle &particle) noexcept;
void PARTICLE_TICK_NORMAL_YELLOW(Context const &context,
                                 Particle &particle) noexcept;

#endif  // ADRENALINE_PARTICLE_TICK_FUNCTIONS_H
