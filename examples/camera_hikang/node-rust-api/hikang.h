#pragma

// std
#include <string>
#include <atomic>
#include <thread>
#include <shared_mutex>
#include <unistd.h>

// opencv
#include <opencv2/opencv.hpp>

// MVC
#include "MvCameraControl.h"
#include "CameraParams.h"
#include "MvErrorDefine.h"
#include "PixelType.h"

bool PrintDeviceInfo(MV_CC_DEVICE_INFO* pstMVDevInfo) {
  if (NULL == pstMVDevInfo) {
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

struct HiKang {
  HiKang();
  ~HiKang();

  bool searchFor();
  bool connect();
  bool disconnect();
  void captureTask();
  cv::Mat getImage();
  void run();

  void setParam(std::string&& ip, bool isEnable) {
    m_ip = ip;
    m_isEnable = isEnable;
  }

  std::string m_ip{};
  std::atomic<bool> m_isEnable = false;

  void* m_handle = nullptr;         // MVC
  unsigned char* m_data = nullptr;  // MVC
  std::uint32_t m_dataSize = 0;
  std::uint32_t m_timeOut = 1000;  // ms

  cv::Mat m_img;
  std::shared_mutex m_imgLock;
  std::atomic<bool> m_imgUpdated{false};
};

HiKang::HiKang() { std::cout << "initialize HiKang" << std::endl; }

HiKang::~HiKang() {
  std::cout << "destory HiKang" << std::endl;
  m_isEnable = false;
  sleep(500);
  auto val = disconnect();
}

[[nodiscard]] bool HiKang::searchFor() {
  int nRet{};
  // ch:初始化SDK | en:Initialize SDK
  nRet = MV_CC_Initialize();
  if (MV_OK != nRet) {
    printf("Initialize SDK fail! nRet [0x%x]\n", nRet);
  }

  MV_CC_DEVICE_INFO_LIST stDeviceList;
  memset(&stDeviceList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

  // 枚举设备
  // enum device
  nRet = MV_CC_EnumDevices(
      MV_GIGE_DEVICE | MV_USB_DEVICE | MV_GENTL_CAMERALINK_DEVICE | MV_GENTL_CXP_DEVICE | MV_GENTL_XOF_DEVICE,
      &stDeviceList);
  if (MV_OK != nRet) {
    printf("MV_CC_EnumDevices fail! nRet [%x]\n", nRet);
  }
  if (stDeviceList.nDeviceNum > 0) {
    for (int i = 0; i < stDeviceList.nDeviceNum; i++) {
      printf("[device %d]:\n", i);
      MV_CC_DEVICE_INFO* pDeviceInfo = stDeviceList.pDeviceInfo[i];
      if (NULL == pDeviceInfo) {
        continue;
      }

      PrintDeviceInfo(pDeviceInfo);
      std::string nIp1 = std::to_string((pDeviceInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
      std::string nIp2 = std::to_string((pDeviceInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
      std::string nIp3 = std::to_string((pDeviceInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
      std::string nIp4 = std::to_string(pDeviceInfo->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);
      std::string found_ip = nIp1.append(".").append(nIp2).append(".").append(nIp3).append(".").append(nIp4);

      if (m_ip == found_ip) {
        nRet = MV_CC_CreateHandle(&m_handle, pDeviceInfo);
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

[[nodiscard]] bool HiKang::connect() {
  int nRet{};

  // 打开设备
  // open device
  nRet = MV_CC_OpenDevice(m_handle);
  if (MV_OK != nRet) {
    printf("MV_CC_OpenDevice fail! nRet [%x]\n", nRet);

    // 未成功打开设备则关闭句柄
    nRet = MV_CC_DestroyHandle(m_handle);
    if (MV_OK != nRet) {
      printf("Destory handle failed\n");
    }
    return false;
  }

  // ch:探测网络最佳包大小(只对GigE相机有效) | en:Detection network optimal package size(It only works for the GigE
  // camera)
  int nPacketSize = MV_CC_GetOptimalPacketSize(m_handle);
  if (nPacketSize > 0) {
    nRet = MV_CC_SetIntValueEx(m_handle, "GevSCPSPacketSize", nPacketSize);
    if (nRet != MV_OK) {
      printf("Warning: Set Packet Size fail nRet [0x%x]!\n", nRet);
    }
  } else {
    printf("Warning: Get Packet Size fail nRet [0x%x]!\n", nPacketSize);
  }

  // 设置触发模式为off
  // set trigger mode as off
  nRet = MV_CC_SetEnumValue(m_handle, "TriggerMode", MV_TRIGGER_MODE_OFF);
  if (MV_OK != nRet) {
    printf("MV_CC_SetTriggerMode fail! nRet [%x]\n", nRet);
  }

  // 设置图像格式
  nRet = MV_CC_SetEnumValue(m_handle, "PixelFormat", PixelType_Gvsp_BGR8_Packed);
  if (MV_OK != nRet) {
    printf("Set image type fail! nRet [%x]\n", nRet);
  }

  // ch:获取数据包大小 | en:Get payload size
  MVCC_INTVALUE stParam;
  memset(&stParam, 0, sizeof(MVCC_INTVALUE));
  nRet = MV_CC_GetIntValue(m_handle, "PayloadSize", &stParam);
  if (MV_OK != nRet) {
    printf("Get PayloadSize fail! nRet [0x%x]\n", nRet);

    int nRet = MV_CC_CloseDevice(m_handle);
    if (MV_OK != nRet) {
      printf("Close device failed\n");
    }
    nRet = MV_CC_DestroyHandle(m_handle);
    if (MV_OK != nRet) {
      printf("Destory handle failed\n");
    }
    return false;
  }

  // 获得图像所需内存大小并开辟内存
  m_dataSize = stParam.nCurValue;
  m_data = (unsigned char*)malloc(sizeof(unsigned char) * stParam.nCurValue);
  if (NULL == m_data) {
    printf("pData is null\n");
  }

  // 开始取流
  // start grab image
  nRet = MV_CC_StartGrabbing(m_handle);
  if (MV_OK != nRet) {
    printf("Start grabbing failed\n");
    return false;
  }

  return true;
}

void HiKang::captureTask() {
  MV_FRAME_OUT_INFO_EX stImageInfo = {0};
  memset(&stImageInfo, 0, sizeof(MV_FRAME_OUT_INFO_EX));
  while (m_isEnable) {
    if (!m_handle) {
      break;
    }

    int nRet{};
    nRet = MV_CC_GetOneFrameTimeout(m_handle, m_data, m_dataSize, &stImageInfo, m_timeOut);
    if (nRet == MV_OK) {
      // printf("Now you GetOneFrame, Width[%d], Height[%d], nFrameNum[%d]\n\n", stImageInfo.nWidth,
      // stImageInfo.nHeight,
      //        stImageInfo.nFrameNum);
      // 处理图像
      // image processing
      cv::Mat img = cv::Mat(stImageInfo.nHeight, stImageInfo.nWidth, CV_8UC3, (void*)m_data);
      if (!img.empty()) {
        std::unique_lock lk(m_imgLock);
        m_img = img.clone();
        m_imgUpdated = true;
        printf("update image succeed\n");
      }
    } else {
      printf("No data[%x]\n", nRet);
      continue;
    }
  }
}

[[nodiscard]] bool HiKang::disconnect() {
  int nRet{};
  // 停止取流
  // end grab image
  nRet = MV_CC_StopGrabbing(m_handle);
  if (MV_OK != nRet) {
    printf("MV_CC_StopGrabbing fail! nRet [%x]\n", nRet);
  }

  // 销毁句柄
  // destroy handle
  nRet = MV_CC_DestroyHandle(m_handle);
  if (MV_OK != nRet) {
    printf("MV_CC_DestroyHandle fail! nRet [%x]\n", nRet);
    return false;
  }
  m_handle = NULL;

  if (m_handle != NULL) {
    MV_CC_DestroyHandle(m_handle);
    m_handle = NULL;
  }

  if (m_data) {
    free(m_data);
    m_data = NULL;
  }

  // ch:反初始化SDK | en:Finalize SDK
  MV_CC_Finalize();

  return true;
}

cv::Mat HiKang::getImage() {
  cv::Mat img;
  if (m_imgUpdated) {
    std::shared_lock lk(m_imgLock);
    img = m_img.clone();
  } else {
    std::cout << "update image fail" << std::endl;
  }

  return img;
}

void HiKang::run() {
  m_isEnable = true;

  std::thread([this]() {
    std::cout << "HiKang..." << std::endl;
    while (m_isEnable) {
      if (!searchFor()) {
        std::cout << "Search camera..." << std::endl;
        sleep(500);
        continue;
      }
      if (!connect()) {
        std::cout << "Connect camera..." << std::endl;
        sleep(500);
        continue;
      }
      captureTask();
    }
  }).detach();
}
