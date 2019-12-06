# CompileDemo

## 简介

一个简单验证交叉编译的程序。

## 支持的产品

Atlas 500

## 编译

1. 在build.sh中的第4行填写Atlas500 DDK路径
2. 在build.sh中的第7行填写Euler_env_cross交叉编译路径
3. 运行build.sh

## 运行

编译成功后，脚本会自动将程序依赖的所有so拷贝到out目录中，只需要将整个out目录拷贝Atlas 500环境上既可以运行
