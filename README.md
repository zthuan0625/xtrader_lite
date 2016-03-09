xTrader by t0trader

a win32 MFC/C++ project based on CTP API

(Shanghai Futures Information Technology)

基于上期技术综合交易平台的MFC/C++程序。

金融大时代迎面而来，特开放以便于修复各种小bugs，在期货市场共进步。

◆本软件特色:

MFC/C++开发,基于CTP平台接口,可交易国内四大期货交易所品种.

加入股票/期权接口,同样可继续扩充.

所有功能请参考bin64目录的readme.htm


◆源码编译:

Vc6.0 sp6 + M$ SDK2003 以上均可,默认UNICODE,

ansi版需要做大幅修改,不建议.

brokers目录xml均为ansi GBK编码,其它配置为utf-8

(已配置好VC6/vs2015 x86/x64工程)


brokers_real为实盘期商列表目录.

编译时开启global.h 里的 #define _REAL_CTP_

可以编译出完整支持实盘的程序

◆意见和建议:

t0trader@qq.com

◆在此致谢:

上期技术CTP开放平台

http://www.sfit.com.cn/5_2_DocumentDown.htm

	