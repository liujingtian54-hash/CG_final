//#include "hello_triangle.h"
#include "core.h"
#include <filesystem>
#include <iostream>

Options getOptions(int argc, char* argv[]) {
    Options options;
    options.windowTitle = "Final Progect";
    options.windowWidth = 1280;
    options.windowHeight = 720;
    options.windowResizable = true;
    options.vSync = true;
    options.glVersion = {3, 3};
    options.backgroundColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);

    return options;
}

int main(int argc, char* argv[]) {
    Options options = getOptions(argc, argv);
//    std::cout << std::filesystem::current_path() << std::endl;
    try {
        Core app(options);
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        exit(EXIT_FAILURE);
    } catch (...) {
        std::cerr << "Unknown exception" << std::endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}