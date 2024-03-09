#pragma once

#include <cstdlib>
#include <ctime>

namespace nell {
    void CPURandomInit() {
        srand(time(NULL));
    }

    float GetCPURandom() {
        return (float)rand() / (RAND_MAX + 1.0);
    }
}