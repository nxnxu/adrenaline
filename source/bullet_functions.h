#ifndef ADRENALINE_BULLET_FUNCTIONS_H
#define ADRENALINE_BULLET_FUNCTIONS_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>

#include "ParticleManager.h"
#include "PrimitiveManager.h"
#include "bullet.h"
#include "context.h"
#include "helper.h"
#include "particle_tick_functions.h"

void BULLET_WL45_TICK(Context const &context, Bullet &bullet,
                      ParticleManager &pm);
void BULLET_WL45_HIT(Context const &context, Bullet &bullet,
                     glm::vec3 const &hit, ParticleManager &pm);

void BULLET_EB95_TICK(Context const &context, Bullet &bullet,
                      ParticleManager &pm);
void BULLET_EB95_HIT(Context const &context, Bullet &bullet,
                     glm::vec3 const &hit, ParticleManager &pm);

void BULLET_RL96_TICK(Context const &context, Bullet &bullet,
                      ParticleManager &pm);
void BULLET_RL96_HIT(Context const &context, Bullet &bullet,
                     glm::vec3 const &hit, ParticleManager &pm);

#endif  // ADRENALINE_BULLET_FUNCTIONS_H
