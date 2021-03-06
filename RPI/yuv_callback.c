#include "arducam_mipicamera.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define LOG(fmt, args...) fprintf(stderr, fmt "\n", ##args)

FILE *fd;
int frame_count = 0;
int yuv_callback(BUFFER *buffer) {
    // This buffer will be automatically released after the callback function ends.
    // If you do a time-consuming operation here, it will affect other parts,
    // such as preview, video encoding(This issue may be fixed in the future).
    if (buffer->length) {
        if (TIME_UNKNOWN == buffer->pts) {
            // Frame data in the second half
        }
        LOG("buffer length = %d, pts = %lld, flags = 0x%X", buffer->length, buffer->pts, buffer->flags);
        frame_count++;
    }
    return 0;
}

int main(int argc, char **argv) {
    CAMERA_INSTANCE camera_instance;
    int count = 0;
    int width = 1280;
    int height = 720;

    LOG("Open camera...");
    int res = arducam_init_camera(&camera_instance);
    if (res) {
        LOG("init camera status = %d", res);
        return -1;
    }
    LOG("Setting the resolution...");
    // To get more precise control, use the set_mode method to replace the set_resolution method.
    res = arducam_set_resolution(camera_instance, &width, &height);
    if (res) {
        LOG("set resolution status = %d", res);
        return -1;
    } else {
        LOG("Current resolution is %dx%d", width, height);
        LOG("Notice:You can use the list_format sample program to see the resolution and control supported by the camera.");
    }
    // start raw callback
    LOG("Start yuv data callback...");
    res = arducam_set_yuv_callback(camera_instance, yuv_callback, NULL);
    if (res) {
        LOG("Failed to start raw data callback.");
        return -1;
    }

    struct timespec start, end;
    clock_gettime(CLOCK_REALTIME, &start);
    usleep(1000 * 1000 * 10);
    clock_gettime(CLOCK_REALTIME, &end);

    double timeElapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000.0;
    LOG("Total frame count = %d", frame_count);
    LOG("TimeElapsed = %f", timeElapsed);
    // stop yuv data callback
    LOG("Stop yuv data callback...");
    arducam_set_yuv_callback(camera_instance, NULL, NULL);

    LOG("Close camera...");
    res = arducam_close_camera(camera_instance);
    if (res) {
        LOG("close camera status = %d", res);
    }
    return 0;
}