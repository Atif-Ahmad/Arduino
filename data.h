typedef struct {
    int row;
    int col;
    int length;
} Obstacle;

bool equal_obstacles(Obstacle a, Obstacle b) {
    return (a.row == b.row &&
            a.col == b.col &&
            a.length == b.length);
}

Obstacle obstacles[10];
int size = 0;


void insert_obstacle(Obstacle o){
    obstacles[size] = o;
    size++;
}

void remove_obstacle(Obstacle o){
    for (int i = 0; i < size; i++){
        if (equal_obstacles(obstacles[i], o)){
            for (int j = i; j < size - 1; j++) obstacles[j] = obstacles[j + 1];
            size--;
            return;
        }
    }
}