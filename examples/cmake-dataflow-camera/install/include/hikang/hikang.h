#pragma once

// std
#include <string>
#include <atomic>
#include <thread>
#include <shared_mutex>
#include <unistd.h>

// MVC
#include "MvCameraControl.h"
#include "CameraParams.h"
#include "MvErrorDefine.h"
#include "PixelType.h"

#include "camera_base.h"

class HiKangCamera : public CameraBase {
public:
    HiKangCamera();
    virtual ~HiKangCamera();
    void SetIP(const std::string &str) override;
    void SetSN(const std::string &str) override;
    bool Wait4Device() override;
    bool Init() override;
    void Run() override;

    std::optional<cv::Mat> GetImg() override;
    std::optional<int> GetImgSizeW() override;
    std::optional<int> GetImgSizeH() override;
    std::optional<cv::Mat> GetDepth() override;
    std::optional<int> GetDepthSizeW() override;
    std::optional<int> GetDepthSizeH() override;
    std::optional<bool> SaveImg() override;
    std::optional<bool> SaveDepth() override;

private:
    void *handle_{ nullptr };        // MVC
    unsigned char *data_{ nullptr }; // MVC

    cv::Mat intrinsic_{ cv::Mat(3, 3, CV_32F) };
    cv::Mat distort_{ cv::Mat(1, 8, CV_32F) };
    std::shared_mutex img_lock_;
    std::shared_mutex depth_lock_;
    cv::Mat img_{ cv::Mat() };
    cv::Mat depth_{ cv::Mat() };
    int rw_{ 480 };
    int rh_{ 640 };
    int data_size_{ 0 };
};
