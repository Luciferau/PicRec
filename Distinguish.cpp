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

#define MOUSE_Click(mButton) (GetAsyncKeyState(mButton)&0x8000?1:0)

string Image_route;//ͼƬ·�� �ַ���
IMAGE img;//����ͼ��
LPCTSTR l;//ͼƬ·�� ��������
RECT rect,rectDesktop;

//�������� 
void GetAllFiles(string path,vector<string>&files);//��ȡ���е��ļ����������ļ����ļ�
void gotoXY(short x,short y);//���ù��λ��
int FirstWindow();
void CenterWindow(RECT &rect,RECT &desktop);//ʹ�����ƶ����������λ��

void GetAllFiles(string path,vector<string>&files){//��ȡ���е��ļ����������ļ����ļ�
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
		}while(_findnext(hFile,&fileinfo)==0);  
		_findclose(hFile); 
	} 
}

void gotoXY(short x,short y){//���ù��λ�ã���������Ͻ�(0,0)��ʼ������ֵ
    COORD position={x,y};
    HANDLE hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hConsole, position);
}

void CenterWindow(RECT &rect,RECT &desktop){//ʹ�����ƶ����������λ�� 
	int width,height,dtWidth,dtHeight;
	bool bSuccess=false;
	HWND hwnd=GetDesktopWindow();
	GetWindowRect(hwnd,&desktop); 
	dtWidth=desktop.right-desktop.left;
	dtHeight=desktop.bottom-desktop.top;
	hwnd=GetForegroundWindow(); 
	GetWindowRect(hwnd,&rect); 
	width=rect.right-rect.left;
	height=rect.bottom-rect.top;
	bSuccess=MoveWindow(hwnd,(dtWidth-width)/2,(dtHeight-height)/2,width,height,true); 
	GetWindowRect(hwnd,&rect); 
}

int FirstWindow(){
	initgraph(300,50,NOMINIMIZE);
	
	HWND WinName=GetHWnd();//��ȡ���ھ������
	POINT mouse;//������������x��y����   
	
	SetWindowText(WinName,"ͼ��ʶ��");//�޸Ĵ������ֺ���
	outtextxy(0,0,"����Target�ļ����������Ҫʶ���ͼƬ");
	
	while(1){
		outtextxy(0,30,"��ʶ��  ���˳���");
		while(1){
    		GetCursorPos(&mouse);//����GetCursorPos������ȡ����ֵ
	    	gotoXY(0,0);
			GetWindowRect(WinName,&rect);
			
			if(MOUSE_Click(VK_LBUTTON)){
	            if((mouse.x-rect.left)>10&&(mouse.y-rect.top)>60&&(mouse.x-rect.left)<60&&(mouse.y-rect.top)<70){//���ʶ�� 
				    break; 
			    }else if((mouse.x-rect.left)>80&&(mouse.y-rect.top)>60&&(mouse.x-rect.left)<130&&(mouse.y-rect.top)<70){//���ʶ�� 
				    return 1; 
			    }
			}
			Sleep(10);
    	}
    	
		vector<string> files;
    	GetAllFiles("./Target",files);
    	int size=files.size();
    	if(size==1){
    		Image_route=files[0];//��ȡͼƬ���·��
    		break;
		}else if(size==0){//�޿�ʶ��jpg�ļ�
    	    MessageBox(WinName,"�����ʶ��ͼƬ�ļ�","��ʾ",MB_OKCANCEL);//��Ϣ�򵯳����ú���
    	}else if(size>1){
    		MessageBox(WinName,"������һ��ͼƬ","��ʾ",MB_OKCANCEL);
		}
	}
	closegraph();
	return 0;	
}

int main(){
	string Lable[16];
	LPCTSTR LABLE;
		
	Lable[0]="����!";
	Lable[1]="��";
	Lable[2]="��";
	Lable[3]="è";
	Lable[4]="ţ";
	Lable[5]="��";
	Lable[6]="����";
	Lable[7]="ƻ��";
	Lable[8]="����";
	Lable[9]="����";
	Lable[10]="�㽶";
	Lable[11]="��ͳ�";
	Lable[12]="�ɻ�";
	Lable[13]="��";
	Lable[14]="�ִ�";
	Lable[15]="С����";
	
	while(1){
		if(FirstWindow()){
			break;
		}
		
		initgraph(400,480,NOMINIMIZE);
	    HWND WinTarget=GetHWnd();
	    SetWindowText(WinTarget,"ͼ��ʶ��");
	    
        l=Image_route.c_str();//ת������
        loadimage(&img,l,400,400);//����ͼ��,ת��Ϊָ�����
	    putimage(0,0,&img);
	    outtextxy(150,401,"[ȷ��]   [ȡ��]");
	    
		POINT mouse;//������������x��y����  
	    RECT rect;
	    bool CHOSE;
		while(1){
    		GetCursorPos(&mouse);//����GetCursorPos������ȡ����ֵ
	    	gotoXY(0,0);
			GetWindowRect(WinTarget,&rect);
			if(MOUSE_Click(VK_LBUTTON)){
	            if((mouse.x-rect.left)>155&&(mouse.y-rect.top)>430&&(mouse.x-rect.left)<190&&(mouse.y-rect.top)<440){//ȷ�� 
				    CHOSE=0; 
				    break; 
			    }else if((mouse.x-rect.left)>210&&(mouse.y-rect.top)>430&&(mouse.x-rect.left)<240&&(mouse.y-rect.top)<440){//ȡ�� 
				    CHOSE=1;
					break;
			    }
			}
			Sleep(10);
    	}
    	closegraph();
		if(CHOSE)continue;
		
		initgraph(800,400,NOMINIMIZE);
	    HWND WinTarget2=GetHWnd();
	    SetWindowText(WinTarget2,"ͼ��ʶ��");
		putimage(0,0,&img);
		outtextxy(401,0,"ʶ����...");
		Sleep(1000);
		
		int ans,ans_id[16];
		for(int i=0;i<16;i++)ans_id[i]=0; 
		system("mainR.exe 2");
		ifstream in("ans.txt",ios::in);
		in>>ans;
		ans_id[ans]++;
		system("mainG.exe 2");
		ifstream in2("ans.txt",ios::in);
		in2>>ans;
		ans_id[ans]++;
		system("mainB.exe 2");
		ifstream in3("ans.txt",ios::in);
		in3>>ans;
		ans_id[ans]++;
		
		in.close();
		in2.close();
		in3.close();
		
		int maxx=-998;
		for(int i=0;i<16;i++){
			if(ans_id[i]>maxx){
				maxx=ans_id[i];
				ans=i;
			}
		}
		
		LABLE=Lable[ans].c_str();
		
		outtextxy(400,0,"ʶ������");
		outtextxy(471,0,LABLE);
		
		outtextxy(400,41,"[�˳�]");
		while(1){
    		GetCursorPos(&mouse);
	    	gotoXY(0,0);
			GetWindowRect(WinTarget2,&rect);
			if(MOUSE_Click(VK_LBUTTON)&&(mouse.x-rect.left)>405&&(mouse.y-rect.top)>70&&(mouse.x-rect.left)<440&&(mouse.y-rect.top)<80){//ȷ�� 
				break; 
			}
			Sleep(10);
    	}
		closegraph();
	}
	
	return 0; 
}
