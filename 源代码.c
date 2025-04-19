#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<windows.h>  //��Ƶ����ͷ�ļ� 
#include<time.h>  //ʱ�亯��ͷ�ļ� 
typedef int Elem;
typedef char T;
#define MaxSize 16
int hell=-1;  //hello[hell][128]
char hello[4][128]; //����ʱ���ж�ʱ�� 
int year,month,day,hour,minute;  //��¼ʱ�� 
int bus; //����·���� 
int vbus; //�����ٶ� 
int vwalk; //�����ٶ� 
int head[MaxSize] = {0};  //�洢ÿ��������·��һ��ͷ��� 
int road[MaxSize] = {0};  //�洢ÿ������·��ӵ�е�վ����,ֵС��2ʱ��·������ 
int length=0;  //����ɾ��������·������ʱ��¼����䳤�� 
int Pwalk[MaxSize][MaxSize];  //�洢���·�������Ķ���(����) 
int Dwalk[MaxSize][MaxSize];  //�洢�������·����Ȩֵ��(����)  
int Round[MaxSize][MaxSize];  //���ߺ͹������ڽӾ���(���) 
int Pall[MaxSize][MaxSize];  //�洢���·�������Ķ���(���) 
int Dall[MaxSize][MaxSize];  //�洢�������·����Ȩֵ��(���) 
int stack[MaxSize]={0};  //�����������·�������·����� 
int visit[MaxSize]={0};  //��������·���������¼���ʴ�����cisited����  

typedef struct
{
	T vertex[MaxSize][128];  //��Ŷ�����Ϣ 
	int Arc[MaxSize][MaxSize];  //���������ͼ���������·�� 
	int arc[MaxSize][MaxSize][MaxSize]; //����ά�����Ŷ���ڽӾ���ÿ��������һ������·�� 
	int arcNum[MaxSize][MaxSize]; //��¼ÿ����·��ÿ��վ��ӵ�еı��� 
	int vertexNum; //������    
}Mgraph;
typedef Mgraph* Mg;

//*************************************************************************��ʼ������ 

//���ص�����¼ 
void Initvertex(Mg G)
{
	strcpy(G->vertex[0],"����¥��ѧ¥");    strcpy(G->vertex[1],"��ʳ��");    strcpy(G->vertex[2],"E1�����������");
	strcpy(G->vertex[3],"��һ��");    strcpy(G->vertex[4],"������");    strcpy(G->vertex[5],"��ɽ�ٳ������");
	strcpy(G->vertex[6],"��ɽ¥");    strcpy(G->vertex[7],"�����");    strcpy(G->vertex[8],"ͼ���");
	strcpy(G->vertex[9],"Уʷ��");    strcpy(G->vertex[10],"��ְ��ҽԺ");    strcpy(G->vertex[11],"��ɽ�ٳ�");
	strcpy(G->vertex[12],"������");    strcpy(G->vertex[13],"ʵ��¥");    strcpy(G->vertex[14],"�����¥");
	strcpy(G->vertex[15],"C4��������̨��");
}

//��ʼ������·�� 
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

//��ʼ����������·�� 
void Initarc(Mg G)
{
	int i,j,k;
	FILE *p1 = fopen("E:/keshe/allarc.txt","r"); //�ļ�ָ�� 
	FILE *p2 = fopen("E:/keshe/arcNum.txt","r");
	FILE *p3 = fopen("E:/keshe/road.txt","r");
	FILE *p4 = fopen("E:/keshe/head.txt","r");
	if(p1==NULL)    exit(0);
	if(p2==NULL)    exit(0);
	if(p3==NULL)    exit(0);
	if(p4==NULL)    exit(0);
	for(i=0 ; i<bus ; i++)  //��ʼ������·�� 
		for(j=0 ; j<G->vertexNum ; j++)
			for(k=0 ; k<G->vertexNum ; k++)
				fscanf(p1,"%d",&G->arc[i][j][k]);
	for(i=0 ; i<bus ; i++)  //��ʼ��ÿ��·����ÿ�������ٱ��� 
		for(j=0 ; j<G->vertexNum ; j++)
			fscanf(p2,"%d",&G->arcNum[i][j]);
	for(i=0 ; i<bus ; i++)  //��ʼ��ÿ��·�ߵ�վ���� 
		fscanf(p3,"%d",&road[i]);
	for(i=0 ; i<bus ; i++)  //��ʼ��ÿ��·��ͷ��� 
	    fscanf(p4,"%d",&head[i]);
	fclose(p1); //�ر��ļ� 
	fclose(p2);
	fclose(p3);
	fclose(p4);
}

//��ʼ������·�����·�����þ��� 
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

//��ʼ�����·��
void InitAll(Mg G)
{
	int i,j,k;
	int v,w;
	for(i=0 ; i < G->vertexNum ; i++)
	{
		for(j=0 ; j < G->vertexNum ; j++)
		{
			if(G->Arc[i][j]==10000||G->Arc[i][j]==0)    Round[i][j]=G->Arc[i][j];
			else    Round[i][j]=6*G->Arc[i][j];  //���о���Ŵ����� 
		}
	}
	for(i=0 ; i<bus ; i++)
	{
		if(road[i]<2)    continue;  //��Ŵ��ڵ�·�� 
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
			Dall[v][w]=Round[v][w];  //�洢�������·����Ȩֵ��(���) 
			Pall[v][w]=w;  //�洢���·�������Ķ���(���) 
		}
	}
 } 

//��ʼ������ 
void InitMg(Mg G)
{
	InitArc(G); //��ʼ������·��  
	Initarc(G); //��ʼ����������·��  
	InitWalk(G); //��ʼ������·�����·�����þ���  
	InitAll(G); //��ʼ�����·��(���滹��ʵʱ����)  
	Initvertex(G); //¼��ص��� 
}

//************************************************************************�������� 

//ѧ�����غ��� 
void Studentback(Mg G)
{
	void Wrong(Mg G);        //���������� 
	void StudentMenu(Mg G);     //ѧ���˵����� 
	int x;
	printf("\n");
	printf("\t\t\t����'0'  ����\n");
	printf("\n");
	printf("\t\t\t**********************************************************\n");
	printf("\t\t\t������:");
	scanf("%d",&x);
	if(x!=0)
	{
		system("cls");
		Wrong(G);      //�������˵� 
	}
	if(x==0)
	{
		system("cls");
		StudentMenu(G);      //ѧ���˵� 
	}
}

//����Ա���غ���
void Rootback(Mg G)
{
	void Wrong(Mg G);        //���������� 
	void RootMenu(Mg G);     //����Ա�������� 
	int x;
	printf("\n");
	printf("\t\t\t����'0'  ����\n");
	printf("\n");
	printf("\t\t\t**********************************************************\n");
	printf("\t\t\t������:");
	scanf("%d",&x);
	if(x!=0)
	{
		system("cls");
		Wrong(G);          //�������˵� 
	}
	if(x==0)
	{
		system("cls");
		RootMenu(G);       //����Ա�˵� 
	}
}

//��ʾ·�ߺ��� 
void Showload(Mg G)
{
	void DFSTraverse(Mg G,int load,int v);
	int i;
	printf("\n");
	printf("\t\t\t�������й���·��:\n");
	for(i=0 ; i<bus ; i++)
	{
		if(road[i]<2)    continue;
		DFSTraverse(G,i,head[i]);      //���·�� 
	}
	printf("\n");
	printf("\t\t\t���մ�ѧվ��:[0]����¥��ѧ¥     [1]��ʳ��            \n");
	printf("\t\t\t             [2]E1�����������   [3]��һ��            \n");
	printf("\t\t\t             [4]������           [5]��ɽ�ٳ������    \n");
	printf("\t\t\t             [6]��ɽ¥           [7]�����            \n");
	printf("\t\t\t             [8]ͼ���           [9]Уʷ��            \n");
	printf("\t\t\t             [10]��ְ��ҽԺ      [11]��ɽ�ٳ�         \n");
	printf("\t\t\t             [12]������          [13]ʵ��¥           \n");
	printf("\t\t\t             [14]�����¥        [15]C4��������̨��   \n");
	printf("\n");
}

//��ͷ��ʼ��������ȱ��������·�� 
void DFSTraverse(Mg G,int load,int v)  //�˴�load����·��-1 
{
	int i,j,k=1;
	int visited[MaxSize][MaxSize] = {0};  //���ʱ�ǣ�0û���ʣ�1���ʹ� 
	visited[load][v] = 1;
	printf("\t\t\t%d����: ",load+1);
	printf("%s",G->vertex[v]);
	for(j=0 ; j < G->vertexNum ; j++) //������ȱ��� 
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

//��������·�� 
void AddLoad(Mg G)
{
	void Wrong(Mg G);  //���������� 
	int add;  //������·������ 
	int m,n;  //�����ڽӾ��� 
	int i,j,k;
	int D[MaxSize][MaxSize]; //��ŷ�ֱ��������·��
	FILE *p = fopen("E:/keshe/AddLoad.txt","w"); //�ļ�ָ�� 
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
	}  //�������·������ 
	printf("\t\t\t*******************���մ�ѧ·�߲�ѯϵͳ*******************\n");
	printf("\t\t\t\t\t   %d��%d��%d��%d��%d��\n",year,month,day,hour,minute);
	printf("\t\t\t\t\t\t   %s\n",hello[hell]);
	printf("\n");
	printf("\t\t\t������������·������:");
	scanf("%d",&add);  //������·������ 
	if(add < 0)  //������� 
	{
		system("cls");
		Wrong(G);
	}
	for(i=bus ; i<bus+add ; i++)  //����������ѭ�� 
	{
		printf("\t\t\t������%d���ߵ�·��(����'-1'ʱֹͣ):",i+1);
		scanf("%d",&n);
		if(n==-1)    break;
		m=n;
		G->arcNum[i][m] = 0;
		head[i] = m;  //��¼ͷ��� 
		road[i] = 1;
		scanf("%d",&n);
		while(n!=-1)
		{
			road[i]++;
			G->arcNum[i][m]++;    G->arcNum[i][n]++; //��¼ÿ��վ��ӵ�еı��� 
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

//ɾ������վ�㣨�ӣ� 
void Delete_1(Mg G,int load,int dele)
{
	int i,j;
	FILE *p = fopen("E:/keshe/Delete.txt","w"); //�ļ�ָ�� 
	if(p==NULL)    exit(0);
	road[load-1]--;
	for(i=0 ; i < G->vertexNum ; i++) //ɸѡ 
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
	if(road[load-1]==1)  //ɾ�����·����ֻ��һ��վ�㣬��·��ȥ��  
	{
		printf("\t\t\t%d������ȥ��!\n",load);
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

//ɾ��˫��վ�㣨�ӣ� 
void Delete_2(Mg G,int load,int dele)
{
	int i,j,k=1;
	int m,n;
	FILE *p = fopen("E:/keshe/Delete.txt","w"); //�ļ�ָ�� 
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

//ɾ��վ�㣨ĸ�� 
void Delete_main(Mg G)
{
	void Wrong(Mg G);        //���������� 
	int load;    //Ҫɾ��վ�������·�� 
	int num;     //Ҫɾ��վ��ĸ��� 
	int dele;    //ɾ����վ�� 
	int i,j,k;
	printf("\t\t\t*******************���մ�ѧ·�߲�ѯϵͳ*******************\n");
	printf("\t\t\t\t\t   %d��%d��%d��%d��%d��\n",year,month,day,hour,minute);
	printf("\t\t\t\t\t\t   %s\n",hello[hell]);
	Showload(G);    //��ʾ����·�� 
	printf("\t\t\t������Ҫɾ��վ�������·��:");
	scanf("%d",&load);
	if(load<1||load>bus)      //������� 
	{
		system("cls");
		Wrong(G);
	}
	if(road[load-1]<2)
	{
		printf("\t\t\t%d���߲�����!\n",load);
	}
	if(road[load-1]>1)
	{
		printf("\t\t\t������Ҫɾ��վ��ĸ���:");
	    scanf("%d",&num);
	    for(i=0 ; i<num ; i++)      //վ��ɾ�� 
	    {
		    printf("\t\t\t������Ҫɾ����վ��:");
		    scanf("%d",&dele);
		    if(G->arcNum[load-1][dele]==0)
		    {
		    	printf("\t\t\t\t��·�߲����ڴ�վ��!\n");
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

//��ʼ������·�ߣ��ӣ� 
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

//�޸�·�ߡ�����վ�㣨ĸ��  
void Change(Mg G)
{
	void Wrong(Mg G);        //���������� 
	int m,n;
	int i,j,k;
	int load;
	int D[MaxSize][MaxSize];     //��ŷ�ֱ��������·�� 
	FILE *p = fopen("E:/keshe/Change.txt","w"); //�ļ�ָ�� 
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
	}  //�������·������ 
	printf("\t\t\t*******************���մ�ѧ·�߲�ѯϵͳ*******************\n");
	printf("\t\t\t\t\t   %d��%d��%d��%d��%d��\n",year,month,day,hour,minute);
	printf("\t\t\t\t\t\t   %s\n",hello[hell]);
	Showload(G);    //��ʾ����·�� 
	printf("\t\t\tѡ��Ҫ�޸ĵ�·��:");
	scanf("%d",&load);
	if(load<1||load>bus)
	{
		system("cls");
		Wrong(G);
	}
	Initload(G,load);     //��ʼ��·��
	printf("\n");
	printf("\t\t\t�����޸ĵ�����·��(����'-1'ʱֹͣ):"); 
	scanf("%d",&n);
	if(n!=-1)
	{
		m=n;
		head[load-1]=m;  //��¼ͷ�ڵ� 
		road[load-1]=1;  //վ������ʼ�� 
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
		DFSTraverse(G,load-1,head[load-1]);  //���·�� 
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

//·�߲�ѯ 
void FindLoad(Mg G)
{
	void Wrong(Mg G);        //����˵����� 
	int x,k=0;            //k��¼����aվ��Ĺ������� 
	int a=0;            //Ҫ��ѯ��վ�� 
	int i,j;
	printf("\t\t\t*******************���մ�ѧ·�߲�ѯϵͳ*******************\n");
	printf("\t\t\t\t\t   %d��%d��%d��%d��%d��\n",year,month,day,hour,minute);
	printf("\t\t\t\t\t\t   %s\n",hello[hell]);
	Showload(G);    //��ʾ����·�� 
	while(a>=0)
	{
		printf("\t\t\t������Ҫ��ѯ��վ��(����'-1'ֹͣ��ѯ):");
	    scanf("%d",&a);
	    if(a<-1||a>MaxSize)
	    {
	    	system("cls");
	    	Wrong(G);
		}
	    if(a==-1)    break;
	    printf("\t\t\t����%s�Ĺ���·����:",G->vertex[a]);
	    for(i=0 ; i<bus ; i++)       //����·�в����Ƿ���aվ�� 
	    {
	    	for(j=0 ; j < G->vertexNum ; j++)
	    	{
	    		if(G->arc[i][a][j]!=0&&G->arc[i][a][j]!=10000)
	    		{
	    			printf("%d����  ",i+1);
	    			j=G->vertexNum;
	    			k++;
				}
			}
		}
		if(k==0)    printf("��Ǹ!��վ��û�й���·�߾���");
	    printf("\n");
	}
	Studentback(G);
} 

//�����������·����� 
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

//������·�ߣ��ӣ� 
void Shortest(Mg G,int start,int end,int P[][MaxSize],int D[][MaxSize])
{
	int temp;    //�������  
	Shortes_Floyed(G,P,D);     //�������·�� 
	printf("%s-->%s",G->vertex[start],G->vertex[P[start][end]]);
	temp=P[start][end];
	while(temp!=end)
	{
		printf("-->%s",G->vertex[P[temp][end]]);
		temp=P[temp][end];
	}
}

//����ɵ���������·����ĸ�� 
void Shortest_Fivetree(Mg G)
{
	int i,j;
	int t=7;
	printf("\t\t\t������ɵ����ص�����·��Ϊ:\n");
	for(i=0 ; i < G->vertexNum ; i++)
	{
		if(i==t)    continue;
		printf("\t\t\t\t\t");
		Shortest(G,t,i,Pwalk,Dwalk);
		printf("\t����Ϊ:%d��",Dwalk[t][i]);
		printf("\t��Լ��Ҫ:%.2f����\n",(double)Dwalk[t][i]/(double)vwalk);
	}
}

//�жϻ���
void Ring(Mg G)
{
	int i,k=0;
	int load;
	printf("\n");
	printf("\t\t\tУ�ڵĻ���:");
	for(load=0 ; load<bus ; load++)
	{
		if(G->arcNum[load][head[load]]==2)
		{
			printf("%d���� ",load+1);
			k++;
		}
		if(G->arcNum[load][head[load]]==1)
		{
			for(i=0 ; i < G->vertexNum ; i++)
			{
				if(G->arcNum[load][i]==3)
				{
					printf("%d���� ",load+1);
					k++;
				}
			}
		}
	}
	if(k==0)    printf("û�л���!");
	printf("\n");
 } 

//���·���滮 
void Bestload(Mg G)
{
	void Wrong(Mg G);        //����˵����� 
	int start,end;
	int i,j,k;
	printf("\t\t\t*******************���մ�ѧ·�߲�ѯϵͳ*******************\n");
	printf("\t\t\t\t\t   %d��%d��%d��%d��%d��\n",year,month,day,hour,minute);
	printf("\t\t\t\t\t\t   %s\n",hello[hell]);
	Showload(G);    //��ʾ����·�� 
	printf("\t\t\t������ʼ��:");
	scanf("%d",&start);
	printf("\t\t\t����Ŀ�ĵ�:");
	scanf("%d",&end);
	if(start<0||start>G->vertexNum||end<0||end>G->vertexNum)
	{
		system("cls");
		Wrong(G);        //���뱨�� 
	}
	//�������·�� 
	printf("\n");
	printf("\t\t\t���е����·��Ϊ:");
	Shortest(G,start,end,Pwalk,Dwalk);
	printf("\t����Ϊ:%d��",Dwalk[start][end]);
	printf("\t��·ԼΪ:%.2f����\n",(double)Dwalk[start][end]/(double)vwalk);
	//���ʱ��·��(��·+����) 
	printf("\n");
	printf("\t\t\t����·��Ϊ:");
	InitAll(G);     //��ʼ�����·��(��¼���º��·��) 
	Shortest(G,start,end,Pall,Dall);
	printf("\t��Լ��Ҫ:%.2f����\n",(double)Dall[start][end]/(double)vbus);
	//����ɵ���������·�� 
	printf("\n");
	Shortest_Fivetree(G);
	printf("\n");
	//�жϻ���
	Ring(G);
	
	Studentback(G);
 } 

//���������·����ĸ�� 
void AllPath(Mg G)
{
	void Path(Mg G,int m,int n,int k);     //����·�����ӣ� 
	void Wrong(Mg G);        //����˵����� 
	int k=0;
	int start,end;
	printf("\t\t\t*******************���մ�ѧ·�߲�ѯϵͳ*******************\n");
	printf("\t\t\t\t\t   %d��%d��%d��%d��%d��\n",year,month,day,hour,minute);
	printf("\t\t\t\t\t\t   %s\n",hello[hell]);
	Showload(G);    //��ʾ����·�� 
	printf("\t\t\t��������ʼ��:");
	scanf("%d",&start);
	printf("\t\t\t������Ŀ�ĵ�:");
	scanf("%d",&end);
	printf("\n");
	if(start<0||start>G->vertexNum||end<0||end>G->vertexNum)
	{
		system("cls");
		Wrong(G);        //���뱨�� 
	}
	printf("\t\t\t�ɴ������·����:\n");
	stack[0]=start;       //�洢��� 
	for(k=0 ; k<G->vertexNum ; k++)
	    visit[k]=0;
	visit[start]=1;
	Path(G,start,end,0);
	printf("\n");
	Studentback(G);
}

//�������·�����ӣ� 
void Path(Mg G,int m,int n,int k)
{
	int s,t,i;
	t=k+1;
	length=0;
	if(stack[k]==n&&k < G->vertexNum)  //�ﵽĿ�ĵ�ֹͣ����������ֹ����ѭ�� 
	{
		for(s=0 ; s<k ; s++)     //·������ 
		{
			length=length+Round[stack[s]][stack[s+1]];
		}
		if(length<10000)
		{
			printf("\t\t\t\t");
		    for(s=0 ; s<k ; s++)     //���·�� 
		    {
			    printf("%s-->",G->vertex[stack[s]]);
		    }
		    printf("%s",G->vertex[stack[s]]);
		    printf("\t��Լ��Ҫ:%.2f����\n",(double)length/(double)vbus);    //ʱ�� 
		}
	}
	else   //δ��Ŀ�ĵؼ����ݹ� 
	{
		s=0;
		while(s < G->vertexNum)
		{
			if(Round[stack[k]][s]!=0&&Round[stack[k]][s]!=10000&&visit[s]==0)
			{
				visit[s]=1;   //���ʼ�¼ 
				stack[t]=s;    //�洢���� 
				Path(G,m,n,t);    //�ݹ�ֱ���ҵ�Ŀ�ĵ� 
				visit[s]=0;     //�������ã����ڼ���Ѱ�� 
			}
			s++;
		}
	}
}

//*********************************************************************�˵����� 

//���뱨�� 
void Wrong(Mg G)
{
	void MainMenu(Mg G);       //���˵����� 
	int x;
	printf("\t\t\t*******************���մ�ѧ·�߲�ѯϵͳ*******************\n");
	printf("\t\t\t||                                                      ||\n");
	printf("\t\t\t||                       �������                     ||\n");
	printf("\t\t\t||                 ����'0'  �Է������˵�                ||\n");
	printf("\t\t\t||                                                      ||\n");
	printf("\t\t\t**********************************************************\n");
	printf("\t\t\t������:");
	scanf("%d",&x);
	if(x!=0)
	{
		system("cls");
		Wrong(G);  //���� 
	}
	if(x==0)
	{
		system("cls");
		MainMenu(G);  //���˵� 
	}
} 

//�˳�ϵͳ��β 
void Thank()
{
	printf("\t\t\t*******************���մ�ѧ·�߲�ѯϵͳ*******************\n");
	printf("\t\t\t||                                                      ||\n");
	printf("\t\t\t||                       ��лʹ�ã�                     ||\n");
	printf("\t\t\t||                                                      ||\n");
	printf("\t\t\t**********************************************************\n");
} 

//����Ա�˵� 
void RootMenu(Mg G)
{
	void MainMenu(Mg G);       //���˵����� 
	int x;
	printf("\t\t\t*******************���մ�ѧ·�߲�ѯϵͳ*******************\n");
	printf("\t\t\t\t\t   %d��%d��%d��%d��%d��\n",year,month,day,hour,minute);
	printf("\t\t\t\t\t\t   %s\n",hello[hell]);
	printf("\t\t\t||-----------------------����Ա��-----------------------||\n");
	printf("\t\t\t||                                                      ||\n");
	printf("\t\t\t||                 ����'1' ��������·��                 ||\n");
	printf("\t\t\t||                 ����'2' �޸����й���·��             ||\n");
	printf("\t\t\t||                 ����'3' ɾ������վ��                 ||\n");
	printf("\t\t\t||                 ����'0' �������˵�                   ||\n");
	printf("\t\t\t||                                                      ||\n");
	printf("\t\t\t**********************************************************\n");
	printf("\t\t\t������:");
	scanf("%d",&x);
	if(x<0||x>3)    x=5;
	switch(x)
	{
		case 0:system("cls");
		       MainMenu(G);    break;  //���˵� 
		case 1:system("cls");
		       AddLoad(G);     break;  //����·�� 
		case 2:system("cls");
		       Change(G);       break;  //�޸�·�ߡ����վ�� 
		case 3:system("cls");
		       Delete_main(G);   break;  //ɾ��վ�� 
		case 5:system("cls");
		       Wrong(G);  //���� 
	}
}

//ѧ���˵� 
void StudentMenu(Mg G)
{
	void MainMenu(Mg G);         //���˵����� 
	char ch;
	printf("\t\t\t**********************���մ�ѧ·�߲�ѯϵͳ**********************\n");
	printf("\t\t\t\t\t   %d��%d��%d��%d��%d��\n",year,month,day,hour,minute);
	printf("\t\t\t\t\t\t   %s\n",hello[hell]);
	printf("\t\t\t||---------------------------ѧ����---------------------------||\n");
	printf("\t\t\t||                                                            ||\n");
	printf("\t\t\t||      ���մ�ѧվ��:[0]����¥��ѧ¥     [1]��ʳ��            ||\n");
	printf("\t\t\t||                   [2]E1�����������   [3]��һ��            ||\n");
	printf("\t\t\t||                   [4]������           [5]��ɽ�ٳ������    ||\n");
	printf("\t\t\t||                   [6]��ɽ¥           [7]�����            ||\n");
	printf("\t\t\t||                   [8]ͼ���           [9]Уʷ��            ||\n");
	printf("\t\t\t||                   [10]��ְ��ҽԺ      [11]��ɽ�ٳ�         ||\n");
	printf("\t\t\t||                   [12]������          [13]ʵ��¥           ||\n");
	printf("\t\t\t||                   [14]�����¥        [15]C4��������̨��   ||\n");
	printf("\t\t\t||                                                            ||\n");
	printf("\t\t\t||                   ����'A'  ������·��ѯ                    ||\n");
	printf("\t\t\t||                   ����'B'  ������·�滮                    ||\n");
	printf("\t\t\t||                   ����'C'  ���·�߹滮                    ||\n");
	printf("\t\t\t||                   ����'Y'  �������˵�                      ||\n");
	printf("\t\t\t||                                                            ||\n");
	printf("\t\t\t****************************************************************\n");
	printf("\t\t\t������:");
	ch=getchar();        //�Իس� 
	scanf("%c",&ch);
	if(ch!='A'&&ch!='B'&&ch!='C'&&ch!='Y')    ch='Z';
	switch(ch)
	{
		case 'A':system("cls");
		         FindLoad(G);    break;  //·�߲�ѯ 
		case 'B':system("cls");
		         AllPath(G);     break;  //����·�� 
		case 'Y':system("cls");
		         MainMenu(G);    break;  //���˵� 
		case 'C':system("cls");
		         Bestload(G);    break;  //���·�� 
		case 'Z':system("cls");
		         Wrong(G);  //���� 
	}
}

//���˵� 
void MainMenu(Mg G)
{
	int x;
	printf("\t\t\t*******************���մ�ѧ·�߲�ѯϵͳ*******************\n");
	printf("\t\t\t\t\t   %d��%d��%d��%d��%d��\n",year,month,day,hour,minute);
	printf("\t\t\t\t\t\t   %s\n",hello[hell]);
	printf("\t\t\t||-------------------------���˵�-----------------------||\n");
	printf("\t\t\t||                                                      ||\n");
	printf("\t\t\t||                 ����'1'  ��¼����Ա��                ||\n");
	printf("\t\t\t||                 ����'2'  ��¼ѧ����                  ||\n");
	printf("\t\t\t||                 ����'0'  �˳�ϵͳ                    ||\n");
	printf("\t\t\t||                                                      ||\n");
	printf("\t\t\t**********************************************************\n");
	printf("\t\t\t������:");
	scanf("%d",&x);
	if(x<0||x>2)    x=3;
	switch(x)
	{
		case 0:system("cls");
		       Thank();           break;
		case 1:system("cls");
		       RootMenu(G);       break;  //����Ա�˵� 
		case 2:system("cls");
		       StudentMenu(G);    break;  //ѧ���˵� 
		case 3:system("cls");
		       Wrong(G);  //����˵� 
	}
}

int main()
{
	Mg G;
	G = (Mg)malloc(sizeof(Mgraph));
	G->vertexNum = MaxSize;
	//ʱ�� 
	time_t tmpcal_t;
	struct tm*tmp_ptr = NULL;
	time(&tmpcal_t);
	tmp_ptr = localtime(&tmpcal_t);
	year=1900+tmp_ptr->tm_year; //�� 
	month=1+tmp_ptr->tm_mon; //�� 
	day=tmp_ptr->tm_mday; //�� 
	hour=tmp_ptr->tm_hour; //Сʱ 
	minute=tmp_ptr->tm_min; //���� 
	if(hour>=6&&hour<=10)    hell=0;
	if(hour>=11&&hour<=13)    hell=1;
	if(hour>=14&&hour<=18)    hell=2;
	if(hour>=19&&hour<=24||hour>=0&&hour<=5)    hell=3;
	strcpy(hello[0],"�����!");    strcpy(hello[1],"�����!");
	strcpy(hello[2],"�����!");    strcpy(hello[3],"���Ϻ�!");
	//
	bus=5; //��¼���й���·����������ʼ5�� 
	vwalk=80;  //�����ٶ�80��ÿ���� 
	vbus=480;  //�����ٶ�480��ÿ���� 
	InitMg(G);
	MainMenu(G);
	return 0;
}
/*
��ʼ����·�� 
һ���ߣ�1-0-13-8 
�����ߣ�1-15-8-6-7 
�����ߣ�1-3-5-6-8 
�ĺ��ߣ�3-12-11-4 
����ߣ�10-11-7-6-8 
*/
