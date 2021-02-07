#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define SIZE 997

int gen_counter=0;

struct dict{
	char kelime[50];//kelime
	char adres[10][50];//kelimenin bulunduðu dosyalar
	int count;//kaç dosyada bulunduðu
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
	while(strcmp(hmap[adr].kelime,"Nan__Nan")!=0 && i<SIZE){//adres doluysa tekrar hashlenir (double hash kurallarýna göre)
		i++;
		adr=(hash1+i*hash2)%SIZE;
	}
	if(i<SIZE){//boþ yer bulunduysa o adrese konur
		hmap[adr]=aday;
		gen_counter++;
	}
	else{
		printf("Kelime eklenemedi (%s)",aday.kelime);//while'dan çýkýnca i=SIZE ise map doludur ve kelime eklenemez
	}
}

void read_from_hmap(struct dict *hmap){
	FILE *fp;
	fp=fopen("17011039.txt","r");//hashdoc(17011039.txt)den kelimeler sýrayla alýnýr
	int flag=1;
	struct dict aday;
	aday.count=0;
	char *temp;
	temp=(char*)malloc(sizeof(temp)*50);
	while(!feof(fp)){
		fscanf(fp,"%s",temp);
		if(strcmp(temp,"||")==0){//Benim dokuman formatýmda en sona || iþareti konuyor. sebebi ise fscanf fonksiyonunun son kelimeyi 2 kez almasý
			fclose(fp);
			return;
		}
		else if(strcmp(temp,"&&end&&")==0){//kelimenin adreslerinin alýmýnýn bittiðini gösteren  bitiþ string'i
			flag=1;
			put_on_hashmap_doubleh(aday,hmap);//kelie hmap'e konur
			aday.count=0;//yeni aday için count 0 yapýlýr
		}
		else if(flag==1){//flag=1 olmasý &&end&& stringinden hemen sonra okunduðunu gösterir ve yeni kelime geleceðini belirtir
			strcpy(aday.kelime,temp);
			flag=0;
		}
		else if(flag==0){
			strcpy(aday.adres[aday.count++],temp);//flag=0 ise adres alýmý yapýlýr
		}
		
	}
	fclose(fp);
}



void print_hmap(struct dict *hmap){//Tüm hmap'in bastýrýlmasý için fonksiyon
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

void put_on_hmap_withpath(char *path,struct dict *hmap,struct dict aday){//farklý bir dokumandan ekleme yapýlýrsa çalýþan fonksiyon
	int i=0;
	int key=horners_hash(aday);
	int hash2=hashfunc_2(key);
	int hash1=hashfunc_1(key);
	int adr=(hash1+i*hash2)%SIZE;
	while(i<SIZE){
		if(strcmp(hmap[adr].kelime,aday.kelime)==0){//hashmap'in adresindeki kelime ile alýnan kelime aynýysa hashmap'teki kelimenin count deðeri 1 artýrýlýp kelimenin alýndýðý dosyanýn adý adreslerine eklenir 
			strcpy(hmap[adr].adres[hmap[adr].count++],path);
			return;
		}
		else if(strcmp(hmap[adr].kelime,"Nan__Nan")==0){//adres eðer boþsa o kelime daha önce alýnmamýþtýr. Kelime hashmap'e eklenir
			strcpy(aday.adres[aday.count++],path);
			hmap[adr]=aday;
			gen_counter++;
			return;
		}
		else{//adres dolu ve kelimeyle ayný deðilse tekrar hashlenir
			i++;
			adr=(hash1+i*hash2)%SIZE;;
		}
	}
		if(i==SIZE){//i=SIZE ise 997 kere hash fonksiyonu çalýþtýrýlmýþtýr ve boþ yer bulunamamýþtýr. Boþ yer yoksa kelime eklenemez.
			printf("\nKelime=%s eklenemedi (MAP DOLU) \n",aday.kelime);
		}	

}

void read_from_docs(char *path,struct dict *hmap){//Path deðeri alýnýp o dosya açýlýr ve kelimeler teker teker okunur
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

void rewrite_hash(struct dict* hmap){//her yeni dosya okunduktan sonra hashdoc(17011039.txt) silinir ve programdaki güncel hashmap yazýlýr
	FILE *fp;
	fp=fopen("17011039.txt","w");
	int i;
	int j;
	for(i=0;i<SIZE;i++){
		if(strcmp(hmap[i].kelime,"Nan__Nan")!=0){//Programýn içindeki tabloda eðer bir kelime Nan__Nan stringine eþitse o bölmede bilgi yoktur
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
	for(j=0;j<strlen(temp);j++){//harflerin hepsi küçültülür
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
		if(strcmp(hmap[adr].kelime,"Nan__Nan")==0){//adresteki kelime Nan__Nan ise kelime adrese konmamýþtýr
			printf("------------------------------------------\n");
			printf("Bu kelime tabloda gecmiyor\n ");
			printf("%d arama denemesi yapildi\n",i+1);
			return;
		}
		else if(strcmp(hmap[adr].kelime,aday.kelime)==0){//kelime adrestekiyle aynýysa bastýrýlýr ve arama sayýsý yazdýrýlýr
			printf("------------------------------------------\n");
			printf("Kelime tabloda geciyor\n");
			printf("%d aramada bulundu\n",i+1);
			for(j=0;j<hmap[adr].count;j++)
			printf("%s ",hmap[adr].adres[j]);
			printf("Dosyalarinda geciyor\n");
			printf("Bu kelime bu dosyalarda toplam %d kere geciyor\n",hmap[adr].count);
			return;
		}
		else{//bölme boþ deðil ve kelimeyle ayný deðilse tekrar hashlenir
			i++;
			adr=(hash1+i*hash2)%SIZE;
		}
		if(i==SIZE){//i=SIZE olduysa tüm map gezilmiþ fakat kelime bulunamamýþtýr
			printf("------------------------------------------\n");
			printf("Kelime=(%s) tabloda bulunmuyor\n",aday.kelime);
			printf("%d arama denemesi yapildi\n",i);
		}
	}
}


int main(){
	struct dict *hmap;
	hmap=(struct dict*)malloc(sizeof(struct dict)*SIZE);//map array olarak açýldý
	int i;
	double LoadFactor;
	char temp[50];
	char flag='1';
	char caser;
	for(i=0;i<SIZE;i++)
		strcpy(hmap[i].kelime,"Nan__Nan");//Null veremediðimden boþ bölmeleri bu stringle doldurdum
	read_from_hmap(hmap);
	while(flag=='1'){
	printf("Bir dosyadan okumak icin 'a',\nHashmap'in su anki halini gormek icin 'h',\nLoadFactor degerini gormek icin 'l',\nArama yapmak icin 's',\nCikmak icin 'q' tusuna basiniz:\n");
	//scanf(" %c",&caser); 
	caser=getch();//DÝKKAT BAZEN getch() fonksiyonu bazý bilgisayarlarda çalýþmýyor üstteki scanf fonksiyonunu kullanýrsanýz yardýmcý olur
	printf("------------------------------------------\n");
		if(caser=='a'){//dosyadan kelime alma kýsmý
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
		else if(caser=='s'){//arama kýsmý
				printf("Aranmak istenen kelimeyi giriniz:");
				scanf("%s",temp);
				printf("------------------------------------------\n");
				search_hmap(temp,hmap);
		}
		else if(caser=='h'){//hashmap'in güncel deðerlerini yazdýran kýsým
			printf("------------------------------------------\n");
			printf("Hashmap:\n");
			print_hmap(hmap);
		}
		else if(caser=='q'){//çýkýþ
				flag='0';
		}
		else if(caser=='l'){//loadfactor deðerini yazdýran kýsým
			printf("LoadFactor=%1.3f\n",(float)gen_counter/SIZE);
		}
		else
			printf("Yanlis tus girdiniz. Tekrar deneyin\n");//default case
			printf("------------------------------------------\n");
	}
	return 0;
}


