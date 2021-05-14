#pragma once
#include <algorithm>
#include <ctime>

void PrintMaze(std::vector<std::vector<int>> maze) {

	for (int i = 0; i < maze.size(); i++) {
		for (int j = 0; j < maze[i].size(); j++) {
			std::cout << maze[i][j] << ' ';

		}
		std::cout << std::endl;
	}
	std::cout << std::endl << std::endl;

}

bool CanCarve(std::vector<std::vector<int>> &maze, glm::vec2 point) {

	if(point.x < 0 || point.y < 0 || point.x >= maze.size() || point.y >= maze[point.x].size() || maze[point.x][point.y] == 2)
		return false;

	std::vector<glm::vec2> dirs = {
		glm::vec2(0, -1),
		glm::vec2(1, 0),
		glm::vec2(0, 1),
		glm::vec2(-1, 0),
	};

	glm::vec2 newPoint;
	int countPath = 0;
	for (int i = 0; i < dirs.size(); i++) {
		newPoint = glm::vec2(point.x + dirs[i].x, point.y + dirs[i].y);

		if (newPoint.x >= 0 && newPoint.y >=0 && newPoint.x < maze.size() && newPoint.y < maze[newPoint.x].size()) {
	
			if (maze[newPoint.x][newPoint.y] == 0) 
				countPath++;
		}

	}

	if (countPath > 1)
		return false;

	return true;

}


void CarveMaze(std::vector<std::vector<int>> &maze, glm::vec2 point) {
	std::vector<glm::vec2> dirs = {
		glm::vec2(0, -1),
		glm::vec2(1, 0),
		glm::vec2(0, 1),
		glm::vec2(-1, 0),
	};

	std::random_shuffle(dirs.begin(), dirs.end());
	glm::vec2 newPoint;

	for (int i = 0; i < dirs.size(); i++) {

		newPoint = glm::vec2(point.x + dirs[i].x, point.y + dirs[i].y);

		if (CanCarve(maze, newPoint)) {
			maze[newPoint.x][newPoint.y] = 0;
			CarveMaze(maze, newPoint);
		}

	}
}

std::vector<std::vector<int>> CreateMaze() {

	int size = 26;

	std::vector<std::vector<int>>maze;
	maze.reserve(size+1);

	std::srand(std::time(0));
	for (int i = 0; i < size+1; i++) {
		std::vector<int> row;
		row.reserve(size+1);
		for (int j = 0; j < size + 1; j++) {
			if(i == 0 || j ==0 || i == size || j == size) row.push_back(2);
			else
			row.push_back(1);
		}
		maze.push_back(row);
	}

	
	CarveMaze(maze, glm::vec2(1, 1));

	PrintMaze(maze);
	return maze;
}