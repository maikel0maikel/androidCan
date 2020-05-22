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
extern "C" JNIEXPORT jstring JNICALL
Java_mo_singou_ai_opencv_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject obj/* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
extern "C" JNIEXPORT jint JNICALL
Java_mo_singou_ai_opencv_MainActivity_startCan(
        JNIEnv *env,
        jobject obj/* this */) {
    int s, nbytes;
    unsigned char number=0;
    struct sockaddr_can addr;
    struct ifreq ifr;
    struct can_frame frame;
    struct can_frame frame_send;
    struct can_filter rfilter[1];
    pid_t pid = -1;
    int i;
    int argc = 2;
    /* handle (optional) flags first */
    /* create socket */
    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0)
    {
        perror("Create socket failed");
        exit(-1);
    }

    /* set up can interface */
    strcpy(ifr.ifr_name, "tunl0");
    printf("can port is %s\n",ifr.ifr_name);
    /* assign can device */
    ioctl(s, SIOCGIFINDEX, &ifr);
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    /* bind can device */
    if(bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("Bind can device failed\n");
        close(s);
        exit(-2);
    }

    pid = fork();
    if(pid == -1)
    {
        perror("\n\tfailed to fork!!\n\n");
        return  -1;
    }
    else if(pid==0)/* configure receiving can data*/
    {
        /* set filter for only receiving packet with can id 0x88 */
        rfilter[0].can_id = 0x88;
        rfilter[0].can_mask = CAN_SFF_MASK;
        if(setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter)) < 0)
        {
            perror("set receiving filter error\n");
            close(s);
            exit(-3);
        }
        /* keep reading */
        while(1){

            nbytes = read(s, &frame, sizeof(frame));
            if(nbytes > 0)
            {
                printf("read datas:%s ID=%#x data length=%d\n", ifr.ifr_name, frame.can_id, frame.can_dlc);
                for ( i=0; i < frame.can_dlc; i++)
                    printf("%#x ", frame.data[i]);
                printf("\n");
            }
            printf("read can data over\n");

        }
    }/* configure sending can data*/
    else
    {
        while(1)
        {
            /* configure can_id and can data length */
            frame_send.can_id = 0x88;
            frame_send.can_dlc = 8;
            printf("%s ID=%#x data length=%d\n", ifr.ifr_name, frame_send.can_id, frame_send.can_dlc);
            /* prepare data for sending: 0x11,0x22...0x88 */
            for (i=0; i<8; i++)
            {
                frame_send.data[i] = ((i+1)<<4) | (i+1);
                frame_send.data[7] =number;
                printf("%#x ", frame_send.data[i]);
            }
            printf("success to Sent out\n");
            /* Sending data */
            if(write(s, &frame_send, sizeof(frame_send)) < 0)
            {
                perror("Send failed");
                close(s);
                exit(-4);
            }
            sleep(1);
            number++;
        }

    }
    close(s);
    return 0;
}