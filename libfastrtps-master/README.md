
## Fast-DDS
https://www.eprosima.com/

eprosima Fast DDS (formerly Fast RTPS) is a C++ implementation of the DDS (Data Distribution Service) standard of the OMG (Object Management Group). eProsima Fast DDS implements the RTPS (Real Time Publish Subscribe) protocol, which provides publisher-subscriber communications over unreliable transports such as UDP, as defined and maintained by the Object Management Group (OMG) consortium. RTPS is also the wire interoperability protocol defined for the Data Distribution Service (DDS) standard. eProsima Fast DDS expose an API to access directly the RTPS protocol, giving the user full access to the protocol internals.

Download: `https://github.com/eProsima/Fast-DDS.git`

Commit: 9b9da88f0cd0b7884b0325747b7ac5b16bebeec4

Tag: v2.5.1

Version: v2.5.1

## Fast-DDS APP 工程配置
1、创建 C++ 工程；

2、修改 xxx.mk，添加需要编译的文件；

3、修改 mk 配置：
```
C++ Flags ：-std=c++11 -fexceptions -frtti
Macro :  _GLIBCXX_USE_C99
```
4、添加头文件搜索路径
```
-I"$(WORKSPACE_libfastrtps)/src/Fast-CDR/include"
-I"$(WORKSPACE_libfastrtps)/src/Fast-DDS/include"
-I"$(WORKSPACE_libfastrtps)/src/Fast-CDR/build/include"
-I"$(WORKSPACE_libfastrtps)/src/Fast-DDS/build/include"
```

5、添加库与库文件搜索路径
```
-lfastrtps 
-lfastcdr
-lfastrtps_pre
-L"$(WORKSPACE_libfastrtps)/$(Output)/strip"
```

6、如果需要打开 Log 则在应用程序中添加如下语句
```
#include <fastrtps/log/Log.h>
 Log::SetVerbosity(Log::Info);
```

7、运行需要加载 xsiipc.ko