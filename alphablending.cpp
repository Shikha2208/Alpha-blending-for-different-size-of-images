#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

// Alpha Blending using direct pointer access
Mat& alphaBlendDirectAccess(Mat& alpha, Mat& alpha2, Mat& foreground, Mat& foreground2, Mat& background, Mat& outImage, Mat& outImage2, int posx1, int posy1, int posx2, int posy2)
{
	float* fptr = reinterpret_cast<float*>(foreground.data);
	float* fptr2 = reinterpret_cast<float*>(foreground2.data);
	float* bptr = reinterpret_cast<float*>(background.data);
	float* aptr = reinterpret_cast<float*>(alpha.data);
	float* aptr2 = reinterpret_cast<float*>(alpha2.data);
	float* outImagePtr = reinterpret_cast<float*>(outImage.data);
	float* outImagePtr2 = reinterpret_cast<float*>(outImage2.data);
	float *temp;
	temp = outImagePtr;
	int i, j, k;

	for (i = 0; i < background.rows; i++)
	{
		for (j = 0; j < background.cols; j++)
		{
			for (k = 0; k < background.channels(); k++)
			{
				if (((j - posy1) >= 0) && ((j - posy1) < foreground.cols))
				{
					if (((i - posx1) >= 0) && ((i - posx1) < foreground.rows))
					{
						*outImagePtr = (*fptr)*(*aptr) + (*bptr)*(1 - *aptr);
						outImagePtr++;
						fptr++;
						bptr++;
						aptr++;
					}

				}
				else
				{
					*outImagePtr = (*bptr);
					outImagePtr++;
					bptr++;
				}

			}

		}
	}

	outImagePtr=temp;
	for (i = 0; i < outImage.rows; i++)
	{
		for (j = 0; j < outImage.cols; j++)
		{
			for (k = 0; k < outImage.channels(); k++)
			{
				if (((j - posy2) >= 0) && ((j - posy2) < foreground2.cols))
				{
					if (((i - posx2) >= 0) && ((i - posx2) < foreground2.rows))
					{
						*outImagePtr2 = (*fptr2)*(*aptr2) + (*outImagePtr)*(1 - *aptr2);
						outImagePtr2++;
						fptr2++;
						outImagePtr++;
						aptr2++;
					}

				}
				else
				{
					*outImagePtr2 = (*outImagePtr);
					outImagePtr2++;
					outImagePtr++;
				}

			}

		}
	}
	return outImage2;
}


int main(int argc, char** argv)
{
	int posx1=0, posy1=0, posx2=0, posy2=0; //initialize positions

	Mat foreGroundImage = imread("fore5.png", -1);
	if (foreGroundImage.empty())
	{
		cout << "Could not open or find the image" << endl;
		cin.get(); //wait for any key press
		return -1;
	}
	imshow("FORGROUNDImage", foreGroundImage);
	waitKey(1);
	Mat foreGroundImage2 = imread("fore6.png", -1);
	if (foreGroundImage2.empty())
	{
		cout << "Could not open or find the image" << endl;
		cin.get(); //wait for any key press
		return -1;
	}
	imshow("FORGROUNDImage2", foreGroundImage2);
	waitKey(1);
	Mat background = imread("backgroundLarge.jpg");
	if (background.empty())
	{
		cout << "Could not open or find the image" << endl;
		cin.get(); //wait for any key press
		return -1;
	}
	imshow("backgroundImage", background);
	waitKey(1);

	int height = foreGroundImage.rows;
	int width = foreGroundImage.cols;
	cout << height << " " << width;
	resize(foreGroundImage2, foreGroundImage2, foreGroundImage.size());
	int height2 = foreGroundImage2.rows;
	int width2 = foreGroundImage2.cols;
	cout << height2 << " " << width2;
	
	
	posy2 = background.cols - foreGroundImage2.cols;
    //posx1 = background.rows - foreGroundImage2.rows;
	//posx2 = posx1;
	


	Mat bgra[4];
	split(foreGroundImage, bgra);//split png foreground
	// Save the foregroung RGB content into a single Mat
	vector<Mat> foregroundChannels;
	foregroundChannels.push_back(bgra[0]);
	foregroundChannels.push_back(bgra[1]);
	foregroundChannels.push_back(bgra[2]);
	Mat foreground = Mat::zeros(foreGroundImage.size(), CV_8UC3);
	merge(foregroundChannels, foreground);
	// Save the alpha information into a single Mat
	vector<Mat> alphaChannels;
	alphaChannels.push_back(bgra[3]);
	alphaChannels.push_back(bgra[3]);
	alphaChannels.push_back(bgra[3]);
	Mat alpha = Mat::zeros(foreGroundImage.size(), CV_8UC3);
	merge(alphaChannels, alpha);
	
	
	Mat bgra2[4];
	split(foreGroundImage2, bgra2);//split png foreground
	// Save the foregroung RGB content into a single Mat
	vector<Mat> foregroundChannels2;
	foregroundChannels2.push_back(bgra2[0]);
	foregroundChannels2.push_back(bgra2[1]);
	foregroundChannels2.push_back(bgra2[2]);
	Mat foreground2 = Mat::zeros(foreGroundImage2.size(), CV_8UC3);
	merge(foregroundChannels2, foreground2);
	// Save the alpha information into a single Mat
	vector<Mat> alphaChannels2;
	alphaChannels2.push_back(bgra2[3]);
	alphaChannels2.push_back(bgra2[3]);
	alphaChannels2.push_back(bgra2[3]);
	Mat alpha2 = Mat::zeros(foreGroundImage2.size(), CV_8UC3);
	merge(alphaChannels2, alpha2);

	
	// Convert Mat to float data type
	foreground.convertTo(foreground, CV_32FC3);
	foreground2.convertTo(foreground2, CV_32FC3);

	background.convertTo(background, CV_32FC3);
	alpha.convertTo(alpha, CV_32FC3, 1.0 / 255);
	alpha2.convertTo(alpha2, CV_32FC3, 1.0 / 255);// keeps the alpha values betwen 0 and 1
	// Alpha blending using direct Mat access with for loop
	Mat outImage = Mat::zeros(background.size(), background.type());
	Mat outImage2 = Mat::zeros(background.size(), background.type());
	outImage2 = alphaBlendDirectAccess(alpha, alpha2, foreground, foreground2, background, outImage, outImage2, posx1,posy1, posx2, posy2);//function calling
	namedWindow("alpha blended image", WINDOW_AUTOSIZE);
	imshow("alpha blended image", outImage2 / 255);
	imwrite("alpha111.png", outImage2 / 255);
	waitKey(0);
	return 0;
}