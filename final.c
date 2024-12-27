#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define MAX_NODES 100
#define MAX_HOUSING_PRICE 1000 // Replace with an appropriate value
#define MAX_DISTANCE 1000      // Replace with an appropriate value
#define INF 999999             // Replace with an appropriate value

// Structure to represent a weighted edge
struct Edge
{
    int destination;
    int distance;
    char traffic;
};

// Structure to represent a graph node
struct Node
{
    int location;
    int housingPrice;
    char type[20];
    int subtreeCount; // Count of nodes in the subtree rooted at this node
    int subtreeSum;   // Sum of housing prices in the subtree rooted at this node
};

// Structure to represent the graph
struct Graph
{
    struct Node nodes[MAX_NODES];
    struct Edge edges[MAX_NODES][MAX_NODES];
    int numNodes;
};

// AVL tree
struct AVLNode
{
    struct Node *data;
    int height;
    int subtreeCount;
    int subtreeSum;
    struct AVLNode *left;
    struct AVLNode *right;
};

struct SegmentTreeNode
{
    int left, right;
    int sum;
    int count;
};

// Function to get the height of an AVL tree node
int getHeight(struct AVLNode *node)
{
    if (node == NULL)
        return 0;
    return node->height;
}

// Function to get the balance factor of an AVL tree node
int getBalance(struct AVLNode *node)
{
    if (node == NULL)
        return 0;
    return getHeight(node->left) - getHeight(node->right);
}

// Function to perform a right rotation in AVL tree
struct AVLNode *rightRotate(struct AVLNode *y)
{
    struct AVLNode *x = y->left;
    struct AVLNode *T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = 1 + fmax(getHeight(y->left), getHeight(y->right));
    x->height = 1 + fmax(getHeight(x->left), getHeight(x->right));

    return x;
}

// Function to perform a left rotation in AVL tree
struct AVLNode *leftRotate(struct AVLNode *x)
{
    struct AVLNode *y = x->right;
    struct AVLNode *T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update heights
    x->height = 1 + fmax(getHeight(x->left), getHeight(x->right));
    y->height = 1 + fmax(getHeight(y->left), getHeight(y->right));

    return y;
}

// Function to insert a node into AVL tree
struct AVLNode *insertAVL(struct AVLNode *root, struct Node *data)
{
    if (root == NULL)
    {
        struct AVLNode *newNode = (struct AVLNode *)malloc(sizeof(struct AVLNode));
        newNode->data = data;
        newNode->left = newNode->right = NULL;
        newNode->height = 1;
        return newNode;
    }

    if (data->housingPrice < root->data->housingPrice)
        root->left = insertAVL(root->left, data);
    else if (data->housingPrice > root->data->housingPrice)
        root->right = insertAVL(root->right, data);
    else
        return root; // Duplicate keys are not allowed

    // Update height of the current node
    root->height = 1 + fmax(getHeight(root->left), getHeight(root->right));

    // Get the balance factor to check for rotation
    int balance = getBalance(root);

    // Left Left Case
    if (balance > 1 && data->housingPrice < root->left->data->housingPrice)
        return rightRotate(root);

    // Right Right Case
    if (balance < -1 && data->housingPrice > root->right->data->housingPrice)
        return leftRotate(root);

    // Left Right Case
    if (balance > 1 && data->housingPrice > root->left->data->housingPrice)
    {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    // Right Left Case
    if (balance < -1 && data->housingPrice < root->right->data->housingPrice)
    {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    root->subtreeCount++;
    root->subtreeSum += data->housingPrice;

    return root;
}

// Function to find the node with the maximum value in AVL tree
struct AVLNode *findMaxAVL(struct AVLNode *root)
{
    while (root->right != NULL)
        root = root->right;
    return root;
}

// Function to delete a node from AVL tree
struct AVLNode *deleteNodeAVL(struct AVLNode *root, int housingPrice)
{
    if (root == NULL)
        return root;

    if (housingPrice < root->data->housingPrice)
        root->left = deleteNodeAVL(root->left, housingPrice);
    else if (housingPrice > root->data->housingPrice)
        root->right = deleteNodeAVL(root->right, housingPrice);
    else
    {
        if (root->left == NULL)
        {
            struct AVLNode *temp = root->right;
            free(root);
            return temp;
        }
        else if (root->right == NULL)
        {
            struct AVLNode *temp = root->left;
            free(root);
            return temp;
        }

        struct AVLNode *temp = findMaxAVL(root->left);
        root->data = temp->data;
        root->left = deleteNodeAVL(root->left, temp->data->housingPrice);
    }

    // Update height of the current node
    root->height = 1 + fmax(getHeight(root->left), getHeight(root->right));

    // Get the balance factor to check for rotation
    int balance = getBalance(root);

    // Left Left Case
    if (balance > 1 && getBalance(root->left) >= 0)
        return rightRotate(root);

    // Left Right Case
    if (balance > 1 && getBalance(root->left) < 0)
    {
        root->left = leftRotate(root->left);
        return rightRotate(root);
    }

    // Right Right Case
    if (balance < -1 && getBalance(root->right) <= 0)
        return leftRotate(root);

    // Right Left Case
    if (balance < -1 && getBalance(root->right) > 0)
    {
        root->right = rightRotate(root->right);
        return leftRotate(root);
    }

    root->subtreeCount--;
    root->subtreeSum -= housingPrice;

    return root;
}

// Function to display the AVL tree in-order
void inOrderTraversal(struct AVLNode *root)
{
    if (root != NULL)
    {
        inOrderTraversal(root->left);
        printf("Node %d: Housing Price = %d\n", root->data->location,
               root->data->housingPrice);
        inOrderTraversal(root->right);
    }
}

struct AVLNode *buildAVLTree(struct Graph *graph)
{
    struct AVLNode *avlTree = NULL;
    for (int i = 0; i < graph->numNodes; ++i)
    {
        if (strcmp(graph->nodes[i].type, "empty") == 0)
        {
            avlTree = insertAVL(avlTree, &graph->nodes[i]);
        }
    }
    return avlTree;
}

// Function to find and display empty land with the highest cost using AVL tree
void findAndDisplayMaxCostEmptyLand(struct AVLNode *root)
{
    if (root == NULL)
    {
        printf("No empty lands in AVL tree.\n");
        return;
    }

    struct AVLNode *maxNode = findMaxAVL(root);

    printf("Empty Land with Highest Cost: Node %d, Housing Price = %d\n",
           maxNode->data->location, maxNode->data->housingPrice);
}

// Declare a global array to keep track of chosen optimal locations
int chosenOptimalLocations[MAX_NODES];

// Function to find the optimal location excluding already chosen optimal
// Function to find the optimal location for a house excluding specific locations
// Input: Graph structure representing the city, distance matrix, next matrix, array of excluded locations
// Output: Optimal location for the house
// Description: Uses Dijkstra's algorithm to find the optimal location for a house, excluding specified locations.
int findOptimalLocationExcluding(struct Graph *graph,
                                 int dist[MAX_NODES][MAX_NODES],
                                 int next[MAX_NODES][MAX_NODES],
                                 int excludedLocations[MAX_NODES])
{
    int minCost = INF;
    int optimalLocation = -1;

    for (int i = 0; i < graph->numNodes; ++i)
    {
        if (strcmp(graph->nodes[i].type, "empty") == 0)
        {
            int alreadyChosen = 0;
            for (int j = 0; j < MAX_NODES; ++j)
            {
                if (excludedLocations[j] == i + 1)
                {
                    alreadyChosen = 1;
                    break;
                }
            }

            if (!alreadyChosen)
            {
                int currentCost = graph->nodes[i].housingPrice;

                for (int j = 0; j < graph->numNodes; ++j)
                {
                    currentCost += dist[i][j];
                    if (graph->edges[i][j].traffic == 'L')
                    {
                        currentCost += 1;
                    }
                    else if (graph->edges[i][j].traffic == 'M')
                    {
                        currentCost += 2;
                    }
                    else if (graph->edges[i][j].traffic == 'H')
                    {
                        currentCost += 3;
                    }
                }

                if (currentCost < minCost)
                {
                    minCost = currentCost;
                    optimalLocation = i + 1;
                }
            }
        }
    }

    return optimalLocation;
}

// Function to add an edge to the graph
void addEdge(struct Graph *graph, int source, int destination, int distance,
             char traffic)
{
    graph->edges[source][destination].destination = destination;
    graph->edges[source][destination].distance = distance;
    graph->edges[source][destination].traffic = traffic;
}

// Function to input data for each node
void inputNodeData(struct Graph *graph)
{
    printf("Enter the number of nodes: ");
    scanf("%d", &graph->numNodes);

    for (int i = 0; i < graph->numNodes; ++i)
    {
        printf("\nNode %d:\n", i + 1);
        graph->nodes[i].location = i + 1;

        printf("Enter housing price: ");
        scanf("%d", &graph->nodes[i].housingPrice);

        printf("Enter node type (shop, park, gas station, mall, emptyland): ");
        scanf("%s", graph->nodes[i].type);
    }
}

// Function to input data for each edge (distance and traffic)
void inputEdgeData(struct Graph *graph)
{
    char manualInput;
    printf("Do you want to manually input edges? (y/n): ");
    scanf(" %c", &manualInput);

    if (manualInput == 'y' || manualInput == 'Y')
    {
        int numEdges;
        printf("Enter the number of edges: ");
        scanf("%d", &numEdges);

        printf(
            "Enter edges (source destination distance traffic) - one per line:\n");

        for (int i = 0; i < numEdges; ++i)
        {
            int source, destination, distance;
            char traffic[10];

            scanf("%d %d %d %s", &source, &destination, &distance, traffic);

            addEdge(graph, source - 1, destination - 1, distance, traffic[0]);
            addEdge(graph, destination - 1, source - 1, distance,
                    traffic[0]); // Make the graph undirected
        }
    }
    else
    {
        for (int i = 0; i < graph->numNodes; ++i)
        {
            for (int j = i + 1; j < graph->numNodes; ++j)
            {
                printf("\nEnter distance between Node %d and Node %d: ", i + 1, j + 1);
                scanf("%d", &graph->edges[i][j].distance);
                graph->edges[j][i].distance =
                    graph->edges[i][j].distance; // Make the graph undirected

                printf("Enter traffic between Node %d and Node %d (Low(L), Medium(M), "
                       "High(H)): ",
                       i + 1, j + 1);
                char trafficInput[10];
                scanf(" %c", trafficInput);

                if (strcmp(trafficInput, "L") == 0)
                {
                    graph->edges[i][j].traffic = 'L';
                    graph->edges[j][i].traffic = 'L';
                }
                else if (strcmp(trafficInput, "M") == 0)
                {
                    graph->edges[i][j].traffic = 'M';
                    graph->edges[j][i].traffic = 'M';
                }
                else if (strcmp(trafficInput, "H") == 0)
                {
                    graph->edges[i][j].traffic = 'H';
                    graph->edges[j][i].traffic = 'H';
                }
                else
                {
                    printf("Invalid traffic input. Using default value 'L'.\n");
                    graph->edges[i][j].traffic = 'L';
                    graph->edges[j][i].traffic = 'L';
                }
            }
        }
    }
}

// Function to calculate the overall cost for each location
void calculateOverallCost(struct Graph *graph)
{
    for (int i = 0; i < graph->numNodes; ++i)
    {
        int overallCost = graph->nodes[i].housingPrice;

        for (int j = 0; j < graph->numNodes; ++j)
        {
            overallCost += graph->edges[i][j].distance;
            // Additional cost based on traffic
            if (graph->edges[i][j].traffic == 'L')
            {
                overallCost += 1; // Low traffic
            }
            else if (graph->edges[i][j].traffic == 'M')
            {
                overallCost += 2; // Medium traffic
            }
            else if (graph->edges[i][j].traffic == 'H')
            {
                overallCost += 3; // High traffic
            }
        }

        printf("Overall Cost for Node %d (%s): %d\n", i + 1, graph->nodes[i].type,
               overallCost);
    }
}

// Function to run Floyd's algorithm and display shortest paths
void floydsAlgorithm(struct Graph *graph, int dist[MAX_NODES][MAX_NODES],
                     int next[MAX_NODES][MAX_NODES])
{
    // Initialize dist and next matrices
    for (int i = 0; i < graph->numNodes; ++i)
    {
        for (int j = 0; j < graph->numNodes; ++j)
        {
            dist[i][j] = graph->edges[i][j].distance;
            next[i][j] = (i != j && dist[i][j] < INF) ? j : -1;
        }
    }

    // Floyd's algorithm
    for (int k = 0; k < graph->numNodes; ++k)
    {
        for (int i = 0; i < graph->numNodes; ++i)
        {
            for (int j = 0; j < graph->numNodes; ++j)
            {
                if (dist[i][j] > dist[i][k] + dist[k][j])
                {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    next[i][j] = next[i][k];
                }
            }
        }
    }
}

// Function to check if the graph is connected
bool isGraphConnected(struct Graph *graph, int visited[MAX_NODES],
                      int startNode)
{
    // Depth-First Search (DFS) to check connectivity
    if (startNode < 0 || startNode >= graph->numNodes)
    {
        return false;
    }

    visited[startNode] = 1;

    for (int i = 0; i < graph->numNodes; ++i)
    {
        if (graph->edges[startNode][i].distance != INF && !visited[i])
        {
            isGraphConnected(graph, visited, i);
        }
    }

    for (int i = 0; i < graph->numNodes; ++i)
    {
        if (!visited[i])
        {
            return false;
        }
    }

    return true;
}

// Function to find the optimal location based on the minimum overall cost
int findOptimalLocation(struct Graph *graph, int dist[MAX_NODES][MAX_NODES],
                        int next[MAX_NODES][MAX_NODES])
{
    int minCost = INF;
    int optimalLocation = -1;

    for (int i = 0; i < graph->numNodes; ++i)
    {
        if (strcmp(graph->nodes[i].type, "empty") == 0)
        {
            int currentCost = graph->nodes[i].housingPrice;

            for (int j = 0; j < graph->numNodes; ++j)
            {
                currentCost += graph->edges[i][j].distance;
                // Additional cost based on traffic
                if (graph->edges[i][j].traffic == 'L')
                {
                    currentCost += 1; // Low traffic
                }
                else if (graph->edges[i][j].traffic == 'M')
                {
                    currentCost += 2; // Medium traffic
                }
                else if (graph->edges[i][j].traffic == 'H')
                {
                    currentCost += 3; // High traffic
                }
            }

            if (currentCost < minCost)
            {
                minCost = currentCost;
                optimalLocation = i + 1; // Adding 1 to convert to 1-indexing
            }
        }
    }

    return optimalLocation;
}

// Function to perform heapify operation on an array of nodes
// Input: Array of nodes, size of the array, index for heapification
// Output: None
// Description: Ensures that the array satisfies the heap property at a given index.
void heapify(struct Node arr[], int n, int i)
{
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if (left < n && arr[left].housingPrice > arr[largest].housingPrice)
        largest = left;

    if (right < n && arr[right].housingPrice > arr[largest].housingPrice)
        largest = right;

    if (largest != i)
    {
        struct Node temp = arr[i];
        arr[i] = arr[largest];
        arr[largest] = temp;

        heapify(arr, n, largest);
    }
}

// Function to perform heap sort on an array of nodes
// Input: Array of nodes, size of the array
// Output: None
// Description: Sorts the array of nodes in non-decreasing order using heap sort.

void heapSort(struct Node arr[], int n)
{
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    for (int i = n - 1; i > 0; i--)
    {
        struct Node temp = arr[0];
        arr[0] = arr[i];
        arr[i] = temp;

        heapify(arr, i, 0);
    }
}

// Function to list all nodes in increasing order of housing cost
// Function to list all nodes in increasing order of housing cost
// Input: Graph structure representing the city
// Output: None (Prints the list to the console)
// Description: Prints the list of nodes sorted by housing cost to the console.
void listAllNodes(struct Graph *graph)
{
    struct Node sortedNodes[MAX_NODES];
    for (int i = 0; i < graph->numNodes; ++i)
    {
        sortedNodes[i] = graph->nodes[i];
    }

    heapSort(sortedNodes, graph->numNodes);

    printf("\nList of All Nodes in Increasing Order of Housing Cost:\n");
    for (int i = 0; i < graph->numNodes; ++i)
    {
        printf("Node %d: Housing Price = %d\n", sortedNodes[i].location,
               sortedNodes[i].housingPrice);
    }
}

// Function to list all edges with distance and traffic values
// Function to list all edges with distance and traffic values
// Input: Graph structure representing the city
// Output: None (Prints the list to the console)
// Description: Prints the list of edges with distance and traffic values to the console.

void listAllEdges(struct Graph *graph)
{
    printf("\nList of All Edges with Distance and Traffic Values:\n");
    for (int i = 0; i < graph->numNodes; ++i)
    {
        for (int j = i + 1; j < graph->numNodes; ++j)
        {
            printf("Edge between Node %d and Node %d: Distance = %d, Traffic = %c\n",
                   i + 1, j + 1, graph->edges[i][j].distance,
                   graph->edges[i][j].traffic);
        }
    }
}

// Function to display the shortest path from the optimal empty land to a
// specified location
// Function to display the shortest path from the optimal empty land to a specified location
// Input: Graph structure representing the city, next matrix, optimal empty land, destination node
// Output: None (Prints the shortest path to the console)
// Description: Uses the next matrix to display the shortest path from the optimal empty land to a specified location.
void displayShortestPath(struct Graph *graph, int next[MAX_NODES][MAX_NODES],
                         int optimalEmptyLand, int destination)
{
    printf("Optimal Empty Land to Node %d (%s): ", destination,
           graph->nodes[destination - 1].type);
    int currentNode = optimalEmptyLand;
    while (next[currentNode][destination] != destination)
    {
        printf("%d -> ", next[currentNode][destination] + 1);
        currentNode = next[currentNode][destination];
    }

    printf("%d\n", destination);
}

// Function to display the menu and execute corresponding actions
// Function to display the menu and execute corresponding actions
// Input: None
// Output: None (Prints the menu to the console)
// Description: Displays the menu to the console and executes the corresponding actions based on user input.
void displayMenu()
{
    printf("\n<------------------------------------------------------------------------->\n\n");
    printf("\nMenu:\n");
    printf("1. Find the best optimal location for the house\n");
    printf("2. Find the second best optimal location for the house\n");
    printf("3. Find the third best optimal location for the house\n");
    printf("4. Find empty land with the highest cost\n");
    printf("5. List all nodes in increasing order of housing cost\n");
    printf("6. List all edges with distance and traffic values\n");
    printf("7. Calculate average housing cost in the area\n");
    printf("8. Display City\n"); // New option
    printf("Enter your choice (1-8): ");
}

// Function to run Dijkstra's algorithm and display shortest paths
// Function to run Dijkstra's algorithm and display shortest paths
// Input: Graph structure representing the city, distance array, next array, starting node
// Output: None (Prints the shortest distances to the console)
// Description: Executes Dijkstra's algorithm and prints the shortest distances to the console.
void dijkstrasAlgorithm(struct Graph *graph, int dist[MAX_NODES],
                        int next[MAX_NODES], int startNode)
{
    bool visited[MAX_NODES] = {false};

    for (int i = 0; i < graph->numNodes; ++i)
    {
        dist[i] = INF;
        next[i] = -1;
    }

    dist[startNode] = 0;

    for (int count = 0; count < graph->numNodes - 1; ++count)
    {
        int u = -1;

        for (int i = 0; i < graph->numNodes; ++i)
        {
            if (!visited[i] && (u == -1 || dist[i] < dist[u]))
                u = i;
        }

        visited[u] = true;

        for (int v = 0; v < graph->numNodes; ++v)
        {
            int alt = dist[u] + graph->edges[u][v].distance;
            if (!visited[v] && graph->edges[u][v].distance != INF && alt < dist[v])
            {
                dist[v] = alt;
                next[v] = u;
            }
        }
    }
}

// Function to run Bellman-Ford algorithm and display shortest paths
// Function to run Bellman-Ford algorithm and display shortest paths
// Input: Graph structure representing the city, distance array, next array, starting node
// Output: None (Prints the shortest distances to the console)
// Description: Executes Bellman-Ford algorithm and prints the shortest distances to the console.
void bellmanFordAlgorithm(struct Graph *graph, int dist[MAX_NODES],
                          int next[MAX_NODES], int startNode)
{
    for (int i = 0; i < graph->numNodes; ++i)
    {
        dist[i] = INF;
        next[i] = -1;
    }

    dist[startNode] = 0;

    for (int count = 0; count < graph->numNodes - 1; ++count)
    {
        for (int u = 0; u < graph->numNodes; ++u)
        {
            for (int v = 0; v < graph->numNodes; ++v)
            {
                int alt = dist[u] + graph->edges[u][v].distance;
                if (graph->edges[u][v].distance != INF && alt < dist[v])
                {
                    dist[v] = alt;
                    next[v] = u;
                }
            }
        }
    }
}

// Updated main function
// Function to display the total distance from the optimal node to other nodes
// Input: Graph structure representing the city, distance matrix, optimal node
// Output: None (Prints the total distance to the console)
// Description: Displays the total distance from the optimal node to other nodes in the city.
void displayShortestDistances(struct Graph *graph,
                              int dist[MAX_NODES][MAX_NODES], int optimalNode)
{
    printf("\nTotal Distance from Optimal Node to Other Nodes:\n");

    for (int i = 0; i < graph->numNodes; ++i)
    {
        if (i != optimalNode)
        {
            printf("Optimal Node to Node %d (%s): ", i + 1, graph->nodes[i].type);

            // Display the total distance
            printf("%d\n", dist[optimalNode][i]);
        }
    }
}


// Function to display the total distance from the optimal node to other empty nodes
// Input: Graph structure representing the city, distance matrix, optimal node
// Output: None (Prints the total distance to the console)
// Description: Displays the total distance from the optimal node to other empty nodes in the city.
void displayTotalDistanceFromOptimal(struct Graph *graph,
                                     int dist[MAX_NODES][MAX_NODES],
                                     int optimalNode)
{
    printf("\nTotal Distance from Optimal Node to Other Nodes:\n");

    for (int i = 0; i < graph->numNodes; ++i)
    {
        if (i != optimalNode && strcmp(graph->nodes[i].type, "empty") == 0)
        {
            printf("Optimal Node to Node %d (%s): ", i + 1, graph->nodes[i].type);

            // Display the total distance
            printf("%d\n", dist[optimalNode][i]);
        }
    }
}

// Function to check if there are any empty lands left
// Function to check if there are any empty lands left
// Input: Graph structure representing the city
// Output: Boolean indicating whether there are empty lands
// Description: Checks if there are any empty lands left in the city.
bool hasEmptyLands(struct Graph *graph)
{
    for (int i = 0; i < graph->numNodes; ++i)
    {
        if (strcmp(graph->nodes[i].type, "empty") == 0)
        {
            return true;
        }
    }
    return false;
}

// Function to calculate the average housing cost of empty lands in the area
// Input: Graph structure representing the city
// Output: Average housing cost (float)
// Description: Calculates and returns the average housing cost of empty lands in the city.
float calculateAverageHousingCost(struct Graph *graph)
{
    int totalCost = 0;
    int numEmptyLands = 0;

    for (int i = 0; i < graph->numNodes; ++i)
    {
        if (strcmp(graph->nodes[i].type, "empty") == 0)
        {
            totalCost += graph->nodes[i].housingPrice;
            numEmptyLands++;
        }
    }

    if (numEmptyLands == 0)
    {
        printf("No empty lands in the area.\n");
        return 0.0;
    }

    return (float)totalCost / numEmptyLands;
}

// Function to display the city graph representation
// Input: Graph structure representing the city
// Output: None (Prints the graph to the console)
// Description: Displays the city graph representation to the console.
void displayCityGraph(struct Graph *graph)
{
    printf("\nCity Graph Representation:\n");

    for (int i = 0; i < graph->numNodes; ++i)
    {
        printf("Node %d (%s): ", i + 1, graph->nodes[i].type);

        for (int j = 0; j < graph->numNodes; ++j)
        {
            if (graph->edges[i][j].distance != INF)
            {
                printf("%d", j + 1);

                if (graph->edges[i][j].traffic == 'L')
                {
                    printf(" ->(L) ");
                    printf("\n");
                }
                else if (graph->edges[i][j].traffic == 'M')
                {
                    printf(" ->(M) ");
                    printf("\n");
                }
                else if (graph->edges[i][j].traffic == 'H')
                {
                    printf(" ->(H) ");
                    printf("\n");
                }
                else
                    printf(" -> ");
            }
        }

        printf("\n");
        printf("\n");
    }
}

// Function to generate a random city graph
// Function to generate a random city graph
// Input: Graph structure representing the city
// Output: None (Generates random data for the city graph)
// Description: Populates the city graph with random data for testing and demonstration purposes.
void generateRandomCity(struct Graph *graph)
{
    const int MAX_NODES_LIMIT = 30;    // Maximum number of nodes for the random city
    const int MIN_HOUSING_PRICE = 100; // Minimum housing price
                                       // Maximum housing price

    srand(time(NULL)); // Seed for random number generation

    // Random number of nodes (1 to MAX_NODES_LIMIT)
    int numNodes = rand() % MAX_NODES_LIMIT + 1;

    // Adjust the ranges based on your requirements
    for (int i = 0; i < numNodes; ++i)
    {
        graph->nodes[i].location =
            i + 1; // Assuming locations are numbered from 1 to numNodes
        graph->nodes[i].housingPrice =
            rand() % (MAX_HOUSING_PRICE - MIN_HOUSING_PRICE + 1) +
            MIN_HOUSING_PRICE; // Random housing price

        // Assuming five types of locations: "empty", "residential", "commercial",
        // "mall", "park", "shop", "gas_station"
        const char *locationTypes[] = {"empty", "residential", "commercial", "mall",
                                       "park", "shop", "gas_station"};
        int typeIndex = rand() % 7; // Random index for location type
        strcpy(graph->nodes[i].type, locationTypes[typeIndex]);

        for (int j = 0; j < numNodes; ++j)
        {
            if (i != j)
            {
                // Assuming MAX_DISTANCE is the maximum distance between nodes
                graph->edges[i][j].distance =
                    rand() % MAX_DISTANCE + 1; // Random distance

                // Assuming three levels of traffic: 'L', 'M', 'H'
                const char trafficLevels[] = {'L', 'M', 'H'};
                int trafficIndex = rand() % 3; // Random index for traffic level
                graph->edges[i][j].traffic = trafficLevels[trafficIndex];
            }
            else
            {
                // No self-loops
                graph->edges[i][j].distance = INF;
                graph->edges[i][j].traffic = 'N';
            }
        }
    }

    // Make the graph connected
    for (int i = 1; i < numNodes; ++i)
    {
        int randomNeighbor =
            rand() % i; // Choose a random already created node as a neighbor
        graph->edges[i][randomNeighbor].distance =
            rand() % MAX_DISTANCE + 1; // Add an edge to make the graph connected
        graph->edges[randomNeighbor][i] =
            graph->edges[i][randomNeighbor]; // Make the graph undirected
    }

    graph->numNodes = numNodes;
}


// Main function to execute the program
// Input: None
// Output: Returns 0 upon successful execution
// Description: The main entry point of the program. Executes the main logic and controls the flow of the program.

int main()
{
    struct Graph cityGraph;
    int dist[MAX_NODES][MAX_NODES];
    int next[MAX_NODES][MAX_NODES];
    struct AVLNode *avlTree = NULL;

    int choice;

    int optimalLocation;

    // Initialize the 'next' array before entering the loop
    for (int i = 0; i < MAX_NODES; ++i)
    {
        for (int j = 0; j < MAX_NODES; ++j)
        {
            next[i][j] = -1;
        }
    }

    int chosenOptimalLocations[MAX_NODES];
    for (int i = 0; i < MAX_NODES; ++i)
    {
        chosenOptimalLocations[i] = -1;
    }

    char generateRandomCityChoice;
    printf("\033[1;36m");
    printf("\n Welcome to Urban-Aalaya, where your dream home awaits. Discover optimal living with our personalized real estate solutions. \n");
    printf("\033[0m");
    printf("\n");
    printf("Do you want to generate a random city? (Y/N): ");
    scanf(" %c", &generateRandomCityChoice);

    if (generateRandomCityChoice == 'Y' || generateRandomCityChoice == 'y')
    {
        generateRandomCity(&cityGraph);
        printf("Random city generated.\n");
        // Check if the graph is connected
        int dist[MAX_NODES][MAX_NODES];
        int next[MAX_NODES][MAX_NODES];
        floydsAlgorithm(&cityGraph, dist,
                        next); // Initialize next array with Floyd's algorithm

        calculateOverallCost(&cityGraph);

        int choice;
        int optimalLocation;

        // Initialize the 'next' array before entering the loop
        for (int i = 0; i < MAX_NODES; ++i)
        {
            for (int j = 0; j < MAX_NODES; ++j)
            {
                next[i][j] = -1;
            }
        }

        int chosenOptimalLocations[MAX_NODES];
        for (int i = 0; i < MAX_NODES; ++i)
        {
            chosenOptimalLocations[i] = -1;
        }

        struct AVLNode *avlTree = NULL;
        do
        {

            displayMenu();
            scanf("%d", &choice);

            switch (choice)
            {
            case 1:
                printf("Choose algorithm for finding total distance:\n");
                printf("1. Floyd's Algorithm\n");
                printf("2. Bellman-Ford Algorithm\n");
                printf("\n");
                printf("Enter your choice (1, 2): ");
                int algorithmChoice1;
                scanf("%d", &algorithmChoice1);

                switch (algorithmChoice1)
                {
                case 1:
                    optimalLocation = findOptimalLocationExcluding(
                        &cityGraph, dist, next, chosenOptimalLocations);
                    chosenOptimalLocations[0] = optimalLocation;
                    floydsAlgorithm(&cityGraph, dist[optimalLocation - 1],next[optimalLocation - 1]);
                    break;
                case 2:
                   optimalLocation = findOptimalLocationExcluding(
                        &cityGraph, dist, next, chosenOptimalLocations);
                    chosenOptimalLocations[0] = optimalLocation;
                    bellmanFordAlgorithm(&cityGraph, dist[optimalLocation - 1],next[optimalLocation - 1],optimalLocation - 1);
                    break;
                default:
                    printf("Invalid choice. Exiting...\n");
                    return 0;
                }

                if (optimalLocation == -1 || !hasEmptyLands(&cityGraph))
                {
                    printf("\nThere are no empty lands.\n");
                }
                else
                {
                    avlTree = insertAVL(avlTree, &cityGraph.nodes[optimalLocation - 1]);
                    printf("\nThe best optimal location for the house is Node %d (empty "
                           "land).\n",
                           optimalLocation);
                    displayTotalDistanceFromOptimal(&cityGraph, dist,
                                                    optimalLocation - 1);
                }
                break;
            case 2:
                printf("Choose algorithm for finding total distance:\n");
                printf("1. Floyd's Algorithm\n");
                printf("2. Bellman-Ford Algorithm\n");
                printf("\n");
                printf("Enter your choice (1, 2): ");
                int algorithmChoice2;
                scanf("%d", &algorithmChoice2);

                switch (algorithmChoice2)
                {
                case 1:
                    optimalLocation = findOptimalLocationExcluding(
                        &cityGraph, dist, next, chosenOptimalLocations);
                    chosenOptimalLocations[1] = optimalLocation;
                    floydsAlgorithm(&cityGraph, dist[optimalLocation - 1],next[optimalLocation - 1]);
                    break;
                case 2:
                    optimalLocation = findOptimalLocationExcluding(
                        &cityGraph, dist, next, chosenOptimalLocations);
                    chosenOptimalLocations[1] = optimalLocation;
                    bellmanFordAlgorithm(&cityGraph, dist[optimalLocation - 1],next[optimalLocation - 1],optimalLocation - 1);
                    break;
                default:
                    printf("Invalid choice. Exiting...\n");
                    return 0;
                }

                if (optimalLocation == -1 || !hasEmptyLands(&cityGraph))
                {
                    printf("\nThere are no empty lands.\n");
                }
                else
                {
                    avlTree = insertAVL(avlTree, &cityGraph.nodes[optimalLocation - 1]);
                    printf("\nThe second best optimal location for the house is Node %d "
                           "(empty land).\n",
                           optimalLocation);
                    displayTotalDistanceFromOptimal(&cityGraph, dist,
                                                    optimalLocation - 1);
                }
                break;
            case 3:
                printf("Choose algorithm for finding total distance:\n");
                printf("1. Floyd's Algorithm\n");
                printf("2. Bellman-Ford Algorithm\n");
                printf("\n");
                printf("Enter your choice (1, 2): ");
                int algorithmChoice3;
                scanf("%d", &algorithmChoice3);

                switch (algorithmChoice3)
                {
                case 1:
                    optimalLocation = findOptimalLocationExcluding(
                        &cityGraph, dist, next, chosenOptimalLocations);
                    chosenOptimalLocations[2] = optimalLocation;
                    floydsAlgorithm(&cityGraph, dist[optimalLocation - 1],next[optimalLocation - 1]);
                    break;
                case 2:
                    optimalLocation = findOptimalLocationExcluding(
                        &cityGraph, dist, next, chosenOptimalLocations);
                    chosenOptimalLocations[2] = optimalLocation;
                    bellmanFordAlgorithm(&cityGraph, dist[optimalLocation - 1],next[optimalLocation - 1],optimalLocation - 1);
                    break;
                default:
                    printf("Invalid choice. Exiting...\n");
                    return 0;
                }

                if (optimalLocation == -1 || !hasEmptyLands(&cityGraph))
                {
                    printf("\nThere are no empty lands.\n");
                }
                else
                {
                    avlTree = insertAVL(avlTree, &cityGraph.nodes[optimalLocation - 1]);
                    printf("\nThe third best optimal location for the house is Node %d "
                           "(empty land).\n",
                           optimalLocation);
                    displayTotalDistanceFromOptimal(&cityGraph, dist,
                                                    optimalLocation - 1);
                }
                break;
            case 4:
                avlTree = buildAVLTree(&cityGraph);
                printf("\nEmpty land with highest cost found using AVL tree:\n");
                findAndDisplayMaxCostEmptyLand(avlTree);
                break;
            case 5:
                listAllNodes(&cityGraph);
                break;
            case 6:
                listAllEdges(&cityGraph);
                break;

            case 7:
                printf("\nAverage Housing Cost (of empty land) in the Area: %.2f\n",
                       calculateAverageHousingCost(&cityGraph));
                break;
            case 8:
                displayCityGraph(&cityGraph);
                break;

            case 9:
                if (generateRandomCityChoice == 'Y' ||
                    generateRandomCityChoice == 'y')
                {
                    printf("Random city already generated. Choose another option.\n");
                }
                else
                {
                    generateRandomCity(&cityGraph);
                    printf("Random city generated.\n");
                }
                break;

            default:
                printf("Invalid choice. Exiting...\n");
                return 0;
            }

        } while (choice >= 1 && choice <= 9); // Continue the loop for valid choices
    }
    else if (generateRandomCityChoice != 'N' &&
             generateRandomCityChoice != 'n')
    {
        printf("Invalid choice. Exiting...\n");
        return 0;
    }

    // If the user didn't choose to generate a random city or chose to generate
    // one, continue with the usual procedure
    if (generateRandomCityChoice == 'N' || generateRandomCityChoice == 'n' ||
        cityGraph.numNodes == 0)
    {
        // Check if the graph is connected
        int visited[MAX_NODES] = {0};

        inputNodeData(&cityGraph);
        // if (!isGraphConnected(&cityGraph, visited, 0))
        // {
        //     printf("The city is not connected.\n");
        //     return 0;
        // }

        inputEdgeData(&cityGraph);

        int dist[MAX_NODES][MAX_NODES];
        int next[MAX_NODES][MAX_NODES];
        floydsAlgorithm(&cityGraph, dist,
                        next); // Initialize next array with Floyd's algorithm

        calculateOverallCost(&cityGraph);

        int choice;
        int optimalLocation;

        // Initialize the 'next' array before entering the loop
        for (int i = 0; i < MAX_NODES; ++i)
        {
            for (int j = 0; j < MAX_NODES; ++j)
            {
                next[i][j] = -1;
            }
        }

        int chosenOptimalLocations[MAX_NODES];
        for (int i = 0; i < MAX_NODES; ++i)
        {
            chosenOptimalLocations[i] = -1;
        }

        struct AVLNode *avlTree = NULL;

        do
        {

            displayMenu();
            scanf("%d", &choice);

            switch (choice)
            {
            case 1:
                printf("Choose algorithm for finding total distance:\n");
                printf("1. Floyd's Algorithm\n");
                printf("2. Bellman-Ford Algorithm\n");
                printf("\n");
                printf("Enter your choice (1, 2): ");
                int algorithmChoice1;
                scanf("%d", &algorithmChoice1);

                switch (algorithmChoice1)
                {
                case 1:
                    optimalLocation = findOptimalLocationExcluding(
                        &cityGraph, dist, next, chosenOptimalLocations);
                    chosenOptimalLocations[0] = optimalLocation;
                    break;
                case 2:
                    printf("Enter the starting node for Bellman-Ford Algorithm: ");
                    int bellmanFordStartNode;
                    scanf("%d", &bellmanFordStartNode);
                    bellmanFordAlgorithm(&cityGraph, dist[bellmanFordStartNode - 1],
                                         next[bellmanFordStartNode - 1],
                                         bellmanFordStartNode - 1);
                    optimalLocation = findOptimalLocationExcluding(
                        &cityGraph, dist, next, chosenOptimalLocations);
                    chosenOptimalLocations[0] = optimalLocation;
                    break;
                default:
                    printf("Invalid choice. Exiting...\n");
                    return 0;
                }

                if (optimalLocation == -1 || !hasEmptyLands(&cityGraph))
                {
                    printf("\nThere are no empty lands.\n");
                }
                else
                {
                    avlTree = insertAVL(avlTree, &cityGraph.nodes[optimalLocation - 1]);
                    printf("\nThe best optimal location for the house is Node %d (empty "
                           "land).\n",
                           optimalLocation);
                    displayTotalDistanceFromOptimal(&cityGraph, dist,
                                                    optimalLocation - 1);
                }
                break;
            case 2:
                printf("Choose algorithm for finding total distance:\n");
                printf("1. Floyd's Algorithm\n");
                printf("2. Bellman-Ford Algorithm\n");
                printf("\n");
                printf("Enter your choice (1, 2): ");
                int algorithmChoice2;
                scanf("%d", &algorithmChoice2);

                switch (algorithmChoice2)
                {
                case 1:
                    optimalLocation = findOptimalLocationExcluding(
                        &cityGraph, dist, next, chosenOptimalLocations);
                    chosenOptimalLocations[1] = optimalLocation;
                    break;
                case 2:
                    printf("Enter the starting node for Bellman-Ford Algorithm: ");
                    int bellmanFordStartNode;
                    scanf("%d", &bellmanFordStartNode);
                    bellmanFordAlgorithm(&cityGraph, dist[bellmanFordStartNode - 1],
                                         next[bellmanFordStartNode - 1],
                                         bellmanFordStartNode - 1);
                    optimalLocation = findOptimalLocationExcluding(
                        &cityGraph, dist, next, chosenOptimalLocations);
                    chosenOptimalLocations[1] = optimalLocation;
                    break;
                default:
                    printf("Invalid choice. Exiting...\n");
                    return 0;
                }

                if (optimalLocation == -1 || !hasEmptyLands(&cityGraph))
                {
                    printf("\nThere are no empty lands.\n");
                }
                else
                {
                    avlTree = insertAVL(avlTree, &cityGraph.nodes[optimalLocation - 1]);
                    printf("\nThe second best optimal location for the house is Node %d "
                           "(empty land).\n",
                           optimalLocation);
                    displayTotalDistanceFromOptimal(&cityGraph, dist,
                                                    optimalLocation - 1);
                }
                break;
            case 3:
                printf("Choose algorithm for finding total distance:\n");
                printf("1. Floyd's Algorithm\n");
                printf("2. Bellman-Ford Algorithm\n");
                printf("\n");
                printf("Enter your choice (1, 2): ");
                int algorithmChoice3;
                scanf("%d", &algorithmChoice3);

                switch (algorithmChoice3)
                {
                case 1:
                    optimalLocation = findOptimalLocationExcluding(
                        &cityGraph, dist, next, chosenOptimalLocations);
                    chosenOptimalLocations[2] = optimalLocation;
                    break;
                case 2:
                    printf("Enter the starting node for Bellman-Ford Algorithm: ");
                    int bellmanFordStartNode;
                    scanf("%d", &bellmanFordStartNode);
                    bellmanFordAlgorithm(&cityGraph, dist[bellmanFordStartNode - 1],
                                         next[bellmanFordStartNode - 1],
                                         bellmanFordStartNode - 1);
                    optimalLocation = findOptimalLocationExcluding(
                        &cityGraph, dist, next, chosenOptimalLocations);
                    chosenOptimalLocations[2] = optimalLocation;
                    break;
                default:
                    printf("Invalid choice. Exiting...\n");
                    return 0;
                }

                if (optimalLocation == -1 || !hasEmptyLands(&cityGraph))
                {
                    printf("\nThere are no empty lands.\n");
                }
                else
                {
                    avlTree = insertAVL(avlTree, &cityGraph.nodes[optimalLocation - 1]);
                    printf("\nThe third best optimal location for the house is Node %d "
                           "(empty land).\n",
                           optimalLocation);
                    displayTotalDistanceFromOptimal(&cityGraph, dist,
                                                    optimalLocation - 1);
                }
                break;
            case 4:
                avlTree = buildAVLTree(&cityGraph);
                printf("\nEmpty land with highest cost found using AVL tree:\n");
                findAndDisplayMaxCostEmptyLand(avlTree);
                break;
            case 5:
                listAllNodes(&cityGraph);
                break;
            case 6:
                listAllEdges(&cityGraph);
                break;

            case 7:
                printf("\nAverage Housing Cost (of empty land) in the Area: %.2f\n",
                       calculateAverageHousingCost(&cityGraph));
                break;
            case 8:
                displayCityGraph(&cityGraph);
                break;

            case 9:
                if (generateRandomCityChoice == 'Y' ||
                    generateRandomCityChoice == 'y')
                {
                    printf("Random city already generated. Choose another option.\n");
                }
                else
                {
                    generateRandomCity(&cityGraph);
                    printf("Random city generated.\n");
                }
                break;

            default:
                printf("Invalid choice. Exiting...\n");
                return 0;
            }

        } while (choice >= 1 && choice <= 9); // Continue the loop for valid choices
    }
    return 0;
}