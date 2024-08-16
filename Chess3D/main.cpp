#define STB_IMAGE_IMPLEMENTATION

#include "SceneManager.h"

int main() {
    try {
        SceneManager scene;
        if (0 == scene.init()) {
            return scene.run();
        }
    }
    catch (std::exception& e) {
        std::cout << e.what();
    }
    return 1;
}