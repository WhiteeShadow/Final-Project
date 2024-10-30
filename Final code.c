#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#define MAX_NODES 100

// Structure to represent an adjacency list node
typedef struct AdjListNode {
    int vertex;
    int weight;
    struct AdjListNode* next;
} AdjListNode;

// Structure for the graph containing adjacency lists
typedef struct Graph {
    int numVertices;
    AdjListNode** adjLists;
} Graph;

// Structure representing a min-heap node
typedef struct MinHeapNode {
    int vertex;
    int distance;
} MinHeapNode;

// Structure representing a min-heap
typedef struct MinHeap {
    int size;
    int capacity;
    int* position;
    MinHeapNode** array;
} MinHeap;

// Function to create a new adjacency list node
AdjListNode* createAdjListNode(int vertex, int weight) {
    AdjListNode* newNode = (AdjListNode*)malloc(sizeof(AdjListNode));
    newNode->vertex = vertex;
    newNode->weight = weight;
    newNode->next = NULL;
    return newNode;
}

// Function to initialize a graph with specified vertices
Graph* initializeGraph(int vertices) {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->numVertices = vertices;
    graph->adjLists = (AdjListNode**)malloc(vertices * sizeof(AdjListNode*));

    for (int i = 0; i < vertices; i++)
        graph->adjLists[i] = NULL;

    return graph;
}

// Function to add an edge in a directed graph
void insertEdge(Graph* graph, int src, int dest, int weight) {
    AdjListNode* newNode = createAdjListNode(dest, weight);
    newNode->next = graph->adjLists[src];
    graph->adjLists[src] = newNode;
}

// Function to create a min-heap node
MinHeapNode* initializeMinHeapNode(int v, int dist) {
    MinHeapNode* minHeapNode = (MinHeapNode*)malloc(sizeof(MinHeapNode));
    minHeapNode->vertex = v;
    minHeapNode->distance = dist;
    return minHeapNode;
}

// Function to initialize a min-heap
MinHeap* createMinHeap(int capacity) {
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    minHeap->position = (int*)malloc(capacity * sizeof(int));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (MinHeapNode**)malloc(capacity * sizeof(MinHeapNode*));
    return minHeap;
}

// Swap nodes in the min-heap
void swapNodes(MinHeapNode** a, MinHeapNode** b) {
    MinHeapNode* temp = *a;
    *a = *b;
    *b = temp;
}

// Heapify function to maintain min-heap properties
void minHeapify(MinHeap* minHeap, int index) {
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < minHeap->size && minHeap->array[left]->distance < minHeap->array[smallest]->distance)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->distance < minHeap->array[smallest]->distance)
        smallest = right;

    if (smallest != index) {
        MinHeapNode* smallestNode = minHeap->array[smallest];
        MinHeapNode* idxNode = minHeap->array[index];

        minHeap->position[smallestNode->vertex] = index;
        minHeap->position[idxNode->vertex] = smallest;

        swapNodes(&minHeap->array[smallest], &minHeap->array[index]);
        minHeapify(minHeap, smallest);
    }
}

// Check if the min-heap is empty
int isMinHeapEmpty(MinHeap* minHeap) {
    return minHeap->size == 0;
}

// Extract minimum node from the min-heap
MinHeapNode* extractMinNode(MinHeap* minHeap) {
    if (isMinHeapEmpty(minHeap))
        return NULL;

    MinHeapNode* root = minHeap->array[0];
    MinHeapNode* lastNode = minHeap->array[minHeap->size - 1];
    minHeap->array[0] = lastNode;

    minHeap->position[root->vertex] = minHeap->size - 1;
    minHeap->position[lastNode->vertex] = 0;

    --minHeap->size;
    minHeapify(minHeap, 0);

    return root;
}

// Decrease key (distance) of a vertex in the min-heap
void updateKey(MinHeap* minHeap, int vertex, int distance) {
    int i = minHeap->position[vertex];
    minHeap->array[i]->distance = distance;

    while (i && minHeap->array[i]->distance < minHeap->array[(i - 1) / 2]->distance) {
        minHeap->position[minHeap->array[i]->vertex] = (i - 1) / 2;
        minHeap->position[minHeap->array[(i - 1) / 2]->vertex] = i;
        swapNodes(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);

        i = (i - 1) / 2;
    }
}

// Check if a vertex is in the min-heap
int isVertexInMinHeap(MinHeap* minHeap, int vertex) {
    return minHeap->position[vertex] < minHeap->size;
}

// Dijkstra's algorithm for finding shortest paths from a source
void dijkstraShortestPath(Graph* graph, int source) {
    int V = graph->numVertices;
    int distances[V];

    MinHeap* minHeap = createMinHeap(V);

    for (int v = 0; v < V; ++v) {
        distances[v] = INT_MAX;
        minHeap->array[v] = initializeMinHeapNode(v, distances[v]);
        minHeap->position[v] = v;
    }

    minHeap->array[source] = initializeMinHeapNode(source, distances[source]);
    minHeap->position[source] = source;
    distances[source] = 0;
    updateKey(minHeap, source, distances[source]);

    minHeap->size = V;

    while (!isMinHeapEmpty(minHeap)) {
        MinHeapNode* minHeapNode = extractMinNode(minHeap);
        int u = minHeapNode->vertex;

        AdjListNode* adjNode = graph->adjLists[u];
        while (adjNode != NULL) {
            int v = adjNode->vertex;

            if (isVertexInMinHeap(minHeap, v) && distances[u] != INT_MAX &&
                adjNode->weight + distances[u] < distances[v]) {
                distances[v] = distances[u] + adjNode->weight;
                updateKey(minHeap, v, distances[v]);
            }
            adjNode = adjNode->next;
        }
    }

    printf("Dijkstra's Algorithm:\n");
    printf("Vertex Distance from Source\n");
    for (int i = 0; i < V; ++i)
        printf("%d \t\t %d\n", i, distances[i]);
}

// Bellman-Ford algorithm for finding shortest paths from a source
void bellmanFord(Graph* graph, int source) {
    int V = graph->numVertices;
    int distances[V];

    for (int i = 0; i < V; i++)
        distances[i] = INT_MAX;

    distances[source] = 0;

    for (int i = 0; i < V - 1; i++) {
        for (int u = 0; u < V; u++) {
            AdjListNode* adjNode = graph->adjLists[u];
            while (adjNode != NULL) {
                int v = adjNode->vertex;
                int weight = adjNode->weight;
                if (distances[u] != INT_MAX && distances[u] + weight < distances[v])
                    distances[v] = distances[u] + weight;
                adjNode = adjNode->next;
            }
        }
    }

    for (int u = 0; u < V; u++) {
        AdjListNode* adjNode = graph->adjLists[u];
        while (adjNode != NULL) {
            int v = adjNode->vertex;
            int weight = adjNode->weight;
            if (distances[u] != INT_MAX && distances[u] + weight < distances[v]) {
                printf("Graph contains negative weight cycle\n");
                return;
            }
            adjNode = adjNode->next;
        }
    }

    printf("\nBellman-Ford Algorithm:\n");
    printf("Vertex Distance from Source\n");
    for (int i = 0; i < V; ++i)
        printf("%d \t\t %d\n", i, distances[i]);
}

// Function to measure and print execution time
void measureTime(void (*algorithm)(Graph*, int), Graph* graph, int source, const char* algorithmName) {
    clock_t start, end;
    double cpu_time_used;

    start = clock();
    algorithm(graph, source);
    end = clock();

    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("\n%s execution time: %f seconds\n", algorithmName, cpu_time_used);
}

// Main function to test both algorithms and measure their execution times
int main() {
    int vertices = 5;
    Graph* graph = initializeGraph(vertices);
    insertEdge(graph, 0, 1, 10);
    insertEdge(graph, 0, 2, 5);
    insertEdge(graph, 1, 3, 1);
    insertEdge(graph, 2, 1, 3);
    insertEdge(graph, 2, 3, 8);
    insertEdge(graph, 3, 4, 4);

    measureTime(dijkstraShortestPath, graph, 0, "Dijkstra's Algorithm");
    measureTime(bellmanFord, graph, 0, "Bellman-Ford Algorithm");

    return 0;
}
