//
// Created by singou on 2019/11/26.
//
#include <jni.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <android/log.h>
#include "pthread.h"

#define command "/sbin/ip link set tunl0 type can bitrate 125000"//将CAN0波特率设置为125000 bps
#define up "ifconfig tunl0 up"//打开CAN0
#define down "ifconfig tunl0 down"//关闭CAN0


#define  LOG_TAG    "can-test"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
bool isExit = false;
void * readCan(void * s ){
    LOGD("start read from can bitrate 125000 s=%d",s);
    struct can_frame frame[2] = {{0}};
    int nbytes;
    while (!isExit){
        nbytes = read(reinterpret_cast<long>(s), &frame[1], sizeof(frame[1]));//接收总线上的报文保存在frame[1]中
        LOGD("the nbytes:%d\n", nbytes);
        LOGD("length:%d", sizeof(frame[1]));
        LOGD("ID=0x%X DLC=%d\n", frame[1].can_id, frame[1].can_dlc);
        LOGD("data0=0x%02x\n", frame[1].data[0]);
        LOGD("data1=0x%02x\n", frame[1].data[1]);
        LOGD("data2=0x%02x\n", frame[1].data[2]);
        LOGD("data3=0x%02x\n", frame[1].data[3]);
        LOGD("data4=0x%02x\n", frame[1].data[4]);
        LOGD("data5=0x%02x\n", frame[1].data[5]);
        LOGD("data6=0x%02x\n", frame[1].data[6]);
        LOGD("data7=0x%02x\n", frame[1].data[7]);
    }
}

void * writeCan(void * s){
    LOGD("start write  can bitrate 125000 s=%d",s);
    struct can_frame frame[2] = {{0}};
    int nbytes;
    frame[0].can_id = 0x666;
    frame[0].can_dlc = 8;
    frame[0].data[0] = 0x40;
    frame[0].data[1] = 0x20;
    frame[0].data[2] = 0x10;
    frame[0].data[3] = 0x00;
    frame[0].data[4] = 0x03;
    frame[0].data[5] = 0x04;
    frame[0].data[6] = 0x05;
    frame[0].data[7] = 0x06;
    while (!isExit){
        frame[0].data[7]++;
        nbytes = write(reinterpret_cast<long>(s), &frame[0], sizeof(frame[0])); //发送 frame[0]
        if (nbytes != sizeof(frame[0])) {
            LOGD("Send Error frame[0]nbytes=%d\n!",nbytes);
        }
        sleep(20);
    }

}

int startCan() {
    system(down);
    system(command);
    system(up);//上面三行关闭CAN设备，设置波特率后，重新打开CAN设备
    int s;
    struct sockaddr_can addr;
    struct ifreq ifr;
    //struct can_frame frame[2] = {{0}};
    s = socket(PF_CAN, SOCK_RAW, CAN_RAW);//创建套接字
    if(s == NULL){
        LOGD("create can error error!!!!");
        return -1;
    }

    strcpy(ifr.ifr_name, "tunl0");
    ioctl(s, SIOCGIFINDEX, &ifr); //指定 tunl0 设备
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(s, (struct sockaddr *) &addr, sizeof(addr));//将套接字与 can0 绑定
    //setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);//设置过滤规则，取消当前注释为禁用过滤规则，即不接收所有报文，不设置此项（即如当前代码被注释）为接收所有ID的报文。
    LOGD("create socket success!!! s=%d",s);
    pthread_t readThread;
    //创建 thread 对象
    pthread_create(&readThread, NULL, readCan, reinterpret_cast<void *>(s));

    pthread_t writeThread;
    pthread_create(&writeThread, NULL, writeCan, reinterpret_cast<void *>(s));
    /*frame[0].can_id = 0x666;
    frame[0].can_dlc = 8;
    frame[0].data[0] = 0x40;
    frame[0].data[1] = 0x20;
    frame[0].data[2] = 0x10;
    frame[0].data[3] = 0x00;
    frame[0].data[4] = 0x03;
    frame[0].data[5] = 0x04;
    frame[0].data[6] = 0x05;
    frame[0].data[7] = 0x06;
    for (int i = 0; i < 100; i++) {
        frame[0].data[7]++;
        nbytes = write(s, &frame[0], sizeof(frame[0])); //发送 frame[0]
        if (nbytes != sizeof(frame[0])) {
            LOGD("Send Error frame[0]\n!");
        }

        nbytes = read(s, &frame[1], sizeof(frame[1]));//接收总线上的报文保存在frame[1]中
        LOGD("the nbytes:%d\n", nbytes);
        LOGD("length:%d", sizeof(frame[1]));
        LOGD("ID=0x%X DLC=%d\n", frame[1].can_id, frame[1].can_dlc);
        LOGD("data0=0x%02x\n", frame[1].data[0]);
        LOGD("data1=0x%02x\n", frame[1].data[1]);
        LOGD("data2=0x%02x\n", frame[1].data[2]);
        LOGD("data3=0x%02x\n", frame[1].data[3]);
        LOGD("data4=0x%02x\n", frame[1].data[4]);
        LOGD("data5=0x%02x\n", frame[1].data[5]);
        LOGD("data6=0x%02x\n", frame[1].data[6]);
        LOGD("data7=0x%02x\n", frame[1].data[7]);
        sleep(1);
    }
    close(s);*/
    return 0;
}


extern "C" JNIEXPORT jint JNICALL
Java_mo_singou_ai_opencv_can_CanUtils_startCan(
        JNIEnv *env,
        jclass clz/* this */) {
   LOGD("Java_mo_singou_ai_opencv_can_CanUtils_startCan");
    return startCan();
}