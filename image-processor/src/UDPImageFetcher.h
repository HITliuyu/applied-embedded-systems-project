/*
 * UDPImageFetcher.h
 *
 *  Created on: 08/06/2015
 *      Author: avaccari
 */

#ifndef UDPIMAGEFETCHER_H_
#define UDPIMAGEFETCHER_H_

#include <stdio.h>
#include <unistd.h>
#include <netdb.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <pthread.h>
#include <errno.h>

using namespace cv;
using namespace std;

#define SERVER_PORT 8888                               //Define the port
#define MAX_PKG_SIZE 20480                             //Define max package size

// Internal Macros
#define LEFT_CAMERA 0
#define RIGHT_CAMERA 1

bool initUDPFetcher(bool debug);
void showError(const char *msg);
void *fetchImageLeft(void *arg);
void *fetchImageRight(void *arg);
void udps_respon(Mat &image, int imgSize,  int sockfd);
void enableUDPImageFetcher(bool b);
void getCurrentFrame(Mat &img, int camera);

#endif /* UDPIMAGEFETCHER_H_ */
