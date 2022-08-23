#ifndef APP_ACTIONS_H
#define APP_ACTIONS_H

#include "input.h"

struct Actions {
    Event m_fire;
    Event m_next_weapon;
    Event m_prev_weapon;
    Event m_home_view_increase;
    Event m_home_view_decrease;
    Event m_home_view_play;
    Event m_move_forward;
    Event m_move_backward;
    Event m_move_left;
    Event m_move_right;
    Event m_quit_game;
    Event m_quit_app;
    Event m_restart_game;
};

#endif  // APP_ACTIONS_H
