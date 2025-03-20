#include "dora-node-api.h"

#include <iostream>
#include <ratio>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

int main()
{
    std::cout << "HELLO FROM plot image node" << std::endl;
    unsigned char counter = 0;

    auto dora_node = init_dora_node();

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

            std::cout << "Rust API operator received input, id: " << std::string(input.id) <<  std::endl;

            // 解码图像数据
            std::vector<unsigned char> img_vec(input.data.begin(), input.data.end());
            cv::Mat img = cv::imdecode(img_vec, cv::IMREAD_COLOR);
            if (img.empty())
            {
                std::cout << "Failed to decode image" << std::endl;
            }
            else
            {
                cv::namedWindow("frame", cv::WINDOW_NORMAL);
                cv::imshow("frame", img);
                if (int key = cv::waitKey(10) && key == 27)
                {
                    return 0;
                }
            }
        }
        else
        {
            std::cerr << "Unknown event type " << static_cast<int>(ty) << std::endl;
        }
    }

    std::cout << "GOODBYE FROM plot image node (using Rust API)" << std::endl;

    return 0;
}
