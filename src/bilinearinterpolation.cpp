#include "bilinearinterpolation/bilinearinterpolation.h"
#include"iostream"
#include"iomanip"
using namespace std;
void printMat(cv::Mat m,char *info)
{
  cout<<"----------------"<<endl;
  cout<<info<<":"<<endl;
  for(int i=0;i<m.rows;i++)
  {
    for(int j=0;j<m.cols;j++)
    {
      cout<<setw(10)<<m.at<float>(i,j)<<" ";
    }
    cout<<endl;
  }
  cout<<"----------------"<<endl;
}
void compareResult(cv::Mat a,cv::Mat b)
{
  CV_Assert(a.cols==b.cols && a.rows==b.rows);
  for(int i=0;i<a.rows;i++)
    for(int j=0;j<a.cols;j++)
    {
      if(fabs(a.at<float>(i,j)-b.at<float>(i,j))>0.000001) //注意：浮点数的比较
      {
	cout<<"diff in ("<<i<<","<<j<<")\t"<<a.at<float>(i,j)<<"-"<<b.at<float>(i,j)<<endl;
	cout<<a.at<float>(i,j)-b.at<float>(i,j)<<endl;
	return;
      }
    }
  cout<<"same!"<<endl;
}
void biLinearInterpolation(cv::Mat src,cv::Mat &dst,cv::Size sz)
{
  cv::Size src_sz(src.cols,src.rows);
  cv::Mat src_extend(src_sz.height+2,src_sz.width+2,CV_32FC1);
  src.copyTo(src_extend.rowRange(1,src_extend.rows-1).colRange(1,src_extend.cols-1));
  //printMat(src_extend,"1");
  for(int j=1;j<src_extend.cols-1;j++)
  {
    src_extend.at<float>(0,j)=src.at<float>(0,j-1);
    src_extend.at<float>(src_extend.rows-1,j)=src.at<float>(src.rows-1,j-1);
  }
  //printMat(src_extend,"2");
  for(int i=0;i<src_extend.rows;i++)
  {
    src_extend.at<float>(i,0)=src_extend.at<float>(i,1);
    src_extend.at<float>(i,src_extend.cols-1)=src_extend.at<float>(i,src_extend.cols-1-1);
  }
  //printMat(src_extend,"3");    
     
  double scalew=(double)src_sz.width/sz.width;  //注意：除法最好用double，影响精度
  double scaleh=(double)src_sz.height/sz.height;
  //float scalew=(float)src_sz.width/sz.width;
  //float scaleh=(float)src_sz.height/sz.height;
  float x,y,dx,dy,p00,p01,p10,p11;
  int xi,yi;
  cv::Mat temp(sz.height,sz.width,CV_32FC1);
  for(int i=0;i<temp.rows;i++)
    for(int j=0;j<temp.cols;j++)
    {
      x=(i+0.5f)*scaleh-0.5f;
      y=(j+0.5f)*scalew-0.5f;
      xi=cvFloor(x);
      yi=cvFloor(y);
      //cout<<"x,y,xi,yi: "<<x<<","<<y<<","<<xi<<","<<yi<<endl;
      CV_Assert(xi>=-1 && xi<src.cols && yi>=-1 && yi<src.rows);
      p00=src_extend.at<float>(1+xi,1+yi);
      p01=src_extend.at<float>(1+xi,1+yi+1);
      p10=src_extend.at<float>(1+xi+1,1+yi);
      p11=src_extend.at<float>(1+xi+1,1+yi+1);
      //cout<<"p00,p01,p10,p11: "<<p00<<","<<p01<<","<<p10<<","<<p11<<endl;
      dx=x-xi;
      dy=y-yi;
      //cout<<"dx,dy: "<<dx<<","<<dy<<endl;
      //  dst[i][j] = |1-dx dx| |p00 p01| |1-dy|
      //                        |p10 p11| |  dy|
      temp.at<float>(i,j)=((1-dx)*p00+dx*p10)*(1-dy)+((1-dx)*p01+dx*p11)*dy;
    }
  temp.copyTo(dst);
  
}
int main(int argc,char**argv)
{
  cv::Mat img(4,4,CV_32FC1),img_out1,img_out2;
  cv::Size sz(15,15);
  for(int i=0,k=1;i<img.rows;i++)
    for(int j=0;j<img.cols;j++,k++)
      img.at<float>(i,j)=k*0.34;
  //printMat(img,"img_raw");
  
  biLinearInterpolation(img,img_out1,sz);
  //printMat(img_out1,"biLinearInterpolation");
  
  cv::resize(img,img_out2,sz);//bilinear interpolation by default
  //printMat(img_out2,"resize");
  
  compareResult(img_out1,img_out2);
  return 0;
}
