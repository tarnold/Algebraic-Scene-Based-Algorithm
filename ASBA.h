#include <stdio.h>
#include <cv.h>
#include <highgui.h>
#include <dirent.h>
#include <time.h>

int image_num = 0;

void VerticalCorrectionMatrix(char *path1, char *path2, IplImage **Matrices, int Vp_num);
void CorrectionAverage(IplImage **Images, IplImage **Average, int image_num);
void HorizontalCorrectionMatrix(char *path1, IplImage **Matrix, IplImage **VerticalMatrix, int image_num);
void DisplayImage(IplImage *image, char *name);
