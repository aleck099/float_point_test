## 读60M个浮点数，求其和

数据间允许有空格/换行等

clang编译：

`make CXX="clang++ -stdlib=libc++"`

gcc编译：

`make CXX="g++"`

msvc编译：没做，自己看着来

## 运行

生成数据

`./gen`

生成numbers.txt (大小约1G)

运行测试程序

`./legacy`
`./s2`
`./s3`
