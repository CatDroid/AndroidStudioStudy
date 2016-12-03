//
// Created by rd0394 on 2016/12/2.
//

#include "RtmpPusher.h"

void RtmpPusher::send264(uint8_t* buf,  int len , int64_t pts )
{
    int type;
    int64_t timeoffset;
    RTMPPacket *packet;
    unsigned char *body;

    //start_time为开始直播时的时间戳
    timeoffset = pts - mStartTime;

    //去掉帧界定符
    if (buf[2] == 0x00) { //00 00 00 01
        buf += 4;
        len -= 4;
    } else if (buf[2] == 0x01){ //00 00 01
        buf += 3;
        len -= 3;
    }
    type = buf[0]&0x1f;

    packet = (RTMPPacket *)malloc(RTMP_HEAD_SIZE+len+9);
    memset(packet,0,RTMP_HEAD_SIZE);

    packet->m_body = (char*)packet + RTMP_HEAD_SIZE;
    packet->m_nBodySize = len + 9;

    //send video packet
    body = (unsigned char*)packet->m_body;
    memset(body,0,len+9);

    //key frame
    body[0] = 0x27;
    if (type == NAL_SLICE_IDR) {
    body[0] = 0x17;
    }

    body[1] = 0x01;   //nal unit
    body[2] = 0x00;
    body[3] = 0x00;
    body[4] = 0x00;

    body[5] = (len >> 24) & 0xff;
    body[6] = (len >> 16) & 0xff;
    body[7] = (len >>  8) & 0xff;
    body[8] = (len ) & 0xff;

    //copy data
    memcpy(&body[9],buf,len);

    packet->m_hasAbsTimestamp = 0;
    packet->m_packetType = RTMP_PACKET_TYPE_VIDEO;
    packet->m_nInfoField2 = mRtmp->m_stream_id;
    packet->m_nChannel = 0x04;
    packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
    packet->m_nTimeStamp = (int)timeoffset;

    //调用发送接口
    RTMP_SendPacket(mRtmp,packet,TRUE);
    free(packet);
}

void RtmpPusher::sendSPSPPS(uint8_t* spsbuf, int spslen, uint8_t* ppsbuf, int ppslen)
{
    RTMPPacket* packet;
    uint8_t* body;
    int i;

    packet = (RTMPPacket*)malloc(RTMP_HEAD_SIZE+1024);
    memset(packet,0,RTMP_HEAD_SIZE);

    packet->m_body = (char *)packet + RTMP_HEAD_SIZE;
    body = (unsigned char *)packet->m_body;

    i = 0;
    body[i++] = 0x17;
    body[i++] = 0x00;

    body[i++] = 0x00;
    body[i++] = 0x00;
    body[i++] = 0x00;

    //AVCDecoderConfigurationRecord
    body[i++] = 0x01;
    body[i++] = spsbuf[1];
    body[i++] = spsbuf[2];
    body[i++] = spsbuf[3];
    body[i++] = 0xff;

    //sps
    body[i++] = 0xe1;
    body[i++] = (spslen >> 8) & 0xff;
    body[i++] = spslen & 0xff;
    memcpy(&body[i],spsbuf,spslen);
    i +=  spslen;

    //pps
    body[i++] = 0x01;
    body[i++] = (ppslen >> 8) & 0xff;
    body[i++] = (ppslen) & 0xff;
    memcpy(&body[i],ppsbuf ,ppslen);
    i +=  ppslen;

    packet->m_packetType = RTMP_PACKET_TYPE_VIDEO;
    packet->m_nBodySize = i;
    packet->m_nChannel = 0x04;
    packet->m_nTimeStamp = 0;
    packet->m_hasAbsTimestamp = 0;
    packet->m_headerType = RTMP_PACKET_SIZE_MEDIUM;
    packet->m_nInfoField2 = mRtmp->m_stream_id;

    //调用发送接口
    RTMP_SendPacket(mRtmp,packet,TRUE);
    free(packet);
}


void RtmpPusher::sendAAC(uint8_t* buf, int len ,int64_t pts )
{
    int64_t timeoffset = pts - mStartTime;
    buf += 7;
    len += 7;

    RTMPPacket * packet;
    unsigned char * body;

    packet = (RTMPPacket *)malloc(RTMP_HEAD_SIZE+len+2);
    memset(packet,0,RTMP_HEAD_SIZE);

    packet->m_body = (char *)packet + RTMP_HEAD_SIZE;
    body = (unsigned char *)packet->m_body;

    //AF 01 + AAC RAW data
    body[0] = 0xAF;
    body[1] = 0x01;
    memcpy(&body[2],buf,len);
    packet->m_packetType = RTMP_PACKET_TYPE_AUDIO;
    packet->m_nBodySize = len+2;
    packet->m_nChannel = 0x05;
    packet->m_nTimeStamp = (int)timeoffset;
    packet->m_hasAbsTimestamp = 0;
    packet->m_headerType = RTMP_PACKET_SIZE_MEDIUM;
    packet->m_nInfoField2 = mRtmp->m_stream_id;
    //static int64_t last = 0;
    //NSLog(@"ts=%lld inter=%lld",timeoffset,timeoffset-last);
    //last = timeoffset;
    //调用发送接口
    RTMP_SendPacket(mRtmp,packet,TRUE);
    free(packet);
}



void RtmpPusher::sendESDS(uint8_t* esds, int esdslen)
{
    RTMPPacket* packet;
    uint8_t* body;
    int len;

    packet = (RTMPPacket*) malloc(RTMP_HEAD_SIZE + esdslen + 2);
    memset( packet, 0, RTMP_HEAD_SIZE);

    packet->m_body = (char *)packet + RTMP_HEAD_SIZE;
    body = (unsigned char *)packet->m_body;

    //AF 00 + AAC RAW data
    body[0] = 0xAF;
    body[1] = 0x00;
    //spec_buf是AAC sequence header数据
    memcpy(&body[2], esds, esdslen);

    packet->m_packetType = RTMP_PACKET_TYPE_AUDIO;
    packet->m_nBodySize = esdslen + 2;
    packet->m_nChannel = 0x05;
    packet->m_nTimeStamp = 0;
    packet->m_hasAbsTimestamp = 0;
    packet->m_headerType = RTMP_PACKET_SIZE_LARGE;
    packet->m_nInfoField2 = mRtmp->m_stream_id;

    RTMP_SendPacket(mRtmp, packet,TRUE);
    free(packet);
}


bool RtmpPusher::openWithURL(uint8_t* url)
{
    mRtmp = RTMP_Alloc();
    if(!mRtmp){
        return -1;
    }
    RTMP_Init(mRtmp);
    //设置URL
    if (RTMP_SetupURL(mRtmp, (char *)url) == FALSE) {
        RTMP_Free(mRtmp);
        return false;
    }
    return true;
}


// @brief 连接服务器
bool RtmpPusher::connect()
{
    //连接服务器
    if (RTMP_Connect(mRtmp, NULL) == FALSE) {
        RTMP_Free(mRtmp);
        mRtmp = NULL;
        return false;
    }
    //连接流
    if (RTMP_ConnectStream(mRtmp, 0) == FALSE) {
        RTMP_Close(mRtmp);
        RTMP_Free(mRtmp);
        mRtmp = NULL;
        return false;
    }
    return true ;
}


bool RtmpPusher::publish()
{
    if(!mRtmp) return -1;
    //设置可写,即发布流,这个函数必须在连接前使用,否则无效
    RTMP_EnableWrite(mRtmp);
    bool ret = connect() ;
    if(ret == false ){
        RTMP_Free(mRtmp);
        mRtmp = NULL;
    }
    return ret;
}


void RtmpPusher::stop()
{
    /* FIXME：
     */
    RTMP_Close(mRtmp);
    RTMP_Free(mRtmp);
    mRtmp = NULL;
}

