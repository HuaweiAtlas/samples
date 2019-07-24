# RTPDemo

## 简介

主要用于演示如何从Atlas 500拉流和拉到流后如何进行解析。

### 功能特性
主体功能为：将从Atlas 500获取的TCP包拼接为一个完整的帧

辅助功能有：
1. 创建一个TCP服务，并在某个端口进行监听，用于后续交互后进行流的接收
2. 通过智能小站的restful API，通知小站将视频流推到上述的端口
3. 收到TCP流后，将其放入缓存中，然后通过本demo中RTP的CheckPkg寻找RTP的起始与长度，如果符合要求，则调用InsertPkg函数。

## 支持的产品

Atlas 500

## 依赖

如上所述，本demo分为2部分：

1. python脚本，用于通过restful接口完成拉流请求。其需要依赖requests包
2. C++11代码部分，该部分没有依赖


## 编译

将python脚本部署到合适的位置(与RtpDemo.cpp中定义的变量路径相符)
```
constexpr char CALLBACK_PYTHON_FILE[] = "/home/mss/python/pull_stream.py";
```
,此时，需要将拉流的python脚本部署到/home/mss/python/pull_stream.py的位置。
部署完成后，可以先尝试下脚本能否正常执行，执行命令:
python3 pull_stream.py 0000100 127.0.0.1 8888, 如果需要对这个脚本进行定制修改，可以修改pull_stream.py中的pull_stream函数，如下：

```python
def pull_stream(parameter):
    try:
        if len(parameter) != 4:
            sys.stdout.write("pull stream parameter num error" + '\n')
            exit(1)

        channel_id = parameter[1]
        serverIp = parameter[2]
        serverPort = parameter[3]

        if not isinstance(channel_id, str):
            sys.stdout.write("channel id type error" + '\n')
            exit(1)

    except Exception as e:
        sys.stdout.write(e + '\n')
        exit(1)

    status_code, ret_dict = UriInfoCallback(channel_id, serverIp, serverPort)
    if status_code:
        sys.stdout.write(ret_dict + '\n')
        exit(0)
    else:
        sys.stdout.write("uri info callback failed." + '\n')
        exit(1)
```
当上述验证ok后，可以直接执行./rtpDemo

对于rtpDemo，其为编译生成文件，在代码所在目录下执行:
```bash
make clean
make
```

## 目录结构描述
* include目录中是Rtp相关的头文件
* script目录中是python脚本，用于完成拉流请求
* src目录中是rtp解析的代码，实现网络包到帧的转换
* RtpDemo.cpp是主函数
* TCPServer为了支持TCP收流
