
#include"ORB_code.h"
#include"stitch_image.h"


Mat  Copy_Pic(Mat image_one, Mat image_two);


void main()
{
	//��׼ͼ������ȡ
	Mat Base_Pic = imread("E:\\ͼ��ƴ�ӡ�����Ŀ\\ƴ���ز�\\material_2\\r1.jpg");
	Mat Base_Pic_Grey;
	vector<Fast_Keypoint>Features_Base;
	cvtColor(Base_Pic, Base_Pic_Grey, CV_BGR2GRAY);
	ORB_extractor Base_Extracor(2000, 1.414, 1);		//��׼ͼ�Ľ���������С��ʵʱͼ��������������Ҫ��
	Base_Extracor.Get_features(Base_Pic_Grey, Features_Base);

	//ʵʱͼ������ȡ
	Mat Pic1 = imread("E:\\ͼ��ƴ�ӡ�����Ŀ\\ƴ���ز�\\material_2\\small_2.jpg");
	Mat Pic1_Grey;
	vector<Fast_Keypoint>Features_pic1;
	cvtColor(Pic1, Pic1_Grey, CV_BGR2GRAY);
	ORB_extractor Extracor_Pic1(500, 1.2, 10);
	Extracor_Pic1.Get_features(Pic1_Grey, Features_pic1);

	//ƥ��
	vector<Match_pairs> match_result;		//�洢ƥ����
	LSH_match Matching(16, 3, 0.5);
	Matching.Create_HashLists(Features_Base);
	Matching.BF_match(Features_pic1,Features_Base,match_result);
	cout << "����ƥ�������" << match_result.size() << endl;
	cout << endl;

	Mat H = My_RANSAC(match_result);
	//Mat stitch_image = Stitch(H, Pic1, Base_Pic);
	//imshow("ƴ�ӽ��", stitch_image);
	//imwrite("E:\\ͼ��ƴ�ӡ�����Ŀ\\ƴ���ز�\\material_2\\r2.jpg",stitch_image);
	
	///////չʾЧ��////////////
	Mat result = Copy_Pic(Base_Pic, Pic1);   //Base����ߣ�ͼ1����ʵʱͼ���ұߣ�ͼ2��
	//��һ��ͼƬ����
	for (int i = 0; i < Features_Base.size(); i++)
	{
		cv::Point point;			//�����㣬���Ի���ͼ����  
		point.x = Features_Base[i].real_x;	//��������ͼ���к�����  
		point.y = Features_Base[i].real_y;	//��������ͼ����������  
		cv::circle(result, point, 1, cv::Scalar(255, 0, 0));	//��ͼ���л��������㣬1��Բ�İ뾶
	}
	//�ڶ���ͼƬ����
	for (int i = 0; i <Features_pic1 .size(); i++)
	{
		cv::Point point;						//�����㣬���Ի���ͼ����  
		point.x = Features_pic1[i].real_x + Base_Pic.cols;	//��������ͼ���к�����  
		point.y = Features_pic1[i].real_y;				//��������ͼ����������  
		cv::circle(result, point, 1, cv::Scalar(0, 0, 255));	//��ͼ���л��������㣬1��Բ�İ뾶
	}
	//���ߣ�����ƥ���
	for (int i = 0; i < match_result.size(); i++)
	{
		Point2i des_point=match_result[i].corresponding_point;
		des_point.x += Base_Pic.cols;
		line(result,match_result[i].base_point, des_point, cv::Scalar(0, 255, 0));
	}

	//resize(result, result, Size(result.cols/1.5, result.rows/1.5));
	imshow("ƥ����", result);
	waitKey(0);

	/*
	Mat Base_Pic = imread("E:\\ͼ��ƴ�ӡ�����Ŀ\\ƴ���ز�\\material_2\\small_3.jpg");
	resize(Base_Pic, Base_Pic,Size(600,600));
	imwrite("E:\\ͼ��ƴ�ӡ�����Ŀ\\ƴ���ز�\\material_2\\small_3.jpg", Base_Pic);
	*/
	
}

Mat  Copy_Pic(Mat image_one, Mat image_two)//��Ҫ����ͼƬ�ȴ�
{
	int Height = 0;
	if (image_one.rows >= image_two.rows)  //�������ߴ�ȷ����С
		Height = image_one.rows;
	else
		Height = image_two.rows;

	Mat result = Mat::zeros(Height, image_one.cols + image_two.cols + 1, CV_8UC3);
	Mat Roi = result(Rect(0, 0, image_one.cols, image_one.rows));   //Rect( _x, _y, _Tp _width, _Tp _height)������һ�����Ͻǵ�����Ϊ(_x, _y)��_width*_height���δ��ڣ�
	image_one.copyTo(Roi);   //����ͼƬ1
	Mat Roi1 = result(Rect(image_one.cols + 1, 0, image_two.cols, image_two.rows));
	image_two.copyTo(Roi1);  //����ͼƬ2

	return result;

}
