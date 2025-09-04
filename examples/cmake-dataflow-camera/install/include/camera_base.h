#pragma once
#include <atomic>
#include <string>
#include <optional>

#include <opencv2/opencv.hpp>

class CameraBase {
public:
    CameraBase()
        : stop_flag_(false) {}
    virtual ~CameraBase() {}

    virtual void SetIP(const std::string &str) = 0;
    virtual void SetSN(const std::string &str) = 0;
    virtual bool Wait4Device() = 0;
    virtual bool Init() = 0;
    virtual void Run() = 0;

    virtual std::optional<cv::Mat> GetImg() = 0;
    virtual std::optional<int> GetImgSizeW() = 0;
    virtual std::optional<int> GetImgSizeH() = 0;
    virtual std::optional<cv::Mat> GetDepth() = 0;
    virtual std::optional<int> GetDepthSizeW() = 0;
    virtual std::optional<int> GetDepthSizeH() = 0;
    virtual std::optional<bool> SaveImg() = 0;
    virtual std::optional<bool> SaveDepth() = 0;

public:
    std::string ip_{}; // 网口相机 IP
    std::string sn_{}; // USB相机序列号
    std::atomic<bool>
        stop_flag_{}; // 相机禁用 flag
};
