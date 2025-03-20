#include "orbbec.h"
#include "utils.h"
#include <optional>

OrbbecCamera::OrbbecCamera()
{
    ob::Context::setLoggerSeverity(OB_LOG_SEVERITY_FATAL);
}

OrbbecCamera::~OrbbecCamera()
{
    if (pipe_)
    {
        pipe_->stop();
    }
    pipe_ = nullptr;
    config_ = nullptr;
    device_ = nullptr;
}

void OrbbecCamera::SetIP(const std::string &str)
{
    this->ip_ = str;
}

void OrbbecCamera::SetSN(const std::string &str)
{
    this->sn_ = str;
}

bool OrbbecCamera::Wait4Device()
{
    ob::Context ctx;
    auto dev_list = ctx.queryDeviceList();
    int cam_num = dev_list->deviceCount();
    std::stringstream ss;
    ss << "discover camera number is " << cam_num;
    std::cout << ss.str() << std::endl;

    try
    { // Maybe sudo permissions are needed
        for (int i = 0; i < cam_num; i++)
        {
            if (!device_)
            {
                device_ = dev_list->getDevice(i);
            }

            auto dev_info = device_->getDeviceInfo();
            auto serial_number = dev_info->serialNumber();
            break;
        }
    }
    catch (ob::Error &e)
    {
        std::stringstream ss;
        ss << "Open camera failed!";
        std::cout << ss.str() << std::endl;

        return false;
    }

    if (!device_)
    {
        return false;
    }
    return true;
}

bool OrbbecCamera::Init()
{
    pipe_ = std::make_shared<ob::Pipeline>(device_);
    config_ = std::make_shared<ob::Config>();
    try
    {
        // 配置rgb摄像头的参数
        auto color_profiles = pipe_->getStreamProfileList(OB_SENSOR_COLOR);
        std::shared_ptr<ob::VideoStreamProfile> color_profile = nullptr;
        try
        {
            // 640，480，RGB，30帧
            color_profile = color_profiles->getVideoStreamProfile(640, 480, OB_FORMAT_RGB, 30);
        }
        catch (ob::Error &e)
        {
            color_profile = color_profiles->getVideoStreamProfile(640, 480, OB_FORMAT_UNKNOWN, 30);
        }
        int rw = color_profile->width();
        int rh = color_profile->height();
        std::stringstream ss;
        ss << "rgb size: " << rh << " * " << rw;
        std::cout << ss.str() << std::endl;
        config_->enableStream(color_profile);
    }
    catch (ob::Error &e)
    {
        std::stringstream ss;
        ss << "Current device is not support color sensor!";
        std::cout << ss.str() << std::endl;
    }
    try
    {
        auto depth_profiles = pipe_->getStreamProfileList(OB_SENSOR_DEPTH);
        std::shared_ptr<ob::VideoStreamProfile> depth_profile = nullptr;
        try
        {
            depth_profile = depth_profiles->getVideoStreamProfile(640, 480, OB_FORMAT_Y11, 30);
        }
        catch (ob::Error &e)
        {
            depth_profile = depth_profiles->getVideoStreamProfile(640, 480, OB_FORMAT_UNKNOWN, 30);
        }

        int pw = depth_profile->width();
        int ph = depth_profile->height();
        std::stringstream ss;
        ss << "depth size : " << ph << " * " << pw;
        std::cout << ss.str() << std::endl;
        config_->enableStream(depth_profile);
    }
    catch (ob::Error &e)
    {
        std::stringstream ss;
        ss << "Current device is not support depth sensor!";
        std::cout << ss.str() << std::endl;
    }
    if (device_->isPropertySupported(OB_PROP_DEPTH_ALIGN_HARDWARE_BOOL, OB_PERMISSION_READ))
    {
        config_->setAlignMode(ALIGN_D2C_HW_MODE);
    }
    else
    {
        config_->setAlignMode(ALIGN_D2C_SW_MODE);
    }

    if (!pipe_)
    {
        return false;
    }

    pipe_->start(config_);

    auto camera_param = pipe_->getCameraParam();
    point_cloud_.setCameraParam(camera_param);
    intrinsic_.at<float>(0, 0) = camera_param.rgbIntrinsic.fx;
    intrinsic_.at<float>(0, 1) = 0;
    intrinsic_.at<float>(0, 2) = camera_param.rgbIntrinsic.cx;
    intrinsic_.at<float>(1, 0) = 0;
    intrinsic_.at<float>(1, 1) = camera_param.rgbIntrinsic.fy;
    intrinsic_.at<float>(1, 2) = camera_param.rgbIntrinsic.cy;
    intrinsic_.at<float>(2, 0) = 0;
    intrinsic_.at<float>(2, 1) = 0;
    intrinsic_.at<float>(2, 2) = 1;
    distort_.at<float>(0, 0) = camera_param.rgbDistortion.k1;
    distort_.at<float>(0, 1) = camera_param.rgbDistortion.k2;
    distort_.at<float>(0, 2) = camera_param.rgbDistortion.p1;
    distort_.at<float>(0, 3) = camera_param.rgbDistortion.p2;
    distort_.at<float>(0, 4) = camera_param.rgbDistortion.k3;
    distort_.at<float>(0, 5) = camera_param.rgbDistortion.k4;
    distort_.at<float>(0, 6) = camera_param.rgbDistortion.k5;
    distort_.at<float>(0, 7) = camera_param.rgbDistortion.k6;
    return true;
}

void OrbbecCamera::Run()
{
    while (true)
    {
        auto frame_set = pipe_->waitForFrames(1000);

        if (frame_set == nullptr)
        {
            std::stringstream ss;
            ss << "get frame fail!";
            std::cout << ss.str() << std::endl;
            continue;
        }
        if (frame_set != nullptr && frame_set->depthFrame() != nullptr && frame_set->colorFrame() != nullptr)
        {
            auto depth_value_scale = frame_set->depthFrame()->getValueScale();
            break;
        };
        if (!frame_set || !frame_set->depthFrame() || !frame_set->colorFrame())
        {
            std::stringstream ss;
            ss << "frameSet or depthFrame or colorFrame is null";
            std::cout << ss.str() << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    auto start = std::chrono::high_resolution_clock::now();
    while (true)
    {
        auto frame_set = pipe_->waitForFrames(100);
        if (frame_set == nullptr)
        {
            continue;
        }
        if (frame_set != nullptr && frame_set->depthFrame() != nullptr && frame_set->colorFrame() != nullptr)
        {
            auto depth_value_scale = frame_set->depthFrame()->getValueScale();
            point_cloud_.setPositionDataScaled(depth_value_scale);
            try
            {
                int rw = frame_set->colorFrame()->width();
                int rh = frame_set->colorFrame()->height();
                {
                    cgu::WRITE_LOCK(this->img_lock_);
                    img_ = cv::Mat(rh, rw, CV_8UC3, frame_set->colorFrame()->data());
                }
                {
                    cgu::WRITE_LOCK(this->depth_lock_);
                    depth_ = cv::Mat(frame_set->depthFrame()->height(), frame_set->depthFrame()->width(), CV_16UC1, frame_set->depthFrame()->data());
                }
                point_cloud_.setCreatePointFormat(OB_FORMAT_POINT);
                std::shared_ptr<ob::Frame> frame = point_cloud_.process(frame_set);

                // calc frame freq
                auto now = std::chrono::high_resolution_clock::now();
                auto freq = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
                start = now;
                std::stringstream ss;
                ss << "frame freq is " << freq << " ms";
                std::cout << ss.str() << std::endl;
            }
            catch (std::exception &e)
            {
                std::stringstream ss;
                ss << "handle frame err " << e.what();
                std::cout << ss.str() << std::endl;
            }
        }
        else
        {
            std::stringstream ss;
            ss << "get color frame or depth frame failed!";
            std::cout << ss.str() << std::endl;
        }
    }
}

std::optional<cv::Mat> OrbbecCamera::GetImg()
{
    cgu::READ_LOCK(this->img_lock_);
    if (img_.empty())
    {
        return std::nullopt;
    }
    return img_;
}

std::optional<int> OrbbecCamera::GetImgSizeW()
{
    return rw_;
}

std::optional<int> OrbbecCamera::GetImgSizeH()
{
    return rh_;
}

std::optional<cv::Mat> OrbbecCamera::GetDepth()
{
    cgu::READ_LOCK(this->depth_lock_);
    if (depth_.empty())
    {
        return std::nullopt;
    }
    return depth_;
}

std::optional<int> OrbbecCamera::GetDepthSizeW()
{
    return rw_;
}

std::optional<int> OrbbecCamera::GetDepthSizeH()
{
    return rh_;
}

std::optional<bool> OrbbecCamera::SaveImg()
{
    cv::Mat img{};
    auto res = GetImg();
    if (res.has_value())
    {
        img = res.value();
        std::cout << "save color img" << std::endl;
    }
    else
    {
        std::cout << "save color img failed" << std::endl;
    }

    return true;
}

std::optional<bool> OrbbecCamera::SaveDepth()
{
    cv::Mat depth{};
    auto res = GetDepth();
    if (res.has_value())
    {
        depth = res.value();
        std::cout << "save depth img" << std::endl;
    }
    else
    {
        std::cout << "save depth img failed" << std::endl;
    }

    return true;
}
