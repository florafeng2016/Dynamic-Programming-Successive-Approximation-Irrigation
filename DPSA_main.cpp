//  main.cpp
//  Dynamic Programming for Hydrology
//
//  Created by FengFlora on 2018/8/24.
//  Copyright © 2018年 FengFlora. All rights reserved.
//

#include <iostream>
#include <cmath>
using namespace std;

int U,k,water,twwi;
int i,n[20];
double v0,ita,rmax;
double ym[20],a[20],p[20],c[20],s0[20],x[20][20],pe[20][20],etm[20][20],smin[20],smax[20];
double ff[20][100000],bestallo[20][100000][20];
int crop,q0,qi;

struct gmax
{
  double maximum = 0;
  int cropk[20] = {0};
}gg,gtry[20][100000],ggmax;

struct ratio
{
  double ff = 0,ww[20],s;
}r[20][100000],ttemp;

void fmmax()
{
  int stage,w,mi;
  for(w=0;w<=water;w++)
  {
    r[0][w].ff = 1;
    r[0][w].s = s0[k];
  }
  for(stage = 1;stage<=n[k];stage++)
  {
    for(w = 0;w<=water;w++)
    { r[stage][w].ff=0;}
  }
  
  for(stage = 1;stage<=n[k];stage++)
  {
    for(w = 0;w<=water;w++)
    {
      ttemp.ff = 0;
      for(mi = 0;mi<=w;mi++)
      {
        r[stage][w] = r[stage-1][w-mi];
        r[stage][w].ww[stage]=mi;
        if((pe[k][stage]+r[stage-1][w-mi].s+double((mi/a[k])*1000)-smin[k])<etm[k][stage])
        {
          r[stage][w].ff = r[stage-1][w-mi].ff*pow((pe[k][stage]+r[stage-1][w-mi].s+double((mi/a[k])*1000)-smin[k])/etm[k][stage],x[k][stage]);
          r[stage][w].s = smin[k];
        }
        else
        {
          r[stage][w].ff = r[stage-1][w-mi].ff;
          r[stage][w].s = pe[k][stage]+r[stage-1][w-mi].s+double((mi/a[k])*1000)-etm[k][stage];
          ttemp=r[stage][w];
          break;
        }
        if(ttemp.ff<r[stage][w].ff) ttemp=r[stage][w];
      }
      r[stage][w] = ttemp;
    }
  }
  int allstage = n[k];
  for(w = 0;w<=water;w++)
  {
    if(rmax<r[allstage][w].ff)
    {
      rmax = r[allstage][w].ff;
      for(stage = 1;stage<=n[k];stage++)
        bestallo[k][water][stage] = r[allstage][w].ww[stage];
    }
  }
}

int main(){
  printf("请输入下列参数.\n");
  printf("作物种类数量和总供水量(万m^3): ");
  cin>>U>>v0;
  printf("灌溉水利用系数: ");
  cin>>ita;
  for(k=1;k<=U;k++)
  {
    printf("(%d) 作物 %d 的基本参数.\n",k,k);
    printf("作物 %d 生育期数: ",k);
    cin>>n[k];
    printf("作物 %d 潜在产量(Ym)(kg/hm^2): ",k);
    cin>>ym[k];
    printf("作物 %d 种植面积(A)(hm^2): ",k);
    cin>>a[k];
    printf("作物 %d 单价(P)(yuan/kg): ",k);
    cin>>p[k];
    printf("作物 %d 生产费用(C)(yuan/hm^2): ",k);
    cin>>c[k];
    printf("作物 %d 播种时的土壤含水量(S0)(mm): ",k);
    cin>>s0[k];
    printf("作物 %d 土壤含水量下限(Smin)(mm): ",k);
    cin>>smin[k];
    printf("作物 %d 土壤含水量上限(Smin)(mm): ",k);
    cin>>smax[k];
    for(i=1;i<=n[k];i++)
    {
      printf("(%d.%d) 作物 %d 生育阶段 %d 的基本系数.\n",k,i,k,i);
      printf("敏感指数(lamda%d%d): ",k,i);
      cin>>x[k][i];
      printf("有效降水量(Pe%d%d)(mm): ",k,i);
      cin>>pe[k][i];
      printf("潜在腾发量(ETm%d%d)(mm): ",k,i);
      cin>>etm[k][i];
    }
  }
  
  /* Model 1 */
  
  for(k=1;k<=U;k++)
  {
    for(water=0;water<=int(ita*v0*10);water++)
    {
      rmax = 0;
      fmmax();
      ff[k][water] = rmax;
    }
  }
  
  /* Model 2 */
  
  q0 = int(ita*v0*10);
  for(k=1;k<=U;k++)
  {
    for(water=0;water<=int(ita*v0*10);water++)
    {
      gg.maximum = 0;
      for(qi = 0;qi<=water;qi++)
      {
        gtry[k][water] = gtry[k-1][water-qi];
        gtry[k][water].cropk[k] = qi;
        gtry[k][water].maximum = gtry[k-1][water-qi].maximum + ff[k][qi]*a[k]*p[k]*ym[k]-a[k]*c[k];
        if(gg.maximum<gtry[k][water].maximum) gg = gtry[k][water];
      }
      gtry[k][water] = gg;
    }
  }
  
  for(water = 0;water<=q0;water++)
  {
    if(ggmax.maximum<gtry[U][water].maximum) ggmax = gtry[U][water];
  }
  
  /* Output */
  
  cout<<"优化结果如下."<<endl;
  printf("各种作物净效益之和最大值(Gmax)为 ");
  cout<<ggmax.maximum<<" (元)."<<endl;
  for(crop=1;crop<=U;crop++)
  {
    printf("(%d) 作物 %d 总灌水量为 ",crop,crop);
    cout<<double(ggmax.cropk[crop])/10<<" (万m^3)."<<endl;
    printf("作物 %d 最大相对产量(fmax%d)为 ",crop,crop);
    twwi = ggmax.cropk[crop];
    cout<<ff[crop][twwi]<<"."<<endl;
    for(i=1;i<=n[crop];i++)
    {
      printf("作物 %d 在第 %d 生育期的灌水量为 ",crop,i);
      cout<<double(bestallo[crop][twwi][i])/10<<" (万m^3)."<<endl;
    }
  }
  return 0;
}
