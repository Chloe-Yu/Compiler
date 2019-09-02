/* I'm blahblahblahing*/
//again
int a,b,c;
float m,n;

float b;//变量重复定义
int fibo(int a){
if(a==1 || a==2) return 1;
return fibo(a-1)+fibo(a-2);
}
int func2(int b,char c,float d)
{

    return 1;
}
int func1(int a,char a)//参数名重复定义
{
    float b=1.2;
    return b;//返回类型错误
}
int func1()//函数重复定义
{
    f=1;//
    return 1;
}
int main(){
	int m,n,i;
	char d;
	float e;
	m=read();
	!(i==0);//

    m();//不是一个函数
	i=1;
	1+=1;//
	++i;
	d='x';
	-d;//字符型数据不能为负值
	++d;//
	++1;//
	i+=2;
	i=-i;
	i=d;//
	1=2;//
	-1;//
	 fibo(1,2); //函数参数数目错误
	 fibo(1.0); //函数调用参数类型不对
	fun1=1;//
	func1=1;//
	i=d+e;//
	func2(1);
	while(i<=m)
	{
		n=fibo(i);
		write(n);
		i=i+1;
	}

	return 1;
}
