#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <iostream>
#include <string.h>

#define MAXVEX 100						// define the available maximum number of vertex
int pred[MAXVEX];                       // an array to record predecessor

//******************** Adjacent Matrix ********************
typedef struct
{
    int verx[MAXVEX];					// store the vertex
    int arc[MAXVEX][MAXVEX];			// store the edges
    int numVertexes, numEdges;			// total number of vertex and edges
}Graph;

//******************** Edge structure ********************
typedef struct
{
    int start; // the start of edge
    int end;   // the end of edge
    int weight; // the weight of edge
}EData;

//******************** Build the Graph ********************
void CreateGraph(Graph* G, char input[256], FILE* fp)
{
    int i, j, count;
    int firstVertex = 0, secondVertex = 0, weight = 0;

    fscanf(fp, "%d %d", &(G->numVertexes), &(G->numEdges));   // read the input file
    //printf("%d %d\n", G->numVertexes, G->numEdges);

    for (i = 0; i < (G->numVertexes); i++)                    // read the vertex
        G->verx[i] = i;

    for (i = 0; i < G->numVertexes; i++)					 // initialize the adjacent matrix
        for (j = 0; j < G->numVertexes; j++)
            G->arc[i][j] = 0;

    for (count = 0; count < (G->numEdges); count++)          // store the weights
    {
        fscanf(fp, "%d, %d, %d", &firstVertex, &secondVertex, &weight);
        G->arc[firstVertex][secondVertex] = weight;
        G->arc[secondVertex][firstVertex] = G->arc[firstVertex][secondVertex];		// Because the undirected graph matrix is symmetric
        //printf("%d, %d, %d", firstVertex, secondVertex, G->arc[firstVertex][secondVertex]);
        //printf("\n");
    }

    fclose(fp);
    return;
}

//******************** DFS TREE using recursion ********************
void DFS(Graph G, int i, bool visited[])
{
    int j;
    visited[i] = 1;
    //printf("%d", G.verx[i]);			                    
    for (j = 0; j < G.numVertexes; j++)
        if (G.arc[i][j] > 0 && !visited[j])			         // if the adjacent vertex not visited
        {
            pred[j] = i;
            DFS(G, j, visited);                               //recursion to unvisited vertex
        }

}

void printDFS(Graph G)
{
    int i, j, k;
    bool* visited = (bool*)malloc(sizeof(bool) * G.numVertexes); // bool visited[V]
    for (i = 0; i < G.numVertexes; i++)		// initialize all the vertex as unvisted
        visited[i] = 0;
    for (j = 0; j < G.numVertexes; j++)		//DFS is called for unvisited nodes. Since it is a connected graph, it will be executed only once
        if (!visited[j])
            DFS(G, j, visited);
    for (k = 1; k < G.numVertexes; k++)
        printf("%d--%d  ", pred[k], k);
}

//******************** Kruskal's Algorithm to construct MST ********************

static int get_position(Graph G, int k)           // return the position of vertex in the matrix
{
    int i;
    for (i = 0; i < G.numVertexes; i++)
        if (G.verx[i] == k)
            return i;
    return -1;
}

EData* get_edges(Graph G)                        // return edges
{
    int i, j;
    int index = 0;
    EData* edges;

    edges = (EData*)malloc(G.numEdges * sizeof(EData));
    for (i = 0; i < G.numVertexes; i++)
    {
        for (j = i + 1; j < G.numVertexes; j++)
        {
            if (G.arc[i][j] != 0)
            {
                edges[index].start = G.verx[i];
                edges[index].end = G.verx[j];
                edges[index].weight = G.arc[i][j];
                index++;
            }
        }
    }

    return edges;
}

void sorted_edges(EData* edges, int elen)        // sort edges
{
    int i, j;

    for (i = 0; i < elen; i++)
    {
        for (j = i + 1; j < elen; j++)
        {
            if (edges[i].weight > edges[j].weight)
            {
                // swap the ith edge and jth edge
                EData tmp = edges[i];
                edges[i] = edges[j];
                edges[j] = tmp;
            }
        }
    }
}

int get_end(int vends[], int i)                 // return the end of i
{
    while (vends[i] != 0)
        i = vends[i];
    return i;
}

void kruskal(Graph G)
{
    int i, m, n, p1, p2;
    int length;
    int index = 0;          // index of rets array
    int vends[MAXVEX] = { 0 };     // save the end point of each vertex in the existing MST in the minimum tree
    EData rets[MAXVEX];        // result array to store the edges in MST
    EData* edges;              // All the edges in the graph

    // get all the edges in the graph
    edges = get_edges(G);
    // sort the edges according to their weights (small to large)
    sorted_edges(edges, G.numEdges);

    for (i = 0; i < G.numEdges; i++)
    {
        p1 = get_position(G, edges[i].start);   // get the number of the starting node of the ith edge
        p2 = get_position(G, edges[i].end);     // get the number of the end node of the ith edge

        m = get_end(vends, p1);                 // get the end node of p1 in the existing MST
        n = get_end(vends, p2);                 // get the end node of p2 in the existing MST

        if (m != n)                             // if m!=n£¬edge i and the vertex added to the MST do not form a loop
        {
            vends[m] = n;                       // the end node of m is n
            rets[index++] = edges[i];           // save the result
        }
    }
    free(edges);

    // print MST using Kruskal
    for (i = 0; i < index; i++)
        printf("%d--%d  ", rets[i].start, rets[i].end);
}

//******************** Dijkstra's Algorithm to construct SPT ********************


int minDistance(int dist[], bool sptSet[], Graph G)           // return the vertex with minmum distance
{
    int min = INT_MAX;
    int min_index;
    for (int i = 0; i < G.numVertexes; i++)
    {
        if (sptSet[i] == false && dist[i] < min)
        {
            min = dist[i];
            min_index = i;
        }
    }
    return min_index;
}

void dijkstra(Graph G, int src)
{
    int V;
    V = G.numVertexes;
    int* dist = (int*)malloc(sizeof(int) * V); // int dist[6]
    bool* sptSet = (bool*)malloc(sizeof(bool) * V); // bool sptSet[V];

    // initialize dist and sptSet
    for (int i = 0; i < V; i++)
    {
        dist[i] = INT_MAX;
        sptSet[i] = false;
    }
    dist[src] = 0;

    // find the shortest path for all adjacent vertex
    for (int count = 0; count < V - 1; count++)
    {
        int u = minDistance(dist, sptSet, G);
        //printf("%d\n",u);
        sptSet[u] = true;
        for (int v = 0; v < V; v++)
        {
            if (sptSet[v] == false && G.arc[u][v] && dist[u] + G.arc[u][v] < dist[v])
            {
                dist[v] = dist[u] + G.arc[u][v];
                pred[v] = u;
                printf("%d--%d  ", pred[v], v);
            }

        }
    }
}

int main(void)
{
    int i, j;
    char con;
    char input[256];
    Graph G;
    FILE* fp;

    printf("************ Welcome to Tree Algorithms System ************\n\n");
    while (1)
    {
        printf("\n***************\n");
        printf("Press i to input file, otherwise, press CTRL+Z to exit the system: ");
        con = getchar();
        while (con != EOF)
        {
            printf("***************\n");
            printf("Input the file name:\n");
            scanf("%s", input);
            fp = fopen(input, "r");

            if (fp == NULL)
                printf("The input file does not exist, please try another one:\n");
            else
            {
                CreateGraph(&G, input, fp);                                                  // create the adjacent matrix

                printf("***************\n\n");
                printf("1. The following are the edges in the constructed DFS Tree\n");
                printDFS(G);

                printf("\n\n***************\n\n");
                printf("3. The following are the edges in the constructed MST:\n");
                kruskal(G);

                printf("\n\n***************\n\n");
                printf("4. The following are the edges in the constructed SPT:\n");
                dijkstra(G, 0);

                printf("\n***************\n");
                printf("Press i to continue to input file, otherwise, press CTRL+Z to exit the system: ");
                getchar();
                con = getchar();
                if (con == EOF)
                    break;
            }
        }
        break;
    }
}

/*
    ²âÊÔ£¬´òÓ¡vertexºÍ¾ØÕó
    for (i = 0; i < G.numVertexes; i++)
        printf("%d ", G.verx[i]);
    printf("\n");
    for (i = 0; i < G.numVertexes; i++)
    {
        for (j = 0; j < G.numVertexes; j++)
            printf("%d  ", G.arc[i][j]);
        printf("\n");
    }
    */