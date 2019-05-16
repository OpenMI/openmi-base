# openmi-base 

basic, common components for openmi 

# 安装第三方依赖

默认安装在`~/.openmi_deps`目录下，配置`third_party/deps_env.conf`

```sh
cd third_party 
./install_deps.sh
```

# 安装与编译 

+ 安装条件

gcc版本至少支持c++11

+ 编译

```sh
cd openmi-base
./build.sh [-b ${HOME}/.openmi_deps]
```

> `-b ${HOME}/.openmi_deps`默认第三方依赖路径

如果成功，会生成一个lib目录，存放base的static和dynamic文件。

+ 清除编译结果

执行`./clean_build.sh`


# 联系

...
