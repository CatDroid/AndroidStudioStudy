//
// Created by rd0394 on 2016/12/2.
//

#ifndef RTMPDEMO_RTMPPUSHER_H
#define RTMPDEMO_RTMPPUSHER_H


#include "jni.h"
#include "string.h"

#define RTMP_HEAD_SIZE (sizeof(RTMPPacket)+RTMP_MAX_HEADER_SIZE)
#define NAL_SLICE_IDR (0x1F&0x65)
#define NAL_SLICE_SPS (0x1F&0x67)
#define NAL_SLICE_PPS (0x1F&0x68)
#define NAL_SLICE_P (0x1F&0x41)

extern "C"
{
#include "librtmp/amf.h"
#include "librtmp/rtmp.h"
//#include "librtmp/rtmp_sys.h"
//#include "librtmp/log.h"
}


class RtmpPusher{
private:
    RTMP * mRtmp;
    int64_t mStartTime;
public:
    void send264(uint8_t* buf, int len ,int64_t pts );
    void sendAAC(uint8_t* buf, int len ,int64_t pts );
    void sendESDS(uint8_t* esds, int esdslen);
    void sendSPSPPS(uint8_t* spsbuf, int spslen, uint8_t* ppsbuf, int ppslen);
    void stop();
    bool connect();
    bool publish();
    bool openWithURL(uint8_t* url);
};

#endif //RTMPDEMO_RTMPPUSHER_H
