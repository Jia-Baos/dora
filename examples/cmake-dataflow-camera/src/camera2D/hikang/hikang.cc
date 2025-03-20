#include "hikang.h"
#include "utils.h"
#include <chrono>
#include <optional>

bool PrintDeviceInfo(MV_CC_DEVICE_INFO *pstMVDevInfo)
{
    if (pstMVDevInfo == nullptr) {
        printf("The Pointer of pstMVDevInfo is NULL!\n");
        return false;
    }
    if (pstMVDevInfo->nTLayerType == MV_GIGE_DEVICE) {
        int nIp1 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
        int nIp2 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
        int nIp3 = ((pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
        int nIp4 = (pstMVDevInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);

        // ch:打印当前相机ip和用户自定义名字 | en:print current ip and user defined name
        printf("Device Model Name: %s\n", pstMVDevInfo->SpecialInfo.stGigEInfo.chModelName);
        printf("CurrentIp: %d.%d.%d.%d\n", nIp1, nIp2, nIp3, nIp4);
        printf("UserDefinedName: %s\n\n", pstMVDevInfo->SpecialInfo.stGigEInfo.chUserDefinedName);
    } else if (pstMVDevInfo->nTLayerType == MV_USB_DEVICE) {
        printf("Device Model Name: %s\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chModelName);
        printf("UserDefinedName: %s\n\n", pstMVDevInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName);
    } else if (pstMVDevInfo->nTLayerType == MV_GENTL_GIGE_DEVICE) {
        printf("UserDefinedName: %s\n", pstMVDevInfo->SpecialInfo.stGigEInfo.chUserDefinedName);
        printf("Serial Number: %s\n", pstMVDevInfo->SpecialInfo.stGigEInfo.chSerialNumber);
        printf("Model Name: %s\n\n", pstMVDevInfo->SpecialInfo.stGigEInfo.chModelName);
    } else if (pstMVDevInfo->nTLayerType == MV_GENTL_CAMERALINK_DEVICE) {
        printf("UserDefinedName: %s\n", pstMVDevInfo->SpecialInfo.stCMLInfo.chUserDefinedName);
        printf("Serial Number: %s\n", pstMVDevInfo->SpecialInfo.stCMLInfo.chSerialNumber);
        printf("Model Name: %s\n\n", pstMVDevInfo->SpecialInfo.stCMLInfo.chModelName);
    } else if (pstMVDevInfo->nTLayerType == MV_GENTL_CXP_DEVICE) {
        printf("UserDefinedName: %s\n", pstMVDevInfo->SpecialInfo.stCXPInfo.chUserDefinedName);
        printf("Serial Number: %s\n", pstMVDevInfo->SpecialInfo.stCXPInfo.chSerialNumber);
        printf("Model Name: %s\n\n", pstMVDevInfo->SpecialInfo.stCXPInfo.chModelName);
    } else if (pstMVDevInfo->nTLayerType == MV_GENTL_XOF_DEVICE) {
        printf("UserDefinedName: %s\n", pstMVDevInfo->SpecialInfo.stXoFInfo.chUserDefinedName);
        printf("Serial Number: %s\n", pstMVDevInfo->SpecialInfo.stXoFInfo.chSerialNumber);
        printf("Model Name: %s\n\n", pstMVDevInfo->SpecialInfo.stXoFInfo.chModelName);
    } else {
        printf("Not support.\n");
    }

    return true;
}

HiKangCamera::HiKangCamera()
{
}

HiKangCamera::~HiKangCamera()
{
    int nRet{};
    // 停止取流 | end grab image
    nRet = MV_CC_StopGrabbing(handle_);
    if (MV_OK != nRet) {
        printf("MV_CC_StopGrabbing fail! nRet [%x]\n", nRet);
    }

    // 销毁句柄 | destroy handle
    nRet = MV_CC_DestroyHandle(handle_);
    if (MV_OK != nRet) {
        printf("MV_CC_DestroyHandle fail! nRet [%x]\n", nRet);
    }
    handle_ = nullptr;

    if (handle_ != nullptr) {
        MV_CC_DestroyHandle(handle_);
        handle_ = nullptr;
    }

    if (data_) {
        free(data_);
        data_ = nullptr;
    }

    // ch:反初始化SDK | en:Finalize SDK
    MV_CC_Finalize();
}

void HiKangCamera::SetIP(const std::string &str)
{
    this->ip_ = str;
}

void HiKangCamera::SetSN(const std::string &str)
{
    this->sn_ = str;
}

bool HiKangCamera::Wait4Device()
{
    int nRet{};
    // ch:初始化SDK | en:Initialize SDK
    nRet = MV_CC_Initialize();
    if (MV_OK != nRet) {
        printf("Initialize SDK fail! nRet [0x%x]\n", nRet);
    }

    MV_CC_DEVICE_INFO_LIST stDeviceList;
    memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

    // 枚举设备 | enum device
    nRet = MV_CC_EnumDevices(
        MV_GIGE_DEVICE | MV_USB_DEVICE | MV_GENTL_CAMERALINK_DEVICE | MV_GENTL_CXP_DEVICE | MV_GENTL_XOF_DEVICE,
        &stDeviceList);
    if (MV_OK != nRet) {
        printf("MV_CC_EnumDevices fail! nRet [%x]\n", nRet);
    }
    if (stDeviceList.nDeviceNum > 0) {
        for (int i = 0; i < stDeviceList.nDeviceNum; i++) {
            printf("[device %d]:\n", i);
            MV_CC_DEVICE_INFO *pDeviceInfo = stDeviceList.pDeviceInfo[i];
            if (pDeviceInfo == nullptr) {
                continue;
            }

            PrintDeviceInfo(pDeviceInfo);
            std::string nIp1 = std::to_string((pDeviceInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
            std::string nIp2 = std::to_string((pDeviceInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
            std::string nIp3 = std::to_string((pDeviceInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
            std::string nIp4 = std::to_string(pDeviceInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);
            std::string found_ip = nIp1.append(".").append(nIp2).append(".").append(nIp3).append(".").append(nIp4);

            if (ip_ == found_ip) {
                nRet = MV_CC_CreateHandle(&handle_, pDeviceInfo);
                if (MV_OK != nRet) {
                    printf("Create Handle Failed!\n");
                    return false;
                }
                return true;
            }
        }
    } else {
        printf("Find No Devices!\n");
    }

    return false;
}

bool HiKangCamera::Init()
{
    int nRet{};

    // 打开设备 | open device
    nRet = MV_CC_OpenDevice(handle_);
    if (MV_OK != nRet) {
        printf("MV_CC_OpenDevice fail! nRet [%x]\n", nRet);

        // 未成功打开设备则关闭句柄
        nRet = MV_CC_DestroyHandle(handle_);
        if (MV_OK != nRet) {
            printf("Destory handle failed\n");
        }
        return false;
    }

    // ch:探测网络最佳包大小(只对GigE相机有效) | en:Detection network optimal package size(It only works for the GigE
    // camera)
    int nPacketSize = MV_CC_GetOptimalPacketSize(handle_);
    if (nPacketSize > 0) {
        nRet = MV_CC_SetIntValueEx(handle_, "GevSCPSPacketSize", nPacketSize);
        if (nRet != MV_OK) {
            printf("Warning: Set Packet Size fail nRet [0x%x]!\n", nRet);
        }
    } else {
        printf("Warning: Get Packet Size fail nRet [0x%x]!\n", nPacketSize);
    }

    // 设置触发模式为off | set trigger mode as off
    nRet = MV_CC_SetEnumValue(handle_, "TriggerMode", MV_TRIGGER_MODE_OFF);
    if (MV_OK != nRet) {
        printf("MV_CC_SetTriggerMode fail! nRet [%x]\n", nRet);
    }

    // 设置图像格式
    nRet = MV_CC_SetEnumValue(handle_, "PixelFormat", PixelType_Gvsp_BGR8_Packed);
    if (MV_OK != nRet) {
        printf("Set image type fail! nRet [%x]\n", nRet);
    }

    // ch:获取数据包大小 | en:Get payload size
    MVCC_INTVALUE stParam;
    memset(&stParam, 0, sizeof(MVCC_INTVALUE));
    nRet = MV_CC_GetIntValue(handle_, "PayloadSize", &stParam);
    if (MV_OK != nRet) {
        printf("Get PayloadSize fail! nRet [0x%x]\n", nRet);

        int nRet = MV_CC_CloseDevice(handle_);
        if (MV_OK != nRet) {
            printf("Close device failed\n");
        }
        nRet = MV_CC_DestroyHandle(handle_);
        if (MV_OK != nRet) {
            printf("Destory handle failed\n");
        }
        return false;
    }

    // 获得图像所需内存大小并开辟内存
    data_size_ = stParam.nCurValue;
    data_ = (unsigned char *)malloc(sizeof(unsigned char) * stParam.nCurValue);
    if (data_ == nullptr) {
        printf("pData is null\n");
    }

    // 开始取流 | start grab image
    nRet = MV_CC_StartGrabbing(handle_);
    if (MV_OK != nRet) {
        printf("Start grabbing failed\n");
        return false;
    }

    return true;
}

void HiKangCamera::Run()
{
    while (true) {
        if (handle_ == nullptr) {
            std::stringstream ss;
            ss << "get frame fail!";
            std::cout << ss.str() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
            continue;
        } else {
            break;
        }
    }

    MV_FRAME_OUT_INFO_EX stImageInfo = { 0 };
    memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));
    auto start = std::chrono::high_resolution_clock::now();
    while (true) {
        int nRet{};
        nRet = MV_CC_GetOneFrameTimeout(handle_, data_, data_size_, &stImageInfo, 100);
        if (nRet != MV_OK) {
            std::stringstream ss;
            ss << "get color frame failed!";
            std::cout << ss.str() << std::endl;
            continue;
        } else if (nRet == MV_OK) {
            try {
                printf("Now you GetOneFrame, Width[%d], Height[%d], nFrameNum[%d]\n\n", stImageInfo.nWidth, stImageInfo.nHeight, stImageInfo.nFrameNum);

                {
                    // 处理图像 | image processing
                    cgu::WRITE_LOCK(this->depth_lock_);
                    rw_ = stImageInfo.nWidth;
                    rh_ = stImageInfo.nHeight;
                    img_ = cv::Mat(stImageInfo.nHeight, stImageInfo.nWidth, CV_8UC3, (void *)data_);
                }

                // calc frame freq
                auto now = std::chrono::high_resolution_clock::now();
                auto freq = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
                start = now;
                std::stringstream ss;
                ss << "frame freq is " << freq << " ms";
                std::cout << ss.str() << std::endl;
            }
            catch (std::exception &e) {
                std::stringstream ss;
                ss << "handle frame err " << e.what();
                std::cout << ss.str() << std::endl;
            }
        }
    }
}

std::optional<cv::Mat> HiKangCamera::GetImg()
{
    cgu::READ_LOCK(this->img_lock_);
    if (depth_.empty()) {
        return std::nullopt;
    }
    return img_;
}

std::optional<int> HiKangCamera::GetImgSizeW()
{
    return rw_;
}

std::optional<int> HiKangCamera::GetImgSizeH()
{
    return rh_;
}

std::optional<cv::Mat> HiKangCamera::GetDepth()
{
    return std::nullopt;
}

std::optional<int> HiKangCamera::GetDepthSizeW()
{
    return std::nullopt;
}

std::optional<int> HiKangCamera::GetDepthSizeH()
{
    return std::nullopt;
}

std::optional<bool> HiKangCamera::SaveImg()
{
    cv::Mat img{};
    auto res = GetImg();
    if (res.has_value()) {
        img = res.value();
        std::cout << "save color img" << std::endl;
    } else {
        std::cout << "save color img failed" << std::endl;
    }

    return true;
}

std::optional<bool> HiKangCamera::SaveDepth()
{
    return false;
}
