#include "../build/dora-node-api.h"

#include <iostream>
#include <vector>
#include "orbbec.h"

int main()
{
    std::cout << "HELLO FROM C++" << std::endl;
    unsigned char counter = 0;

    auto dora_node = init_dora_node();

    OrbbecCam cam;
    cam.wait4Device();
    cam.init();

    std::thread cam_thread = std::thread([&cam]()
                                         { cam.run(); });
    cam_thread.detach();

    while (true)
    {

        auto event = dora_node.events->next();
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

            // std::vector<unsigned char> out_vec{counter};
            // rust::Slice<const uint8_t> out_slice{out_vec.data(), out_vec.size()};
            // auto result = send_output(dora_node.send_output, "counter", out_slice);
            // auto error = std::string(result.error);
            // if (!error.empty())
            // {
            //     std::cerr << "Error: " << error << std::endl;
            //     return -1;
            // }

            cam.saveImg();
            cam.saveDepth();
        }
        else
        {
            std::cerr << "Unknown event type " << static_cast<int>(ty) << std::endl;
        }
    }

    std::cout << "GOODBYE FROM C++ node (using Rust API)" << std::endl;

    return 0;
}
