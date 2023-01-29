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

string Image_route;//图片路径 字符串
IMAGE img;//加载图像
LPCTSTR l;//图片路径 函数参数
RECT rect,rectDesktop;

//函数声明 
void GetAllFiles(string path,vector<string>&files);//读取所有的文件，包括子文件的文件
void gotoXY(short x,short y);//设置光标位置
int FirstWindow();
void CenterWindow(RECT &rect,RECT &desktop);//使窗口移动到桌面居中位置

void GetAllFiles(string path,vector<string>&files){//读取所有的文件，包括子文件的文件
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
		}while(_findnext(hFile,&fileinfo)==0);  
		_findclose(hFile); 
	} 
}

void gotoXY(short x,short y){//设置光标位置，坐标从左上角(0,0)起始，允许负值
    COORD position={x,y};
    HANDLE hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hConsole, position);
}

void CenterWindow(RECT &rect,RECT &desktop){//使窗口移动到桌面居中位置 
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
	
	HWND WinName=GetHWnd();//获取窗口句柄函数
	POINT mouse;//用来储存鼠标的x，y坐标   
	
	SetWindowText(WinName,"图像识别");//修改窗口名字函数
	outtextxy(0,0,"请在Target文件夹中添加需要识别的图片");
	
	while(1){
		outtextxy(0,30,"【识别】  【退出】");
		while(1){
    		GetCursorPos(&mouse);//调用GetCursorPos函数获取坐标值
	    	gotoXY(0,0);
			GetWindowRect(WinName,&rect);
			
			if(MOUSE_Click(VK_LBUTTON)){
	            if((mouse.x-rect.left)>10&&(mouse.y-rect.top)>60&&(mouse.x-rect.left)<60&&(mouse.y-rect.top)<70){//点击识别 
				    break; 
			    }else if((mouse.x-rect.left)>80&&(mouse.y-rect.top)>60&&(mouse.x-rect.left)<130&&(mouse.y-rect.top)<70){//点击识别 
				    return 1; 
			    }
			}
			Sleep(10);
    	}
    	
		vector<string> files;
    	GetAllFiles("./Target",files);
    	int size=files.size();
    	if(size==1){
    		Image_route=files[0];//获取图片相对路径
    		break;
		}else if(size==0){//无可识别jpg文件
    	    MessageBox(WinName,"请添加识别图片文件","提示",MB_OKCANCEL);//消息框弹出设置函数
    	}else if(size>1){
    		MessageBox(WinName,"请仅添加一张图片","提示",MB_OKCANCEL);
		}
	}
	closegraph();
	return 0;	
}

int main(){
	string Lable[16];
	LPCTSTR LABLE;
		
	Lable[0]="错误!";
	Lable[1]="狗";
	Lable[2]="马";
	Lable[3]="猫";
	Lable[4]="牛";
	Lable[5]="猪";
	Lable[6]="橙子";
	Lable[7]="苹果";
	Lable[8]="榴莲";
	Lable[9]="葡萄";
	Lable[10]="香蕉";
	Lable[11]="大客车";
	Lable[12]="飞机";
	Lable[13]="火车";
	Lable[14]="轮船";
	Lable[15]="小汽车";
	
	while(1){
		if(FirstWindow()){
			break;
		}
		
		initgraph(400,480,NOMINIMIZE);
	    HWND WinTarget=GetHWnd();
	    SetWindowText(WinTarget,"图像识别");
	    
        l=Image_route.c_str();//转换类型
        loadimage(&img,l,400,400);//加载图像,转换为指定规格
	    putimage(0,0,&img);
	    outtextxy(150,401,"[确认]   [取消]");
	    
		POINT mouse;//用来储存鼠标的x，y坐标  
	    RECT rect;
	    bool CHOSE;
		while(1){
    		GetCursorPos(&mouse);//调用GetCursorPos函数获取坐标值
	    	gotoXY(0,0);
			GetWindowRect(WinTarget,&rect);
			if(MOUSE_Click(VK_LBUTTON)){
	            if((mouse.x-rect.left)>155&&(mouse.y-rect.top)>430&&(mouse.x-rect.left)<190&&(mouse.y-rect.top)<440){//确认 
				    CHOSE=0; 
				    break; 
			    }else if((mouse.x-rect.left)>210&&(mouse.y-rect.top)>430&&(mouse.x-rect.left)<240&&(mouse.y-rect.top)<440){//取消 
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
	    SetWindowText(WinTarget2,"图像识别");
		putimage(0,0,&img);
		outtextxy(401,0,"识别中...");
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
		
		outtextxy(400,0,"识别结果：");
		outtextxy(471,0,LABLE);
		
		outtextxy(400,41,"[退出]");
		while(1){
    		GetCursorPos(&mouse);
	    	gotoXY(0,0);
			GetWindowRect(WinTarget2,&rect);
			if(MOUSE_Click(VK_LBUTTON)&&(mouse.x-rect.left)>405&&(mouse.y-rect.top)>70&&(mouse.x-rect.left)<440&&(mouse.y-rect.top)<80){//确认 
				break; 
			}
			Sleep(10);
    	}
		closegraph();
	}
	
	return 0; 
}
