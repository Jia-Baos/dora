#include "dora-node-api.h"

#include <iostream>
#include <vector>

#include <opencv2/opencv.hpp>
#include "../camera2D/hikang/hikang.h"
#include "../camera2D/orbbec/orbbec.h"

int main()
{
    std::cout << "HELLO FROM C++" << std::endl;
    unsigned char counter = 0;

    auto dora_node = init_dora_node();

    OrbbecCamera camera;
    bool ok_flag = true;
    if (ok_flag = camera.Wait4Device(); ok_flag == false)
    {
        std::cout << "Wait4Device() failed" << std::endl;
        return 0;
    }
    if (ok_flag = camera.Init(); ok_flag == false)
    {
        std::cout << "Init() failed" << std::endl;
        return 0;
    }

    std::thread camera_thread = std::thread([&camera]()
                                            { camera.Run(); });
    camera_thread.detach();

    while (true)
    {
        auto event = next_event(dora_node.events);
        auto ty = event_type(event);

        if (ty == DoraEventType::AllInputsClosed)
        {
            break;
        }
        else if (ty == DoraEventType::Input)
        {
            auto input = event_as_input(std::move(event));

            counter += 1;

            std::cout << "Received input " << std::string(input.id) << " (counter: " << (unsigned int)counter << ")" << std::endl;

            camera.SaveImg();
            camera.SaveDepth();

            auto res = camera.GetImg();
            if (res.has_value())
            {
                cv::Mat img = res.value();
                cv::resize(img, img, cv::Size(640, 480));
                if (img.channels() != 1)
                {
                    // cv::cvtColor(img, img, cv::COLOR_BGR2RGB);
                }

                // std::vector<unsigned char> out_vec{};
                // if (!cv::imencode(".jpg", img, out_vec))
                // {
                //     std::cout << "capture image failed" << std::endl;
                //     return -1;
                // }

                std::cout << "img bytes" << img.total() * img.elemSize() << std::endl;
                // std::vector<unsigned char> out_vec(img.begin<unsigned char>(), img.end<unsigned char>());
                std::vector<unsigned char> out_vec(img.data, img.data + img.total() * img.elemSize());
                std::cout << "ready to send_output" << std::endl;

                rust::Slice<const uint8_t> out_slice{out_vec.data(), out_vec.size()};
                auto result = send_output(dora_node.send_output, "image", out_slice);
                auto error = std::string(result.error);
                if (!error.empty())
                {
                    std::cerr << "Error: " << error << std::endl;
                    return -1;
                }
            }
        }
        else
        {
            std::cerr << "Unknown event type " << static_cast<int>(ty) << std::endl;
        }
    }

    std::cout << "GOODBYE FROM C++ node (using Rust API)" << std::endl;

    return 0;
}
