#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define SIZE 997

int gen_counter=0;

struct dict{
	char kelime[50];//kelime
	char adres[10][50];//kelimenin bulundu�u dosyalar
	int count;//ka� dosyada bulundu�u
};

int horners_hash(struct dict aday){//horner metodu
	int toplam=0;
	int i;

	for(i=0;i<strlen(aday.kelime);i++){
		if(aday.kelime[i]<'a')
			aday.kelime[i]=aday.kelime[i]+32;}

	for(i=0;i<strlen(aday.kelime);i++){
		toplam+=aday.kelime[i]*pow(3,i);
	}

	return (toplam);
}

int hashfunc_2(int key){
	return (1+(key%(SIZE-1)));
}

int hashfunc_1(int key){
	return (key%SIZE);
}

void put_on_hashmap_doubleh(struct dict aday,struct dict *hmap){//hashdoc'ta (17011039.txt) tutulan kelimeleri programdaki tabloya alma fonksiyonu
	int i=0;
	int key=horners_hash(aday);
	int hash2=hashfunc_2(key);
	int hash1=hashfunc_1(key);
	int adr=(hash1+i*hash2)%SIZE;
	while(strcmp(hmap[adr].kelime,"Nan__Nan")!=0 && i<SIZE){//adres doluysa tekrar hashlenir (double hash kurallar�na g�re)
		i++;
		adr=(hash1+i*hash2)%SIZE;
	}
	if(i<SIZE){//bo� yer bulunduysa o adrese konur
		hmap[adr]=aday;
		gen_counter++;
	}
	else{
		printf("Kelime eklenemedi (%s)",aday.kelime);//while'dan ��k�nca i=SIZE ise map doludur ve kelime eklenemez
	}
}

void read_from_hmap(struct dict *hmap){
	FILE *fp;
	fp=fopen("17011039.txt","r");//hashdoc(17011039.txt)den kelimeler s�rayla al�n�r
	int flag=1;
	struct dict aday;
	aday.count=0;
	char *temp;
	temp=(char*)malloc(sizeof(temp)*50);
	while(!feof(fp)){
		fscanf(fp,"%s",temp);
		if(strcmp(temp,"||")==0){//Benim dokuman format�mda en sona || i�areti konuyor. sebebi ise fscanf fonksiyonunun son kelimeyi 2 kez almas�
			fclose(fp);
			return;
		}
		else if(strcmp(temp,"&&end&&")==0){//kelimenin adreslerinin al�m�n�n bitti�ini g�steren  biti� string'i
			flag=1;
			put_on_hashmap_doubleh(aday,hmap);//kelie hmap'e konur
			aday.count=0;//yeni aday i�in count 0 yap�l�r
		}
		else if(flag==1){//flag=1 olmas� &&end&& stringinden hemen sonra okundu�unu g�sterir ve yeni kelime gelece�ini belirtir
			strcpy(aday.kelime,temp);
			flag=0;
		}
		else if(flag==0){
			strcpy(aday.adres[aday.count++],temp);//flag=0 ise adres al�m� yap�l�r
		}
		
	}
	fclose(fp);
}



void print_hmap(struct dict *hmap){//T�m hmap'in bast�r�lmas� i�in fonksiyon
	int i;
	int j=0;
	for(i=0;i<SIZE;i++){
		if(strcmp(hmap[i].kelime,"Nan__Nan")==0)
			printf("No data in segment(%d)",i);
		else{
			printf("%s",hmap[i].kelime);
			for(j=0;j<hmap[i].count;j++)
				printf(" %s",hmap[i].adres[j]);
		}
		printf("\n");
	}
}

void put_on_hmap_withpath(char *path,struct dict *hmap,struct dict aday){//farkl� bir dokumandan ekleme yap�l�rsa �al��an fonksiyon
	int i=0;
	int key=horners_hash(aday);
	int hash2=hashfunc_2(key);
	int hash1=hashfunc_1(key);
	int adr=(hash1+i*hash2)%SIZE;
	while(i<SIZE){
		if(strcmp(hmap[adr].kelime,aday.kelime)==0){//hashmap'in adresindeki kelime ile al�nan kelime ayn�ysa hashmap'teki kelimenin count de�eri 1 art�r�l�p kelimenin al�nd��� dosyan�n ad� adreslerine eklenir 
			strcpy(hmap[adr].adres[hmap[adr].count++],path);
			return;
		}
		else if(strcmp(hmap[adr].kelime,"Nan__Nan")==0){//adres e�er bo�sa o kelime daha �nce al�nmam��t�r. Kelime hashmap'e eklenir
			strcpy(aday.adres[aday.count++],path);
			hmap[adr]=aday;
			gen_counter++;
			return;
		}
		else{//adres dolu ve kelimeyle ayn� de�ilse tekrar hashlenir
			i++;
			adr=(hash1+i*hash2)%SIZE;;
		}
	}
		if(i==SIZE){//i=SIZE ise 997 kere hash fonksiyonu �al��t�r�lm��t�r ve bo� yer bulunamam��t�r. Bo� yer yoksa kelime eklenemez.
			printf("\nKelime=%s eklenemedi (MAP DOLU) \n",aday.kelime);
		}	

}

void read_from_docs(char *path,struct dict *hmap){//Path de�eri al�n�p o dosya a��l�r ve kelimeler teker teker okunur
	FILE *fp;
	fp=fopen(path,"r");
	struct dict aday;
	while(!feof(fp)){
		aday.count=0;
		fscanf(fp,"%s",aday.kelime);
		put_on_hmap_withpath(path,hmap,aday);
	}
	fclose(fp);
}

void rewrite_hash(struct dict* hmap){//her yeni dosya okunduktan sonra hashdoc(17011039.txt) silinir ve programdaki g�ncel hashmap yaz�l�r
	FILE *fp;
	fp=fopen("17011039.txt","w");
	int i;
	int j;
	for(i=0;i<SIZE;i++){
		if(strcmp(hmap[i].kelime,"Nan__Nan")!=0){//Program�n i�indeki tabloda e�er bir kelime Nan__Nan stringine e�itse o b�lmede bilgi yoktur
			fprintf(fp,"%s ",hmap[i].kelime);
			for(j=0;j<hmap[i].count;j++)
			fprintf(fp,"%s ",hmap[i].adres[j]);
			fprintf(fp,"%s","&&end&&\n");//adres bitirme string'i
		}
	}
	fprintf(fp,"%s","||");
	fclose(fp);
}
void search_hmap(char *temp,struct dict *hmap){//hashmap'te arama fonksiyonu
	int i=0;
	int j;
	for(j=0;j<strlen(temp);j++){//harflerin hepsi k���lt�l�r
		if(temp[j]<'a')
			temp[j]=temp[j]+32;
	}
	struct dict aday;
	strcpy(aday.kelime,temp);
	int key=horners_hash(aday);
	int hash2=hashfunc_2(key);
	int hash1=hashfunc_1(key);
	int adr=(hash1+i*hash2)%SIZE;
	while(i<SIZE){
		if(strcmp(hmap[adr].kelime,"Nan__Nan")==0){//adresteki kelime Nan__Nan ise kelime adrese konmam��t�r
			printf("------------------------------------------\n");
			printf("Bu kelime tabloda gecmiyor\n ");
			printf("%d arama denemesi yapildi\n",i+1);
			return;
		}
		else if(strcmp(hmap[adr].kelime,aday.kelime)==0){//kelime adrestekiyle ayn�ysa bast�r�l�r ve arama say�s� yazd�r�l�r
			printf("------------------------------------------\n");
			printf("Kelime tabloda geciyor\n");
			printf("%d aramada bulundu\n",i+1);
			for(j=0;j<hmap[adr].count;j++)
			printf("%s ",hmap[adr].adres[j]);
			printf("Dosyalarinda geciyor\n");
			printf("Bu kelime bu dosyalarda toplam %d kere geciyor\n",hmap[adr].count);
			return;
		}
		else{//b�lme bo� de�il ve kelimeyle ayn� de�ilse tekrar hashlenir
			i++;
			adr=(hash1+i*hash2)%SIZE;
		}
		if(i==SIZE){//i=SIZE olduysa t�m map gezilmi� fakat kelime bulunamam��t�r
			printf("------------------------------------------\n");
			printf("Kelime=(%s) tabloda bulunmuyor\n",aday.kelime);
			printf("%d arama denemesi yapildi\n",i);
		}
	}
}


int main(){
	struct dict *hmap;
	hmap=(struct dict*)malloc(sizeof(struct dict)*SIZE);//map array olarak a��ld�
	int i;
	double LoadFactor;
	char temp[50];
	char flag='1';
	char caser;
	for(i=0;i<SIZE;i++)
		strcpy(hmap[i].kelime,"Nan__Nan");//Null veremedi�imden bo� b�lmeleri bu stringle doldurdum
	read_from_hmap(hmap);
	while(flag=='1'){
	printf("Bir dosyadan okumak icin 'a',\nHashmap'in su anki halini gormek icin 'h',\nLoadFactor degerini gormek icin 'l',\nArama yapmak icin 's',\nCikmak icin 'q' tusuna basiniz:\n");
	//scanf(" %c",&caser); 
	caser=getch();//D�KKAT BAZEN getch() fonksiyonu baz� bilgisayarlarda �al��m�yor �stteki scanf fonksiyonunu kullan�rsan�z yard�mc� olur
	printf("------------------------------------------\n");
		if(caser=='a'){//dosyadan kelime alma k�sm�
				if(((float)gen_counter/SIZE)>0.8)
				printf("\n!!!Dikkat LoadFactor degeri 0.8'in ustunde!!!\n");
				printf("Okunacak dosyanin PATH giriniz:\n");
				char path[20];
				scanf("%s",path);
				printf("------------------------------------------\n");
				read_from_docs(path,hmap);
				printf("Kelimeler %s dosyasindan basariyla eklendi\n",path);
				rewrite_hash(hmap);
			}
		else if(caser=='s'){//arama k�sm�
				printf("Aranmak istenen kelimeyi giriniz:");
				scanf("%s",temp);
				printf("------------------------------------------\n");
				search_hmap(temp,hmap);
		}
		else if(caser=='h'){//hashmap'in g�ncel de�erlerini yazd�ran k�s�m
			printf("------------------------------------------\n");
			printf("Hashmap:\n");
			print_hmap(hmap);
		}
		else if(caser=='q'){//��k��
				flag='0';
		}
		else if(caser=='l'){//loadfactor de�erini yazd�ran k�s�m
			printf("LoadFactor=%1.3f\n",(float)gen_counter/SIZE);
		}
		else
			printf("Yanlis tus girdiniz. Tekrar deneyin\n");//default case
			printf("------------------------------------------\n");
	}
	return 0;
}


