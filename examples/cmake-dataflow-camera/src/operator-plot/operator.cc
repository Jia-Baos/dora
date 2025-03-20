#include "operator.h"
#include <iostream>
#include <vector>
#include "dora-operator-api.h"

#include <opencv2/opencv.hpp>

Operator::Operator() {}

std::unique_ptr<Operator> new_operator()
{
    return std::make_unique<Operator>();
}

DoraOnInputResult on_input(Operator &op, rust::Str id, rust::Slice<const uint8_t> data, OutputSender &output_sender)
{
    op.counter += 1;
    std::cout << "Rust API operator received input `" << id.data() << "` with data `" << (unsigned int)data[0] << "` (internal counter: " << (unsigned int)op.counter << ")" << std::endl;

    // 解码图像数据
    std::vector<unsigned char> img_vec(data.begin(), data.end());
    cv::Mat img = cv::imdecode(img_vec, cv::IMREAD_COLOR);
    if (img.empty())
    {
        std::cout << "Failed to decode image" << std::endl;
        DoraOnInputResult result = {rust::cxxbridge1::String("failed decode image"), false};
        return result;
    }
    else
    {
        cv::namedWindow("frame", cv::WINDOW_NORMAL);
        cv::imshow("frame", img);
        if (int key = cv::waitKey(10) && key == 27)
        {
            DoraOnInputResult result = {rust::cxxbridge1::String("failed decode image"), true};
            return result;
        }
    }

    std::vector<unsigned char> out_vec{op.counter};
    rust::Slice<const uint8_t> out_slice{out_vec.data(), out_vec.size()};
    auto send_result = send_output(output_sender, rust::Str("status"), out_slice);
    DoraOnInputResult result = {send_result.error, false};
    return result;
}
