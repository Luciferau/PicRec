#include <iostream>
#include <fileapi.h>
#include <conio.h>
#include <fstream>
#include <time.h>
#include <math.h>
#include <graphics.h>
#include <io.h>
#include <string>
#include <vector>
using namespace std;

//系统参数
#define MOUSE_Click(mButton) (GetAsyncKeyState(mButton)&0x8000?1:0)//鼠标点击事件
#define DataNumber 3000
double Accuracy; 

//神经网络参数
#define ImageLengthOriginal 224//初始化图片大小 注意！ImageLength_original必须是ImageLength的倍数 
#define ImageLength 28//图片边长
#define input_layer 785//输入层神经元数量
#define hide_layer 91//隐藏层
#define output_layer 15//输出层神经元数量

double killp=0.6f;//drop-out参数
double yita=0.01f;//激活函数RELU  
double study=0.01;//学习率
double regularize=0.001f;//L_2 
double stb=0.1f;//初始化权重

//单次仅加载一张图片 
double datapiece[ImageLengthOriginal+1][ImageLengthOriginal+1];//图像数据(彩图) 
int datalable;//样本标签(lable)
int wrongs[output_layer+1];//统计错误样本的标签
double x1[input_layer+1], x2[hide_layer+1],x3[hide_layer+1],x4[output_layer+1];//神经细胞
double val1[input_layer+1][hide_layer+1],val2[hide_layer+1][hide_layer+1],val3[hide_layer+1][output_layer+1];//传递权重 
double delta2[hide_layer+1],delta3[hide_layer+1],delta4[output_layer+1];//梯度 
bool avai2[hide_layer+5],avai3[hide_layer+5];//drop-out

//训练层
double x0[DataNumber][input_layer+1];
int D[DataNumber];//样本标签(lable)
int DATANUM;

//卷积层参数 
#define Kernel_num 4//卷积核的数量 
double Kernel_datapiece_1[ImageLengthOriginal+5][ImageLengthOriginal+5];//图像横向边界
double Kernel_datapiece_2[ImageLengthOriginal+5][ImageLengthOriginal+5];//图像竖向边界
double Kernel_datapiece_3[ImageLengthOriginal+5][ImageLengthOriginal+5];//图像右上到左下边界
double Kernel_datapiece_4[ImageLengthOriginal+5][ImageLengthOriginal+5];//图像左上到右下边界
double Kernel_1[9]={-0.25,0,0.25,
	                -0.5,0,0.5,
	                -0.25,0,0.25};//卷积核1
double Kernel_2[9]={-0.25,-0.5,-0.25,
	                0,0,0,
	                0.25,0.5,0.25};//卷积核2
double Kernel_3[9]={0,0.25,0.5,
	                -0.25,0,0.25,
	                -0.5,-0.25,0};//卷积核3
double Kernel_4[9]={0.5,0.25,0,
	                0.25,0,0.25,
	                0,-0.25,-0.5};//卷积核4

//函数声明 
void Train();//训练模式 
void Target();//识别模式 
void readValue();//加载权重
void SaveValue();//保存权重到value.txt 
bool readTrainFirst();//初次读取训练集 
void readTrain();//读取训练集
bool readTest();//读取测试集
void readTarget();//读取识别集
void Target_ans(int ID);//输出识别结果 
void Image_preprocessing(IMAGE* Img);//图像预处理 注：在该函数中才将图片导入datapiece数组 
void DNN_FP();//全连接神经网络 正向传播
void DNN_BP();//全连接神经网络 反向传播 梯度下降算法 
bool randValue();//初始化权重
void GetAllFiles(string path,vector<string>&files);//读取所有的文件，包括子文件的文件
void ReadRGB(IMAGE* Img);//读取RGB值 导入图像数组
void Gd();//高斯模糊 去除噪声
void Normalization(int L2);//归一化
void Convolutional_layer(int L);//卷积层
void Pooling_layer(int L1,int L2);//池化层
void Activitation_layer(int L2);//激活层
double RELU(double x);//Leaky ReLu激活函数
double RELUdao(double x);//梯度下降使用函数之一 

//调试函数声明
void EXE_RUN_TEST(int L1,int L2);//测试用检测图像数组转换图片效果

int main(int argc , char* argv[],char* envp[]){
	readValue();//加载权重
	if(argc==1){
		Train();
		getch();
	}else if(argv[1][0]=='2'){
		Target();
	}else{
		cout<<"Error!"<<endl;
		getch();
	}
	
	return 0;
}

void Train(){//训练模式
    ifstream in("AccuracyG.txt");
	in>>Accuracy;
	in.close();
    if(readTrainFirst())return;
	while(1){//训练网络
    	readTrain();//读取训练集
    	if(MOUSE_Click(VK_RBUTTON)) break; //单击鼠标右键中止循环
    	if(readTest()){//读取测试集
    	    return;
        }	
        if(MOUSE_Click(VK_RBUTTON)) break; //单击鼠标右键中止循环
	}
}
void Target(){//识别模式
	readValue();//加载权重
	readTarget();//读取识别集
	
	double maxx=-99998;
	int id=-1;
	for(int i=1;i<=output_layer;i++){//统计结果
		if(x4[i]>maxx){//选择概率最高的可能性
		    maxx=x4[i];
			id=i;
		}
    }
	Target_ans(id);//输出识别结果到ans.txt
}
void readValue(){//加载权重
    //cout<<"Read value..."<<endl;
	if(randValue()){
    	cout<<"[Error]Can't found value.txt."<<endl;
    	return;
	}
	
	ifstream in("valueG.txt");//读取权重文件
	for(int i=1;i<=input_layer;i++){
		for(int j=1;j<hide_layer;j++){
			in>>val1[i][j];
		}
	}
	for(int i=1;i<=hide_layer;i++){
		for(int j=1;j<hide_layer;j++){
			in>>val2[i][j];
		}
	}
	for(int i=1;i<=hide_layer;i++){
		for(int j=1;j<=output_layer;j++){
			in>>val3[i][j];
		}
	}
	in.close();
	//cout<<"Finish reading"<<endl; 
}
void SaveValue(){//保存权重 
	ofstream out("valueG.txt"); 
	for(int i=1;i<=input_layer;i++){
		for(int j=1;j<hide_layer;j++){
			out<<val1[i][j]<<" ";
		}
		out<<endl;
	}
	out<<endl;
	for(int i=1;i<=hide_layer;i++){
		for(int j=1;j<hide_layer;j++){
			out<<val2[i][j]<<" ";
		}
		out<<endl;
	}
	for(int i=1;i<=hide_layer;i++){
		for(int j=1;j<=output_layer;j++){
			out<<val3[i][j]<<" ";
		}
		out<<endl;
	}
	out.close(); 
}
bool readTrainFirst(){//初次读取训练集
    cout<<"Reading Train's images"<<endl;
	IMAGE img;//加载图像
	LPCTSTR l;//图片路径 函数参数
	string Image_route="./Train";//图片路径 字符串
	vector<string> files;
	DATANUM=0;
	GetAllFiles(Image_route,files);
	
	int size=files.size();
	if(size<=output_layer){
		cout<<"Can't found any image in the Train!"<<endl;
		return true;
	}
	
	srand(time(0));
	for(int i=0;i<size;i++){
		for(int j=1;j<=hide_layer;j++){//drop-out：随机死亡神经元 
			if(killp*RAND_MAX>rand()){//RAND_MAX=32767
				avai2[j]=true;
			}else{
				avai2[j]=false;
			}
		}
		for(int j=1;j<=hide_layer;j++){
			if(killp*RAND_MAX>rand()){
				avai3[j]=true;
			}else{
				avai3[j]=false;
			}
		}
		Image_route=files[i];//获取图片相对路径
		if(Image_route.length()<12)continue;
        l=Image_route.c_str();//转换类型
        loadimage(&img,l,ImageLengthOriginal,ImageLengthOriginal);//加载图像,转换为指定规格
	    Image_preprocessing(&img);//图像预处理
	    //添加图片标签
	    char s[100];
	    strcpy(s,Image_route.c_str());
	    datalable=(s[8]-'0')*10+s[9]-'0';
	    DNN_FP();//正向传播
		DNN_BP();//反向传播
		
		for(int q=0;q<input_layer;q++)x0[DATANUM][q]=x1[q];
		D[DATANUM]=datalable;
		DATANUM++;
		//EXE_RUN_TEST(ImageLength);
	}
	cout<<"Finish reading Train's images"<<endl;
	return false;
}
void readTrain(){//读取训练集
    for(int q=0;q<DATANUM;q++){
    	for(int i=1;i<=ImageLength;i++){
	        for(int j=1;j<=ImageLength;j++){
	        	for(int color_RGB=0;color_RGB<3;color_RGB++){
	    	    	datapiece[i][j]=x0[q][i*ImageLength-ImageLength+j];
		    	}
	    	}
        }
		datalable=D[q];
		for(int j=1;j<=hide_layer;j++){//drop-out：随机死亡神经元 
			if(killp*RAND_MAX>rand()){//RAND_MAX=32767
				avai2[j]=true;
			}else{
				avai2[j]=false;
			}
		}
		for(int j=1;j<=hide_layer;j++){
			if(killp*RAND_MAX>rand()){
				avai3[j]=true;
			}else{
				avai3[j]=false;
			}
		}
    	DNN_FP();//正向传播
		DNN_BP();//反向传播
	}
}
bool readTest(){//读取测试集
	//cout<<"Read Test's image..."<<endl;
	
	IMAGE img;//加载图像
	LPCTSTR l;//图片路径 函数参数
	string Image_route="./Test";//图片路径 字符串
	vector<string> files;
	double tottest=0;//测试样本数量
	double datatrue_num=0;//测试正确样本数量
	int Datacorr[output_layer+5]; 
	for(int i=1;i<=output_layer;i++){
		wrongs[i]=0;
		Datacorr[i]=0;
	}
	
	for(int j=1;j<=hide_layer;j++)avai2[j]=true;
	for(int j=1;j<=hide_layer;j++)avai3[j]=true;
	//注意！！！，文件夹需要和本程序处于同一目录下
	
	//读取所有的文件，包括子文件的文件
	GetAllFiles(Image_route,files);
	
	int size=files.size();
	if(size<=output_layer){
		cout<<"Can't found any image in the Test!"<<endl;
		return true;//无可识别jpg文件
	}
	
	for(int i=0;i<size;i++){
		Image_route=files[i];//获取图片相对路径
		if(Image_route.length()<11)continue;
        l=Image_route.c_str();//转换类型
        loadimage(&img,l,ImageLengthOriginal,ImageLengthOriginal);//加载图像,转换为指定规格
        //图像预处理
	    Image_preprocessing(&img);
	    //添加图片标签
	    char s[100];
	    strcpy(s,Image_route.c_str());
	    datalable=(s[7]-'0')*10+s[8]-'0';
	    //正向传播
	    DNN_FP();
	    //判定结果是否正确并统计
		double maxx=-99998;
	    int id=-1;
	    for(int i=1;i<=output_layer;i++){//统计结果 
		    if(x4[i]>maxx){
	            maxx=x4[i];
			    id=i;
		    }
        }
		if(datalable==id){//结果正确 
			datatrue_num++;//正确样本数量
		}else{//结果错误 
			wrongs[datalable]++;
		}
			
		Datacorr[datalable]++;
		tottest++;//统计测试样本总量
	}
	
	//cout<<"Finish reading Test's images"<<endl;
	
	//显示测试结果
	//system("cls");
	cout<<"Accuracy of test set:"<<datatrue_num*100/tottest<<"%"<<endl;
	
	if(Accuracy<(datatrue_num*100/tottest)){
		SaveValue();
		Accuracy=datatrue_num*100/tottest;
    	ofstream out("AccuracyG.txt");
        out<<Accuracy;
        out.close();
	}else{
		return false;
	}
	//错误标签数量
	//cout<<"Labels with the errors"<<endl;
	//for(int i=1;i<=output_layer;i++)cout<<i<<"->"<<wrongs[i]<<endl;
	
	//更新测试日志
	ofstream out;
	out.open("test_logG.txt",ios::out|ios::ate|ios::app);
	time_t t;
	time(&t);
	out<<ctime(&t)<<"Accuracy of test set:"<<datatrue_num*100/tottest<<"%"<<endl;
	//out<<"Labels with the errors"<<endl;
	//for(int i=1;i<=output_layer;i++)out<<i<<"->"<<wrongs[i]<<"/"<<Datacorr[i]<<endl;
	out.close(); 
	return false;
}
void readTarget(){//读取识别集
    //cout<<"Read Target's image..."<<endl;
    
    for(int j=1;j<=hide_layer;j++){
    	avai2[j]=true;
    	avai3[j]=true;
	}
	IMAGE img;//加载图像
	LPCTSTR l;//图片路径 函数参数 
	string Image_route="./Target";//图片路径 字符串
	vector<string> files;
	GetAllFiles(Image_route,files);
	int size=files.size();
	if(size==0){
		cout<<"Can't found any image!"<<endl;
		return;//无可识别jpg文件
	}
	
	Image_route=files[0];//获取图片相对路径 
	//cout<<Image_route<<endl;
    l=Image_route.c_str();//转换类型
    loadimage(&img,l,ImageLengthOriginal,ImageLengthOriginal);//加载图像,转换为指定规格
	
		
	//图像预处理
	Image_preprocessing(&img);
	//正向传播
	DNN_FP();
	
	//cout<<"Finish reading Target's image"<<endl;
}
void Image_preprocessing(IMAGE* Img){//图像预处理
    //对初步调节大小的图片进行处理 
	ReadRGB(Img);//读取RGB值 导入图像数组
	Convolutional_layer(ImageLengthOriginal);//卷积层
	Pooling_layer(ImageLengthOriginal,ImageLength*2);//池化层
	Activitation_layer(ImageLength*2);//激活层
	
	Convolutional_layer(ImageLength*2);//卷积层
	Pooling_layer(ImageLength*2,ImageLength);//池化层
	Activitation_layer(ImageLength);//激活层
	Normalization(ImageLength);//归一化
	//EXE_RUN_TEST(ImageLength);
}
void DNN_FP(){//全连接神经网络 正向传播
	//输入层
	for(int i=1;i<=ImageLength;i++){
	    for(int j=1;j<=ImageLength;j++){
	    	for(int color_RGB=0;color_RGB<3;color_RGB++){
	    		x1[i*ImageLength-ImageLength+j]=datapiece[i][j];
			}
		}
    }
	x1[input_layer]=1;//偏置值 
	//初始化隐藏层 
	for(int i=1;i<hide_layer;i++){
		x2[i]=0;
		x3[i]=0;
	}
	x2[hide_layer]=1;//偏置值 
	x3[hide_layer]=1;//偏置值 
	//初始化输出层
	for(int i=1;i<=output_layer;i++){ 
		x4[i]=0;
	}
	//神经元模型
	for(int i=1;i<=input_layer;i++){//输入层->隐藏层
		for(int j=1;j<hide_layer;j++){
			if(avai2[j]==false)continue;//死亡神经元不参与传播 
			x2[j]+=x1[i]*val1[i][j];//i-输入层神经元编号，j-隐藏层1神经元编号 
		}
	}
	for(int i=1;i<hide_layer;i++){
		if(avai2[i]==false)continue;
		x2[i]=RELU(x2[i]);//RELU 
	}
	for(int i=1;i<=hide_layer;i++){//隐藏层->隐藏层
	    if(avai2[i]==false)continue;
		for(int j=1;j<hide_layer;j++){
			if(avai3[j]==false)continue;
			x3[j]+=x2[i]*val2[i][j];
		}
	}
	for(int i=1;i<hide_layer;i++){
		if(avai3[i]==false)continue;
		x3[i]=RELU(x3[i]);//RELU
	}
	for(int i=1;i<=hide_layer;i++){//隐藏层->输出层 
		if(avai3[i]==false)continue;
		for(int j=1;j<=output_layer;j++){
			x4[j]+=x3[i]*val3[i][j];
		}
	}
	for(int i=1;i<=output_layer;i++){
		x4[i]=RELU(x4[i]);
	}
}
void DNN_BP(){//全连接神经网络 反向传播 修正权重参数
	for(int i=1;i<=output_layer;i++){//更新输出层梯度 
		if(i==datalable){
			delta4[i]=min(0.0,x4[i]-1);
		}
		else{
			delta4[i]=x4[i];
		}
	}
	//初始化隐藏层梯度
	for(int i=1;i<=hide_layer;i++){
		delta2[i]=0;
		delta3[i]=0;
	}
	//更新权重 
	for(int i=1;i<=output_layer;i++){//输出层 
		for(int j=1;j<=hide_layer;j++){//靠近输出层的隐藏层 
			delta3[j]+=val3[j][i]*RELUdao(x4[i])*delta4[i];//更新梯度 
			val3[j][i]-=study*delta4[i]*RELUdao(x4[i])*x3[j]+study*regularize*(val3[j][i]);//更新权重
			if(val3[j][i]>1.0||val3[j][i]<-1.0)val3[j][i]=0.01;
		}
	}
	for(int i=1;i<hide_layer;i++){
		if(avai3[i]==false)continue;
		for(int j=1;j<=hide_layer;j++){
			if(avai2[j]==false)continue;
			delta2[j]+=val2[j][i]*RELUdao(x3[i])*delta3[i];
			val2[j][i]-=study*delta3[i]*RELUdao(x3[i])*x2[j]+study*regularize*(val2[j][i]);
			if(val2[j][i]>1.0||val2[j][i]<-1.0)val2[j][i]=0.01;
		}
	}
	for(int i=1;i<=hide_layer;i++){
		if(avai2[i]==false)continue;
		for(int j=1;j<=input_layer;j++){
			val1[j][i]-=study*delta2[i]*RELUdao(x2[i])*x1[j]+study*regularize*(val1[j][i]);
			if(val1[j][i]>1.0||val1[j][i]<-1.0)val1[j][i]=0.01;
		}
	}
}
void Target_ans(int ID){//输出识别结果到ans.txt
    //cout<<"The image is:"<<ID<<endl; 
	ofstream out("ans.txt");
	out<<ID<<endl;
	out.close();
}
 
bool randValue(){//初始化权重
	ifstream fin("valueG.txt");
	if(fin.is_open()){//打开正常 
		fin.seekg(0,ios::end);
		int size=fin.tellg();//获取文件大小 
		fin.close();
		if(size>0)return 0;//文件不为空 
    }else{//打开异常，可能不存在该文件 
    	return 1;
	}
	
	cout<<"Initializing value..."<<endl;
	srand(time(0));//当前时间初始化随机数种子
	
	//输入层->隐藏层
	for (int i=1;i<=input_layer;i++) { 
		for (int j=1;j<=hide_layer;j++) {
			val1[i][j]=(double)(rand()%2000-1000)*stb/1000;
		}
	}
    //隐藏层->下一隐藏层 
	for(int i=1;i<=hide_layer;i++){
		for(int j=1;j<=hide_layer;j++){
			val2[i][j]=(double)(rand()%2000-1000)*stb/1000;
		}
	}
	//隐藏层->输出层
	for (int i=1;i<=hide_layer;i++) {
		for (int j=1;j<=output_layer;j++) {
			val3[i][j]=(double)(rand()%2000-1000)*stb/1000;
		}
	}
	
	cout<<"Complete initialization"<<endl;
	
	SaveValue();//保存权重
	return 0;
}
void GetAllFiles( string path, vector<string>& files){//读取所有的文件，包括子文件的文件
	long hFile=0;  
	//文件信息  
	struct _finddata_t fileinfo;  
	string p;  
	if((hFile=_findfirst(p.assign(path).append("\\*").c_str(),&fileinfo))!=-1){  
		do{   
			if((fileinfo.attrib&_A_SUBDIR)){  
				if(strcmp(fileinfo.name,".")!=0&&strcmp(fileinfo.name,"..")!=0){
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
					GetAllFiles(p.assign(path).append("\\").append(fileinfo.name),files); 
				}
			}else{  
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));  
			} 
		}while(_findnext(hFile, &fileinfo)==0);  
		_findclose(hFile); 
	} 
}
void ReadRGB(IMAGE* Img){//读取RGB值 导入图像数组
    //获取原图片的缓冲区数组指针(一维数组） 
	DWORD* pImg=GetImageBuffer(Img);
	for(int i=0;i<(ImageLengthOriginal*ImageLengthOriginal);i++){
		DWORD color=pImg[i];
		//datapiece[i/ImageLengthOriginal+1][i%ImageLengthOriginal+1]=(UCHAR)color;//获取红色 Red值  
		datapiece[i/ImageLengthOriginal+1][i%ImageLengthOriginal+1]=(UCHAR)(color>>8);//获取绿色 Green值  
		//datapiece[2][i/ImageLengthOriginal+1][i%ImageLengthOriginal+1]=(UCHAR)(color>>16);//获取蓝色 Blue值
	}
	//EXE_RUN_TEST(ImageLengthOriginal,ImageLengthOriginal);//测试用检测图像数组转换图片效果
}
void Normalization(int L2){//归一化
	for(int i=1;i<=L2;i++){
		for(int j=1;j<=L2;j++){
			//datapiece[i][j]=(datapiece[i][j]/255.0)*2.0-1.0;
			datapiece[i][j]=datapiece[i][j]/255.0;
		}
	}
}
void Convolutional_layer(int L){//卷积层
	for(int i=1;i<=L;i++){
		for(int j=1;j<=L;j++){
			//判断横向边界 
			Kernel_datapiece_1[i][j]=(datapiece[i-1][j+1]*Kernel_1[0]);
			Kernel_datapiece_1[i][j]+=(datapiece[i][j+1]*Kernel_1[1]);
			Kernel_datapiece_1[i][j]+=(datapiece[i+1][j+1]*Kernel_1[2]);
			Kernel_datapiece_1[i][j]+=(datapiece[i-1][j]*Kernel_1[3]);
			Kernel_datapiece_1[i][j]+=(datapiece[i][j]*Kernel_1[4]);
			Kernel_datapiece_1[i][j]+=(datapiece[i+1][j]*Kernel_1[5]);
			Kernel_datapiece_1[i][j]+=(datapiece[i-1][j-1]*Kernel_1[6]);
			Kernel_datapiece_1[i][j]+=(datapiece[i][j-1]*Kernel_1[7]);
			Kernel_datapiece_1[i][j]+=(datapiece[i+1][j-1]*Kernel_1[8]);
			if(Kernel_datapiece_1[i][j]<0)Kernel_datapiece_1[i][j]=0-Kernel_datapiece_1[i][j];
			//判断竖向边界 
			Kernel_datapiece_2[i][j]=(datapiece[i-1][j+1]*Kernel_2[0]);
			Kernel_datapiece_2[i][j]+=(datapiece[i][j+1]*Kernel_2[1]);
			Kernel_datapiece_2[i][j]+=(datapiece[i+1][j+1]*Kernel_2[2]);
			Kernel_datapiece_2[i][j]+=(datapiece[i-1][j]*Kernel_2[3]);
			Kernel_datapiece_2[i][j]+=(datapiece[i][j]*Kernel_2[4]);
			Kernel_datapiece_2[i][j]+=(datapiece[i+1][j]*Kernel_2[5]);
			Kernel_datapiece_2[i][j]+=(datapiece[i-1][j-1]*Kernel_2[6]);
			Kernel_datapiece_2[i][j]+=(datapiece[i][j-1]*Kernel_2[7]);
			Kernel_datapiece_2[i][j]+=(datapiece[i+1][j-1]*Kernel_2[8]);
			if(Kernel_datapiece_2[i][j]<0)Kernel_datapiece_2[i][j]=0-Kernel_datapiece_2[i][j];
			//判断左上到右下边界
			Kernel_datapiece_3[i][j]=(datapiece[i-1][j+1]*Kernel_3[0]);
			Kernel_datapiece_3[i][j]+=(datapiece[i][j+1]*Kernel_3[1]);
			Kernel_datapiece_3[i][j]+=(datapiece[i+1][j+1]*Kernel_3[2]);
			Kernel_datapiece_3[i][j]+=(datapiece[i-1][j]*Kernel_3[3]);
			Kernel_datapiece_3[i][j]+=(datapiece[i][j]*Kernel_3[4]);
			Kernel_datapiece_3[i][j]+=(datapiece[i+1][j]*Kernel_3[5]);
			Kernel_datapiece_3[i][j]+=(datapiece[i-1][j-1]*Kernel_3[6]);
			Kernel_datapiece_3[i][j]+=(datapiece[i][j-1]*Kernel_3[7]);
			Kernel_datapiece_3[i][j]+=(datapiece[i+1][j-1]*Kernel_3[8]);
			if(Kernel_datapiece_3[i][j]<0)Kernel_datapiece_3[i][j]=0-Kernel_datapiece_3[i][j];
			//判断右上到左下边界
			Kernel_datapiece_4[i][j]=(datapiece[i-1][j+1]*Kernel_4[0]);
			Kernel_datapiece_4[i][j]+=(datapiece[i][j+1]*Kernel_4[1]);
			Kernel_datapiece_4[i][j]+=(datapiece[i+1][j+1]*Kernel_4[2]);
			Kernel_datapiece_4[i][j]+=(datapiece[i-1][j]*Kernel_4[3]);
			Kernel_datapiece_4[i][j]+=(datapiece[i][j]*Kernel_4[4]);
			Kernel_datapiece_4[i][j]+=(datapiece[i+1][j]*Kernel_4[5]);
			Kernel_datapiece_4[i][j]+=(datapiece[i-1][j-1]*Kernel_4[6]);
			Kernel_datapiece_4[i][j]+=(datapiece[i][j-1]*Kernel_4[7]);
			Kernel_datapiece_4[i][j]+=(datapiece[i+1][j-1]*Kernel_4[8]);
			if(Kernel_datapiece_4[i][j]<0)Kernel_datapiece_4[i][j]=0-Kernel_datapiece_4[i][j];
		}
	}
	
	for(int i=1;i<=L;i++){
		for(int j=1;j<=L;j++){
			datapiece[i][j]=(Kernel_datapiece_1[i][j]+Kernel_datapiece_2[i][j]+Kernel_datapiece_3[i][j]+Kernel_datapiece_4[i][j])/Kernel_num;
		}
	}
    //EXE_RUN_TEST(ImageLengthOriginal,ImageLengthOriginal);//测试用检测图像数组转换图片效果
}
void Pooling_layer(int L1,int L2){//池化层
	int Ratio=L1/L2;
	double Poolpiece[L2+5][L2+5];
	
	//扫描卷积图 
	for(int i=1;i<=L2;i++){
		for(int j=1;j<=L2;j++){
			//压缩图片 
			Poolpiece[i][j]=-1;
			for(int i2=0;i2<Ratio;i2++){
				for(int j2=0;j2<Ratio;j2++){
					Poolpiece[i][j]=max(Poolpiece[i][j],datapiece[i*Ratio-Ratio+1+i2][j*Ratio-Ratio+1+j2]);
				}
			}
		}
	}
	//导入图像数组 
	for(int i=1;i<=L2;i++){
		for(int j=1;j<=L2;j++){
			datapiece[i][j]=Poolpiece[i][j];
		}
	}
	
	 //EXE_RUN_TEST(ImageLengthOriginal,ImageLengthOriginal);//测试用检测图像数组转换图片效果
}
void Activitation_layer(int L2){//激活层
	for(int i=1;i<=L2;i++){
		for(int j=1;j<=L2;j++){
			datapiece[i][j]=RELU(datapiece[i][j]);
		}
	}
}
double RELU(double x){//Leaky ReLu激活函数 
	return max(x,0.0)+min(yita*x,0.0);
}
double RELUdao(double x){//梯度下降使用函数之一 
//	return x*(1-x);
	if(x<=0)return yita;
	return 1.0;
}
void Gd(){//高斯模糊 去除噪声
	double GSdatapiece[ImageLengthOriginal+1][ImageLengthOriginal+1];
	double GSKenal[25]={0.003,0.013,0.022,0.013,0.003, 
	                    0.013,0.060,0.098,0.060,0.013,
			            0.022,0.098,0.162,0.098,0.022,
						0.013,0.060,0.098,0.060,0.013,
						0.003,0.013,0.022,0.013,0.003};
	int dxy[25][2]={(-2,-2),(-1,-2),(0,-2),(1,-2),(2,-2),
	                (-2,-1),(-1,-1),(0,-1),(1,-1),(2,-1),
					(-2,0),(-1,0),(0,0),(1,0),(2,0),
					(-2,1),(-1,1),(0,1),(1,1),(2,2),
					(-2,2),(-1,2),(0,2),(1,2),(2,2)};
	for(int i=3;i<=ImageLengthOriginal-2;i++){
		for(int j=3;j<=ImageLengthOriginal-2;j++){
			GSdatapiece[i][j]=0;
			for(int k=0;k<25;k++){
				GSdatapiece[i][j]+=(GSKenal[k]*datapiece[i+dxy[k][0]][j+dxy[k][1]]);
			} 
		}
	}
	for(int i=3;i<=ImageLengthOriginal-2;i++){
		for(int j=3;j<=ImageLengthOriginal-2;j++){
			datapiece[i][j]=GSdatapiece[i][j];
		}
	}
	//EXE_RUN_TEST(ImageLengthOriginal,ImageLengthOriginal);
}

void EXE_RUN_TEST(int L1,int L2){//测试用检测图像数组转换图片效果
    IMAGE Img;
    int arv;
	Resize(&Img,L2,L1);
	DWORD* pImg=GetImageBuffer(&Img);
	for(int i=0;i<(L1*L2);i++){
		arv=datapiece[i/L1+1][i%L2+1]; 
		pImg[i]=RGB(0,arv,0);
	}
	initgraph(L2,L1);//创建窗口 
	setbkcolor(WHITE);//设置窗口背景颜色 
	cleardevice(); 
	putimage(0,0,&Img);//显示图片
	system("pause"); 
	closegraph();
}
