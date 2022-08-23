#ifndef HELPER_H
#define HELPER_H

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>

#ifdef _WIN32
std::string get_path_separator() { return std::string{"\\"}; }

#elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
inline std::string get_path_separator() { return std::string{"/"}; }
#endif

template <typename T>
inline T clamp(T x, T minx, T maxx) noexcept {
    return std::min(std::max(x, minx), maxx);
}

template <typename T>
inline T smoothstep(T edge0, T edge1, T x) noexcept {
    T t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

template <typename T>
inline glm::vec3 vec_to_ndc(glm::mat4 const& proj, T vec) {
    auto tmp = proj * glm::vec4{vec.x, vec.y, vec.z, 1.0f};
    tmp.x /= tmp.w;
    tmp.y /= tmp.w;
    tmp.z /= tmp.w;
    tmp.w /= tmp.w;
    return tmp;
}

template <typename T>
/*return rotation angle in degress, to align a with b in xz plane*/
inline auto vecxz_align_angle(T const& a, T const& b) ->
    typename T::value_type {
    /*
     * direct python snippet for, finding align angle in xz space for 2 vec in
     left hand system 
     def vecxz_align_angle(vec1, vec2):   # return align angle in degrees 
        a1 = degrees(atan2(vec1.z, vec1.x)) 
        a2 = degrees(atan2(vec2.z, vec2.x)) 
        if (a1 >= 0 and a2 >= 0) or (a1 <= 0 and a2 <= 0):
            return a2 - a1
        else:
            a1 = abs(a1)
            a2 = abs(a2)
            mina = (180 - a1) + (180 - a2)
            minb = a1 + a2
            if mina <= minb:
                return mina
            return -minb
    */

    /*convert right hand vec to left hand vec by inverting z coordinate*/
    typename T::value_type x = a.x * b.x + (-1 * a.z) * (-1 * b.z);
    typename T::value_type z = a.x * (-1 * b.z) - (-1 * a.z) * b.x;
    return std::atan2(z, x);
}

template <typename T>
inline bool absoluteToleranceCompare(T x, T y) {
    return std::fabs(x - y) <= std::numeric_limits<T>::epsilon();
}

template <typename T>
inline bool relativeToleranceCompare(T x, T y) {
    float maxXY = std::max(std::fabs(x), std::fabs(y));
    return std::fabs(x - y) <= std::numeric_limits<T>::epsilon() * maxXY;
}

template <typename T>
inline bool combinedToleranceCompare(T x, T y) {
    float maxXYOne = std::max({1.0f, std::fabs(x), std::fabs(y)});
    return std::fabs(x - y) <= std::numeric_limits<T>::epsilon() * maxXYOne;
}

inline uint64_t round_next_pow2(uint64_t v) {
    /*
     * http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
     * */
    v -= 1;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v |= v >> 32;
    v += 1;
    return v;
}

inline float equal_width_ndc_points(float width) {
    static auto rot_mat = glm::rotate(glm::mat4{1.0f}, glm::radians(90.0f),
                                      glm::vec3{0.0f, 0.0f, 1.0f});
    return (rot_mat * glm::vec4{width, 0.0f, 0.0f, 0.0f}).y;
}

inline glm::vec3 nearest(glm::vec3 const& base, glm::vec3 const& p1,
                         glm::vec3 const& p2) {
    /*
     * return nearest points (p1, or p2) to base
     * */

    float a = glm::length(p1 - base);
    float b = glm::length(p2 - base);

    if (a < b) {
        return p1;
    }

    return p2;
}

inline int nearest(glm::vec3 const& base, glm::vec3 const& p1,
                   glm::vec3 const& p2, glm::vec3& nearest) {
    /*
     * find nearest point (p1, or p2) to base and strore its value in nearest,
     * and return which one is nearest, p1 = 0 and p2 = 1
     * */

    float a = glm::length(p1 - base);
    float b = glm::length(p2 - base);

    if (a < b) {
        nearest = p1;
        return 0;
    }

    nearest = p2;
    return 1;
}

template <typename T>
void fill_num(char* buffer, size_t pos, size_t max_len, T num) {
    for (size_t i = 0; i < max_len; ++i) {
        auto index = pos + max_len - 1 - i;
        if (num) {
            buffer[index] = static_cast<char>('0' + (num % 10));
            num /= 10;
        } else {
            buffer[index] = '0';
        }
    }
}

class WindowTitleMaker {
   public:
    WindowTitleMaker(size_t max_size) : max_size_{max_size} {
        assert(max_size);
        buffer_.resize(max_size + 1);
    }

    template <typename T>
    void add(const char* key, T value, size_t max_digits) noexcept {
        /*
         *   'K' 'E' 'Y' 'V' 'A' 'L' 'U' 'E' ' '
         *
         * */

        size_t need_space = strlen(key) + max_digits + 1;
        if (need_space > max_size_ - len_) {
            assert(0);
        }

        for (size_t i = 0; key[i]; ++i) {
            buffer_[len_++] = key[i];
        }

        fill_num(&buffer_[0], len_, max_digits, value);
        len_ += max_digits;

        buffer_[len_++] = ' ';
        buffer_[len_] = '\0';
    }

    void add(const char* msg) noexcept {
        if (strlen(msg) + 1 > max_size_ - len_) {
            assert(0);
        }

        for (size_t i = 0; msg[i]; ++i) {
            buffer_[len_++] = msg[i];
        }

        buffer_[len_++] = ' ';
        buffer_[len_] = '\0';
    }

    char const* buffer() noexcept { return &buffer_[0]; }

    void clean() noexcept { len_ = 0; }

   private:
    size_t len_{};
    size_t max_size_{};

    std::vector<char> buffer_;
};

inline void calculate_time(uint32_t milli, uint32_t& hour, uint32_t& minutes,
                           uint32_t& seconds) {
    hour = milli / (60 * 60);
    minutes = (milli - hour * 60 * 60) / 60;
    seconds = ((milli - hour * 60 * 60) - minutes * 60);
}

inline void print_float(const char* msg, float item) {
    fprintf(stderr, "%s: %.6f\n", msg, item);
}

inline void print_vec(const char* msg, glm::vec3 const& vec) {
    fprintf(stderr, "%s (%.6f, %.6f, %.6f)\n", msg, vec.x, vec.y, vec.z);
}

inline void print_vec(const char* msg, glm::vec4 const& vec) {
    fprintf(stderr, "%s (%.6f, %.6f, %.6f %.6f)\n", msg, vec.x, vec.y, vec.z,
            vec.w);
}

inline void print_vec(const char* msg, glm::vec2 const& vec) {
    fprintf(stderr, "%s (%.6f, %.6f)\n", msg, vec.x, vec.y);
}

inline void print_line() { fprintf(stderr, "\n"); }

inline void print_msg(const char* msg) { fprintf(stderr, "%s\n", msg); }

inline void print_mat4(const char* msg, glm::mat4 const& mat) {
    print_msg(msg);
    for (int i = 0; i < 4; ++i) {
        print_vec("col: ", mat[i]);
    }
}

#endif  // HELPER_H
