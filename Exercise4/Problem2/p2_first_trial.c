#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>

#define MAX_SIZE 1000000
#define APPROACH 1

// Defining the Queue structure
typedef struct {
    int items[MAX_SIZE];
    int front;
    int rear;
} Queue;

// Function to initialize the queue
void initializeQueue(Queue* q)
{
    q->front = -1;
    q->rear = 0;
}

// Function to check if the queue is empty
bool isEmpty(Queue* q) { return (q->front == q->rear - 1); }

// Function to check if the queue is full
bool isFull(Queue* q) { return (q->rear == MAX_SIZE); }

// Function to add an element to the queue (Enqueue
// operation)
void enqueue(Queue* q, int value)
{
    if (isFull(q)) {
        printf("Queue is full\n");
        return;
    }
    q->items[q->rear] = value;
    q->rear++;
}

// Function to remove an element from the queue (Dequeue
// operation)
void dequeue(Queue* q)
{
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return;
    }
    q->front++;
}

// Function to get the element at the front of the queue
// (Peek operation)
int peek(Queue* q)
{
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return -1; // return some default value or handle
                   // error differently
    }
    return q->items[q->front + 1];
}

// Function to print the current queue
void printQueue(Queue* q)
{
    if (isEmpty(q)) {
        printf("Queue is empty\n");
        return;
    }

    printf("Current Queue: ");
    for (int i = q->front + 1; i < q->rear; i++) {
        printf("%d ", q->items[i]);
    }
    printf("\n");
}

int main(int argc, char* argv[]) {
    //Read Input
	FILE * fp;
	char *line = NULL;
    char *l = NULL;
	size_t len = 0;
	ssize_t read;
	
	int n, // number of vertices
        m; // number of edges
	
	int current_vertex = 0;
	int edge_idx = 0;
	
	int *ver, *edges, *tmp_edges;
    int *outdegs, *indegs;

    bool *independent_set;
	
	bool firstline = false;
    fp = fopen("data/road_usa.mtx", "r");
		
	while((read = getline(&line, &len, fp)) != -1) {
		if(line[0] != '%' && !firstline) {
			l = strtok(line, " ");
			
			n = atoi(l);
			l = strtok(NULL, " ");
			l = strtok(NULL, " ");
			m = atoi(l);
			
			ver = malloc(n * sizeof(int));
			memset(ver, 0, n * sizeof(int));

            outdegs = malloc(n * sizeof(int));
            memset(outdegs, 0, n * sizeof(int));

            indegs = malloc(n * sizeof(int));
            memset(indegs, 0, n * sizeof(int));

            edges = malloc(m * sizeof(int));
            memset(edges, -1, m * sizeof(int));

            tmp_edges = malloc(2 * m * sizeof(int));
			
			printf("%d, %d \n", n, m);
			firstline = true;	
		} else if (line[0] != '%') {
			int left, right, tmp;

			l = strtok(line, " ");
			left = atoi(l) - 1;

			l = strtok(NULL, " ");
			right = atoi(l) - 1;

            if (APPROACH == 1) {
                if (right < left) {
                    tmp = left;
                    left = right;
                    right = tmp;
                }
            }
			
			tmp_edges[edge_idx] = left;
			tmp_edges[edge_idx + 1] = right;

            outdegs[left]++;
            indegs[right]++;
			
			edge_idx += 2;
		}
	}

	// Prefix to compute starting indices in edges.
	for (int i = 0; i < n - 1; i++)
		ver[i + 1] += ver[i] + outdegs[i];
	
	for (int i = 0; i < 2 * m; i += 2) {
		int curr_edge_offset = ver[tmp_edges[i]];

		while (edges[curr_edge_offset] != -1)
			curr_edge_offset++;

		edges[curr_edge_offset] = tmp_edges[i + 1];
	}

    free(tmp_edges);
    // free(outdegs);

    independent_set = malloc(n * sizeof(bool));
    memset(independent_set, 0, n * sizeof(bool));
    // Initialize independent set.
        for (int i = 0; i < n; i++)
            independent_set[i] = true;

    Queue q;
    initializeQueue(&q);

    // Construct set of vertices with indegree 0.
    for (int i = 0; i < n; i++)
        if (indegs[i] == 0)
            enqueue(&q, i);

    while (!isEmpty(&q)) {
        int curr_vertex = peek(&q);
        dequeue(&q);

        if (outdegs[curr_vertex] > 0) { //curr_vertex < n - 1 & ver[curr_vertex] != ver[curr_vertex + 1]) {  // outdegs[curr_vertex] > 0
            int edge_offset = ver[curr_vertex];
            // Iterate over all neighbors.
            for (int i = 0; i < outdegs[curr_vertex]; i++) {
                // Get neighbor.
                int neighbor_vertex = edges[edge_offset + i];

                // Decrese indegree of neighbor.
                indegs[neighbor_vertex]--;

                independent_set[neighbor_vertex] = (
                    independent_set[neighbor_vertex] &
                    (!independent_set[curr_vertex])
                );

                // Check if indegree is 0;
                if (indegs[neighbor_vertex] == 0)
                    // Add neighbor to the queue.
                    enqueue(&q, neighbor_vertex);
            }
        }
    }

    // Measure set size for approach 1 & 2.
    if (APPROACH ==  1 | APPROACH == 2) {
        int cnt = 0;
        for (int i = 0; i < n; i++)
            if (independent_set[i] == true)
                cnt++;
        printf("%d ", cnt);
    }

    return 0;
}