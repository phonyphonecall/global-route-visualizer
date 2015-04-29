#include "string.h"
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_set>

int gx, gy;
int totalEdges;
int capacity;
int* usages;

using namespace std;

int get_horizontal_right_edge(int x, int y) {
    return  (y * (gx - 1) + x);
}


int get_vertical_upper_edge(int x, int y) {
    return (gx - 1) * gy + (y * gx) + x;
}

void getSegmentFromEdge(int edgeId, int& x1, int& y1, int& x2, int& y2) {
    int totalHorizEdges =  (gx - 1) * gy;
    if (edgeId < totalHorizEdges) {
        // horizontal edge
        x1 = edgeId % (gx - 1);
        y1 = (edgeId) / (gx - 1);
        x2 =  x1 + 1;
        y2 =  y1;
    } else {
        // vertical edge
        x1 = (edgeId - totalHorizEdges) % (gx);
        y1 = ((edgeId - totalHorizEdges) - x1) / gx;
        x2 =  x1;
        y2 =  y1 + 1;
    }
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
 	char* outputFilename = argv[3];

 	// read benchmark
    ifstream in(inputFilename);
    if (in.is_open()) {
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
    int maxOverflow = 1;   // max usage of overflowed edge
    for (int i = 0; i < totalEdges; i++) {
        ovf = usages[i];
        if (ovf < 0) {
            tof += abs(ovf);
            if (abs(ovf) > maxOverflow)
                maxOverflow = abs(ovf);
        }
    }

    // build svg
    //  use old c files for fprintf convenience
    FILE* f = fopen(outputFilename, "w");
    fprintf(f, "<!DOCTYPE html><html><body>\n");
    fprintf(f, "<p><h3>Total Overflow: %d</h3></p>\n", tof);
    fprintf(f, "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" style='width:%ipx;height:%ipx'>\n", gx * 10, gy * 10);

    for (unsigned int i = 0; i < gx; i++) {
        for (unsigned int j = 0; j < gy; j++) {
            fprintf(f, "<circle cx=\"%d\" cy=\"%d\" r=\"1\" stroke=\"black\" stroke-width=\"0\" fill=\"grey\" />\n", i*10, j*10);
        }
    }

    int usage;
    int r, g, b, width;
    for (int i = 0; i < totalEdges; i++) {
        usage = usages[i];
        if (usage == capacity) {
            continue;
        } else if (usage > 0) {
            // non-overflow case... grey. darker == more usage
            r = int(225.0 * (double(usage) / double(capacity)));
            g = r;
            b = r;
            width = 7 - int(7.0 * (double(usage) / double(capacity)));
        } else {
            // overlfow... red. darker == more usage
//            r = int(245.0 * (double(abs(usage)) / double(maxOverflow)));
            r = 255;
            g = 0;
            b = 0;
            width =  7 - int(7.0 * (double(abs(usage)) / double(maxOverflow)));
        }
        int x1, y1, x2, y2;
        getSegmentFromEdge(i, x1, y1, x2, y2);
        fprintf(f, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" style=\"stroke:rgb(%d,%d,%d);stroke-width:%d\" />\n", 10 * x1, 10*y1, 10*x2, 10*y2, r, g, b, width);
    }

    printf("total overflow: %d\n", tof);
 	return 0;
}
