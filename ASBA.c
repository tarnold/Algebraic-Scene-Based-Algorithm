#include "ASBA.h"

void DisplayImage(IplImage *image, char *name) {

	cvNamedWindow(name, CV_WINDOW_AUTOSIZE);
	cvShowImage(name, image);	
	cvWaitKey(0);
	cvDestroyWindow(name);
}


/*
	VerticalCorrectionMatrix


Description: Generates images to build
the Vertical Correction Matrix.

*/
void VerticalCorrectionMatrix(char *path1, char *path2, IplImage **Matrix, int Vp_num) {

IplImage *input1, *input2, *result;
                
	input1 = cvLoadImage(path1, CV_LOAD_IMAGE_GRAYSCALE);
	input2 = cvLoadImage(path2, CV_LOAD_IMAGE_GRAYSCALE);
	result = cvCreateImage(cvSize(input1->width, input1->height), input1->depth, input1->nChannels);
	cvSub(input1, input2, result, NULL);
	Matrix[Vp_num] = cvCloneImage(result);


	cvReleaseImage(&input1);
	cvReleaseImage(&input2);
	cvReleaseImage(&result); 
}


/*
	HorizontalCorrectionMatrix


Description: Generates images to build
the Horizontal Correction Matrix

*/
void HorizontalCorrectionMatrix(char *path1, IplImage **Matrix, IplImage **VerticalMatrix, int image_num) {

IplImage *input1, *input2, *result;
                
	input1 = cvLoadImage(path1, CV_LOAD_IMAGE_GRAYSCALE);
	result = cvCreateImage(cvSize(input1->width, input1->height), input1->depth, input1->nChannels);
	cvAdd(input1, VerticalMatrix[0], result, NULL);
	Matrix[image_num] = cvCloneImage(result);

	cvReleaseImage(&input1);
	cvReleaseImage(&result); 
}


/*
	  CorrectionAverage


Description: Averages a set of images
into a single image.

*/
void CorrectionAverage(IplImage **Images, IplImage **Average, int image_num) {

int i;
IplImage *summation, *result;

	summation = cvCreateImage(cvSize(Images[0]->width, Images[0]->height), Images[0]->depth, Images[0]->nChannels);
	summation = cvCloneImage(Images[0]);
	for (i = 1; i < image_num; i++) {
		cvAdd(summation, Images[i], summation, NULL);
	}
	
	result = cvCreateImage(cvSize(summation->width, summation->height),summation->depth, summation->nChannels);
	cvConvertScale(summation, result, 1 / (image_num - 1), 0);
	Average[0] = cvCloneImage(result);
	
	cvReleaseImage(&summation); 
	cvReleaseImage(&result); 
}




/*
		Main


Description: Reads images and performs necessary 
transformations before generating the Vertical
and Horizontal Correction Matrices

*/
int main(int argc, char** argv) {

int Vp_num = 0, Hq_num = 0, i, y;
DIR *dir;
struct dirent *entry;
clock_t start, finish; 
double time_spent;
char **Paths; 
IplImage **Vp, **Hq, *VerticalMatrix[2], *HorizontalMatrix[2], *ResultMatrix, *final_image;

	dir = opendir(argv[1]);
	if (dir == NULL) {
		printf("Empty directory\n");
		return 0;
	}

	Paths = malloc((600) * sizeof(char *));
	start = clock();		
	while ((entry = readdir(dir)) != NULL) {
		Paths[image_num] = malloc(100 * sizeof(char));
		if (entry->d_type == 8) {
			sprintf(Paths[image_num++], "%s%s", argv[1], entry->d_name);
		}
	}
		
	closedir(dir);

	Vp = malloc((image_num) * sizeof(IplImage *));
	Hq = malloc((image_num) * sizeof(IplImage *));
	for (i = 0; i < image_num - 1; i++) {
		if (i < image_num / 2) {
			VerticalCorrectionMatrix(Paths[i], Paths[i + 1], Vp, Vp_num++);
		} else {
			if (Hq_num == 0) {
				CorrectionAverage(Vp, VerticalMatrix, Vp_num);
			}
			HorizontalCorrectionMatrix(Paths[i], Hq, VerticalMatrix, Hq_num++);
		}		
	}
	CorrectionAverage(Hq, HorizontalMatrix, Hq_num);
	
	ResultMatrix = cvCreateImage(cvSize(VerticalMatrix[0]->width, VerticalMatrix[0]->height), VerticalMatrix[0]->depth, VerticalMatrix[0]->nChannels);
	cvAdd(HorizontalMatrix[0], VerticalMatrix[0], ResultMatrix, NULL);
	
	final_image = cvLoadImage(Paths[0], CV_LOAD_IMAGE_GRAYSCALE);
	cvAdd(ResultMatrix, final_image, final_image, NULL);
	cvSaveImage("result.jpeg", final_image, 0);
	finish = clock();
	time_spent = ((double) (finish - start)) / CLOCKS_PER_SEC;
	printf("Time spent = %f\n", time_spent);	


}

