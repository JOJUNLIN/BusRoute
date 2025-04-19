#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<windows.h>  //清频函数头文件 
#include<time.h>  //时间函数头文件 
typedef int Elem;
typedef char T;
#define MaxSize 16
int hell=-1;  //hello[hell][128]
char hello[4][128]; //根据时间判断时段 
int year,month,day,hour,minute;  //记录时间 
int bus; //公交路线数 
int vbus; //公交速度 
int vwalk; //行走速度 
int head[MaxSize] = {0};  //存储每条公交线路的一个头结点 
int road[MaxSize] = {0};  //存储每条公交路线拥有的站点数,值小于2时线路不存在 
int length=0;  //用于删除和所有路径，暂时记录两点间长度 
int Pwalk[MaxSize][MaxSize];  //存储最短路径经过的顶点(行走) 
int Dwalk[MaxSize][MaxSize];  //存储各个最短路径的权值和(行走)  
int Round[MaxSize][MaxSize];  //行走和公交的邻接矩阵(混合) 
int Pall[MaxSize][MaxSize];  //存储最短路径经过的顶点(混合) 
int Dall[MaxSize][MaxSize];  //存储各个最短路径的权值和(混合) 
int stack[MaxSize]={0};  //用于输出所有路径，存放路径结点 
int visit[MaxSize]={0};  //用于所有路径输出，记录访问次数，cisited已用  

typedef struct
{
	T vertex[MaxSize][128];  //存放顶点信息 
	int Arc[MaxSize][MaxSize];  //存放完整的图，亦称行走路线 
	int arc[MaxSize][MaxSize][MaxSize]; //用三维数组存放多个邻接矩阵，每个矩阵存放一条公交路线 
	int arcNum[MaxSize][MaxSize]; //记录每条线路中每个站点拥有的边数 
	int vertexNum; //顶点数    
}Mgraph;
typedef Mgraph* Mg;

//*************************************************************************初始化区块 

//将地点名记录 
void Initvertex(Mg G)
{
	strcpy(G->vertex[0],"京江楼教学楼");    strcpy(G->vertex[1],"六食堂");    strcpy(G->vertex[2],"E1区宿舍三叉口");
	strcpy(G->vertex[3],"新一区");    strcpy(G->vertex[4],"体育馆");    strcpy(G->vertex[5],"西山操场三岔口");
	strcpy(G->vertex[6],"三山楼");    strcpy(G->vertex[7],"五棵松");    strcpy(G->vertex[8],"图书馆");
	strcpy(G->vertex[9],"校史馆");    strcpy(G->vertex[10],"教职工医院");    strcpy(G->vertex[11],"东山操场");
	strcpy(G->vertex[12],"大礼堂");    strcpy(G->vertex[13],"实验楼");    strcpy(G->vertex[14],"计算机楼");
	strcpy(G->vertex[15],"C4区宿舍下台阶");
}

//初始化行走路线 
void InitArc(Mg G) 
{
	int i,j;
	FILE *p = fopen("E:/keshe/Arc.txt","r");
	if(p==NULL)    exit(0);
	for(i=0 ; i<G->vertexNum ; i++)
		for(j=0 ; j<G->vertexNum ; j++)
			fscanf(p,"%d",&G->Arc[i][j]);
	fclose(p);
}

//初始化五条公交路线 
void Initarc(Mg G)
{
	int i,j,k;
	FILE *p1 = fopen("E:/keshe/allarc.txt","r"); //文件指针 
	FILE *p2 = fopen("E:/keshe/arcNum.txt","r");
	FILE *p3 = fopen("E:/keshe/road.txt","r");
	FILE *p4 = fopen("E:/keshe/head.txt","r");
	if(p1==NULL)    exit(0);
	if(p2==NULL)    exit(0);
	if(p3==NULL)    exit(0);
	if(p4==NULL)    exit(0);
	for(i=0 ; i<bus ; i++)  //初始化公交路线 
		for(j=0 ; j<G->vertexNum ; j++)
			for(k=0 ; k<G->vertexNum ; k++)
				fscanf(p1,"%d",&G->arc[i][j][k]);
	for(i=0 ; i<bus ; i++)  //初始化每条路线中每个结点的临边数 
		for(j=0 ; j<G->vertexNum ; j++)
			fscanf(p2,"%d",&G->arcNum[i][j]);
	for(i=0 ; i<bus ; i++)  //初始化每条路线的站点数 
		fscanf(p3,"%d",&road[i]);
	for(i=0 ; i<bus ; i++)  //初始化每条路的头结点 
	    fscanf(p4,"%d",&head[i]);
	fclose(p1); //关闭文件 
	fclose(p2);
	fclose(p3);
	fclose(p4);
}

//初始化行走路线最短路径所用矩阵 
void InitWalk(Mg G)
{
	int v,w;
	for(v=0 ; v < G->vertexNum ; v++)
	{
		for(w=0 ; w < G->vertexNum ; w++)
		{
			Dwalk[v][w]=G->Arc[v][w];
			Pwalk[v][w]=w;
		}
	}
} 

//初始化混合路线
void InitAll(Mg G)
{
	int i,j,k;
	int v,w;
	for(i=0 ; i < G->vertexNum ; i++)
	{
		for(j=0 ; j < G->vertexNum ; j++)
		{
			if(G->Arc[i][j]==10000||G->Arc[i][j]==0)    Round[i][j]=G->Arc[i][j];
			else    Round[i][j]=6*G->Arc[i][j];  //步行距离放大六倍 
		}
	}
	for(i=0 ; i<bus ; i++)
	{
		if(road[i]<2)    continue;  //存放存在的路线 
		for(j=0 ; j < G->vertexNum ; j++)
		{
			for(k=0 ; k < G->vertexNum ; k++)
			{
				if(G->arc[i][j][k]!=0&&G->arc[i][j][k]!=10000)
				    Round[j][k]=G->arc[i][j][k];
			}
		}
	}
	for(v=0 ; v < G->vertexNum ; v++)
	{
		for(w=0 ; w < G->vertexNum ; w++)
		{
			Dall[v][w]=Round[v][w];  //存储各个最短路径的权值和(混合) 
			Pall[v][w]=w;  //存储最短路径经过的顶点(混合) 
		}
	}
 } 

//初始化所有 
void InitMg(Mg G)
{
	InitArc(G); //初始化行走路线  
	Initarc(G); //初始化五条公交路线  
	InitWalk(G); //初始化行走路线最短路径所用矩阵  
	InitAll(G); //初始化混合路线(后面还需实时更新)  
	Initvertex(G); //录入地点名 
}

//************************************************************************功能区块 

//学生返回函数 
void Studentback(Mg G)
{
	void Wrong(Mg G);        //报错函数声明 
	void StudentMenu(Mg G);     //学生菜单声明 
	int x;
	printf("\n");
	printf("\t\t\t输入'0'  返回\n");
	printf("\n");
	printf("\t\t\t**********************************************************\n");
	printf("\t\t\t请输入:");
	scanf("%d",&x);
	if(x!=0)
	{
		system("cls");
		Wrong(G);      //输入错误菜单 
	}
	if(x==0)
	{
		system("cls");
		StudentMenu(G);      //学生菜单 
	}
}

//管理员返回函数
void Rootback(Mg G)
{
	void Wrong(Mg G);        //报错函数声明 
	void RootMenu(Mg G);     //管理员函数声明 
	int x;
	printf("\n");
	printf("\t\t\t输入'0'  返回\n");
	printf("\n");
	printf("\t\t\t**********************************************************\n");
	printf("\t\t\t请输入:");
	scanf("%d",&x);
	if(x!=0)
	{
		system("cls");
		Wrong(G);          //输入错误菜单 
	}
	if(x==0)
	{
		system("cls");
		RootMenu(G);       //管理员菜单 
	}
}

//显示路线函数 
void Showload(Mg G)
{
	void DFSTraverse(Mg G,int load,int v);
	int i;
	printf("\n");
	printf("\t\t\t江大现有公交路线:\n");
	for(i=0 ; i<bus ; i++)
	{
		if(road[i]<2)    continue;
		DFSTraverse(G,i,head[i]);      //输出路线 
	}
	printf("\n");
	printf("\t\t\t江苏大学站点:[0]京江楼教学楼     [1]六食堂            \n");
	printf("\t\t\t             [2]E1区宿舍三叉口   [3]新一区            \n");
	printf("\t\t\t             [4]体育馆           [5]西山操场三岔口    \n");
	printf("\t\t\t             [6]三山楼           [7]五棵松            \n");
	printf("\t\t\t             [8]图书馆           [9]校史馆            \n");
	printf("\t\t\t             [10]教职工医院      [11]东山操场         \n");
	printf("\t\t\t             [12]大礼堂          [13]实验楼           \n");
	printf("\t\t\t             [14]计算机楼        [15]C4区宿舍下台阶   \n");
	printf("\n");
}

//从头开始的深度优先遍历，输出路线 
void DFSTraverse(Mg G,int load,int v)  //此处load是线路数-1 
{
	int i,j,k=1;
	int visited[MaxSize][MaxSize] = {0};  //访问标记，0没访问，1访问过 
	visited[load][v] = 1;
	printf("\t\t\t%d号线: ",load+1);
	printf("%s",G->vertex[v]);
	for(j=0 ; j < G->vertexNum ; j++) //深度优先遍历 
	{
		
		if(G->arc[load][v][j]!=0&&G->arc[load][v][j]!=10000&&visited[load][j]==0)
		{
			k++;
			printf("--%s",G->vertex[j]);
			v=j;
			visited[load][j] = 1;
			j=0;
		}
	}
	if(k<road[load])    printf("--%s",G->vertex[head[load]]);
	printf("\n");
}

//新增公交路线 
void AddLoad(Mg G)
{
	void Wrong(Mg G);  //报错函数声明 
	int add;  //新增线路的条数 
	int m,n;  //输入邻接矩阵 
	int i,j,k;
	int D[MaxSize][MaxSize]; //存放非直达点间的最短路径
	FILE *p = fopen("E:/keshe/AddLoad.txt","w"); //文件指针 
	if(p==NULL)    exit(0);
	for(i=0 ; i < G->vertexNum ; i++)
	{
		for(j=0 ; j < G->vertexNum ; j++)
		{
			D[i][j]=G->Arc[i][j];
		}
	}
	for(k=0 ; k < G->vertexNum ; k++)
	{
		for(i=0 ; i < G->vertexNum ; i++)
		{
			for(j=0 ; j < G->vertexNum ; j++)
			{
				if(D[i][j] > D[i][k]+D[k][j])
				{
					D[i][j]=D[i][k]+D[k][j];
				}
			}
		}
	}  //计算最短路径长度 
	printf("\t\t\t*******************江苏大学路线查询系统*******************\n");
	printf("\t\t\t\t\t   %d年%d月%d日%d点%d分\n",year,month,day,hour,minute);
	printf("\t\t\t\t\t\t   %s\n",hello[hell]);
	printf("\n");
	printf("\t\t\t请输入新增线路的条数:");
	scanf("%d",&add);  //增加线路的条数 
	if(add < 0)  //输入错误 
	{
		system("cls");
		Wrong(G);
	}
	for(i=bus ; i<bus+add ; i++)  //按新增条数循环 
	{
		printf("\t\t\t请输入%d号线的路线(输入'-1'时停止):",i+1);
		scanf("%d",&n);
		if(n==-1)    break;
		m=n;
		G->arcNum[i][m] = 0;
		head[i] = m;  //记录头结点 
		road[i] = 1;
		scanf("%d",&n);
		while(n!=-1)
		{
			road[i]++;
			G->arcNum[i][m]++;    G->arcNum[i][n]++; //记录每个站点拥有的边数 
			G->arc[i][m][n] = G->arc[i][n][m] = D[m][n];
			m=n;
			scanf("%d",&n);
		}
		DFSTraverse(G,i,head[i]);
		for(j=0 ; j<G->vertexNum ; j++)
		{
			for(k=0 ; k<G->vertexNum ; k++)
			{
				fprintf(p,"%d  ",G->arc[i][j][k]);
			}
			fprintf(p,"\n");
		}
		fprintf(p,"\n");
	}
	fclose(p);
	bus=bus+add;
	Rootback(G);
}

//删除单边站点（子） 
void Delete_1(Mg G,int load,int dele)
{
	int i,j;
	FILE *p = fopen("E:/keshe/Delete.txt","w"); //文件指针 
	if(p==NULL)    exit(0);
	road[load-1]--;
	for(i=0 ; i < G->vertexNum ; i++) //筛选 
	{
		if(G->arc[load-1][dele][i]!=0&&G->arc[load-1][dele][i]!=10000)
		{
			G->arc[load-1][dele][i]=G->arc[load-1][i][dele]=10000;
			head[load-1]=i;
			G->arcNum[load-1][dele]--;
			G->arcNum[load-1][i]--;
			break;
		}
	}
	if(road[load-1]==1)  //删除后该路线若只有一个站点，则路线去除  
	{
		printf("\t\t\t%d号线已去除!\n",load);
		return;
	}
	if(road[load-1]>1)
	{
		DFSTraverse(G,load-1,i);
		for(i=0 ; i<G->vertexNum ; i++)
	    {
		    for(j=0 ; j<G->vertexNum ; j++)
		    {
			    fprintf(p,"%d  ",G->arc[load-1][i][j]);
		    }
		    fprintf(p,"\n");
	    }
	    fprintf(p,"\n");
	    fclose(p);
		return;
	}
}

//删除双边站点（子） 
void Delete_2(Mg G,int load,int dele)
{
	int i,j,k=1;
	int m,n;
	FILE *p = fopen("E:/keshe/Delete.txt","w"); //文件指针 
	if(p==NULL)    exit(0);
	length=0;
	road[load-1]--;
	for(i=0 ; i < G->vertexNum ; i++)
	{
		if(G->arc[load-1][dele][i]!=0&&G->arc[load-1][dele][i]!=10000)
		{
			if(k==1)    m=i;
			if(k==2)    n=i;
			length=length+G->arc[load-1][dele][i];
			G->arc[load-1][dele][i]=G->arc[load-1][i][dele]=10000;
			G->arcNum[load-1][dele]--;
			k++;
		}
	}
	G->arc[load-1][m][n] = G->arc[load-1][n][m] = length;
	DFSTraverse(G,load-1,head[load-1]);
	for(i=0 ; i<G->vertexNum ; i++)
	{
		for(j=0 ; j<G->vertexNum ; j++)
		{
			fprintf(p,"%d  ",G->arc[load-1][i][j]);
		}
		fprintf(p,"\n");
	}
	fprintf(p,"\n");
	fclose(p);
	return;
}

//删除站点（母） 
void Delete_main(Mg G)
{
	void Wrong(Mg G);        //报错函数声明 
	int load;    //要删除站点的所在路线 
	int num;     //要删除站点的个数 
	int dele;    //删除的站点 
	int i,j,k;
	printf("\t\t\t*******************江苏大学路线查询系统*******************\n");
	printf("\t\t\t\t\t   %d年%d月%d日%d点%d分\n",year,month,day,hour,minute);
	printf("\t\t\t\t\t\t   %s\n",hello[hell]);
	Showload(G);    //显示所有路线 
	printf("\t\t\t请输入要删除站点的所在路线:");
	scanf("%d",&load);
	if(load<1||load>bus)      //输入错误 
	{
		system("cls");
		Wrong(G);
	}
	if(road[load-1]<2)
	{
		printf("\t\t\t%d号线不存在!\n",load);
	}
	if(road[load-1]>1)
	{
		printf("\t\t\t请输入要删除站点的个数:");
	    scanf("%d",&num);
	    for(i=0 ; i<num ; i++)      //站点删除 
	    {
		    printf("\t\t\t请输入要删除的站点:");
		    scanf("%d",&dele);
		    if(G->arcNum[load-1][dele]==0)
		    {
		    	printf("\t\t\t\t该路线不存在此站点!\n");
		    	break;
	    	}
		    if(G->arcNum[load-1][dele]==1)
		    {
			    Delete_1(G,load,dele);
		    }
		    if(G->arcNum[load-1][dele]==2)
		    {
			    Delete_2(G,load,dele);
		    }
	    }
	}
	Rootback(G);
} 

//初始化单个路线（子） 
void Initload(Mg G,int load)
{
	int i,j,k;
	road[load-1]=0;
	for(i=0 ; i < G->vertexNum ; i++)
	{
		for(j=0 ; j < G->vertexNum ; j++)
		{
			G->arc[load-1][i][j] = 10000;
			if(i==j)    G->arc[load-1][i][j] = 0;
		}
		G->arcNum[load-1][i] = 0;
	}
} 

//修改路线、增加站点（母）  
void Change(Mg G)
{
	void Wrong(Mg G);        //报错函数声明 
	int m,n;
	int i,j,k;
	int load;
	int D[MaxSize][MaxSize];     //存放非直达点间的最短路径 
	FILE *p = fopen("E:/keshe/Change.txt","w"); //文件指针 
	if(p==NULL)    exit(0);
	for(i=0 ; i < G->vertexNum ; i++)
	{
		for(j=0 ; j < G->vertexNum ; j++)
		{
			D[i][j]=G->Arc[i][j];
		}
	}
	for(k=0 ; k < G->vertexNum ; k++)
	{
		for(i=0 ; i < G->vertexNum ; i++)
		{
			for(j=0 ; j < G->vertexNum ; j++)
			{
				if(D[i][j] > D[i][k]+D[k][j])
				{
					D[i][j]=D[i][k]+D[k][j];
				}
			}
		}
	}  //计算最短路径长度 
	printf("\t\t\t*******************江苏大学路线查询系统*******************\n");
	printf("\t\t\t\t\t   %d年%d月%d日%d点%d分\n",year,month,day,hour,minute);
	printf("\t\t\t\t\t\t   %s\n",hello[hell]);
	Showload(G);    //显示所有路线 
	printf("\t\t\t选择要修改的路线:");
	scanf("%d",&load);
	if(load<1||load>bus)
	{
		system("cls");
		Wrong(G);
	}
	Initload(G,load);     //初始化路线
	printf("\n");
	printf("\t\t\t输入修改的完整路线(输入'-1'时停止):"); 
	scanf("%d",&n);
	if(n!=-1)
	{
		m=n;
		head[load-1]=m;  //记录头节点 
		road[load-1]=1;  //站点数初始化 
		scanf("%d",&n);
		while(n!=-1)
		{
			road[load-1]++;
			G->arcNum[load-1][m]++;
			G->arcNum[load-1][n]++;
			G->arc[load-1][m][n] = G->arc[load-1][n][m] = D[m][n];
			m=n;
			scanf("%d",&n);
		}
		DFSTraverse(G,load-1,head[load-1]);  //输出路线 
		for(i=0 ; i<G->vertexNum ; i++)
		{
			for(j=0 ; j<G->vertexNum ; j++)
			{
				fprintf(p,"%d  ",G->arc[load-1][i][j]);
			}
			fprintf(p,"\n");
		}
		fprintf(p,"\n");
	}
	fclose(p);
	Rootback(G);
}

//路线查询 
void FindLoad(Mg G)
{
	void Wrong(Mg G);        //报错菜单声明 
	int x,k=0;            //k记录经过a站点的公交线数 
	int a=0;            //要查询的站点 
	int i,j;
	printf("\t\t\t*******************江苏大学路线查询系统*******************\n");
	printf("\t\t\t\t\t   %d年%d月%d日%d点%d分\n",year,month,day,hour,minute);
	printf("\t\t\t\t\t\t   %s\n",hello[hell]);
	Showload(G);    //显示所有路线 
	while(a>=0)
	{
		printf("\t\t\t请输入要查询的站点(输入'-1'停止查询):");
	    scanf("%d",&a);
	    if(a<-1||a>MaxSize)
	    {
	    	system("cls");
	    	Wrong(G);
		}
	    if(a==-1)    break;
	    printf("\t\t\t经过%s的公交路线有:",G->vertex[a]);
	    for(i=0 ; i<bus ; i++)       //在线路中查找是否有a站点 
	    {
	    	for(j=0 ; j < G->vertexNum ; j++)
	    	{
	    		if(G->arc[i][a][j]!=0&&G->arc[i][a][j]!=10000)
	    		{
	    			printf("%d号线  ",i+1);
	    			j=G->vertexNum;
	    			k++;
				}
			}
		}
		if(k==0)    printf("抱歉!此站点没有公交路线经过");
	    printf("\n");
	}
	Studentback(G);
} 

//弗洛伊德最短路径（孙） 
void Shortes_Floyed(Mg G,int P[][MaxSize],int D[][MaxSize])
{
	int v,w,k;
	for(k=0 ; k < G->vertexNum ; k++)
	{
		for(v=0 ; v < G->vertexNum ; v++)
		{
			for(w=0 ; w < G->vertexNum ; w++)
			{
				if(D[v][w] > D[v][k]+D[k][w])
				{
					D[v][w]=D[v][k]+D[k][w];
					P[v][w]=k;
				}
			}
		}
	}
}

//输出最短路线（子） 
void Shortest(Mg G,int start,int end,int P[][MaxSize],int D[][MaxSize])
{
	int temp;    //替代作用  
	Shortes_Floyed(G,P,D);     //计算最短路线 
	printf("%s-->%s",G->vertex[start],G->vertex[P[start][end]]);
	temp=P[start][end];
	while(temp!=end)
	{
		printf("-->%s",G->vertex[P[temp][end]]);
		temp=P[temp][end];
	}
}

//五棵松到各点的最短路径（母） 
void Shortest_Fivetree(Mg G)
{
	int i,j;
	int t=7;
	printf("\t\t\t从五棵松到各地点的最短路径为:\n");
	for(i=0 ; i < G->vertexNum ; i++)
	{
		if(i==t)    continue;
		printf("\t\t\t\t\t");
		Shortest(G,t,i,Pwalk,Dwalk);
		printf("\t长度为:%d米",Dwalk[t][i]);
		printf("\t大约需要:%.2f分钟\n",(double)Dwalk[t][i]/(double)vwalk);
	}
}

//判断环线
void Ring(Mg G)
{
	int i,k=0;
	int load;
	printf("\n");
	printf("\t\t\t校内的环线:");
	for(load=0 ; load<bus ; load++)
	{
		if(G->arcNum[load][head[load]]==2)
		{
			printf("%d号线 ",load+1);
			k++;
		}
		if(G->arcNum[load][head[load]]==1)
		{
			for(i=0 ; i < G->vertexNum ; i++)
			{
				if(G->arcNum[load][i]==3)
				{
					printf("%d号线 ",load+1);
					k++;
				}
			}
		}
	}
	if(k==0)    printf("没有环线!");
	printf("\n");
 } 

//最佳路径规划 
void Bestload(Mg G)
{
	void Wrong(Mg G);        //报错菜单声明 
	int start,end;
	int i,j,k;
	printf("\t\t\t*******************江苏大学路线查询系统*******************\n");
	printf("\t\t\t\t\t   %d年%d月%d日%d点%d分\n",year,month,day,hour,minute);
	printf("\t\t\t\t\t\t   %s\n",hello[hell]);
	Showload(G);    //显示所有路线 
	printf("\t\t\t输入起始点:");
	scanf("%d",&start);
	printf("\t\t\t输入目的地:");
	scanf("%d",&end);
	if(start<0||start>G->vertexNum||end<0||end>G->vertexNum)
	{
		system("cls");
		Wrong(G);        //输入报错 
	}
	//步行最短路径 
	printf("\n");
	printf("\t\t\t步行的最短路线为:");
	Shortest(G,start,end,Pwalk,Dwalk);
	printf("\t长度为:%d米",Dwalk[start][end]);
	printf("\t走路约为:%.2f分钟\n",(double)Dwalk[start][end]/(double)vwalk);
	//最短时间路线(走路+公交) 
	printf("\n");
	printf("\t\t\t最快的路线为:");
	InitAll(G);     //初始化混合路线(记录更新后的路线) 
	Shortest(G,start,end,Pall,Dall);
	printf("\t大约需要:%.2f分钟\n",(double)Dall[start][end]/(double)vbus);
	//五棵松到各点的最短路径 
	printf("\n");
	Shortest_Fivetree(G);
	printf("\n");
	//判断环线
	Ring(G);
	
	Studentback(G);
 } 

//两点间所有路径（母） 
void AllPath(Mg G)
{
	void Path(Mg G,int m,int n,int k);     //所有路径（子） 
	void Wrong(Mg G);        //报错菜单声明 
	int k=0;
	int start,end;
	printf("\t\t\t*******************江苏大学路线查询系统*******************\n");
	printf("\t\t\t\t\t   %d年%d月%d日%d点%d分\n",year,month,day,hour,minute);
	printf("\t\t\t\t\t\t   %s\n",hello[hell]);
	Showload(G);    //显示所有路线 
	printf("\t\t\t请输入起始点:");
	scanf("%d",&start);
	printf("\t\t\t请输入目的地:");
	scanf("%d",&end);
	printf("\n");
	if(start<0||start>G->vertexNum||end<0||end>G->vertexNum)
	{
		system("cls");
		Wrong(G);        //输入报错 
	}
	printf("\t\t\t可达的所有路径有:\n");
	stack[0]=start;       //存储起点 
	for(k=0 ; k<G->vertexNum ; k++)
	    visit[k]=0;
	visit[start]=1;
	Path(G,start,end,0);
	printf("\n");
	Studentback(G);
}

//输出所有路径（子） 
void Path(Mg G,int m,int n,int k)
{
	int s,t,i;
	t=k+1;
	length=0;
	if(stack[k]==n&&k < G->vertexNum)  //达到目的地停止，条件二防止无限循环 
	{
		for(s=0 ; s<k ; s++)     //路径长度 
		{
			length=length+Round[stack[s]][stack[s+1]];
		}
		if(length<10000)
		{
			printf("\t\t\t\t");
		    for(s=0 ; s<k ; s++)     //输出路径 
		    {
			    printf("%s-->",G->vertex[stack[s]]);
		    }
		    printf("%s",G->vertex[stack[s]]);
		    printf("\t大约需要:%.2f分钟\n",(double)length/(double)vbus);    //时间 
		}
	}
	else   //未到目的地继续递归 
	{
		s=0;
		while(s < G->vertexNum)
		{
			if(Round[stack[k]][s]!=0&&Round[stack[k]][s]!=10000&&visit[s]==0)
			{
				visit[s]=1;   //访问记录 
				stack[t]=s;    //存储顶点 
				Path(G,m,n,t);    //递归直到找到目的地 
				visit[s]=0;     //访问重置，便于继续寻找 
			}
			s++;
		}
	}
}

//*********************************************************************菜单区块 

//输入报错 
void Wrong(Mg G)
{
	void MainMenu(Mg G);       //主菜单声明 
	int x;
	printf("\t\t\t*******************江苏大学路线查询系统*******************\n");
	printf("\t\t\t||                                                      ||\n");
	printf("\t\t\t||                       输入错误！                     ||\n");
	printf("\t\t\t||                 输入'0'  以返回主菜单                ||\n");
	printf("\t\t\t||                                                      ||\n");
	printf("\t\t\t**********************************************************\n");
	printf("\t\t\t请输入:");
	scanf("%d",&x);
	if(x!=0)
	{
		system("cls");
		Wrong(G);  //报错 
	}
	if(x==0)
	{
		system("cls");
		MainMenu(G);  //主菜单 
	}
} 

//退出系统结尾 
void Thank()
{
	printf("\t\t\t*******************江苏大学路线查询系统*******************\n");
	printf("\t\t\t||                                                      ||\n");
	printf("\t\t\t||                       感谢使用！                     ||\n");
	printf("\t\t\t||                                                      ||\n");
	printf("\t\t\t**********************************************************\n");
} 

//管理员菜单 
void RootMenu(Mg G)
{
	void MainMenu(Mg G);       //主菜单声明 
	int x;
	printf("\t\t\t*******************江苏大学路线查询系统*******************\n");
	printf("\t\t\t\t\t   %d年%d月%d日%d点%d分\n",year,month,day,hour,minute);
	printf("\t\t\t\t\t\t   %s\n",hello[hell]);
	printf("\t\t\t||-----------------------管理员端-----------------------||\n");
	printf("\t\t\t||                                                      ||\n");
	printf("\t\t\t||                 输入'1' 新增公交路线                 ||\n");
	printf("\t\t\t||                 输入'2' 修改已有公交路线             ||\n");
	printf("\t\t\t||                 输入'3' 删除已有站点                 ||\n");
	printf("\t\t\t||                 输入'0' 返回主菜单                   ||\n");
	printf("\t\t\t||                                                      ||\n");
	printf("\t\t\t**********************************************************\n");
	printf("\t\t\t请输入:");
	scanf("%d",&x);
	if(x<0||x>3)    x=5;
	switch(x)
	{
		case 0:system("cls");
		       MainMenu(G);    break;  //主菜单 
		case 1:system("cls");
		       AddLoad(G);     break;  //增添路线 
		case 2:system("cls");
		       Change(G);       break;  //修改路线、添加站点 
		case 3:system("cls");
		       Delete_main(G);   break;  //删除站点 
		case 5:system("cls");
		       Wrong(G);  //报错 
	}
}

//学生菜单 
void StudentMenu(Mg G)
{
	void MainMenu(Mg G);         //主菜单声明 
	char ch;
	printf("\t\t\t**********************江苏大学路线查询系统**********************\n");
	printf("\t\t\t\t\t   %d年%d月%d日%d点%d分\n",year,month,day,hour,minute);
	printf("\t\t\t\t\t\t   %s\n",hello[hell]);
	printf("\t\t\t||---------------------------学生端---------------------------||\n");
	printf("\t\t\t||                                                            ||\n");
	printf("\t\t\t||      江苏大学站点:[0]京江楼教学楼     [1]六食堂            ||\n");
	printf("\t\t\t||                   [2]E1区宿舍三叉口   [3]新一区            ||\n");
	printf("\t\t\t||                   [4]体育馆           [5]西山操场三岔口    ||\n");
	printf("\t\t\t||                   [6]三山楼           [7]五棵松            ||\n");
	printf("\t\t\t||                   [8]图书馆           [9]校史馆            ||\n");
	printf("\t\t\t||                   [10]教职工医院      [11]东山操场         ||\n");
	printf("\t\t\t||                   [12]大礼堂          [13]实验楼           ||\n");
	printf("\t\t\t||                   [14]计算机楼        [15]C4区宿舍下台阶   ||\n");
	printf("\t\t\t||                                                            ||\n");
	printf("\t\t\t||                   输入'A'  公交线路查询                    ||\n");
	printf("\t\t\t||                   输入'B'  公交线路规划                    ||\n");
	printf("\t\t\t||                   输入'C'  最佳路线规划                    ||\n");
	printf("\t\t\t||                   输入'Y'  返回主菜单                      ||\n");
	printf("\t\t\t||                                                            ||\n");
	printf("\t\t\t****************************************************************\n");
	printf("\t\t\t请输入:");
	ch=getchar();        //吃回车 
	scanf("%c",&ch);
	if(ch!='A'&&ch!='B'&&ch!='C'&&ch!='Y')    ch='Z';
	switch(ch)
	{
		case 'A':system("cls");
		         FindLoad(G);    break;  //路线查询 
		case 'B':system("cls");
		         AllPath(G);     break;  //所有路径 
		case 'Y':system("cls");
		         MainMenu(G);    break;  //主菜单 
		case 'C':system("cls");
		         Bestload(G);    break;  //最佳路线 
		case 'Z':system("cls");
		         Wrong(G);  //报错 
	}
}

//主菜单 
void MainMenu(Mg G)
{
	int x;
	printf("\t\t\t*******************江苏大学路线查询系统*******************\n");
	printf("\t\t\t\t\t   %d年%d月%d日%d点%d分\n",year,month,day,hour,minute);
	printf("\t\t\t\t\t\t   %s\n",hello[hell]);
	printf("\t\t\t||-------------------------主菜单-----------------------||\n");
	printf("\t\t\t||                                                      ||\n");
	printf("\t\t\t||                 输入'1'  登录管理员端                ||\n");
	printf("\t\t\t||                 输入'2'  登录学生端                  ||\n");
	printf("\t\t\t||                 输入'0'  退出系统                    ||\n");
	printf("\t\t\t||                                                      ||\n");
	printf("\t\t\t**********************************************************\n");
	printf("\t\t\t请输入:");
	scanf("%d",&x);
	if(x<0||x>2)    x=3;
	switch(x)
	{
		case 0:system("cls");
		       Thank();           break;
		case 1:system("cls");
		       RootMenu(G);       break;  //管理员菜单 
		case 2:system("cls");
		       StudentMenu(G);    break;  //学生菜单 
		case 3:system("cls");
		       Wrong(G);  //报错菜单 
	}
}

int main()
{
	Mg G;
	G = (Mg)malloc(sizeof(Mgraph));
	G->vertexNum = MaxSize;
	//时间 
	time_t tmpcal_t;
	struct tm*tmp_ptr = NULL;
	time(&tmpcal_t);
	tmp_ptr = localtime(&tmpcal_t);
	year=1900+tmp_ptr->tm_year; //年 
	month=1+tmp_ptr->tm_mon; //月 
	day=tmp_ptr->tm_mday; //日 
	hour=tmp_ptr->tm_hour; //小时 
	minute=tmp_ptr->tm_min; //分钟 
	if(hour>=6&&hour<=10)    hell=0;
	if(hour>=11&&hour<=13)    hell=1;
	if(hour>=14&&hour<=18)    hell=2;
	if(hour>=19&&hour<=24||hour>=0&&hour<=5)    hell=3;
	strcpy(hello[0],"上午好!");    strcpy(hello[1],"中午好!");
	strcpy(hello[2],"下午好!");    strcpy(hello[3],"晚上好!");
	//
	bus=5; //记录现有公交路线数量，初始5条 
	vwalk=80;  //行走速度80米每分钟 
	vbus=480;  //公交速度480米每分钟 
	InitMg(G);
	MainMenu(G);
	return 0;
}
/*
初始公交路线 
一号线：1-0-13-8 
二号线：1-15-8-6-7 
三号线：1-3-5-6-8 
四号线：3-12-11-4 
五号线：10-11-7-6-8 
*/
