#include <iostream>
#include <thread>
#include "hikang.h"
#include "orbbec.h"

int main()
{
    // HiKangCamera cam_hikang;
    // cam_hikang.Wait4Device();
    // cam_hikang.Init();

    // std::thread cam_hikang_thread = std::thread([&cam_hikang]() {
    //     cam_hikang.Run();
    // });
    // cam_hikang_thread.detach();

    OrbbecCamera cam_orbbec;
    cam_orbbec.Wait4Device();
    cam_orbbec.Init();

    std::thread cam_orbbec_thread = std::thread([&cam_orbbec]() {
        cam_orbbec.Run();
    });
    cam_orbbec_thread.detach();

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        // cam_hikang.SaveImg();
        // cam_hikang.SaveDepth();

        cam_orbbec.SaveImg();
        cam_orbbec.SaveDepth();
    }

    std::cout << "Exiting..." << std::endl;

    return 0;
}
