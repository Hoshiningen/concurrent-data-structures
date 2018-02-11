#pragma once

#include <memory>

namespace utility {

template<typename T, typename ...Args>
inline std::unique_ptr<T> make_unique(Args&& ...args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}
}  // namespace utility
