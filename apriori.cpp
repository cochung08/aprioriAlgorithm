#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <string>
#include <iomanip>
#include <map>
#include <cmath>
#include <bitset>
#include <algorithm>

using namespace std;

ifstream file("D:\\src\\label.txt");   //打开数据文件
ofstream outfile("D:\\src\\output.txt");   //输出规则到该文件

const double MinSupport=35,MinConf=0.70;  //设置最小绝对支持度和最小置信度
long All_items_count=0;   //标签的数量
vector<string> Confidence_Set;     //用于存放满足最低置信度要求的关联规则
map<string,int> items_count;   //统计各个项集的数目
vector<vector<string> > Data_Set;        //原始数据项集
vector<string> item;   //项集的临时vector 
vector<vector<string> > Candidate_Set;   //候选项集
vector<vector<string> > Frequent_Set;   //频繁项集
vector<string> AssociationRule_Set;   //关联规则项集
vector<map<string,int> > bitmap;     //判断某个项目在某一个事务中是否存在，存在则值为1，反之为0


void ReadFile(std::ifstream &infile);//读取文件
vector<string> Gen_Next_Candidate(vector<string> vect1,vector<string> vect2,int pass);     //连接生成下一轮的候选集
int isExist(vector<string> item,vector<vector<string> >items);         //判断项集item是否已经存在候选项集集合items中,存在则返回1
void computeConfidence(vector<string> vec,int pass);         //计算并输出置信度
void Output(std::ofstream &outfile ,vector<string> vec);//输出频繁项集到指定文件
void Cut_infrequent_sub(vector<string> item,int found );//剪枝操作
void Gen_Single_Set(vector<vector<string> > Data_Set);//生成频繁1项集
void Gen_Multi_Set();//生成频繁多项集

/*主函数*/
int main()
{
  if(!file)       
  {
     cout<<"读取文件失败"<<endl;
     return 1;
  }
  else
  {
	 ReadFile(file);
     Gen_Single_Set(Data_Set);
     Gen_Multi_Set();
	 file.close();
     outfile.close();
	 system("pause");
	 return 0;
   }
}

/*
*读取文件函数
*/
void ReadFile(std::ifstream &infile)
{
	 string temp;
     cout<<"The original data:"<<endl;
     int begin,end;
     while(getline(infile,temp))     //读入数据
     {
        All_items_count++;
        begin=0;
        temp.erase(0,temp.find_first_not_of("\r\t\n "));   //去除字符串首部的空格
        temp.erase(temp.find_last_not_of("\r\t\n")+1);        //去除字符串尾部的空格
        while((end=temp.find(',',begin))!=string::npos)    //每一个事务中的项是以空格为分隔符的
        {
           item.push_back(temp.substr(begin,end-begin));   //将每一个项插入item中
           begin=end+1;
        }
        item.push_back(temp.substr(begin));     //一个事务中的最后一项
        Data_Set.push_back(item);       //将一个事务中的所有项当成一个整体插入另一个大的vector中
        item.clear();   //清空item
        cout<<temp<<endl;
     }
	 cout<<endl<<"一共有 "<<All_items_count<<" 个事务"<<endl;//输出事务的总数
	 cout<<"请按Enter键继续：";  //pause
     getchar();
}

/*
*连接函数：
*当两个频繁项集中有一项不同，就会连接成下一轮的候选项集
*/
vector<string> Gen_Next_Candidate(vector<string> vect1,vector<string> vect2,int num)     //num是v1和v2项的个数
{
   int count=0;     //记录v1和v2有相同的项的个数
   vector<string> tempvec;//储存连接后的项集
   map<string,int> tempMap;   //记录项出现的次数
   for(int i=0;i<vect1.size();i++)
   {
      tempMap[vect1[i]]++;
      tempvec.push_back(vect1[i]);
   }
   for(int j=0;j<vect2.size();j++)
   {
      tempMap[vect2[j]]++;
      if(tempMap[vect2[j]]==2)  //如果项出现的次数为2，表示v1和v2有相同的项，count就加1
      {
         count++;
      }
      else tempvec.push_back(vect2[j]);//把vec2中与vect1不相同的项放进vec
   }
   if( count+1 == num)     return tempvec; //恰好有一项不同
   else{  
   tempvec.clear();
   return tempvec;
   }
}

/*
*判断tempItem是否存在于tempItems里面，存在就返回1，不存在返回0
*/
int isExist(vector<string> item,vector<vector<string> >items)  //判断项集item是否已经存在候选项集集合items中,存在则返回1
{
  
    int count;   //统计item中与items里每一个项集相同的项的数目
    if(!items.empty())
    {
       for(vector<vector<string> >::size_type ix=0;ix!=items.size();ix++)
       {
           count=0;
           for(vector<string>::size_type iy=0;iy!=items[ix].size();iy++)
           {
               for(vector<string>::size_type iz=0;iz!=item.size();iz++)
               {
                   if(item[iz]==items[ix].at(iy))
                   {
                      count++;
                   }
               }
           }
           if(count==item.size())     //表示存在
           {
              return 1;
           }
       }
     }
    return 0;
}

/*
*计算和输出置信度函数
*/
void computeConfidence(vector<string> vec,int pass)            //计算并输出置信度
{
    string t_str;
    vector<string> RealSubset;   
    double subset_count=pow(2.0,pass)-1;   //vec里非空真子集的个数
    char s[100];
    int st;
	int k=1;
	/*求出所有非空真子集*/
    for(int i=1;i<subset_count;i++)
    {
        bitset<20>  bitvec(k);         
        for(int j=0;j<pass;j++)
        {
            if(bitvec[j]==1)    t_str+=vec[j];   //串接字符串形成一个子集
        }
        RealSubset.push_back(t_str);   
        t_str.erase();
        k++;
    }
	int count = 0;//用于换行
    for(int i=0;i<RealSubset.size();i++)
    {
       for(int j=0;j<RealSubset.size();j++)
       {
          if( i!=j && RealSubset[i].find(RealSubset[j])==string::npos 
			       && RealSubset[j].find(RealSubset[i])==string::npos)//判断真子集之间是否有相同项
          {
			  //求置信度：I1I2I3的个数/I1的个数
             if((float)(items_count[RealSubset[i]+RealSubset[j]])/(float)(items_count[RealSubset[i]])>MinConf)   
             {
				 count++;
                 cout.setf(ios::fixed);
                 cout<<RealSubset[i]<<"=>"<<RealSubset[j]<<"<"<<setprecision(4)
					 <<(float)(items_count[RealSubset[i]+RealSubset[j]])/(float)(items_count[RealSubset[i]])<<"> ";
				 if(pass<=4 && count%3 == 0) cout<<endl;
				 else if(count%2==0) cout<<endl;
                 t_str="{" + RealSubset[i] + "}=>{" + RealSubset[j] + "} 置信度:";
                 sprintf(s,"%f",(float)(items_count[RealSubset[i]+RealSubset[j]])/(float)(items_count[RealSubset[i]]));//把置信度转换成字符串
                 string s2(s);
                 t_str += s2;//连接
                 for(st=0;st<Confidence_Set.size();st++)      //检查是否已经有该关联规则存进去了，以防止重复
                 {
                     if(Confidence_Set[st] == t_str) break;
                        
                 }
                 if(st==Confidence_Set.size())     //没有发生重复，则加入关联规则集合
                 {
                    Confidence_Set.push_back(t_str);      
                 }
                 t_str.erase();            
             }

          }
       }
    }
    cout<<endl<<"-----------------------------------------------"<<endl;
}

/*
*  输出结果函数
*/
void Output(std::ofstream &outfile,vector<string> Confidence_Set)
{
	     cout<<"得出的关联规则如下:"<<endl;
         for(int st=0;st!=Confidence_Set.size();st++)
         {
             cout<<Confidence_Set[st]<<endl;
         }
         outfile<<"输出的关联规则如下:"<<endl;
         for(vector<string>::iterator it=Confidence_Set.begin();it!=Confidence_Set.end();it++)
         {
            outfile<<*it<<endl;
         }
         
}

/*
*  剪枝操作函数
*/
void Cut_infrequent_sub(vector<string> tempItem,int found )
{
	                    string temp_s;
	                	vector<string> tempvec;
                        string teststr;
                        int testint;
                        tempvec=tempItem;
                        sort(tempvec.begin(),tempvec.end());
                        while(next_permutation(tempvec.begin(),tempvec.end())) //置换
                        {
                           for(int i=0;i!=tempvec.size();i++) temp_s+=tempvec[i]; //把各个项集连接成一个字符串
                          
                           for(map<string,int>::const_iterator tempit=items_count.begin();tempit!=items_count.end();tempit++)
                           {
                                if(tempit->second < MinSupport)  //小于支持度
                                {
                                    if(temp_s.find(tempit->first)!=string::npos)   //表示包含有非频繁子项集
                                   {
                                       found=1;
                                       teststr=tempit->first;
                                       testint=tempit->second;
                                       break;
                                   }
                                }
                           }
                           temp_s.erase();
                           if(found)   break;//包含非频繁子项集                       
                        }//end_while
                        if(!found)     //只有不包含有非频繁子项集才加入候选项集中，否则剪枝掉
                           Candidate_Set.push_back(tempItem);
                        else  //根据apriori性质删除具有非频繁子集
                        {
                           cout<<"剪枝:";
                           for(int st2=0;st2!=tempItem.size();st2++)
                               cout<<tempItem[st2];
                           //cout<<" 含有非频繁子项集:"<<teststr<<" "<<testint<<"/"<<All_items_count<<"="<<((float)(testint)/(float)All_items_count);
                          cout<<"  含有非频繁子项集:"<<teststr<<" 出现次数："<<testint<<"，低于最小支持度";
                           cout<<endl;
                        }
                        found=0;   
}

/*
*  生成频繁1项集函数
*/
void Gen_Single_Set(vector<vector<string> > Data_Set)
{
	map<string,int> item_map; //记录项是否在事务存在，1为存在，0为不存在 
     for(int i=0;i!=Data_Set.size();++i)
     {
        for(int j=0;j!=Data_Set[i].size();++j)
        {
         items_count[Data_Set[i].at(j)]++;    //该项集的计数加1
         item_map[Data_Set[i].at(j)]=1;      
        }
        bitmap.push_back(item_map);
        item_map.clear();     
     }
     
     cout<<"候选1项集:"<<endl;
	 cout<<"-----------------------------------------------"<<endl;
	 cout<<setiosflags(ios::left)<<setw(18)<<"项集"<<"支持度计数"<<endl;
	 cout<<"-----------------------------------------------"<<endl;
	 map<string,int>::const_iterator it=items_count.begin();
     while(it!=items_count.end())      //输出候选1项集
     {		
		cout<<setiosflags(ios::left)<<setw(18)<<it->first<<items_count[it->first]<<endl;
        it++;
     }
	 cout<<"-----------------------------------------------"<<endl;
	 cout<<"按Enter键继续；";  //pause
     getchar();
	 it=items_count.begin();
     cout<<"频繁1项集:"<<endl;
	 cout<<"-----------------------------------------------"<<endl;
	 cout<<setiosflags(ios::left)<<setw(15)<<"项集"<<"支持度"<<endl;
	 cout<<"-----------------------------------------------"<<endl;
	 int count=0;
     while(it!=items_count.end())          //频繁1项集
     {
        if( it->second >= MinSupport)    //大于等于支持度
        {
			count++;           
       	    cout<<setiosflags(ios::left)<<setw(15)<<it->first<<it->second<<endl;
            item.push_back(it->first);
            Frequent_Set.push_back(item);   //插入候选1项集的vector中
            item.clear();    
        }
        it++;
     }
	 cout<<"-----------------------------------------------"<<endl;
	 cout<<"一共有 "<<count<<" 个频繁1项集"<<endl;
}

/*
*  生成频繁多项集函数
*/
void Gen_Multi_Set()
{
	int found;    //记录是否包含非频繁项集
	 if(!Frequent_Set.empty())   //判断频繁1项集是否为空，为空则退出
     {
		 int pass=1;
         cout<<"按Enter键继续；"; 
         getchar();                  
         while(!Frequent_Set.empty())
         {
            //生成下一轮的候选项集
			Candidate_Set.clear();        
            int st=Frequent_Set.size();
            for(int st1=0;st1<st;st1++)
            {
                for(int st2=st1+1;st2<st;st2++)
                {
                    found=0;
                    item=Gen_Next_Candidate(Frequent_Set[st1],Frequent_Set[st2],pass);    //调用函数合并生成下一轮的候选项集
                    if(!item.empty()&&!isExist(item,Candidate_Set))   //若经过判断处理后返回的vector不为空且还不存在该项集，则作为候选项集加入候选vector中
                    {
                        Cut_infrequent_sub( item,found );//检查需不需要进行剪枝
                    }
                    
                }
            }
            Frequent_Set.clear();        
            pass++;
            cout<<endl<<"候选"<<pass<<"项集:"<<endl;
            for(int ix=0;ix!=Candidate_Set.size();++ix)      //输出候选项集
            {
               cout<<"{";
               for(int iy=0;iy!=Candidate_Set[ix].size();++iy)
               {
                cout<<Candidate_Set[ix].at(iy);
               }
               cout<<"}"<<endl;
            }
            if(Candidate_Set.empty())   cout<<"候选"<<pass<<"项集为空!"<<endl;            
            int flag;    //判断某个项集在某条事务中是否出现，出现为1，否则为0
            int count;    //统计某个项集在整个事务集中出现的次数
            string tempstr;  //用于把各个项集组成字符串 
            int check;   //检测串连项集的次数是否为1 
            for(int sx=0;sx!=Candidate_Set.size();++sx)      
            {
                check=1;
                count=0;
                for(int sy=0;sy!=bitmap.size();++sy)
                {
                    flag=1;       //初始化为1，表示出现
                    for(int sz=0;sz!=Candidate_Set[sx].size();++sz)
                    {
                       if(bitmap[sy][Candidate_Set[sx].at(sz)]==0)   //存在某一个子项不存在，则没出现项集
                       {
                           flag=0;
                       }
                       if(check==1)  //只连接一次
                       {
                           tempstr+=Candidate_Set[sx].at(sz);  //串接字符串
                       }
                    }
                    
                    if(flag)  //表示有出现
                    {
                       count++;
                    }
                    check++;
                }//end_for
                
                if(count>=MinSupport)    Frequent_Set.push_back(Candidate_Set[sx]);        //count大于最小支持度，表示是频繁项集
               
                items_count[tempstr]=count;       //对应该项集的计数值
               
                sort(Candidate_Set[sx].begin(),Candidate_Set[sx].end());   
                string tempstr2;
                while(next_permutation(Candidate_Set[sx].begin(),Candidate_Set[sx].end()))  //生成全排列
                {
                     for(int tempst=0;tempst!=Candidate_Set[sx].size();tempst++) //拼接出该字符串组合
                     {
                         tempstr2+=Candidate_Set[sx][tempst];
                     }
                     items_count[tempstr2]=count;  //对应该项集的计数值
                     tempstr2.erase();
                }               
                tempstr.erase();
            }
            cout<<"按Enter键继续；"; 
            getchar();
            if(!Frequent_Set.empty())     //频繁项集不为空
            {
				
                 cout<<"频繁"<<pass<<"项集:"<<endl;
				 cout<<"-------------------------------------------------------------------"<<endl;
                 for(int i=0;i!=Frequent_Set.size();++i)      //输出频繁项集
                 {
                     for(int j=0;j!=Frequent_Set[i].size();++j)
                     {
   						 if(j==0) cout<<Frequent_Set[i].at(j);
						 else cout<<"/"<<Frequent_Set[i].at(j);
                        tempstr+=Frequent_Set[i].at(j);  //串接字符串
                     }
                     cout<<endl;
					 cout<<"支持度计数： "<<items_count[tempstr]<<endl;
					 cout<<"置信度如下:  "<<endl;
                     computeConfidence(Frequent_Set[i],pass);
                     cout<<endl;
                     tempstr.erase();
                 }//end_for
				 cout<<"一共有 "<<Frequent_Set.size()<<" 个频繁 "<<pass<<" 项集"<<endl;
                 cout<<"按Enter键继续；"; 
                 getchar();
             }
             else
             {
                 cout<<"没有"<<pass<<"-频繁项集"<<endl;
				 cout<<"程序运行结束！"<<endl;
             }
         }//end_while   
		 Output(outfile,Confidence_Set);
     }
     else
     {  

        //do nothing
     }    //end of if(!Frequent_Set.empty())
}


