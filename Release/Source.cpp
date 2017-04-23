#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <set>

# define M_PI           3.14159265358979323846

using namespace std;


struct triangle {
	vector<int> vertices;
	int deleted = 0;

};

struct edge {
	vector<int> vertices;
};


struct vertex {
	float x;
	float y;
	float z;
	vector<int> ntriangles;
	vector<int> newtriangles;

};

void updateVertexTriangles(vector<vertex>& vertices)
{
	for (int k = 0; k < vertices.size(); ++k)
	{
		vertices[k].ntriangles=vertices[k].newtriangles;
		vertices[k].newtriangles = vector<int>();
	}

}

void removeNtriangle(vertex& v, int triangle) ////////////////////////////////////////CAREFUL
{
	for (int k = 0; k < v.ntriangles.size(); ++k)
	{
		if (v.ntriangles[k] == triangle)
		{
			v.ntriangles.erase(v.ntriangles.begin() + k); 
		}
	}

}


float beta(int valence)
{
	return (4 - 2 * cos(2 * M_PI / valence)) / (9 * valence);
}

int calcvalence(vertex& v,vector<triangle>& triangles)
{
	vector<int> edges;
	for (int k = 0; k < v.ntriangles.size(); ++k)
	{
		edges.push_back(triangles[v.ntriangles[k]].vertices[0]);
		edges.push_back(triangles[v.ntriangles[k]].vertices[1]);
		edges.push_back(triangles[v.ntriangles[k]].vertices[2]);
	}
//	cout << "valence before set: " << edges.size()-1 << "\n";

	//get rid of duplicates
	set<int> s(edges.begin(), edges.end());
	edges.assign(s.begin(), s.end());


	//will always count itself in the process
	return edges.size()-1;
}


void commontriangles(vertex& pivot1, vertex& pivot2,vector<int>& common,vector<triangle>& triangles)
{

	for (vector<int>::iterator i = pivot1.ntriangles.begin(); i != pivot1.ntriangles.end(); ++i)
	{
		if (find(pivot2.ntriangles.begin(), pivot2.ntriangles.end(), *i) != pivot2.ntriangles.end())
		{
			//cout << "found something \n";
			if (triangles[*i].deleted == 0)
			{
				common.push_back(*i);
			}
		}
	}

	/*if (common.size() == 0 || common.size()>2)
	{
		cout << "Error in common triangles \n";
	}*/
}

void updatetriangleIndexing(vector<triangle>& triangles)
{
	for (int k = 0; k < triangles.size();++k)
	{
		triangles[k].vertices[0] += 1;
		triangles[k].vertices[1] += 1;
		triangles[k].vertices[2] += 1;
	}
}


int main(void)
{
	int iterations = 3;

	string filename = "";
	cout << "Type in filename \n";
	getline(cin, filename);

	cout << "How many iterations? \n";
	cin >> iterations;

	vector<vertex> vertices;
	vector<triangle> triangles;
	vector<triangle> newtris;

	string Line;
	std::ifstream File(filename);
	while (getline(File, Line))
	{
		if (Line == "" || Line[0] == '#')			continue;

		//check v for vertices
		else if (Line.substr(0, 2) == "v ")
		{
			std::istringstream v(Line.substr(2));
			float x, y, z;
			v >> x; v >> y; v >> z;
			vertex vert = vertex();
			vert.x = x;
			vert.y = y;
			vert.z = z;
			vertices.push_back(vert);

		}
		else if (Line.substr(0, 2) == "f ")
		{
			std::istringstream v(Line.substr(2));
			int x, y, z;
			v >> x; v >> y; v >> z;
			triangle tr  = triangle();
			tr.vertices.push_back(x-1);
			tr.vertices.push_back(y-1);
			tr.vertices.push_back(z-1);
			vertices[x - 1].ntriangles.push_back(triangles.size());
			vertices[y - 1].ntriangles.push_back(triangles.size());
			vertices[z - 1].ntriangles.push_back(triangles.size());
			triangles.push_back(tr);

		}

	}





	//start algo

	for (int j = 0; j < iterations;++j)
	{

		//cout << "vert2  " << vertices[2].x << " " << vertices[2].y << " " << vertices[2].z << "\n";
		//structure for new triangles
		vector<triangle> newtriangles;
		//how many old vertices
		int howmanyoldvertices = vertices.size();






		//compute and add center points
		for (int k = 0; k < triangles.size(); ++k)
		{
			vertex vert = vertex();
			vert.x = (vertices[triangles[k].vertices[0]].x + vertices[triangles[k].vertices[1]].x + vertices[triangles[k].vertices[2]].x) / 3;
			vert.y = (vertices[triangles[k].vertices[0]].y + vertices[triangles[k].vertices[1]].y + vertices[triangles[k].vertices[2]].y) / 3;
			vert.z = (vertices[triangles[k].vertices[0]].z + vertices[triangles[k].vertices[1]].z + vertices[triangles[k].vertices[2]].z) / 3;
			
			//add center point index to the triangle's vertex indices
			triangles[k].vertices.push_back(vertices.size());
			//add center point to the vertices
			vertices.push_back(vert);



		}



			//update old points///////////////////////////////////////////////////////////////////////////////////////
			for (int k = 0; k < howmanyoldvertices; ++k)
			{
				//calculate valence
				int n = calcvalence(vertices[k] , triangles);

				//get all neighbors
				vector<int> edges;
			
				for (int kk = 0; kk < vertices[k].ntriangles.size(); ++kk)
				{
					edges.push_back(triangles[vertices[k].ntriangles[kk]].vertices[0]);
					edges.push_back(triangles[vertices[k].ntriangles[kk]].vertices[1]);
					edges.push_back(triangles[vertices[k].ntriangles[kk]].vertices[2]);
				}

				//get rid of duplicates
				set<int> s(edges.begin(), edges.end());
				edges.assign(s.begin(), s.end());

				//iterate over all neighbors and sum
				float newx = 0.0;
				float newy = 0.0;
				float newz = 0.0;
				for (int l = 0; l < edges.size(); ++l)
				{
					//if neighbor is the point itself
					if (edges[l] == k)
					{
				//		cout << "itsef \n";
						newx += (1 - beta(n)*n)*vertices[edges[l]].x;
						newy += (1 - beta(n)*n)*vertices[edges[l]].y;
						newz += (1 - beta(n)*n)*vertices[edges[l]].z;
					}
					else
					{
						newx += beta(n)*vertices[edges[l]].x;
						newy += beta(n)*vertices[edges[l]].y;
						newz += beta(n)*vertices[edges[l]].z;

					}

				}
				//update old vertex
				vertices[k].x = newx;
				vertices[k].y = newy;
				vertices[k].z = newz;
				//cout << "vert2  " << vertices[2].x << " " << vertices[2].y << " " << vertices[2].z << "\n";

			}////////////////////////////////////////////////////////////////////////////////////////////////////////



			/////////////////////////////////////////updating triangles
			for (int k = 0; k < triangles.size(); ++k)
			{
				int v1 = triangles[k].vertices[0];
				int v2 = triangles[k].vertices[1];
				int v3 = triangles[k].vertices[2];

				vector<int> comm1;
				vector<int> comm2;
				vector<int> comm3;

				commontriangles(vertices[v1], vertices[v2], comm1,triangles);
				commontriangles(vertices[v1], vertices[v3], comm2,triangles);
				commontriangles(vertices[v2], vertices[v3], comm3,triangles);

				//create six triangles , each pair is a "diamond" connecting the two midpoints of adjacent triangles
				if (comm1.size() == 2)
				{
					int center1=  triangles[comm1[0]].vertices[3];
					int center2 = triangles[comm1[1]].vertices[3];

					triangle tr1 = triangle();
					tr1.vertices.push_back(center1);
					tr1.vertices.push_back(center2);
					tr1.vertices.push_back(v1);

					triangle tr2 = triangle();
					tr2.vertices.push_back(center1);
					tr2.vertices.push_back(center2);
					tr2.vertices.push_back(v2);

					newtriangles.push_back(tr1);
					vertices[center1].newtriangles.push_back(newtriangles.size() - 1);
					vertices[center2].newtriangles.push_back(newtriangles.size() - 1);
					vertices[v1].newtriangles.push_back(newtriangles.size() - 1);

					newtriangles.push_back(tr2);
					vertices[center1].newtriangles.push_back(newtriangles.size() - 1);
					vertices[center2].newtriangles.push_back(newtriangles.size() - 1);
					vertices[v2].newtriangles.push_back(newtriangles.size() - 1);

				}
				


				if (comm2.size() == 2)
				{
					int center1 = triangles[comm2[0]].vertices[3];
					int center2 = triangles[comm2[1]].vertices[3];

					triangle tr1 = triangle();
					tr1.vertices.push_back(center1);
					tr1.vertices.push_back(center2);
					tr1.vertices.push_back(v1);

					triangle tr2 = triangle();
					tr2.vertices.push_back(center1);
					tr2.vertices.push_back(center2);
					tr2.vertices.push_back(v3);

					newtriangles.push_back(tr1);
					vertices[center1].newtriangles.push_back(newtriangles.size() - 1);
					vertices[center2].newtriangles.push_back(newtriangles.size() - 1);
					vertices[v1].newtriangles.push_back(newtriangles.size() - 1);

					newtriangles.push_back(tr2);
					vertices[center1].newtriangles.push_back(newtriangles.size() - 1);
					vertices[center2].newtriangles.push_back(newtriangles.size() - 1);
					vertices[v3].newtriangles.push_back(newtriangles.size() - 1);

				}


				if (comm3.size() == 2)
				{
					int center1 = triangles[comm3[0]].vertices[3];
					int center2 = triangles[comm3[1]].vertices[3];

					triangle tr1 = triangle();
					tr1.vertices.push_back(center1);
					tr1.vertices.push_back(center2);
					tr1.vertices.push_back(v2);

					triangle tr2 = triangle();
					tr2.vertices.push_back(center1);
					tr2.vertices.push_back(center2);
					tr2.vertices.push_back(v3);

					newtriangles.push_back(tr1);
					vertices[center1].newtriangles.push_back(newtriangles.size() - 1);
					vertices[center2].newtriangles.push_back(newtriangles.size() - 1);
					vertices[v2].newtriangles.push_back(newtriangles.size() - 1);

					newtriangles.push_back(tr2);
					vertices[center1].newtriangles.push_back(newtriangles.size() - 1);
					vertices[center2].newtriangles.push_back(newtriangles.size() - 1);
					vertices[v3].newtriangles.push_back(newtriangles.size() - 1);

				}

				
				triangles[k].deleted = 1;
				removeNtriangle(vertices[v1], k);
				removeNtriangle(vertices[v2], k);
				removeNtriangle(vertices[v3], k);
			

			}///////////////////////////////////////////////////////weow buddy

			//update triangle data in vertices
			updateVertexTriangles(vertices);
			triangles = newtriangles;


	}









	//fixed triangle indices
	updatetriangleIndexing(triangles);




	string output = "Improved";
	string improvout = output.append(filename);
	string mview = "start mview-qt4.exe ";
	string mview2 = "start mview-qt4.exe ";
	string syscom = mview.append(improvout);
	string ampersand = " &";
	string oldsyscom = mview2.append(filename);
	oldsyscom = oldsyscom.append(ampersand);
	
	const char * c = syscom.c_str();
	const char * cold = oldsyscom.c_str();


	ofstream myfile(improvout);
	if (myfile.is_open())
	{
		for (int k = 0; k < vertices.size(); ++k)
		{
			myfile << "v " << vertices[k].x << " " << vertices[k].y << " " << vertices[k].z <<"\n"  ;
		}
		myfile << "\n";
		for (int k = 0; k < triangles.size(); ++k)
		{
			myfile << "f " << triangles[k].vertices[0] << " " << triangles[k].vertices[1] << " " << triangles[k].vertices[2] << "\n";
		}
		myfile.close();
	}
	else cout << "Unable to open file";

	system(cold);
	system(c);
	



	return 0;
}