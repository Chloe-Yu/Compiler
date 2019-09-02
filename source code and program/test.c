/* I'm blahblahblahing*/
//again

int a,b,c;
float m,n;
int fibo(int a){
if(a==1 || a==2) return 1;
return fibo(a-1)+fibo(a-2);
}
int main(){
	int m,n,i;
	char d;
	m=read();
	i=1;
	++i;
	d='x';
	i+=2;
	while(i<=m)
	{
		n=fibo(i);
		write(n);
		i=i+1;
	}
	return 1;
}

