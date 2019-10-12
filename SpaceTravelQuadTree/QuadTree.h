#ifndef QuadTree_239847
#define QuadTree_239847

#include <list>
#include "Asteroid.h"

using namespace std;

// Quadtree node class.
class QuadtreeNode
{
public:
   QuadtreeNode(float x, float z, float s);
   int numberAsteroidsIntersected(); // Return the number of asteroids intersecting the square.
   void addIntersectingAsteroidsToList(); // Add the intersected asteoroids to the local list of asteroids.

   void build(); // Recursive routine to split a square that intersects more than one asteroid; 
                 // if it intersects at most one asteroid leave it as a leaf and add the intersecting 
                 // asteroid, if any, to a local list of asteroids.

   void drawAsteroids(float x1, float z1, float x2, float z2,  // Recursive routine to draw the asteroids
					  float x3, float z3, float x4, float z4); // in a square's list if the square is a
															   // leaf and it intersects the frustum (which
                                                               // is specified by the input parameters); 
															   // if the square is not a leaf, the routine
                                                               // recursively calls itself on its children.
   void setRowsCols(int rows, int cols) { this->rows = rows; this->cols = cols; }
   void setArray(Asteroid **arrayAsteroids) { this->arrayAsteroids = arrayAsteroids; }

private: 
	int rows;
	int cols;
	Asteroid **arrayAsteroids; // Global array of asteroids.
   float SWCornerX, SWCornerZ; // x and z co-ordinates of the SW corner of the square.
   float size; // Side length of square.
   QuadtreeNode *SWChild, *NWChild, *NEChild, *SEChild; // Children nodes.
   list<Asteroid> asteroidList; // Local list of asteroids intersecting the square - only filled for leaf nodes.
   friend class Quadtree;
};

struct Quadtree2 {
	struct dimension {
		float x, z, size;
	};
	struct nodeData {
		Asteroid asteroids[4];
		size_t count;
	};

	//dimension treeDimensions;

	dimension* dimensions;
	nodeData* objects; 
	int levelCount;

	void init(int depth, float minx, float minz, float size) {
		if (depth < 1) cout << "Quadtree creation Failed";

		levelCount = depth;
		size_t treeNodesNeeded = ((1 << (2 * depth)) - 1) / 3;

		//treeDimensions = { minx, miny, size };

		// Forgive the sloppy pointers, this is just getting arrays of size treeNodesNeeded for both
		dimensions = (dimension*)new char[(sizeof(dimension) + sizeof(nodeData)) * treeNodesNeeded];
		objects = reinterpret_cast<nodeData*>(&dimensions[treeNodesNeeded]);

		initBranch(depth - 1, 0, minx, minz, size);
	}

	void cleanup() {
		delete[] dimensions;
	}

	// Structure  A1
	//       B1 B2 B3 B4
	//    C1 C2 C3 C4 C(5-16)
	//
	//                 Children of B1
	//                  /---------\
	// In Memory: A1 B1 C1 C2 C3 C4 B2 C(5-8) B3 C(9-12)...
	void populate(Asteroid* items, size_t count) {
		size_t currentBranch;
		size_t testingBranch;
		int level = levelCount;
		for (int i = 0; i < count; i++) {
			currentBranch = 0;
			testingBranch = 1;

			while (true) {
				level--;
				if (level <= 0) {
					if (objects[currentBranch].count > 4) {
						cout << "Error, out of space in subbranch!";
						//Fix for this would be to have overflow memory ready to go
						break;
					}
					else {
						objects[currentBranch].asteroids[objects[currentBranch].count++] = items[i];
						break;
					}
				} 
				else if (items[i].getCenterX() + items[i].getRadius() < dimensions[currentBranch].x + dimensions[currentBranch].size / 2.0f) {
					if (items[i].getCenterZ() + items[i].getRadius() < dimensions[currentBranch].z + dimensions[currentBranch].size / 2.0f) {
						// Top Left
						currentBranch += 1;
						continue;
					}
					else if (items[i].getCenterZ() - items[i].getRadius() > dimensions[currentBranch].z + dimensions[currentBranch].size / 2.0f) {
						// Bottom Left
						currentBranch += 1 + 2 * ((1 << (2 * level)) - 1) / 3;
						continue;
					}
				}
				else if (items[i].getCenterX() + items[i].getRadius() < dimensions[currentBranch].x + dimensions[currentBranch].size / 2.0f) {
					if (items[i].getCenterZ() + items[i].getRadius() < dimensions[currentBranch].z + dimensions[currentBranch].size / 2.0f) {
						// Top Right
						currentBranch += 1 + 1 * ((1 << (2 * level)) - 1) / 3;
						continue;
					}
					else if (items[i].getCenterZ() - items[i].getRadius() > dimensions[currentBranch].z + dimensions[currentBranch].size / 2.0f) {
						// Bottom Right
						currentBranch += 1 + 3 * ((1 << (2 * level)) - 1) / 3;
						continue;
					}
				}
				else {
					level = 0;
				}
			}
		}
	}

	//Frustrum defined by corners
	void drawQuadtreeItems(float x1, float x2, float x3, float x4, float z1, float z2, float z3, float z4) {
		float n1x = z1 - z2;
		float n1z = -(x1 - x2);
		float n2x = z3 - z4;
		float n2z = -(x3 - x4);
		float n3x = z1 - z4;
		float n3z = -(x1 - x4);

		drawQuadtreeItems(
			n1x, n1z, x1 * n1x + z1 * n1z,
			n2x, n2z, x4 * n2x + z4 * n2z,
			n3x, n3z, x1 * n1x + z1 * n3z,
			x3 * n1x + z3 * n3z,
			levelCount, 0
		);
	}

	//Frustrum defined by normals and distances
	void drawQuadtreeItems(float f1nx, float f1nz, float f1nd,
						   float f2nx, float f2nz, float f2nd,
						   float f3nx, float f3nz, float f3nd1, 
						   float f3nd2) {
		drawQuadtreeItems(f1nx, f1nz, f1nd, f2nx, f2nz, f2nd, f3nx, f3nz, f3nd1, f3nd2, levelCount, 0);
	}

	private:
	// read "Frustrum Plane 1 Normal xy, Distance from Origin
	void drawQuadtreeItems(float f1nx, float f1nz, float f1nd,
						   float f2nx, float f2nz, float f2nd,
						   float f3nx, float f3nz, float f3nd1, 
						   float f3nd2, int depth, size_t branch) {
		// Test if the current branch is 
		// a. outside the frustrum
		// b. partaily in the frustrum
		// c. entirely in the frustrum
		// adapted from http://www.racer.nl/reference/vfc_markmorley.htm, CubeInFrustrum()
		
		int c = 0;
		int c2 = 0;
		// Test each of the corners against each plane of the frustrum (in 2d) by 
		// taking the dot of the normal with each point and comparing it to the distance
		if (dimensions[branch].x * f1nx + dimensions[branch].z * f1nz + f1nd > 0) c++;
		if (dimensions[branch].x * f1nx + dimensions[branch].z + dimensions[branch].size * f1nz + f1nd > 0) c++;
		if (dimensions[branch].x + dimensions[branch].size * f1nx + dimensions[branch].z * f1nz + f1nd > 0) c++;
		if (dimensions[branch].x + dimensions[branch].size * f1nx + dimensions[branch].z + dimensions[branch].size * f1nz + f1nd > 0) c++;

		if (c == 0) return;
		if (c == 4) c2++;

		c = 0;
		if (dimensions[branch].x * f2nx + dimensions[branch].z * f2nz + f2nd > 0) c++;
		if (dimensions[branch].x * f2nx + dimensions[branch].z + dimensions[branch].size * f2nz + f2nd > 0) c++;
		if (dimensions[branch].x + dimensions[branch].size * f2nx + dimensions[branch].z * f2nz + f2nd > 0) c++;
		if (dimensions[branch].x + dimensions[branch].size * f2nx + dimensions[branch].z + dimensions[branch].size * f2nz + f2nd > 0) c++;

		if (c == 0) return;
		if (c == 4) c2++;

		c = 0;
		if (dimensions[branch].x * f3nx + dimensions[branch].z * f3nz + f3nd1 > 0) c++;
		if (dimensions[branch].x * f3nx + dimensions[branch].z + dimensions[branch].size * f3nz + f3nd1 > 0) c++;
		if (dimensions[branch].x + dimensions[branch].size * f3nx + dimensions[branch].z * f3nz + f3nd1 > 0) c++;
		if (dimensions[branch].x + dimensions[branch].size * f3nx + dimensions[branch].z + dimensions[branch].size * f3nz + f3nd1 > 0) c++;

		if (c == 0) return;
		if (c == 4) c2++;

		c = 0;
		if (dimensions[branch].x * f3nx + dimensions[branch].z * f3nz + f3nd2 > 0) c++;
		if (dimensions[branch].x * f3nx + dimensions[branch].z + dimensions[branch].size * f3nz + f3nd2 > 0) c++;
		if (dimensions[branch].x + dimensions[branch].size * f3nx + dimensions[branch].z * f3nz + f3nd2 > 0) c++;
		if (dimensions[branch].x + dimensions[branch].size * f3nx + dimensions[branch].z + dimensions[branch].size * f3nz + f3nd2 > 0) c++;

		if (c == 0) return;
		if (c == 4) c2++;


		if (c2 == 4 || depth < 1) {
			drawAllChildren(branch, depth);
		}
		else {
			size_t childOffset = ((1 << (2 * depth)) - 1) / 3;
			drawQuadtreeItems(f1nx, f1nz, f1nd, f2nx, f2nz, f2nd, f3nx, f3nz, f3nd1, f3nd2, --depth, ++branch);
			drawQuadtreeItems(f1nx, f1nz, f1nd, f2nx, f2nz, f2nd, f3nx, f3nz, f3nd1, f3nd2, depth, branch + childOffset);
			drawQuadtreeItems(f1nx, f1nz, f1nd, f2nx, f2nz, f2nd, f3nx, f3nz, f3nd1, f3nd2, depth, branch + childOffset * 2);
			drawQuadtreeItems(f1nx, f1nz, f1nd, f2nx, f2nz, f2nd, f3nx, f3nz, f3nd1, f3nd2, depth, branch + childOffset * 3);
		}
	}

	void drawAllChildren(size_t index, int depth) {
		size_t count = 4 * (1 << (2 * (levelCount - depth)) - 1) / 3;
		for (size_t i = 0; i < count; i++) {
			for(size_t j = 0; j < objects[i + index].count; i++)
			objects[i + index].asteroids->draw();
		}
	}

	void initBranch(int level, size_t startingIndex, float minx, float miny, float size) {
		dimensions[startingIndex] = { minx, miny, size };
		objects[startingIndex].count = 0;
		if (level > 0) {
			size /= 2;
			// read (4^n-1)/3
			size_t childOffset = ((1 << (2 * level)) - 1) / 3;
			initBranch(--level, ++startingIndex, minx, miny, size);
			initBranch(level, startingIndex + 1 * childOffset, minx + size, miny, size);
			initBranch(level, startingIndex + 2 * childOffset, minx, miny + size, size);
			initBranch(level, startingIndex + 3 * childOffset, minx + size, miny + size, size);
		}
	}
};



// Quadtree class.
class Quadtree
{
public:
   Quadtree() { header = NULL; } // Constructor.
   void initialize(float x, float z, float s); // Initialize quadtree by splitting nodes
                                                     // till each leaf node intersects at
                                                     // most one asteroid.

   void drawAsteroids(float x1, float z1, float x2, float z2,  // Routine to draw all the asteroids in the  
					  float x3, float z3, float x4, float z4); // asteroid list of each leaf square that
                                                               // intersects the frustum.

   void setRowsCols(int rows, int cols) { this->rows = rows; this->cols = cols; }
   void setArray(Asteroid **arrayAsteroids) { this->arrayAsteroids = arrayAsteroids; }

private:
   QuadtreeNode *header;
   int rows;
   int cols;
   Asteroid **arrayAsteroids; // Global array of asteroids.
};


#endif 