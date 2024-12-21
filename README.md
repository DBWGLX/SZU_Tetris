<div align="center">
<b>Computer Graphics Course - Shenzhen University</b>
</div>


<div align="center">
<b>Mid-term Assignment - Tetris implementation sample code</b>
</div>

<p align="center">
<b>============================================================</b>
</p>
<br>
<br>

![GIF 2024-10-18 18-35-41](https://github.com/user-attachments/assets/486db654-f4ef-41c4-b6ff-08f319add67c)



## 简介：

![REPO SIZE](https://img.shields.io/github/repo-size/DBWGLX/SZU_Tetris.svg)
![CODE SIZE](https://img.shields.io/github/languages/code-size/DBWGLX/SZU_Tetris.svg)
![License](https://img.shields.io/github/license/DBWGLX/SZU_Tetris.svg)

这是一个基于OpenGL的cpp代码而实现的俄罗斯方块游戏。

基于已有框架代码完成。

框架代码在另一个分支“blank_code”中。（※ 直接下载master中的code无法直接运行，可以先下载blank_code中的cmake文件，cmake构建后，直接把这里的main粘过去即可正常运行）

游戏时请使用英文输入法，否则无法识别字母按键。

* [项目介绍](#项目介绍)
- [代码解析](#代码解析)

<br>

## 项目介绍


### 项目坐标系：

 ![image](https://github.com/user-attachments/assets/365f676c-4f27-4b9a-8645-02d09d40218d)

！注意行是y坐标！

### 方块格式：

![image](https://github.com/user-attachments/assets/bac3e6e8-585f-47a5-867e-01fe30d9bc7e)


<br>

![image](https://github.com/user-attachments/assets/daac9afe-0af2-4c64-b457-9a75c7bc4187)

得分在终端打印，通过 \r 的方式在同一行刷新

<br>

### 暂停功能：

![image](https://github.com/user-attachments/assets/7019f30d-9b2f-4a01-a20c-2f30f21b9752)

再次按P键即可解除暂停。

<br>

### 游戏失败画面：

![image](https://github.com/user-attachments/assets/a79aa29f-bb0b-49bc-b4e1-8d90d0d48844)

此时按R仍可以重新游戏，并且只是清空board和颜色，并没有调用init()。

<br>

## 代码解析

实现的功能：

![image](https://github.com/user-attachments/assets/b101be97-ae95-4f1d-a001-1069ceb2f750)


### 1.不同形状块的绘制：

![image](https://github.com/user-attachments/assets/89f69682-b4c5-4041-ba2e-7f5e7ba6792b)


### 2.不同颜色：

![image](https://github.com/user-attachments/assets/a13f1391-6e98-408d-a3a6-54574e3089ef)

### 3.主循环：

![image](https://github.com/user-attachments/assets/2ccb3cd1-2b62-4821-b2f9-877c4d693e5f)

每次while执行allFallsDown()让所有方块下移，间隔是1s，间隔在其中的userOptions()内部停留实现的。

### forPause()

forPause()是暂停检查函数，每个间隙都会检查是否是暂停状态，随时能够暂停与开始。暂停的逻辑就是while不断检测

![image](https://github.com/user-attachments/assets/5a97ca9f-5cda-4443-a9b8-200cbf3c8a25)

### while (checkIfGameOver())

当最上方格子被占用，游戏结束，进入该循环，并打印结束信息。等待用户退出或者重新开始。

userOptions()用户操作部分：

![image](https://github.com/user-attachments/assets/fb70eb35-a2ee-4107-ac82-f77e95f7c842)

循环20次，每次停留50ms（共1 s ，也就是说用户可以在1s内操作20次并且及时回显），每次也会检查键盘按动的时间，是否有行满，并及时回显。

### allFallsDown()

每秒的下移，如果下移失败，固定这个方块，开始下一个方块。

![image](https://github.com/user-attachments/assets/becd5090-a332-47e8-a672-5d68a1de6cd8)

### 4.其他函数的实现：

### void checkfull()

![image](https://github.com/user-attachments/assets/4a9312bb-76dd-4c83-bbee-6c1ef4248201)

![image](https://github.com/user-attachments/assets/33eb00b5-5b4f-4126-88a3-f7cb0a768a06)

### void restart()

![image](https://github.com/user-attachments/assets/7815c4ca-041d-408f-a8b3-e39281a548a2)

### Note 打印函数：

![image](https://github.com/user-attachments/assets/950d5999-da57-4bce-9d95-579506e31b61)

### 随机数生成：

Cpp的方式

![image](https://github.com/user-attachments/assets/34809a72-5099-449e-83e2-4977e23a0f2c)

### void newtile()

首先是形状的确定，这里的switch写法有些繁琐，可以用个vector来简化，如接下来的颜色选择（本文最开始部分已展示颜色的初始化）。

![image](https://github.com/user-attachments/assets/bceeee36-e5d9-4715-977a-669b5104179d)

![image](https://github.com/user-attachments/assets/616356f0-deb4-4834-94e8-e0309b60b42d)


