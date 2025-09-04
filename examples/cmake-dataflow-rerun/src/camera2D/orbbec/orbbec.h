#pragma once

#include <thread>
#include <shared_mutex>

#include <libobsensor/ObSensor.hpp>
#include <libobsensor/hpp/Error.hpp>
#include <libobsensor/hpp/StreamProfile.hpp>
#include <libobsensor/hpp/Context.hpp>
#include <libobsensor/hpp/Device.hpp>
#include <libobsensor/hpp/Frame.hpp>
#include <libobsensor/hpp/Pipeline.hpp>

#include "camera_base.h"

class OrbbecCamera : public CameraBase {
public:
    OrbbecCamera();
    virtual ~OrbbecCamera();
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
    std::shared_ptr<ob::Device> device_{ nullptr };
    std::shared_ptr<ob::Pipeline> pipe_{ nullptr };
    std::shared_ptr<ob::Config> config_{ nullptr };
    ob::PointCloudFilter point_cloud_;
    cv::Mat intrinsic_{ cv::Mat(3, 3, CV_32F) };
    cv::Mat distort_{ cv::Mat(1, 8, CV_32F) };
    std::shared_mutex img_lock_;
    std::shared_mutex depth_lock_;
    cv::Mat img_{ cv::Mat() };
    cv::Mat depth_{ cv::Mat() };
    int rw_{ 480 };
    int rh_{ 640 };
};
