/*
 * udpimage_server.cpp
 *
 *  Created on: May 29, 2015
 *      Author: liu
 *      Editor: Alberto
 */
#include "UDPImageFetcher.h"

int k_times;                                           //send k_times for each package
Mat image_left = Mat::zeros( 480, 640, CV_8UC3);   //Define image size
Mat image_right = Mat::zeros( 480, 640, CV_8UC3);


int errorT;
void *status;

bool DBG = false;

pthread_t tid_left;	//thread id
pthread_t tid_right;

int sockfd;
int imgSize;

bool isRunning = false;

bool initUDPFetcher(bool debug)
{
/////////////////////////////////UDP Initialization///////////////////////////////////

	DBG = debug;
	struct sockaddr_in addr;

	sockfd=socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd<0)
	{
		fprintf(stderr,"Socket Error:%s\n",strerror(errno));
		return false;
	}

	bzero(&addr,sizeof(struct sockaddr_in));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htonl(INADDR_ANY);
	addr.sin_port=htons(SERVER_PORT);

	if(bind(sockfd,(struct sockaddr *)&addr,sizeof(struct sockaddr_in))<0)
	{
		fprintf(stderr,"Bind Error:%s\n",strerror(errno));
		return false;
	}

/////////////////////////////////Image Initialization///////////////////////////////

	imgSize = image_left.total()*image_left.elemSize();                 //calculate image size

	if(DBG)
		cout<<"image size is "<< imgSize <<endl;

	k_times = imgSize / MAX_PKG_SIZE;

	if(DBG)
		cout << k_times << endl;

//////////////////////////////////multithread stuff///////////////////////////////

	enableUDPImageFetcher(true);

	errorT = pthread_create(&tid_left, NULL, fetchImageLeft, NULL);
	if(errorT){
		printf("pthread Left is not created...\n");
		return false;
	}

	errorT = pthread_create(&tid_right, NULL, fetchImageRight, NULL);
		if(errorT){
			printf("pthread Right is not created...\n");
			return false;
		}


	return true;
}

void showError(const char *msg)
{
    perror(msg);
}


void *fetchImageLeft(void *arg){

	struct sched_param param;
	param.sched_priority = sched_get_priority_max(SCHED_RR);

	pthread_setschedparam(pthread_self(),SCHED_RR,&param);

	while(isRunning){
		udps_respon(image_left, imgSize, sockfd);
	}
	close(sockfd);
	//pthread_join(tid, &status);
	return (void *)0;
}

void *fetchImageRight(void *arg){

	struct sched_param param;
	param.sched_priority = sched_get_priority_max(SCHED_RR);

	pthread_setschedparam(pthread_self(),SCHED_RR,&param);

	while(isRunning){
		udps_respon(image_right, imgSize, sockfd);
	}
	close(sockfd);
	//pthread_join(tid, &status);
	return (void *)0;
}

void enableUDPImageFetcher(bool b)
{
	isRunning = b;
}

struct sockaddr_in addr;
socklen_t addrlen = sizeof(addr);

struct recvbuf {
		uchar buf[MAX_PKG_SIZE];
		int frame_n;
		int seq_n;
	};
struct recvbuf recvData;
Mat img;

void udps_respon(Mat &imgOut, int imgSize, int sockfd)
{
	img = Mat::zeros( 480, 640, (((0) & ((1 << 3) - 1)) + (((3)-1) << 3)));

	uchar sockData[imgSize];

	int bytes = 0;
	int count = 0;

	int prev_frame_n = -1;
	for (int i = 0; i < k_times; i ++) {
			if ((bytes = recvfrom(sockfd, &recvData, sizeof(recvData), 0,(struct sockaddr*) &addr, &addrlen)) == -1) {
				showError("1.receive failed");
			}
			count++;
			for(int j = 0; j < MAX_PKG_SIZE; j ++){
				sockData[recvData.frame_n*MAX_PKG_SIZE + j] = recvData.buf[j];
			}

			if(DBG)
			{
				cout << "Seq:" << recvData.seq_n <<  ", Frame: " << recvData.frame_n << "\n";
				if(abs(recvData.frame_n - prev_frame_n) > 1 && recvData.frame_n != 44)
					cout << "Prev frame was: " << prev_frame_n << " and current is: " << recvData.frame_n << endl;

				prev_frame_n = recvData.frame_n;
			}

//only in this case we store data to image, otherwise we break
		if(recvData.frame_n == 44){
			if(count == 45){
				if(DBG)
						printf("Receive data finished!\n");

				int ptr=0;
				for (int i = 0;  i < img.rows; i++) {
					for (int j = 0; j < img.cols; j++) {
						img.at<cv::Vec3b>(i,j) = cv::Vec3b(sockData[ptr+ 0],sockData[ptr+1],sockData[ptr+2]);
						ptr=ptr+3;
					}
				}
				img.copyTo(imgOut);

				if(DBG)
					cout<< "Write back image data finished!" << endl;




			}else{
				break;
			}
		}
	}


}

void getCurrentFrame(Mat &img, int camera){
	if(camera == LEFT_CAMERA)
		image_left.copyTo(img);
	else
		image_right.copyTo(img);
}



