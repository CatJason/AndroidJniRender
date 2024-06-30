//
// Created by 王之俊 on 2024/6/30.
//

#ifndef ANDROIDJNIRENDER_UNIQUEPTRUTILS_H
#define ANDROIDJNIRENDER_UNIQUEPTRUTILS_H

#include <memory>

namespace std {
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
}


#endif //ANDROIDJNIRENDER_UNIQUEPTRUTILS_H
