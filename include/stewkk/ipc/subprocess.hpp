#pragma once

#include <functional>

namespace stewkk::ipc {

class Subprocess {
public:
        // NOTE: мб сделать 2 враппера: ChildWrapper и ParentWrapper и
        // тестировать их отдельно
        // + можно тестировать в одном потоке
        Subprocess(std::function<void()> programm);

        void Wait();
};

}  // namespace stewkk::ipc
