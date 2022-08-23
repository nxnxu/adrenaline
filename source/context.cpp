#include "context.h"

#include "window.h"

Context::Context() {
    /*initialize frames_dt with some arbitary dt*/
    for (size_t i = 0; i < size_; ++i) {
        frames_dt_[i] = 10;
        frame_dt_sum_ += frames_dt_[i];
    }
}

void Context::tick() noexcept {
    /*remove oldest frame_dt and add current frame_dt in its place*/
    frame_dt_sum_ -= frames_dt_[index_];
    frames_dt_[index_] =
        m_window->m_this_frame_time - m_window->m_last_frame_time;
    frame_dt_sum_ += frames_dt_[index_];
    index_ = (index_ + 1) % size_;

    m_window_width = m_window->m_width;
    m_window_height = m_window->m_height;
    m_this_frame_time = m_window->m_this_frame_time;

    /* current dt is equal to avg of last size_ frames */
    m_dt = static_cast<uint32_t>(frame_dt_sum_ / size_);

    m_fps = m_dt ? (1000 / m_dt) : 0;
    m_window_resized = m_window->m_window_resized;
}