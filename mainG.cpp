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

//ϵͳ����
#define MOUSE_Click(mButton) (GetAsyncKeyState(mButton)&0x8000?1:0)//������¼�
#define DataNumber 3000
double Accuracy; 

//���������
#define ImageLengthOriginal 224//��ʼ��ͼƬ��С ע�⣡ImageLength_original������ImageLength�ı��� 
#define ImageLength 28//ͼƬ�߳�
#define input_layer 785//�������Ԫ����
#define hide_layer 91//���ز�
#define output_layer 15//�������Ԫ����

double killp=0.6f;//drop-out����
double yita=0.01f;//�����RELU  
double study=0.01;//ѧϰ��
double regularize=0.001f;//L_2 
double stb=0.1f;//��ʼ��Ȩ��

//���ν�����һ��ͼƬ 
double datapiece[ImageLengthOriginal+1][ImageLengthOriginal+1];//ͼ������(��ͼ) 
int datalable;//������ǩ(lable)
int wrongs[output_layer+1];//ͳ�ƴ��������ı�ǩ
double x1[input_layer+1], x2[hide_layer+1],x3[hide_layer+1],x4[output_layer+1];//��ϸ��
double val1[input_layer+1][hide_layer+1],val2[hide_layer+1][hide_layer+1],val3[hide_layer+1][output_layer+1];//����Ȩ�� 
double delta2[hide_layer+1],delta3[hide_layer+1],delta4[output_layer+1];//�ݶ� 
bool avai2[hide_layer+5],avai3[hide_layer+5];//drop-out

//ѵ����
double x0[DataNumber][input_layer+1];
int D[DataNumber];//������ǩ(lable)
int DATANUM;

//�������� 
#define Kernel_num 4//����˵����� 
double Kernel_datapiece_1[ImageLengthOriginal+5][ImageLengthOriginal+5];//ͼ�����߽�
double Kernel_datapiece_2[ImageLengthOriginal+5][ImageLengthOriginal+5];//ͼ������߽�
double Kernel_datapiece_3[ImageLengthOriginal+5][ImageLengthOriginal+5];//ͼ�����ϵ����±߽�
double Kernel_datapiece_4[ImageLengthOriginal+5][ImageLengthOriginal+5];//ͼ�����ϵ����±߽�
double Kernel_1[9]={-0.25,0,0.25,
	                -0.5,0,0.5,
	                -0.25,0,0.25};//�����1
double Kernel_2[9]={-0.25,-0.5,-0.25,
	                0,0,0,
	                0.25,0.5,0.25};//�����2
double Kernel_3[9]={0,0.25,0.5,
	                -0.25,0,0.25,
	                -0.5,-0.25,0};//�����3
double Kernel_4[9]={0.5,0.25,0,
	                0.25,0,0.25,
	                0,-0.25,-0.5};//�����4

//�������� 
void Train();//ѵ��ģʽ 
void Target();//ʶ��ģʽ 
void readValue();//����Ȩ��
void SaveValue();//����Ȩ�ص�value.txt 
bool readTrainFirst();//���ζ�ȡѵ���� 
void readTrain();//��ȡѵ����
bool readTest();//��ȡ���Լ�
void readTarget();//��ȡʶ��
void Target_ans(int ID);//���ʶ���� 
void Image_preprocessing(IMAGE* Img);//ͼ��Ԥ���� ע���ڸú����вŽ�ͼƬ����datapiece���� 
void DNN_FP();//ȫ���������� ���򴫲�
void DNN_BP();//ȫ���������� ���򴫲� �ݶ��½��㷨 
bool randValue();//��ʼ��Ȩ��
void GetAllFiles(string path,vector<string>&files);//��ȡ���е��ļ����������ļ����ļ�
void ReadRGB(IMAGE* Img);//��ȡRGBֵ ����ͼ������
void Gd();//��˹ģ�� ȥ������
void Normalization(int L2);//��һ��
void Convolutional_layer(int L);//�����
void Pooling_layer(int L1,int L2);//�ػ���
void Activitation_layer(int L2);//�����
double RELU(double x);//Leaky ReLu�����
double RELUdao(double x);//�ݶ��½�ʹ�ú���֮һ 

//���Ժ�������
void EXE_RUN_TEST(int L1,int L2);//�����ü��ͼ������ת��ͼƬЧ��

int main(int argc , char* argv[],char* envp[]){
	readValue();//����Ȩ��
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

void Train(){//ѵ��ģʽ
    ifstream in("AccuracyG.txt");
	in>>Accuracy;
	in.close();
    if(readTrainFirst())return;
	while(1){//ѵ������
    	readTrain();//��ȡѵ����
    	if(MOUSE_Click(VK_RBUTTON)) break; //��������Ҽ���ֹѭ��
    	if(readTest()){//��ȡ���Լ�
    	    return;
        }	
        if(MOUSE_Click(VK_RBUTTON)) break; //��������Ҽ���ֹѭ��
	}
}
void Target(){//ʶ��ģʽ
	readValue();//����Ȩ��
	readTarget();//��ȡʶ��
	
	double maxx=-99998;
	int id=-1;
	for(int i=1;i<=output_layer;i++){//ͳ�ƽ��
		if(x4[i]>maxx){//ѡ�������ߵĿ�����
		    maxx=x4[i];
			id=i;
		}
    }
	Target_ans(id);//���ʶ������ans.txt
}
void readValue(){//����Ȩ��
    //cout<<"Read value..."<<endl;
	if(randValue()){
    	cout<<"[Error]Can't found value.txt."<<endl;
    	return;
	}
	
	ifstream in("valueG.txt");//��ȡȨ���ļ�
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
void SaveValue(){//����Ȩ�� 
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
bool readTrainFirst(){//���ζ�ȡѵ����
    cout<<"Reading Train's images"<<endl;
	IMAGE img;//����ͼ��
	LPCTSTR l;//ͼƬ·�� ��������
	string Image_route="./Train";//ͼƬ·�� �ַ���
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
		for(int j=1;j<=hide_layer;j++){//drop-out�����������Ԫ 
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
		Image_route=files[i];//��ȡͼƬ���·��
		if(Image_route.length()<12)continue;
        l=Image_route.c_str();//ת������
        loadimage(&img,l,ImageLengthOriginal,ImageLengthOriginal);//����ͼ��,ת��Ϊָ�����
	    Image_preprocessing(&img);//ͼ��Ԥ����
	    //���ͼƬ��ǩ
	    char s[100];
	    strcpy(s,Image_route.c_str());
	    datalable=(s[8]-'0')*10+s[9]-'0';
	    DNN_FP();//���򴫲�
		DNN_BP();//���򴫲�
		
		for(int q=0;q<input_layer;q++)x0[DATANUM][q]=x1[q];
		D[DATANUM]=datalable;
		DATANUM++;
		//EXE_RUN_TEST(ImageLength);
	}
	cout<<"Finish reading Train's images"<<endl;
	return false;
}
void readTrain(){//��ȡѵ����
    for(int q=0;q<DATANUM;q++){
    	for(int i=1;i<=ImageLength;i++){
	        for(int j=1;j<=ImageLength;j++){
	        	for(int color_RGB=0;color_RGB<3;color_RGB++){
	    	    	datapiece[i][j]=x0[q][i*ImageLength-ImageLength+j];
		    	}
	    	}
        }
		datalable=D[q];
		for(int j=1;j<=hide_layer;j++){//drop-out�����������Ԫ 
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
    	DNN_FP();//���򴫲�
		DNN_BP();//���򴫲�
	}
}
bool readTest(){//��ȡ���Լ�
	//cout<<"Read Test's image..."<<endl;
	
	IMAGE img;//����ͼ��
	LPCTSTR l;//ͼƬ·�� ��������
	string Image_route="./Test";//ͼƬ·�� �ַ���
	vector<string> files;
	double tottest=0;//������������
	double datatrue_num=0;//������ȷ��������
	int Datacorr[output_layer+5]; 
	for(int i=1;i<=output_layer;i++){
		wrongs[i]=0;
		Datacorr[i]=0;
	}
	
	for(int j=1;j<=hide_layer;j++)avai2[j]=true;
	for(int j=1;j<=hide_layer;j++)avai3[j]=true;
	//ע�⣡�������ļ�����Ҫ�ͱ�������ͬһĿ¼��
	
	//��ȡ���е��ļ����������ļ����ļ�
	GetAllFiles(Image_route,files);
	
	int size=files.size();
	if(size<=output_layer){
		cout<<"Can't found any image in the Test!"<<endl;
		return true;//�޿�ʶ��jpg�ļ�
	}
	
	for(int i=0;i<size;i++){
		Image_route=files[i];//��ȡͼƬ���·��
		if(Image_route.length()<11)continue;
        l=Image_route.c_str();//ת������
        loadimage(&img,l,ImageLengthOriginal,ImageLengthOriginal);//����ͼ��,ת��Ϊָ�����
        //ͼ��Ԥ����
	    Image_preprocessing(&img);
	    //���ͼƬ��ǩ
	    char s[100];
	    strcpy(s,Image_route.c_str());
	    datalable=(s[7]-'0')*10+s[8]-'0';
	    //���򴫲�
	    DNN_FP();
	    //�ж�����Ƿ���ȷ��ͳ��
		double maxx=-99998;
	    int id=-1;
	    for(int i=1;i<=output_layer;i++){//ͳ�ƽ�� 
		    if(x4[i]>maxx){
	            maxx=x4[i];
			    id=i;
		    }
        }
		if(datalable==id){//�����ȷ 
			datatrue_num++;//��ȷ��������
		}else{//������� 
			wrongs[datalable]++;
		}
			
		Datacorr[datalable]++;
		tottest++;//ͳ�Ʋ�����������
	}
	
	//cout<<"Finish reading Test's images"<<endl;
	
	//��ʾ���Խ��
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
	//�����ǩ����
	//cout<<"Labels with the errors"<<endl;
	//for(int i=1;i<=output_layer;i++)cout<<i<<"->"<<wrongs[i]<<endl;
	
	//���²�����־
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
void readTarget(){//��ȡʶ��
    //cout<<"Read Target's image..."<<endl;
    
    for(int j=1;j<=hide_layer;j++){
    	avai2[j]=true;
    	avai3[j]=true;
	}
	IMAGE img;//����ͼ��
	LPCTSTR l;//ͼƬ·�� �������� 
	string Image_route="./Target";//ͼƬ·�� �ַ���
	vector<string> files;
	GetAllFiles(Image_route,files);
	int size=files.size();
	if(size==0){
		cout<<"Can't found any image!"<<endl;
		return;//�޿�ʶ��jpg�ļ�
	}
	
	Image_route=files[0];//��ȡͼƬ���·�� 
	//cout<<Image_route<<endl;
    l=Image_route.c_str();//ת������
    loadimage(&img,l,ImageLengthOriginal,ImageLengthOriginal);//����ͼ��,ת��Ϊָ�����
	
		
	//ͼ��Ԥ����
	Image_preprocessing(&img);
	//���򴫲�
	DNN_FP();
	
	//cout<<"Finish reading Target's image"<<endl;
}
void Image_preprocessing(IMAGE* Img){//ͼ��Ԥ����
    //�Գ������ڴ�С��ͼƬ���д��� 
	ReadRGB(Img);//��ȡRGBֵ ����ͼ������
	Convolutional_layer(ImageLengthOriginal);//�����
	Pooling_layer(ImageLengthOriginal,ImageLength*2);//�ػ���
	Activitation_layer(ImageLength*2);//�����
	
	Convolutional_layer(ImageLength*2);//�����
	Pooling_layer(ImageLength*2,ImageLength);//�ػ���
	Activitation_layer(ImageLength);//�����
	Normalization(ImageLength);//��һ��
	//EXE_RUN_TEST(ImageLength);
}
void DNN_FP(){//ȫ���������� ���򴫲�
	//�����
	for(int i=1;i<=ImageLength;i++){
	    for(int j=1;j<=ImageLength;j++){
	    	for(int color_RGB=0;color_RGB<3;color_RGB++){
	    		x1[i*ImageLength-ImageLength+j]=datapiece[i][j];
			}
		}
    }
	x1[input_layer]=1;//ƫ��ֵ 
	//��ʼ�����ز� 
	for(int i=1;i<hide_layer;i++){
		x2[i]=0;
		x3[i]=0;
	}
	x2[hide_layer]=1;//ƫ��ֵ 
	x3[hide_layer]=1;//ƫ��ֵ 
	//��ʼ�������
	for(int i=1;i<=output_layer;i++){ 
		x4[i]=0;
	}
	//��Ԫģ��
	for(int i=1;i<=input_layer;i++){//�����->���ز�
		for(int j=1;j<hide_layer;j++){
			if(avai2[j]==false)continue;//������Ԫ�����봫�� 
			x2[j]+=x1[i]*val1[i][j];//i-�������Ԫ��ţ�j-���ز�1��Ԫ��� 
		}
	}
	for(int i=1;i<hide_layer;i++){
		if(avai2[i]==false)continue;
		x2[i]=RELU(x2[i]);//RELU 
	}
	for(int i=1;i<=hide_layer;i++){//���ز�->���ز�
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
	for(int i=1;i<=hide_layer;i++){//���ز�->����� 
		if(avai3[i]==false)continue;
		for(int j=1;j<=output_layer;j++){
			x4[j]+=x3[i]*val3[i][j];
		}
	}
	for(int i=1;i<=output_layer;i++){
		x4[i]=RELU(x4[i]);
	}
}
void DNN_BP(){//ȫ���������� ���򴫲� ����Ȩ�ز���
	for(int i=1;i<=output_layer;i++){//����������ݶ� 
		if(i==datalable){
			delta4[i]=min(0.0,x4[i]-1);
		}
		else{
			delta4[i]=x4[i];
		}
	}
	//��ʼ�����ز��ݶ�
	for(int i=1;i<=hide_layer;i++){
		delta2[i]=0;
		delta3[i]=0;
	}
	//����Ȩ�� 
	for(int i=1;i<=output_layer;i++){//����� 
		for(int j=1;j<=hide_layer;j++){//�������������ز� 
			delta3[j]+=val3[j][i]*RELUdao(x4[i])*delta4[i];//�����ݶ� 
			val3[j][i]-=study*delta4[i]*RELUdao(x4[i])*x3[j]+study*regularize*(val3[j][i]);//����Ȩ��
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
void Target_ans(int ID){//���ʶ������ans.txt
    //cout<<"The image is:"<<ID<<endl; 
	ofstream out("ans.txt");
	out<<ID<<endl;
	out.close();
}
 
bool randValue(){//��ʼ��Ȩ��
	ifstream fin("valueG.txt");
	if(fin.is_open()){//������ 
		fin.seekg(0,ios::end);
		int size=fin.tellg();//��ȡ�ļ���С 
		fin.close();
		if(size>0)return 0;//�ļ���Ϊ�� 
    }else{//���쳣�����ܲ����ڸ��ļ� 
    	return 1;
	}
	
	cout<<"Initializing value..."<<endl;
	srand(time(0));//��ǰʱ���ʼ�����������
	
	//�����->���ز�
	for (int i=1;i<=input_layer;i++) { 
		for (int j=1;j<=hide_layer;j++) {
			val1[i][j]=(double)(rand()%2000-1000)*stb/1000;
		}
	}
    //���ز�->��һ���ز� 
	for(int i=1;i<=hide_layer;i++){
		for(int j=1;j<=hide_layer;j++){
			val2[i][j]=(double)(rand()%2000-1000)*stb/1000;
		}
	}
	//���ز�->�����
	for (int i=1;i<=hide_layer;i++) {
		for (int j=1;j<=output_layer;j++) {
			val3[i][j]=(double)(rand()%2000-1000)*stb/1000;
		}
	}
	
	cout<<"Complete initialization"<<endl;
	
	SaveValue();//����Ȩ��
	return 0;
}
void GetAllFiles( string path, vector<string>& files){//��ȡ���е��ļ����������ļ����ļ�
	long hFile=0;  
	//�ļ���Ϣ  
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
void ReadRGB(IMAGE* Img){//��ȡRGBֵ ����ͼ������
    //��ȡԭͼƬ�Ļ���������ָ��(һά���飩 
	DWORD* pImg=GetImageBuffer(Img);
	for(int i=0;i<(ImageLengthOriginal*ImageLengthOriginal);i++){
		DWORD color=pImg[i];
		//datapiece[i/ImageLengthOriginal+1][i%ImageLengthOriginal+1]=(UCHAR)color;//��ȡ��ɫ Redֵ  
		datapiece[i/ImageLengthOriginal+1][i%ImageLengthOriginal+1]=(UCHAR)(color>>8);//��ȡ��ɫ Greenֵ  
		//datapiece[2][i/ImageLengthOriginal+1][i%ImageLengthOriginal+1]=(UCHAR)(color>>16);//��ȡ��ɫ Blueֵ
	}
	//EXE_RUN_TEST(ImageLengthOriginal,ImageLengthOriginal);//�����ü��ͼ������ת��ͼƬЧ��
}
void Normalization(int L2){//��һ��
	for(int i=1;i<=L2;i++){
		for(int j=1;j<=L2;j++){
			//datapiece[i][j]=(datapiece[i][j]/255.0)*2.0-1.0;
			datapiece[i][j]=datapiece[i][j]/255.0;
		}
	}
}
void Convolutional_layer(int L){//�����
	for(int i=1;i<=L;i++){
		for(int j=1;j<=L;j++){
			//�жϺ���߽� 
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
			//�ж�����߽� 
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
			//�ж����ϵ����±߽�
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
			//�ж����ϵ����±߽�
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
    //EXE_RUN_TEST(ImageLengthOriginal,ImageLengthOriginal);//�����ü��ͼ������ת��ͼƬЧ��
}
void Pooling_layer(int L1,int L2){//�ػ���
	int Ratio=L1/L2;
	double Poolpiece[L2+5][L2+5];
	
	//ɨ����ͼ 
	for(int i=1;i<=L2;i++){
		for(int j=1;j<=L2;j++){
			//ѹ��ͼƬ 
			Poolpiece[i][j]=-1;
			for(int i2=0;i2<Ratio;i2++){
				for(int j2=0;j2<Ratio;j2++){
					Poolpiece[i][j]=max(Poolpiece[i][j],datapiece[i*Ratio-Ratio+1+i2][j*Ratio-Ratio+1+j2]);
				}
			}
		}
	}
	//����ͼ������ 
	for(int i=1;i<=L2;i++){
		for(int j=1;j<=L2;j++){
			datapiece[i][j]=Poolpiece[i][j];
		}
	}
	
	 //EXE_RUN_TEST(ImageLengthOriginal,ImageLengthOriginal);//�����ü��ͼ������ת��ͼƬЧ��
}
void Activitation_layer(int L2){//�����
	for(int i=1;i<=L2;i++){
		for(int j=1;j<=L2;j++){
			datapiece[i][j]=RELU(datapiece[i][j]);
		}
	}
}
double RELU(double x){//Leaky ReLu����� 
	return max(x,0.0)+min(yita*x,0.0);
}
double RELUdao(double x){//�ݶ��½�ʹ�ú���֮һ 
//	return x*(1-x);
	if(x<=0)return yita;
	return 1.0;
}
void Gd(){//��˹ģ�� ȥ������
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

void EXE_RUN_TEST(int L1,int L2){//�����ü��ͼ������ת��ͼƬЧ��
    IMAGE Img;
    int arv;
	Resize(&Img,L2,L1);
	DWORD* pImg=GetImageBuffer(&Img);
	for(int i=0;i<(L1*L2);i++){
		arv=datapiece[i/L1+1][i%L2+1]; 
		pImg[i]=RGB(0,arv,0);
	}
	initgraph(L2,L1);//�������� 
	setbkcolor(WHITE);//���ô��ڱ�����ɫ 
	cleardevice(); 
	putimage(0,0,&Img);//��ʾͼƬ
	system("pause"); 
	closegraph();
}
