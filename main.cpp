#include "string.h"
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_set>

int gx, gy;
int totalEdges;
int* usages;

using namespace std;

int get_horizontal_right_edge(int x, int y) {
    return  (y * (gx - 1) + x);
}


int get_vertical_upper_edge(int x, int y) {
    return (gx - 1) * gy + (y * gx) + x;
}

int getEdgeId(int x1, int y1, int x2, int y2) {
    int edgeId = -1;
    if(x1 < x2) {
        edgeId = get_horizontal_right_edge(x1, y1);
    } else if (x1 > x2) {
        edgeId = get_horizontal_right_edge(x2, y2);
    } else if (y1 < y2) {
        edgeId = get_vertical_upper_edge(x1, y1);
    } else if (y1 > y2) {
        edgeId = get_vertical_upper_edge(x2, y2);
    } else {
        printf("ERROR: non-adgacent edge lookup\n");
        exit(-1);
    }
    return edgeId;
}

void getEdges(int x1, int y1, int x2, int y2, unordered_set<int>& edges) {
    if (x1 != x2) {
        for (int dx = 0; dx < abs(x1 - x2); dx++) {
            edges.insert(getEdgeId((x1 + dx), y1, (x2 + (dx + 1)), y2));
        }
    } else if (y1 != y2) {
        for (int dy = 0; dy < abs(y1 - y2); dy++) {
            edges.insert(getEdgeId(x1, (y1 + dy), x2, (y2 + (dy + 1))));
        }
    } else {
        printf("<< ERROR: Invalid edge(s) in solution file: (%d,%d)-(%d,%d)\n", x1, y1, x2, y2);
        exit(-1);
    }
//    printf("(%d,%d)-(%d,%d) has %d edges\n", x1, y1, x2, y2, edges.size());
}

int main(int argc, char **argv) {
    string line;
    char junk[80];

 	if(argc!=4){
 		printf("Usage : ./visualize <input_benchmark_filename> <solution_filename> <output_filename>\n");
 		return 1;
 	}

 	int status;

	string inputFilename = argv[1];
	string solutionFilename = argv[2];
 	string outputFilename = argv[3];

 	// read benchmark
    ifstream in(inputFilename);
    if (in.is_open()) {
        int capacity;
        int numNets;
        int numBlockages = -1;
        getline(in,line);
        sscanf(line.c_str(), "%s %d %d", junk, &gx, &gy);
        totalEdges = (2 * gx * gy) - gx - gy;
        usages = (int*) malloc(sizeof(int) * totalEdges);
        getline(in,line);
        sscanf(line.c_str(), "%s %d", junk, &capacity);
        for(int i = 0; i < totalEdges; i++) {
            usages[i] = capacity;
        }
        getline(in,line);
        sscanf(line.c_str(), "%s %s %d", junk, junk, &numNets);
        while (getline(in, line)) {
            // Time for blockages
            if (numNets == 0) {
                if (numBlockages == -1) {
                    sscanf(line.c_str(), "%d", &numBlockages);
                } else {
                    int x1, y1, x2, y2;
                    int updatedCap;
                    sscanf(line.c_str(), "%d %d %d %d %d", &x1, &y1, &x2, &y2, &updatedCap);
                    usages[getEdgeId(x1, y1, x2, y2)] = updatedCap;
                }
            }
            if (line[0] == 'n') {
                numNets--;
                if (numNets == 0) {
                    int numPins;
                    sscanf(line.c_str(), "%s %d", junk, &numPins);
                    // skip over pins to get to blockage line
                    while(numPins != 0) {
                        getline(in, line);
                        numPins--;
                    }
                }
            }
        }
        in.close();
    }

    // read solution
    ifstream soln(solutionFilename);
    if (soln.is_open()) {
        unordered_set<int> netEdges;
        while(getline(soln, line)) {
            if (line[0] == '!') {
//                printf("net has %d edges\n", netEdges.size());
                for (int edge : netEdges) {
                    usages[edge]--;
                }
                netEdges.clear();
            } else if (line[0] != 'n') {
                int x1, y1, x2, y2;
                sscanf(line.c_str(), "(%d,%d)-(%d,%d)", &x1, &y1, &x2, &y2);
                getEdges(x1, y1, x2, y2, netEdges);
            } 
        }
        in.close();
    }

    int tof = 0;
    int ovf = 0;
    for (int i = 0; i < totalEdges; i++) {
        ovf = usages[i];
        if (ovf < 0) {
            tof += abs(ovf);
        }
    }
    printf("total overflow: %d\n", tof);
 	return 0;
}
