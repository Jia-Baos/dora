#include "dora-node-api.h"

#include <iostream>
#include <vector>
#include "hikang.h"

int main()
{
    std::cout << "HELLO FROM C++" << std::endl;
    unsigned char counter = 0;

    auto dora_node = init_dora_node();

    std::shared_ptr<HiKang> ptr = std::make_shared<HiKang>();

    std::thread([&]()
                {
        ptr->setParam("192.168.192.254", true);
        ptr->run(); })
        .join();

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

            if (ptr->m_imgUpdated == false)
            {
                continue;
            }
            else
            {
                std::cout << "capture imgs" << std::endl;
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
