/*
 *        Computer Graphics Course - Shenzhen University
 *    Mid-term Assignment - Tetris implementation sample code
 * ============================================================
 *
 * - 本代码仅仅是参考代码，具体要求请参考作业说明，按照顺序逐步完成。
 * - 关于配置OpenGL开发环境、编译运行，请参考第一周实验课程相关文档。
 *
 * - 已实现功能如下：
 * - 1) 绘制棋盘格和‘L’型方块
 * - 2) 键盘左/右/下键控制方块的移动，上键旋转方块
 *
 * - 未实现功能如下：
 * - 1) 绘制‘J’、‘Z’等形状的方块		: glm::vec2 allRotationsJshape[4][4] 等
 * - 2) 随机生成方块并赋上不同的颜色		: 设置colors向量数组，将颜色放入其中，随机下标来取。newtile()中动态渲染，settile中填充
 * - 3) 方块的自动向下移动					: void allFallsDown()
 * - 4) 方块之间、方块与边界之间的碰撞检测	: checkvalid()函数已实现对当前四个方块移动后的边界检测，我们只需要加上检查移动后位置的board[x][y]是否合法即可。
 * - 5) 棋盘格中每一行填充满之后自动消除	: void checkfull() 从上往下检查，如果一行满了，其上面所有行都往下坠
 * - 6) 按下空格键使方块快速下落			: key_callback() 中 case GLFW_KEY_SPACE: 循环下移直到冲突。然后newtile再次开始即可。 
 *
 * ※ 额外实现的功能如下：
 * - 1) 实现了空格键直接下落
 * - 2) P键可以暂停，再次按下开始
 * - 3) 计分板
 * - 4) 完善了restart()，只需把方块清空即可。
 */

//#pragma execution_character_set("utf-8")//窗口中文乱码

#include "Angel.h"

#include <cstdlib>
#include <iostream>
using namespace std;
#include <string>

#include <random>
//用于每秒方块坠落计时
#include <Windows.h>
#include <thread>
#include <chrono>

int starttime;			// 控制方块向下移动时间
int rotation = 0;		// 控制当前窗口中的方块旋转
glm::vec2 tile[4];			// 表示当前窗口中的方块
bool gameover = false;	// 游戏结束控制变量
bool isPaused = false;	// 游戏暂停控制变量
int xsize = 400;		// 窗口大小（尽量不要变动窗口大小！）
int ysize = 720;

// 创建随机数生成器
std::random_device rd; // 获取随机数种子
std::mt19937 gen(rd()); // 使用 Mersenne Twister 算法生成随机数
std::uniform_int_distribution<> dis(0, 7); // 定义均匀分布范围 [0, 99]
int curShape = 0;
int curColor = 0;

int score = 0;

// 单个网格大小
int tile_width = 33;

// 网格布大小
const int board_width = 10;
const int board_height = 20;

// 网格三角面片的顶点数量
const int points_num = board_height * board_width * 6;

// 我们用画直线的方法绘制网格
// 包含竖线 board_width+1 条
// 包含横线 board_height+1 条
// 一条线2个顶点坐标
// 网格线的数量
const int board_line_num =  (board_width + 1) + (board_height + 1);


// 一个二维数组表示所有可能出现的方块和方向。
glm::vec2 allRotationsCurshape[4][4] =
							  {{glm::vec2(0, 0), glm::vec2(-1,0), glm::vec2(0, -1), glm::vec2(-1,-1)},
							   {glm::vec2(0, 0), glm::vec2(-1,0), glm::vec2(0, -1), glm::vec2(-1,-1)},  
							   {glm::vec2(0, 0), glm::vec2(-1,0), glm::vec2(0, -1), glm::vec2(-1,-1)},  
								{glm::vec2(0, 0), glm::vec2(-1,0), glm::vec2(0, -1), glm::vec2(-1,-1)}};

glm::vec2 allRotationsOshape[4][4] =
							  {{glm::vec2(0, 0), glm::vec2(-1,0), glm::vec2(0, -1), glm::vec2(-1,-1)},
							   {glm::vec2(0, 0), glm::vec2(-1,0), glm::vec2(0, -1), glm::vec2(-1,-1)},  
							   {glm::vec2(0, 0), glm::vec2(-1,0), glm::vec2(0, -1), glm::vec2(-1,-1)},  
								{glm::vec2(0, 0), glm::vec2(-1,0), glm::vec2(0, -1), glm::vec2(-1,-1)}};

glm::vec2 allRotationsIshape[4][4] =
							  {{glm::vec2(-2,0), glm::vec2(-1,0), glm::vec2(0, 0), glm::vec2(1,0)},
							   {glm::vec2(0,-2), glm::vec2(0,-1), glm::vec2(0, 0), glm::vec2(0,1)},
							   {glm::vec2(-2,0), glm::vec2(-1,0), glm::vec2(0, 0), glm::vec2(1,0)},
							   {glm::vec2(0,-2), glm::vec2(0,-1), glm::vec2(0, 0), glm::vec2(0,1)} };

glm::vec2 allRotationsSshape[4][4] =
							  {{glm::vec2(0, 0), glm::vec2(1,0), glm::vec2(0, -1), glm::vec2(-1,-1)},
							   {glm::vec2(1, 0), glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, -1)},   
							   {glm::vec2(0, 0), glm::vec2(1,0), glm::vec2(0, -1), glm::vec2(-1,-1)},
							   {glm::vec2(1, 0), glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(1, -1)}};

glm::vec2 allRotationsZshape[4][4] =
							  {{glm::vec2(0, 0), glm::vec2(-1,0), glm::vec2(0, -1), glm::vec2(1,-1)},
							   {glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1,1), glm::vec2(0, -1)},   
							   {glm::vec2(0, 0), glm::vec2(-1,0), glm::vec2(0, -1), glm::vec2(1,-1)},   
							   {glm::vec2(0, 0), glm::vec2(1, 0), glm::vec2(1,1), glm::vec2(0, -1)}};

glm::vec2 allRotationsLshape[4][4] =
							  {{glm::vec2(0, 0), glm::vec2(-1,0), glm::vec2(-1, -1), glm::vec2(1,0)},	//   "L"
							   {glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(0,-1), glm::vec2(1, -1)},   //
							   {glm::vec2(0, 0), glm::vec2(-1,0), glm::vec2(1, 0), glm::vec2(1,  1)},   //
							   {glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(-1, 1), glm::vec2(0, -1)}};

glm::vec2 allRotationsJshape[4][4] =
								{{glm::vec2(0, 0), glm::vec2(-1,0), glm::vec2(1, 0), glm::vec2(1,-1)},
								{glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(0, -1), glm::vec2(1, 1)},
								{glm::vec2(0, 0), glm::vec2(-1,0), glm::vec2(1,  0), glm::vec2(-1, 1)},
								{glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(0,-1), glm::vec2(-1, -1)}
							   };

glm::vec2 allRotationsTshape[4][4] =
							  {{glm::vec2(0, 0), glm::vec2(-1,0), glm::vec2(1, 0), glm::vec2(0,-1)},
							   {glm::vec2(0, 0), glm::vec2(0, 1), glm::vec2(0,-1), glm::vec2(1, 0)},   
							   {glm::vec2(0, 0), glm::vec2(-1,0), glm::vec2(1, 0), glm::vec2(0, 1)},   
							   {glm::vec2(-1,0), glm::vec2(0, 1), glm::vec2(0,-1), glm::vec2(0, 0)}};

// 绘制窗口的颜色变量
glm::vec4 white  = glm::vec4(1.0, 1.0, 1.0, 1.0);
glm::vec4 gray   = glm::vec4(0.8, 0.8, 0.8, 1.0);
glm::vec4 black  = glm::vec4(0.0, 0.0, 0.0, 1.0);

glm::vec4 red = glm::vec4(1.0, 0.0, 0.0, 1.0); // 赤色
glm::vec4 orange = glm::vec4(1.0, 0.5, 0.0, 1.0); // 橙色
glm::vec4 yellow = glm::vec4(1.0, 1.0, 0.0, 1.0); // 黄色
glm::vec4 green = glm::vec4(0.0, 1.0, 0.0, 1.0); // 绿色
glm::vec4 blue = glm::vec4(0.0, 0.0, 1.0, 1.0); // 蓝色
glm::vec4 indigo = glm::vec4(0.0, 1.0, 1.0, 1.0); // 青色
glm::vec4 violet = glm::vec4(0.56, 0.0, 1.0, 1.0); // 紫色

vector<glm::vec4> colors{
	red,orange,yellow,green,blue,indigo,violet };

//（坐标系以棋盘格的左下角为原点的） #####
// 当前方块的位置
glm::vec2 tilepos = glm::vec2(5, 19);

// 布尔数组表示棋盘格的某位置是否被方块填充，即board[x][y] = true表示(x,y)处格子被填充。（以棋盘格的左下角为原点的坐标系）
bool board[board_width][board_height];

// 当棋盘格某些位置被方块填充之后，记录这些位置上被填充的颜色
glm::vec4 board_colours[points_num];

GLuint locxsize;
GLuint locysize;

GLuint vao[3];
GLuint vbo[6];

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// 修改棋盘格在pos位置的颜色为colour，并且更新对应的VBO
void changecellcolour(glm::vec2 pos, glm::vec4 colour)
{
	// 每个格子是个正方形，包含两个三角形，总共6个定点，并在特定的位置赋上适当的颜色
	for (int i = 0; i < 6; i++)
    	board_colours[(int)( 6 * ( board_width * pos.y + pos.x) + i)] = colour; //【一列width个;每个方块有6个点】

	glm::vec4 newcolours[6] = {colour, colour, colour, colour, colour, colour};

	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);

	// 计算偏移量，在适当的位置赋上颜色
	int offset = 6 * sizeof(glm::vec4) * (int)( board_width * pos.y + pos.x);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(newcolours), newcolours);  // 上色
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// 当前方块移动或者旋转时，更新VBO
void updatetile()
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);

	// 每个方块包含四个格子
	for (int i = 0; i < 4; i++)
	{
		// 计算格子的坐标值
		GLfloat x = tilepos.x + tile[i].x;
		GLfloat y = tilepos.y + tile[i].y;

		glm::vec4 p1 = glm::vec4(tile_width + (x * tile_width), tile_width + (y * tile_width), .4, 1);
		glm::vec4 p2 = glm::vec4(tile_width + (x * tile_width), tile_width*2 + (y * tile_width), .4, 1);
		glm::vec4 p3 = glm::vec4(tile_width*2 + (x * tile_width), tile_width + (y * tile_width), .4, 1);
		glm::vec4 p4 = glm::vec4(tile_width*2 + (x * tile_width), tile_width*2 + (y * tile_width), .4, 1);
		//【】 (1,1) (1,2) (2,1) (2,2) ———— 这个方块的四个顶点坐标

		// 每个格子包含两个三角形，所以有6个顶点坐标
		glm::vec4 newpoints[6] = {p1, p2, p3, p2, p3, p4};
		glBufferSubData(GL_ARRAY_BUFFER, i*6*sizeof(glm::vec4), 6*sizeof(glm::vec4), newpoints); //【放回缓冲区】
	}
	#ifdef __APPLE__
		glBindVertexArrayAPPLE(0);
	#else
		glBindVertexArray(0);
	#endif
}

// 设置当前方块为下一个即将出现的方块。在游戏开始的时候调用来创建一个初始的方块，
// 在游戏结束的时候判断，没有足够的空间来生成新的方块。
void newtile()
{
	// 将新方块放于棋盘格的最上行中间位置并设置默认的旋转方向 
	tilepos = glm::vec2(5 , 19);
	rotation = 0;

	curShape = dis(gen)%7;
	for(int i=0;i<4;i++){
		for(int j=0;j<4;j++){
			switch(curShape){
			case 0:
				allRotationsCurshape[i][j] = allRotationsOshape[i][j]; break;
			case 1:
				allRotationsCurshape[i][j] = allRotationsIshape[i][j]; break;
			case 2:
				allRotationsCurshape[i][j] = allRotationsSshape[i][j]; break;
			case 3:
				allRotationsCurshape[i][j] = allRotationsZshape[i][j]; break;
			case 4:
				allRotationsCurshape[i][j] = allRotationsLshape[i][j]; break;
			case 5:
				allRotationsCurshape[i][j] = allRotationsJshape[i][j]; break;
			case 6:
				allRotationsCurshape[i][j] = allRotationsTshape[i][j]; break;
			}
		}
	}

	for (int i = 0; i < 4; i++){
		tile[i] = allRotationsCurshape[0][i];
	}

	updatetile();

	// 给新方块赋上颜色
	curColor = dis(gen)%7;
	glm::vec4 newcolours[24]; // 【4 * （2*3）】
	for (int i = 0; i < 24; i++)
		newcolours[i] = colors[curColor];

	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(newcolours), newcolours);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

// 游戏和OpenGL初始化
void init()
{
	// 初始化棋盘格，这里用画直线的方法绘制网格
	// 包含竖线 board_width+1 条
	// 包含横线 board_height+1 条
	// 一条线2个顶点坐标，并且每个顶点一个颜色值
	
	glm::vec4 gridpoints[board_line_num * 2];
	glm::vec4 gridcolours[board_line_num * 2];

	// 绘制网格线 【列出所有点的位置】
	// 纵向线
	for (int i = 0; i < (board_width+1); i++)
	{
		gridpoints[2*i] = glm::vec4((tile_width + (tile_width * i)), tile_width, 0, 1);
		gridpoints[2*i + 1] = glm::vec4((tile_width + (tile_width * i)), (board_height+1) * tile_width, 0, 1);
	}

	// 水平线
	for (int i = 0; i < (board_height+1); i++)
	{
		gridpoints[ 2*(board_width+1) + 2*i ] = glm::vec4(tile_width, (tile_width + (tile_width * i)), 0, 1);
		gridpoints[ 2*(board_width+1) + 2*i + 1 ] = glm::vec4((board_width+1) * tile_width, (tile_width + (tile_width * i)), 0, 1);
	}

	// 将所有线赋成白色 【灰】
	for (int i = 0; i < (board_line_num * 2); i++)
		gridcolours[i] = gray;

	// 初始化棋盘格，并将没有被填充的格子设置成黑色
	glm::vec4 boardpoints[points_num];
	for (int i = 0; i < points_num; i++)
		board_colours[i] = black;

	// 对每个格子，初始化6个顶点，表示两个三角形，绘制一个正方形格子 【】
	for (int i = 0; i < board_height; i++)
		for (int j = 0; j < board_width; j++)
		{
			glm::vec4 p1 = glm::vec4(tile_width + (j * tile_width), tile_width + (i * tile_width), .5, 1);
			glm::vec4 p2 = glm::vec4(tile_width + (j * tile_width), tile_width*2 + (i * tile_width), .5, 1);
			glm::vec4 p3 = glm::vec4(tile_width*2 + (j * tile_width), tile_width + (i * tile_width), .5, 1);
			glm::vec4 p4 = glm::vec4(tile_width*2 + (j * tile_width), tile_width*2 + (i * tile_width), .5, 1);
			boardpoints[ 6 * ( board_width * i + j ) + 0 ] = p1;
			boardpoints[ 6 * ( board_width * i + j ) + 1 ] = p2;
			boardpoints[ 6 * ( board_width * i + j ) + 2 ] = p3;
			boardpoints[ 6 * ( board_width * i + j ) + 3 ] = p2;
			boardpoints[ 6 * ( board_width * i + j ) + 4 ] = p3;
			boardpoints[ 6 * ( board_width * i + j ) + 5 ] = p4;
		}

	// 将棋盘格所有位置的填充与否都设置为false（没有被填充）
	for (int i = 0; i < board_width; i++)
		for (int j = 0; j < board_height; j++)
			board[i][j] = false;

	// 载入着色器
	std::string vshader, fshader;
	vshader = "shaders/vshader.glsl";
	fshader = "shaders/fshader.glsl";
	GLuint program = InitShader(vshader.c_str(), fshader.c_str());//
	glUseProgram(program);

	locxsize = glGetUniformLocation(program, "xsize");
	locysize = glGetUniformLocation(program, "ysize");

	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	GLuint vColor = glGetAttribLocation(program, "vColor");

	
	glGenVertexArrays(3, &vao[0]);

	
	glBindVertexArray(vao[0]);		// 棋盘格顶点
	
	glGenBuffers(2, vbo);//== &vbo[0]

	// 棋盘格顶点位置
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, (board_line_num * 2) * sizeof(glm::vec4), gridpoints, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// 棋盘格顶点颜色
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, (board_line_num * 2) * sizeof(glm::vec4), gridcolours, GL_STATIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);

	
	glBindVertexArray(vao[1]);		// 棋盘格每个格子
	glGenBuffers(2, &vbo[2]);

	// 棋盘格每个格子顶点位置
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, points_num*sizeof(glm::vec4), boardpoints, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// 棋盘格每个格子顶点颜色
	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, points_num*sizeof(glm::vec4), board_colours, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);

	
	glBindVertexArray(vao[2]);		// 当前方块
	glGenBuffers(2, &vbo[4]);

	// 当前方块顶点位置
	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, 24*sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vPosition);

	// 当前方块顶点颜色
	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glBufferData(GL_ARRAY_BUFFER, 24*sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(vColor, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(vColor);

	
	glBindVertexArray(0);
	glClearColor(0, 0, 0, 0);

	// 游戏初始化
	newtile();
	// starttime = glutGet(GLUT_ELAPSED_TIME);
}

// 检查在cellpos位置的格子是否被填充或者是否在棋盘格的边界范围内
bool checkvalid(glm::vec2 cellpos)
{
	if((cellpos.x >=0) && (cellpos.x < board_width) && (cellpos.y >= 0) && (cellpos.y < board_height) ){
		//【】已检查边界
		//【】还要检查下个位置是否已被占用
		if (board[(int)cellpos.x][(int)cellpos.y])
			return false;
		else
			return true;
	}
	else
		return false;
}

// 在棋盘上有足够空间的情况下旋转当前方块
void rotate()
{
	// 计算得到下一个旋转方向
	int nextrotation = (rotation + 1) % 4;

	// 检查当前旋转之后的位置的有效性
	if (checkvalid((allRotationsCurshape[nextrotation][0]) + tilepos)
		&& checkvalid((allRotationsCurshape[nextrotation][1]) + tilepos)
		&& checkvalid((allRotationsCurshape[nextrotation][2]) + tilepos)
		&& checkvalid((allRotationsCurshape[nextrotation][3]) + tilepos))
	{
		// 更新旋转，将当前方块设置为旋转之后的方块
		rotation = nextrotation;
		for (int i = 0; i < 4; i++)
			tile[i] = allRotationsCurshape[rotation][i];

		updatetile();
	}
}

// 检查棋盘格在row行有没有被填充满
bool checkfullrow(int row)
{
	//cout << "[DEBUG]Checking row " << row << ": "<< endl;
	for (int x = 0; x < board_width; x++) {
		//cout << "[DEBUG] col" << j << ": "<< board[row][j] << " " << endl;
		if (!board[x][row]) {
			return false;
		}
	}
	return true;
}

void movefullrow(int row){
	for (int x = 0; x < board_width; x++) {
		board[x][board_height-1] = false;
		changecellcolour(glm::vec2(x, board_height-1), black);
	}

	for(int y = row;y<board_height-1;y++){
		for (int x = 0; x < board_width; x++) {
			board[x][y] = board[x][y+1];
			changecellcolour(glm::vec2(x, y), board_colours[6 * (board_width * (y + 1) + x)]);
		}
	}
}

void scoreNote();
void checkfull(){
	for(int y = board_height-1; y >= 0; y--){
		if(checkfullrow(y)){
			// 一行满，所有上面的都要往下坠
			//cout << "[Debug]"<<"第"<<i<<"行满"<<endl;
			movefullrow(y);
			score++;
			scoreNote();
		}
	}
}


// 放置当前方块，并且更新棋盘格对应位置顶点的颜色VBO
void settile()
{
	// 每个格子
	for (int i = 0; i < 4; i++)
	{
		// 获取格子在棋盘格上的坐标
		int x = (tile[i] + tilepos).x;
		int y = (tile[i] + tilepos).y;
		// 将格子对应在棋盘格上的位置设置为填充
		board[x][y] = true;
		// 并将相应位置的颜色修改
		changecellcolour(glm::vec2(x, y), colors[curColor]);
	}
}

// 给定位置(x,y)，移动方块。有效的移动值为(-1,0)，(1,0)，(0,-1)，分别对应于向
// 左，向下和向右移动。如果移动成功，返回值为true，反之为false
bool movetile(glm::vec2 direction)
{
	// 计算移动之后的方块的位置坐标
	glm::vec2 newtilepos[4];
	for (int i = 0; i < 4; i++)
		newtilepos[i] = tile[i] + tilepos + direction;

	// 检查移动之后的有效性
	if (checkvalid(newtilepos[0]) && checkvalid(newtilepos[1]) && checkvalid(newtilepos[2]) && checkvalid(newtilepos[3]))
	{
		// 有效：移动该方块
		tilepos.x = tilepos.x + direction.x;
		tilepos.y = tilepos.y + direction.y;

		updatetile();

		return true;
	}

	return false;
}

// 重新启动游戏
void restart()
{
	// 将棋盘格所有位置的填充与否都设置为false（没有被填充）
	for (int i = 0; i < board_width; i++)
		for (int j = 0; j < board_height; j++){
			if(board[i][j]){
				board[i][j] = false;
				changecellcolour(glm::vec2(i, j), black);
			}
		}
	score = 0;
	scoreNote();
	//开始
	newtile();
}

// 游戏渲染部分
void display()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glUniform1i(locxsize, xsize);
	glUniform1i(locysize, ysize);

	glBindVertexArray(vao[1]);
	glDrawArrays(GL_TRIANGLES, 0, points_num); // 绘制棋盘格 (width * height * 2 个三角形)
	glBindVertexArray(vao[2]);
	glDrawArrays(GL_TRIANGLES, 0, 24);	 // 绘制当前方块 (8 个三角形)
	glBindVertexArray(vao[0]);
	glDrawArrays(GL_LINES, 0, board_line_num * 2 );		 // 绘制棋盘格的线

}

// 在窗口被拉伸的时候，控制棋盘格的大小，使之保持固定的比例。
void reshape(GLsizei w, GLsizei h)
{
	xsize = w;
	ysize = h;
	glViewport(0, 0, w, h);
}

// 键盘响应事件中的特殊按键响应
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if(!gameover)
	{
		switch(key)
		{	
			// 控制方块的移动方向，更改形态
			case GLFW_KEY_UP:	// 向上按键旋转方块
				if (action == GLFW_PRESS || action == GLFW_REPEAT)
				{
					rotate();
					break;
				}
				else
				{
					break;
				}
			case GLFW_KEY_DOWN: // 向下按键移动方块
				if (action == GLFW_PRESS || action == GLFW_REPEAT){
					if (!movetile(glm::vec2(0, -1)))
					{
						settile(); //【】方块就放在这里了
						newtile(); //【】产生新的方块
						break;
					}
					else
					{
						break;
					}
				}
			case GLFW_KEY_LEFT:  // 向左按键移动方块
				if (action == GLFW_PRESS || action == GLFW_REPEAT){
					movetile(glm::vec2(-1, 0));
					break;
				}
				else
				{
					break;
				}
			case GLFW_KEY_RIGHT: // 向右按键移动方块
				if (action == GLFW_PRESS || action == GLFW_REPEAT){
					movetile(glm::vec2(1, 0));
					break;
				}
				else
				{
					break;
				}
			// 游戏设置。
			case GLFW_KEY_ESCAPE:
				if(action == GLFW_PRESS){
					exit(EXIT_SUCCESS);
					break;
				}
				else
				{
					break;
				}
			case GLFW_KEY_Q:
				if(action == GLFW_PRESS){
					exit(EXIT_SUCCESS);
					break;
				}
				else
				{
					break;
				}
				
			case GLFW_KEY_R:
				if(action == GLFW_PRESS){
					restart();
					break;
				}
				else
				{
					break;
				}		

			case GLFW_KEY_P:
				if(action == GLFW_PRESS){
					isPaused = !isPaused;
					break;
				}
				else
				{
					break;
				}

			case GLFW_KEY_SPACE:
				if(action == GLFW_PRESS){
					while (movetile(glm::vec2(0, -1))){
						// 循环向下移动方块，直到不能移动
					}
					settile(); //【】方块就放在这里了
					newtile(); //【】产生新的方块
					break;
				}
				else
				{
					break;
				}

			default:
				break;
		}
	}
}

//【while循环中的函数】:

void allFallsDown(){
	if (!movetile(glm::vec2(0, -1)))
	{
		settile();
		newtile();
	}
}

void forPause(){
	int flag = 0;
	while(isPaused){
		if(!flag){
			cout <<  "[INFO]" << "游戏暂停" << endl;
			flag = 1;
		}
		glfwPollEvents();
	}
	if(flag){
	cout << "[INFO]" << "游戏继续" << endl;
	}
}

void note() {
	cout << "# Welcome to play the Tetris made by DBWGLX !" << endl << endl;
	cout << "# For more information,please visit https://github.com/lubenweiNBNBNBNB/SZU_Tetris" << endl << endl;
	cout << "# Instructions:" << endl;
	cout << "    " << "press \U00002191     to rotate the block" << endl;
	cout << "    " << "press \U00002190     to left shift the block" << endl;
	cout << "    " << "press \U00002192     to right shift the block" << endl;
	cout << "    " << "press \U00002193     to shift down the block" << endl;
	cout << "    " << "press SPACE to move the block to the bottom" << endl;
	cout << "    " << "press P     to pause/continue" << endl;
	cout << "    " << "press R     to restart" << endl;
	cout << "    " << "press Q     to quit" << endl << endl;
	cout << "# Score: 0";
}

void scoreNote() {
	cout << "\r# Score: " << score << "                                          " << std::flush;
}

bool checkIfGameOver() {
	if (board[6][19]) {
		cout << "\r# GAME OVER! Your final score: " << score << "   Have a good day!" << std::flush;
		return true;
	}
	return false;
}

void userOptions(GLFWwindow* window){
	int interval = 50;//ms
	for (int i = 0; i < 1000 / interval;i++) {

		forPause();

		glfwPollEvents();
		std::this_thread::sleep_for(std::chrono::milliseconds(interval));
		checkfull();		

		display();
		glfwSwapBuffers(window);
	}
}

int main(int argc, char **argv)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
	#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif

	// 创建窗口。
	GLFWwindow* window = glfwCreateWindow(500, 900, "Tetris by DBWGLX", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window!" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
	
	
	init();
	cout << "[INFO]" << "游戏开始" << endl;
	std::cout << "\033]0;Tetris Game Console\007";
	note();

	while (!glfwWindowShouldClose(window))
    { 
		forPause();

		while (checkIfGameOver()) {
			glfwPollEvents();
		}

        display();//【绘制固定的内容】
        glfwSwapBuffers(window);	

		forPause();

		// 方块下移
		allFallsDown();

		//【DEBUG】 ”行是y坐标“
		//changecellcolour(glm::vec2(0, 5), white);

		forPause();

		//用户操作
		userOptions(window);
	
    }
    glfwTerminate();
    return 0;
}
