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

ifstream file("D:\\src\\label.txt");   //�������ļ�
ofstream outfile("D:\\src\\output.txt");   //������򵽸��ļ�

const double MinSupport=35,MinConf=0.70;  //������С����֧�ֶȺ���С���Ŷ�
long All_items_count=0;   //��ǩ������
vector<string> Confidence_Set;     //���ڴ������������Ŷ�Ҫ��Ĺ�������
map<string,int> items_count;   //ͳ�Ƹ��������Ŀ
vector<vector<string> > Data_Set;        //ԭʼ�����
vector<string> item;   //�����ʱvector 
vector<vector<string> > Candidate_Set;   //��ѡ�
vector<vector<string> > Frequent_Set;   //Ƶ���
vector<string> AssociationRule_Set;   //���������
vector<map<string,int> > bitmap;     //�ж�ĳ����Ŀ��ĳһ���������Ƿ���ڣ�������ֵΪ1����֮Ϊ0


void ReadFile(std::ifstream &infile);//��ȡ�ļ�
vector<string> Gen_Next_Candidate(vector<string> vect1,vector<string> vect2,int pass);     //����������һ�ֵĺ�ѡ��
int isExist(vector<string> item,vector<vector<string> >items);         //�ж��item�Ƿ��Ѿ����ں�ѡ�����items��,�����򷵻�1
void computeConfidence(vector<string> vec,int pass);         //���㲢������Ŷ�
void Output(std::ofstream &outfile ,vector<string> vec);//���Ƶ�����ָ���ļ�
void Cut_infrequent_sub(vector<string> item,int found );//��֦����
void Gen_Single_Set(vector<vector<string> > Data_Set);//����Ƶ��1�
void Gen_Multi_Set();//����Ƶ�����

/*������*/
int main()
{
  if(!file)       
  {
     cout<<"��ȡ�ļ�ʧ��"<<endl;
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
*��ȡ�ļ�����
*/
void ReadFile(std::ifstream &infile)
{
	 string temp;
     cout<<"The original data:"<<endl;
     int begin,end;
     while(getline(infile,temp))     //��������
     {
        All_items_count++;
        begin=0;
        temp.erase(0,temp.find_first_not_of("\r\t\n "));   //ȥ���ַ����ײ��Ŀո�
        temp.erase(temp.find_last_not_of("\r\t\n")+1);        //ȥ���ַ���β���Ŀո�
        while((end=temp.find(',',begin))!=string::npos)    //ÿһ�������е������Կո�Ϊ�ָ�����
        {
           item.push_back(temp.substr(begin,end-begin));   //��ÿһ�������item��
           begin=end+1;
        }
        item.push_back(temp.substr(begin));     //һ�������е����һ��
        Data_Set.push_back(item);       //��һ�������е��������һ�����������һ�����vector��
        item.clear();   //���item
        cout<<temp<<endl;
     }
	 cout<<endl<<"һ���� "<<All_items_count<<" ������"<<endl;//������������
	 cout<<"�밴Enter��������";  //pause
     getchar();
}

/*
*���Ӻ�����
*������Ƶ�������һ�ͬ���ͻ����ӳ���һ�ֵĺ�ѡ�
*/
vector<string> Gen_Next_Candidate(vector<string> vect1,vector<string> vect2,int num)     //num��v1��v2��ĸ���
{
   int count=0;     //��¼v1��v2����ͬ����ĸ���
   vector<string> tempvec;//�������Ӻ���
   map<string,int> tempMap;   //��¼����ֵĴ���
   for(int i=0;i<vect1.size();i++)
   {
      tempMap[vect1[i]]++;
      tempvec.push_back(vect1[i]);
   }
   for(int j=0;j<vect2.size();j++)
   {
      tempMap[vect2[j]]++;
      if(tempMap[vect2[j]]==2)  //�������ֵĴ���Ϊ2����ʾv1��v2����ͬ���count�ͼ�1
      {
         count++;
      }
      else tempvec.push_back(vect2[j]);//��vec2����vect1����ͬ����Ž�vec
   }
   if( count+1 == num)     return tempvec; //ǡ����һ�ͬ
   else{  
   tempvec.clear();
   return tempvec;
   }
}

/*
*�ж�tempItem�Ƿ������tempItems���棬���ھͷ���1�������ڷ���0
*/
int isExist(vector<string> item,vector<vector<string> >items)  //�ж��item�Ƿ��Ѿ����ں�ѡ�����items��,�����򷵻�1
{
  
    int count;   //ͳ��item����items��ÿһ�����ͬ�������Ŀ
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
           if(count==item.size())     //��ʾ����
           {
              return 1;
           }
       }
     }
    return 0;
}

/*
*�����������ŶȺ���
*/
void computeConfidence(vector<string> vec,int pass)            //���㲢������Ŷ�
{
    string t_str;
    vector<string> RealSubset;   
    double subset_count=pow(2.0,pass)-1;   //vec��ǿ����Ӽ��ĸ���
    char s[100];
    int st;
	int k=1;
	/*������зǿ����Ӽ�*/
    for(int i=1;i<subset_count;i++)
    {
        bitset<20>  bitvec(k);         
        for(int j=0;j<pass;j++)
        {
            if(bitvec[j]==1)    t_str+=vec[j];   //�����ַ����γ�һ���Ӽ�
        }
        RealSubset.push_back(t_str);   
        t_str.erase();
        k++;
    }
	int count = 0;//���ڻ���
    for(int i=0;i<RealSubset.size();i++)
    {
       for(int j=0;j<RealSubset.size();j++)
       {
          if( i!=j && RealSubset[i].find(RealSubset[j])==string::npos 
			       && RealSubset[j].find(RealSubset[i])==string::npos)//�ж����Ӽ�֮���Ƿ�����ͬ��
          {
			  //�����Ŷȣ�I1I2I3�ĸ���/I1�ĸ���
             if((float)(items_count[RealSubset[i]+RealSubset[j]])/(float)(items_count[RealSubset[i]])>MinConf)   
             {
				 count++;
                 cout.setf(ios::fixed);
                 cout<<RealSubset[i]<<"=>"<<RealSubset[j]<<"<"<<setprecision(4)
					 <<(float)(items_count[RealSubset[i]+RealSubset[j]])/(float)(items_count[RealSubset[i]])<<"> ";
				 if(pass<=4 && count%3 == 0) cout<<endl;
				 else if(count%2==0) cout<<endl;
                 t_str="{" + RealSubset[i] + "}=>{" + RealSubset[j] + "} ���Ŷ�:";
                 sprintf(s,"%f",(float)(items_count[RealSubset[i]+RealSubset[j]])/(float)(items_count[RealSubset[i]]));//�����Ŷ�ת�����ַ���
                 string s2(s);
                 t_str += s2;//����
                 for(st=0;st<Confidence_Set.size();st++)      //����Ƿ��Ѿ��иù���������ȥ�ˣ��Է�ֹ�ظ�
                 {
                     if(Confidence_Set[st] == t_str) break;
                        
                 }
                 if(st==Confidence_Set.size())     //û�з����ظ��������������򼯺�
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
*  ����������
*/
void Output(std::ofstream &outfile,vector<string> Confidence_Set)
{
	     cout<<"�ó��Ĺ�����������:"<<endl;
         for(int st=0;st!=Confidence_Set.size();st++)
         {
             cout<<Confidence_Set[st]<<endl;
         }
         outfile<<"����Ĺ�����������:"<<endl;
         for(vector<string>::iterator it=Confidence_Set.begin();it!=Confidence_Set.end();it++)
         {
            outfile<<*it<<endl;
         }
         
}

/*
*  ��֦��������
*/
void Cut_infrequent_sub(vector<string> tempItem,int found )
{
	                    string temp_s;
	                	vector<string> tempvec;
                        string teststr;
                        int testint;
                        tempvec=tempItem;
                        sort(tempvec.begin(),tempvec.end());
                        while(next_permutation(tempvec.begin(),tempvec.end())) //�û�
                        {
                           for(int i=0;i!=tempvec.size();i++) temp_s+=tempvec[i]; //�Ѹ�������ӳ�һ���ַ���
                          
                           for(map<string,int>::const_iterator tempit=items_count.begin();tempit!=items_count.end();tempit++)
                           {
                                if(tempit->second < MinSupport)  //С��֧�ֶ�
                                {
                                    if(temp_s.find(tempit->first)!=string::npos)   //��ʾ�����з�Ƶ�����
                                   {
                                       found=1;
                                       teststr=tempit->first;
                                       testint=tempit->second;
                                       break;
                                   }
                                }
                           }
                           temp_s.erase();
                           if(found)   break;//������Ƶ�����                       
                        }//end_while
                        if(!found)     //ֻ�в������з�Ƶ������ż����ѡ��У������֦��
                           Candidate_Set.push_back(tempItem);
                        else  //����apriori����ɾ�����з�Ƶ���Ӽ�
                        {
                           cout<<"��֦:";
                           for(int st2=0;st2!=tempItem.size();st2++)
                               cout<<tempItem[st2];
                           //cout<<" ���з�Ƶ�����:"<<teststr<<" "<<testint<<"/"<<All_items_count<<"="<<((float)(testint)/(float)All_items_count);
                          cout<<"  ���з�Ƶ�����:"<<teststr<<" ���ִ�����"<<testint<<"��������С֧�ֶ�";
                           cout<<endl;
                        }
                        found=0;   
}

/*
*  ����Ƶ��1�����
*/
void Gen_Single_Set(vector<vector<string> > Data_Set)
{
	map<string,int> item_map; //��¼���Ƿ���������ڣ�1Ϊ���ڣ�0Ϊ������ 
     for(int i=0;i!=Data_Set.size();++i)
     {
        for(int j=0;j!=Data_Set[i].size();++j)
        {
         items_count[Data_Set[i].at(j)]++;    //����ļ�����1
         item_map[Data_Set[i].at(j)]=1;      
        }
        bitmap.push_back(item_map);
        item_map.clear();     
     }
     
     cout<<"��ѡ1�:"<<endl;
	 cout<<"-----------------------------------------------"<<endl;
	 cout<<setiosflags(ios::left)<<setw(18)<<"�"<<"֧�ֶȼ���"<<endl;
	 cout<<"-----------------------------------------------"<<endl;
	 map<string,int>::const_iterator it=items_count.begin();
     while(it!=items_count.end())      //�����ѡ1�
     {		
		cout<<setiosflags(ios::left)<<setw(18)<<it->first<<items_count[it->first]<<endl;
        it++;
     }
	 cout<<"-----------------------------------------------"<<endl;
	 cout<<"��Enter��������";  //pause
     getchar();
	 it=items_count.begin();
     cout<<"Ƶ��1�:"<<endl;
	 cout<<"-----------------------------------------------"<<endl;
	 cout<<setiosflags(ios::left)<<setw(15)<<"�"<<"֧�ֶ�"<<endl;
	 cout<<"-----------------------------------------------"<<endl;
	 int count=0;
     while(it!=items_count.end())          //Ƶ��1�
     {
        if( it->second >= MinSupport)    //���ڵ���֧�ֶ�
        {
			count++;           
       	    cout<<setiosflags(ios::left)<<setw(15)<<it->first<<it->second<<endl;
            item.push_back(it->first);
            Frequent_Set.push_back(item);   //�����ѡ1���vector��
            item.clear();    
        }
        it++;
     }
	 cout<<"-----------------------------------------------"<<endl;
	 cout<<"һ���� "<<count<<" ��Ƶ��1�"<<endl;
}

/*
*  ����Ƶ���������
*/
void Gen_Multi_Set()
{
	int found;    //��¼�Ƿ������Ƶ���
	 if(!Frequent_Set.empty())   //�ж�Ƶ��1��Ƿ�Ϊ�գ�Ϊ�����˳�
     {
		 int pass=1;
         cout<<"��Enter��������"; 
         getchar();                  
         while(!Frequent_Set.empty())
         {
            //������һ�ֵĺ�ѡ�
			Candidate_Set.clear();        
            int st=Frequent_Set.size();
            for(int st1=0;st1<st;st1++)
            {
                for(int st2=st1+1;st2<st;st2++)
                {
                    found=0;
                    item=Gen_Next_Candidate(Frequent_Set[st1],Frequent_Set[st2],pass);    //���ú����ϲ�������һ�ֵĺ�ѡ�
                    if(!item.empty()&&!isExist(item,Candidate_Set))   //�������жϴ���󷵻ص�vector��Ϊ���һ������ڸ��������Ϊ��ѡ������ѡvector��
                    {
                        Cut_infrequent_sub( item,found );//����費��Ҫ���м�֦
                    }
                    
                }
            }
            Frequent_Set.clear();        
            pass++;
            cout<<endl<<"��ѡ"<<pass<<"�:"<<endl;
            for(int ix=0;ix!=Candidate_Set.size();++ix)      //�����ѡ�
            {
               cout<<"{";
               for(int iy=0;iy!=Candidate_Set[ix].size();++iy)
               {
                cout<<Candidate_Set[ix].at(iy);
               }
               cout<<"}"<<endl;
            }
            if(Candidate_Set.empty())   cout<<"��ѡ"<<pass<<"�Ϊ��!"<<endl;            
            int flag;    //�ж�ĳ�����ĳ���������Ƿ���֣�����Ϊ1������Ϊ0
            int count;    //ͳ��ĳ��������������г��ֵĴ���
            string tempstr;  //���ڰѸ��������ַ��� 
            int check;   //��⴮����Ĵ����Ƿ�Ϊ1 
            for(int sx=0;sx!=Candidate_Set.size();++sx)      
            {
                check=1;
                count=0;
                for(int sy=0;sy!=bitmap.size();++sy)
                {
                    flag=1;       //��ʼ��Ϊ1����ʾ����
                    for(int sz=0;sz!=Candidate_Set[sx].size();++sz)
                    {
                       if(bitmap[sy][Candidate_Set[sx].at(sz)]==0)   //����ĳһ��������ڣ���û�����
                       {
                           flag=0;
                       }
                       if(check==1)  //ֻ����һ��
                       {
                           tempstr+=Candidate_Set[sx].at(sz);  //�����ַ���
                       }
                    }
                    
                    if(flag)  //��ʾ�г���
                    {
                       count++;
                    }
                    check++;
                }//end_for
                
                if(count>=MinSupport)    Frequent_Set.push_back(Candidate_Set[sx]);        //count������С֧�ֶȣ���ʾ��Ƶ���
               
                items_count[tempstr]=count;       //��Ӧ����ļ���ֵ
               
                sort(Candidate_Set[sx].begin(),Candidate_Set[sx].end());   
                string tempstr2;
                while(next_permutation(Candidate_Set[sx].begin(),Candidate_Set[sx].end()))  //����ȫ����
                {
                     for(int tempst=0;tempst!=Candidate_Set[sx].size();tempst++) //ƴ�ӳ����ַ������
                     {
                         tempstr2+=Candidate_Set[sx][tempst];
                     }
                     items_count[tempstr2]=count;  //��Ӧ����ļ���ֵ
                     tempstr2.erase();
                }               
                tempstr.erase();
            }
            cout<<"��Enter��������"; 
            getchar();
            if(!Frequent_Set.empty())     //Ƶ�����Ϊ��
            {
				
                 cout<<"Ƶ��"<<pass<<"�:"<<endl;
				 cout<<"-------------------------------------------------------------------"<<endl;
                 for(int i=0;i!=Frequent_Set.size();++i)      //���Ƶ���
                 {
                     for(int j=0;j!=Frequent_Set[i].size();++j)
                     {
   						 if(j==0) cout<<Frequent_Set[i].at(j);
						 else cout<<"/"<<Frequent_Set[i].at(j);
                        tempstr+=Frequent_Set[i].at(j);  //�����ַ���
                     }
                     cout<<endl;
					 cout<<"֧�ֶȼ����� "<<items_count[tempstr]<<endl;
					 cout<<"���Ŷ�����:  "<<endl;
                     computeConfidence(Frequent_Set[i],pass);
                     cout<<endl;
                     tempstr.erase();
                 }//end_for
				 cout<<"һ���� "<<Frequent_Set.size()<<" ��Ƶ�� "<<pass<<" �"<<endl;
                 cout<<"��Enter��������"; 
                 getchar();
             }
             else
             {
                 cout<<"û��"<<pass<<"-Ƶ���"<<endl;
				 cout<<"�������н�����"<<endl;
             }
         }//end_while   
		 Output(outfile,Confidence_Set);
     }
     else
     {  

        //do nothing
     }    //end of if(!Frequent_Set.empty())
}


