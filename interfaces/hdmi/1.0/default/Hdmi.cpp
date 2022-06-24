// FIXME: your file license if you have one

#include "Hdmi.h"
#include "log/log.h"
#include <sys/inotify.h>
#include <errno.h>
#include <linux/videodev2.h>

#include "HdmiCallback.h"


#define BASE_VIDIOC_PRIVATE 192     /* 192-255 are private */
#define RKMODULE_GET_HDMI_MODE       \
        _IOR('V', BASE_VIDIOC_PRIVATE + 34, __u32)


namespace rockchip::hardware::hdmi::implementation {

sp<::rockchip::hardware::hdmi::V1_0::IHdmiCallback> mCb = nullptr;

hidl_string mDeviceId;

const int kMaxDevicePathLen = 256;
const char* kDevicePath = "/dev/";
const char kPrefix[] = "v4l-subdev";
const int kPrefixLen = sizeof(kPrefix) - 1;
const int kDevicePrefixLen = sizeof(kDevicePath) + kPrefixLen + 1;

char kV4l2DevicePath[kMaxDevicePathLen];
int mMipiHdmi = 0;

sp<V4L2DeviceEvent> mV4l2Event;
int findMipiHdmi()
{
    DIR* devdir = opendir(kDevicePath);
    if(devdir == 0) {
        ALOGE("%s: cannot open %s! ", __FUNCTION__, kDevicePath);
        return -1;
    }
    struct dirent* de;
    int videofd,ret;
    while ((de = readdir(devdir)) != 0) {
        // Find external v4l devices that's existing before we start watching and add them
        if (!strncmp(kPrefix, de->d_name, kPrefixLen)) {
            std::string deviceId(de->d_name + kPrefixLen);
            ALOGD("found %s", de->d_name);
            char v4l2DeviceDriver[16];
            snprintf(kV4l2DevicePath, kMaxDevicePathLen,"%s%s", kDevicePath, de->d_name);
            videofd = open(kV4l2DevicePath, O_RDWR);
            if (videofd < 0){
                ALOGE("[%s %d] open device failed:%x [%s]", __FUNCTION__, __LINE__, videofd,strerror(errno));
                continue;
            } else {
                // ALOGE("%s open device %s successful.", __FUNCTION__, kV4l2DevicePath);
                uint32_t ishdmi;
                ret = ::ioctl(videofd, RKMODULE_GET_HDMI_MODE, (void*)&ishdmi);
                if (ret < 0) {
                    ALOGE("RKMODULE_GET_HDMI_MODE Failed, error: %s", strerror(errno));
                    close(videofd);
                    continue;
                }
                ALOGE("%s RKMODULE_GET_HDMI_MODE:%d",kV4l2DevicePath,ishdmi);
                if (ishdmi)
                {
                    mMipiHdmi = videofd;
                    ALOGE("mMipiHdmi:%d",mMipiHdmi);
                    if (mMipiHdmi < 0)
                    {
                        return ret;
                    }
                    // struct v4l2_capability cap;
                    // ret = ioctl(videofd, VIDIOC_QUERYCAP, &cap);
                    // if (ret < 0) {
                    //     ALOGE("VIDIOC_QUERYCAP Failed, error: %s", strerror(errno));
                    //     close(videofd);
                    //     continue;
                    // }
                    mV4l2Event->initialize(mMipiHdmi);
                }
            }
        }
    }
    closedir(devdir);
    return ret;
}


Return<void> Hdmi::foundHdmiDevice(const hidl_string& deviceId) {

    ALOGE("@%s,deviceId:%s",__FUNCTION__,deviceId.c_str());
    mDeviceId = deviceId.c_str();
    return Void();
}
Return<void> Hdmi::getHdmiDeviceId(getHdmiDeviceId_cb _hidl_cb) {
    ALOGE("@%s,mDeviceId：%s",__FUNCTION__,mDeviceId.c_str());
    _hidl_cb(mDeviceId);
    return Void();
}
// Methods from ::rockchip::hardware::hdmi::V1_0::IHdmi follow.
Return<void> Hdmi::onStatusChange(uint32_t status) {
    ALOGE("@%s",__FUNCTION__);
    if (mCb.get()!=nullptr)
    {
        ALOGE("@%s,status:%d",__FUNCTION__,status);
        if (status)
        {
            mCb->onConnect(mDeviceId);
        }else{
            mCb->onDisconnect(mDeviceId);
        }
    }
    return Void();
}

Return<void> Hdmi::registerListener(const sp<::rockchip::hardware::hdmi::V1_0::IHdmiCallback>& cb) {
    ALOGE("@%s",__FUNCTION__);
    mCb = cb;
    return Void();
}

Return<void> Hdmi::unregisterListener(const sp<::rockchip::hardware::hdmi::V1_0::IHdmiCallback>& cb) {
    ALOGE("@%s",__FUNCTION__);
    mCb = nullptr;
    return Void();
}

V4L2EventCallBack Hdmi::eventCallback(void* sender,int event_type,struct v4l2_event *event){
    ALOGD("@%s,event_type:%d",__FUNCTION__,event_type);
    if (event_type == V4L2_EVENT_CTRL)
    {
        struct v4l2_event_ctrl* ctrl =(struct v4l2_event_ctrl*) &(event->u);
        if (mCb != nullptr)
        {
            if (!ctrl->value)
            {
                mCb->onDisconnect("0");
            }
        }
        ALOGD("V4L2_EVENT_CTRL event %d\n", ctrl->value);
    }else if (event_type == V4L2_EVENT_SOURCE_CHANGE)
    {
        if (sender!=nullptr)
        {
            V4L2DeviceEvent::V4L2EventThread* eventThread = (V4L2DeviceEvent::V4L2EventThread*)sender;
            sp<V4L2DeviceEvent::FormartSize> format = eventThread->getFormat();
            if (format!=nullptr)
            {
                ALOGE("getFormatWeight:%d,getFormatHeight:%d",format->getFormatWeight(),format->getFormatHeight());
                if (mCb != nullptr)
                {
                    mCb->onFormatChange("0",format->getFormatWeight(),format->getFormatHeight());
                    mCb->onConnect("0");
                }
            }
        }
    }
    return 0;
}

Hdmi::Hdmi(){
    ALOGE("@%s.",__FUNCTION__);
    mCb = new HdmiCallback();
    mV4l2Event = new V4L2DeviceEvent();
    mV4l2Event->RegisterEventvCallBack((V4L2EventCallBack)Hdmi::eventCallback);
    findMipiHdmi();
}
Hdmi::~Hdmi(){
    ALOGE("@%s",__FUNCTION__);
    if (mV4l2Event)
        mV4l2Event->closePipe();
    if (mV4l2Event)
        mV4l2Event->closeEventThread();
}
V1_0::IHdmi* HIDL_FETCH_IHdmi(const char* /* name */) {
    ALOGE("@%s",__FUNCTION__);
    return new Hdmi();
}

}  // namespace rockchip::hardware::hdmi::implementation
