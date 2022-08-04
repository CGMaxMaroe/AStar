#pragma once
#include <string>
#include <list>
#include <queue>
#include <map>

class Node
{
public:
    // Our node's name (for output)
    std::string name;

    // Each node tracks fScore, gScore, and cameFrom, instead of separate maps
    float fScore = std::numeric_limits<float>::infinity();
    float gScore = std::numeric_limits<float>::infinity();
    Node* cameFrom;

    Node(std::string nodeName)
    {
        // We need a name for proper output
        name = nodeName;

        // We add all nodes to this static vector to make ResetNodeScores easy
        allNodes.push_back(this);
    }

    ~Node()
    {
        // If someone deletes a node, we need to remove the node from our list or ResetNodeScores will crash
        allNodes.remove(this);
    }

    void AddConnection(Node* const connectedNode, int cost, bool isOneWay = false)
    {
        // We add the new node to the map of connected (neighbor) nodes
        connectedNodes.insert({ connectedNode, cost });

        // We keep a separate list of neighbors too as we can't easily iterate on C++ map keys
        connectedNodeList.push_back(connectedNode);

        // We also need to connect the new node to this node for bidirectional linkage
        if (!isOneWay)
        {
            connectedNode->AddConnection(this, cost, true);
        }
    }

    std::map<Node*, float> GetConnectedNodes()
    {
        return connectedNodes;
    }

    std::vector<Node*> GetConnectedNodeList()
    {
        return connectedNodeList;
    }

    // Resets all node scores to infinity so we can pathfind more than once
    static void ResetNodeScores()
    {
        for(Node* node : Node::allNodes)
        {
            node->gScore = std::numeric_limits<float>::infinity();
            node->fScore = std::numeric_limits<float>::infinity();
        }
    }
    
    // We need this comparitor for our priority_queue to "just work"
    // For some reason though, it's not "just working" in fact it's not being called at all...
    // I've left it here for someone else to point out my error, instead we explicitly use NodeComparer
    // (I assume my error has something to do with Node* vs Node)
    bool operator<(const Node& nodeToCompare)
    {
        std::cout << "Are we really doing this?";
        // Priority queue sorts high score first, we need low score so note the >
        return this->fScore > nodeToCompare.fScore;
    }

private:
	std::map<Node*,float> connectedNodes;
    std::vector<Node*> connectedNodeList;

    // Stores all nodes for easy reset
    static std::list<Node*> allNodes;
};

// Need this for static vector to work...
std::list <Node*> Node::allNodes;

struct NodeComparer
{
    bool operator() (const Node* lhs, const Node* rhs)
    {
        // Priority queue sorts high score first, we need low score so note the >
        return lhs->fScore > rhs->fScore;
    }
};

class AStar
{
    //  I wish to note I usually organize private after public, but I put private first here to match the order in Wikipedia for reference
private:

    // In my C++ implementation, each node contains the cameFrom pointer, so we only need to pass the endNode to reconstruct_path
    static std::vector<Node*> reconstruct_path(Node* const endNode)
    {
        std::list<Node*> total_path; // Using list as we need push_front
        Node* current = endNode;

        total_path.push_front(current);
        while(current->cameFrom)
        {
            current = current->cameFrom;
            total_path.push_front(current);
        }

        // convert list to vector because main() is lazy
        std::vector<Node*> total_path_vector(total_path.begin(), total_path.end());

        return total_path_vector;
    }

public:
    static std::vector<Node*> FindPath(Node* const start, Node* const goal)
    {
        std::priority_queue<Node*,std::vector<Node*>,NodeComparer> openSet;
        std::list<Node*> openSetList; // We keep a separate list of nodes because you can't check for presence of an element in a c++ priority_queue

        openSet.push(start);
        openSetList.push_back(start);

        // Reset nodes to infinity (in case you want to call FindPath twice...)
        Node::ResetNodeScores();

        // Remember, all scores are infinity at first, so we have to set the scores to 0 for our starting node
        start->gScore = 0;
        start->fScore = 0; // Note: plug in heuristic here if you have one (can't do on non-spatial graph)

        while (!openSet.empty())
        {
            Node* current;

            // Top usually returns the highest value in the priority_queue; in this case it's the lowest value though because we reverse our operator<
            // (In other words, the priority_queue *thinks* the lowest value Node is the highest value Node because that's what they return)
            current = openSet.top();
            if (current == goal)
            {
                return reconstruct_path(current);
            }

            // Pop only removes the element, it doesn't return it
            // We remove from the associated list too
            openSet.pop();
            openSetList.remove(current);

            for (Node* neighbor : current->GetConnectedNodeList())
            {
                // Calculate the tentative score for neighbor
                float tentative_gScore = current->gScore + current->GetConnectedNodes()[neighbor];
                if (tentative_gScore < neighbor->gScore)
                {
                    // New low score! We set the cameFrom pointer to current and officially set the scores for this new best path to neighbor
                    neighbor->cameFrom = current;
                    neighbor->gScore = tentative_gScore;
                    neighbor->fScore = tentative_gScore + 0; // Insert neighbor heuristic here

                    // This says "if neighbor is not in openSetList" in C++'s... elegant language
                    if (!(std::find(openSetList.begin(), openSetList.end(), neighbor) != openSetList.end()))
                    {
                        openSet.push(neighbor);
                        openSetList.push_back(neighbor);
                    }
                }
            }
        }
        return std::vector<Node*>(); // Failure... return empty vector
    }
};