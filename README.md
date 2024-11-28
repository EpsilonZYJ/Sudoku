# Sudoku

## Windows构建：

**记得修改CMakeLists中编译器的地址**

*注：本人此项目采用的generator为Visual Studio 2019, 若使用GNU make等需要在makefile修改相应的路径*

在该目录下使用powershell运行以下命令即可构建并运行

```shell
sh
# 若没有单独复制并配置过make.exe, 可以采用
# mingw32-make build
# 来进行构建，以下其它命令同理，使用mingw32-make替换make
make build	#构建
make run	#运行
```

使用

```shell
make clean
```

可以清除构建文件build

## 运行须知

在Sudoku目录下使用命令

```sh
make build
make copy
./Sudoku.exe
```

构建并运行Sudoku.exe

**注意：Sudoku.exe必须与Data在同一目录下，且优化率示例文件和X-sudoku.txt必须在其目录下，否则将无法显示优化率，除此之外其余功能均能正常使用。**

Answer文件夹若不存在会自动创建
