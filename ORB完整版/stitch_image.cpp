#include"stitch_image.h"


int  Check_pixel(int y0, int x0, Point2i Pic1_vertex[], int min_x, int min_y, const Mat& Pic2)
{
	int flag = 0;
	Point2i A = Pic1_vertex[0];
	Point2i B = Pic1_vertex[1];
	Point2i C = Pic1_vertex[3];
	Point2i D = Pic1_vertex[2];

	if (((D.x - A.x)*(y0 - A.y) - (D.y - A.y)*(x0 - A.x)) < 0 && ((B.x - A.x)*(y0 - A.y) - (B.y - A.y)*(x0 - A.x)) > 0 &&
		((D.x - C.x)*(y0 - C.y) - (D.y - C.y)*(x0 - C.x)) > 0 && ((C.x - B.x)*(y0 - B.y) - (C.y - B.y)*(x0 - B.x)) > 0)
	{
		flag = flag + 1;  //�����ĸ��߽��Լ����ȷ����������Pic1ͶӰ����
	}

	if (x0 >= abs(min_x) && y0 >= abs(min_y) && x0 <= (Pic2.cols - 1 + abs(min_x)) && y0 <= (Pic2.rows - 1 + abs(min_y)))
	{
		flag = flag + 2;  //��������Base_Pic��������
	}

	return flag;

}
int  Bilinear_interpolation(unsigned char RGB[], Mat Pic1, int x_d, int y_d, const Mat& H)
{

	float a = 0, b = 0, c = 0, d = 0, e = 0, f = 0;
	float x = 0, y = 0;

	a = H.at<float>(2, 0)*x_d - H.at<float>(0, 0);
	b = H.at<float>(2, 1)*x_d - H.at<float>(0, 1);
	c = H.at<float>(0, 2) - x_d;
	d = H.at<float>(2, 0)*y_d - H.at<float>(1, 0);
	e = H.at<float>(2, 1)*y_d - H.at<float>(1, 1);
	f = H.at<float>(1, 2) - y_d;
	x = (b * f - c * e) / (b * d - a * e); //��(x_d,y_d)��ͶӰƽ�棨Base_Pic����ӳ���Դͼ��Pic1����ȷ������������(x,y)
	y = (c * d - a * f) / (b * d - a * e);
	float u = x - int(x), v = y - int(y);

	if ((int(x) >= (Pic1.cols - 2)) || (int(y) >= (Pic1.rows - 2)) || (x < 0) || (y < 0)) //�����߽��Ĳ�ֵ
		return 0;

	Point2i A = Point2i(int(x), int(y));
	Point2i B = Point2i(int(x + 1), int(y));
	Point2i C = Point2i(int(x), int(y + 1));
	Point2i D = Point2i(int(x + 1), int(y + 1));

	for (int i = 0; i < 3; i++)
	{
		RGB[i] = (1 - u)*(1 - v)*(Pic1.at<Vec3b>(A.y, A.x)[i]) + v * (1 - u)*(Pic1.at<Vec3b>(C.y, C.x)[i])    //��RGB��ͨ����˫���Բ�ֵ
			+ u * (1 - v)*(Pic1.at<Vec3b>(B.y, B.x)[i]) + v * u*(Pic1.at<Vec3b>(D.y, D.x)[i]);
	}

}
int  Check_distance(Point2i E, Point2i F, int x_d, int y_d, const int Width, float& distance)
{
	//ֱ��ʹ�ù�ʽ����㵽ֱ�ߵľ���
	float A = F.y - E.y;
	float B = -1 * (F.x - E.x);
	float C = (F.x - E.x)*E.y - (F.y - E.y)*E.x;
	distance = abs((A*x_d + B * y_d + C) / sqrt(A * A + B * B));
	if (distance <= Width)
		return 1;  //�����ֱ��EF����С��Width
	else
		return 0;

}
Mat Stitch(const Mat& H, const Mat& Pic1, const Mat& Base_Pic)
{
	int min_x = 0, min_y = 0, max_x = 0, max_y = 0;			 //��¼Pic1 ����ת��������꼫ֵ
	Point2i Pic1_vertex[4] = { Point2i(0,0),Point2i(Pic1.cols - 1,0),Point2i(0,Pic1.rows - 1),Point2i(Pic1.cols - 1,Pic1.rows - 1) };//Pic1���ĸ���������(x,y),��ʼʱΪδͶӰ����

	//ʹ��Pic1�ĸ��������ͶӰ���꼫ֵ����ֵ����Base_Picƽ���ϵ�����ƽ��
	for (int i = 0; i < 4; i++)
	{
		int predict_x = (H.at<float>(0, 0)* Pic1_vertex[i].x + H.at<float>(0, 1)* Pic1_vertex[i].y + H.at<float>(0, 2)) /
			(H.at<float>(2, 0)* Pic1_vertex[i].x + H.at<float>(2, 1)* Pic1_vertex[i].y + H.at<float>(2, 2));
		int predict_y = (H.at<float>(1, 0)* Pic1_vertex[i].x + H.at<float>(1, 1)* Pic1_vertex[i].y + H.at<float>(1, 2)) /
			(H.at<float>(2, 0)* Pic1_vertex[i].x + H.at<float>(2, 1)* Pic1_vertex[i].y + H.at<float>(2, 2));
		Pic1_vertex[i].x = predict_x;
		Pic1_vertex[i].y = predict_y;   //�洢Pic1ͶӰ�������
		cout << "Pic1 ӳ����ĸ�����: (" << predict_y << "," << predict_x << " )" << endl;

		if (predict_x > max_x)
			max_x = predict_x;
		if (predict_y > max_y)
			max_y = predict_y;
		if (predict_x < min_x)
			min_x = predict_x;
		if (predict_y < min_y)
			min_y = predict_y;
	}
	cout << "ӳ������꼫ֵ��" << endl;
	cout << "min_x:" << min_x << " min_y:" << min_y << endl;
	cout << "max_x:" << max_x << " max_y:" << max_y << endl;
	Mat Transformed_image = Mat::zeros(max_y + abs(min_y), Base_Pic.cols + abs(min_x) + 5, CV_8UC3);  //ת�����Ŀ��ͼ��Base_Pic'��׼��

	//ƽ��Base_Pic����ϵ(ƽ��Pic1ͼ4������ӳ�䵽Base_Pic�ϵ����꣬�����Ӧ��Ҫƽ��Base_Picԭͼ)��Base_Pic '���꣬��Pic1ͶӰ�����ܹ�ȫ����ʾ����������������
	for (int i = 0; i < 4; i++)
	{
		Pic1_vertex[i].x = Pic1_vertex[i].x + abs(min_x);
		Pic1_vertex[i].y = Pic1_vertex[i].y + abs(min_y);
	}

	//��Base_Pic'����ϵ����ֿ飨Pic1ͶӰ����\��������\Base_Pic���򣩣�ȷ��Base_Pic '�����µ�ÿ�����ص�ֵ
	for (int i = 0; i < Transformed_image.rows; i++)
	{
		for (int j = 0; j < Transformed_image.cols; j++)
		{
			int temp = Check_pixel(i, j, Pic1_vertex, min_x, min_y, Base_Pic);
			if (temp == 1)	//ֻ��Pic1��ͶӰ��
			{
				unsigned char RGB[3] = { 0,0,0 };
				Bilinear_interpolation(RGB, Pic1, j - abs(min_x), i - abs(min_y), H);  //˫���Բ�ֵ��ȡ����ֵ��ע�⣨j,i������Ӧ�Ȼָ�Ϊδƽ�Ƶ�ֵ����
				Transformed_image.at<Vec3b>(i, j)[0] = RGB[0];
				Transformed_image.at<Vec3b>(i, j)[1] = RGB[1];
				Transformed_image.at<Vec3b>(i, j)[2] = RGB[2];
			}
			else if (temp == 2) //ֻ��Base_Pic�ϵĵ�
			{
				Transformed_image.at<Vec3b>(i, j)[0] = Base_Pic.at<Vec3b>(i - abs(min_y), j - abs(min_x))[0]; //RGB��Ӧ��ֵ,�����ȥƽ��������Ӧ��δƽ��ʱ��Base_Pic����
				Transformed_image.at<Vec3b>(i, j)[1] = Base_Pic.at<Vec3b>(i - abs(min_y), j - abs(min_x))[1];
				Transformed_image.at<Vec3b>(i, j)[2] = Base_Pic.at<Vec3b>(i - abs(min_y), j - abs(min_x))[2];
			}
			else if (temp == 3) //���������
			{
				const int Width = abs(max_x) / 3;	//����������
				unsigned char RGB[3] = { 0,0,0 };
				float distance = 0;				//�������ص�pic1��Ե�ľ���  
				Bilinear_interpolation(RGB, Pic1, j - abs(min_x), i - abs(min_y), H);		//˫���Բ�ֵ��ȡ����ֵ��ע�⣨j,i������Ӧ�Ȼָ�Ϊδƽ�Ƶ�ֵ����

				if (Check_distance(Pic1_vertex[1], Pic1_vertex[3], j, i, Width, distance))  //�����ڹ��������ڣ�ֻ�����˵��ص������Ե�����Ǳ�����������
				{
					float Weight = (1.0 / Width)*distance; //����Ȩֵ�����뽥������
					Transformed_image.at<Vec3b>(i, j)[0] = (unsigned char)(Weight * RGB[0] + (1 - Weight)*  Base_Pic.at<Vec3b>(i - abs(min_y), j - abs(min_x))[0]);  //��Ȩƽ����������϶
					Transformed_image.at<Vec3b>(i, j)[1] = (unsigned char)(Weight * RGB[1] + (1 - Weight)*  Base_Pic.at<Vec3b>(i - abs(min_y), j - abs(min_x))[1]);
					Transformed_image.at<Vec3b>(i, j)[2] = (unsigned char)(Weight * RGB[2] + (1 - Weight)*  Base_Pic.at<Vec3b>(i - abs(min_y), j - abs(min_x))[2]);

				}
				else //�����ڹ���������
				{
					Transformed_image.at<Vec3b>(i, j)[0] = RGB[0];
					Transformed_image.at<Vec3b>(i, j)[1] = RGB[1];
					Transformed_image.at<Vec3b>(i, j)[2] = RGB[2];
				}
			}

		}
	}

	return Transformed_image;

}