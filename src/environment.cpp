#include <unordered_map>
#include <iostream>
#include <fstream>
#include <glm/glm.hpp>
#include "boid.hpp"
#include "environment.hpp"

using namespace std;

Environment::Environment(int argc, char *argv[])
{
    //general settings
    addRecord("appType", 0);
    addRecord("boidCount", 1024);
    addRecord("spaceLow", -1000, -1000, -1000);
    addRecord("spaceHigh", 1000, 1000, 1000);
    addRecord("camera", 0, 0, 100);
    addRecord("dark", 1);

    //flocking
    addRecord("flockingZone", 40.0f);
    addRecord("speedFactor", 2.0f);
    addRecord("forceLimit", 0.1f);

    //rendering
    addRecord("treeMemoryLimit", 4096);
    addRecord("boidSize", 2.f);

	size_t i = 0, j = 0;
	while (true)
	{
		if (argv[0][i] == '\0')
			break;

		if (argv[0][i] == '\\' || argv[0][i] == '/')
			j = i + 1;
		i++;
	}

	path = string(argv[0], j);
    cout << "Enviroment settings:" << endl;
	cout << path << endl;

    string envfile = "env.txt";
    if (argc > 1){
        envfile = argv[1];
    }

    //read environment file
    ifstream env(path + envfile);
    if (env.is_open())
    {
        string title, type;

        int32_t integer;
        float a, b, c;

        while(!env.eof())
        {
            env >> title;

            if (env.bad())
            {
                addRecord("appType", -1);
                return;
            }

            if (title[0] == '#')
            {
                //comment, skip line
                while ((env.peek()!='\n') && (env>>title));
            } else {
                env >> type;

                cout << title << " " << type << " " << flush;
            
                if (env.bad())
                {
                    cout << "Error reading environment file, last read: " << title << " " << type << endl;
                    addRecord("appType", -1);
                    return;
                }

                if (type == "int")
                {
                    env >> integer;
                    addRecord(title, integer);
                    cout << integer << endl;
                } 
                else if (type == "float")
                {
                    env >> a;
                    addRecord(title, a);
                    cout << a << endl;
                } 
                else if (type == "vec3")
                {
                    env >> a >> b >> c;
                    addRecord(title, a, b, c);
                    cout << a << " " << b << " " << c << endl;
                } 
                else 
                {
                    cout << "Unknwn format found in env file, skipping: " << type << endl;
                }
            }

        }
    } else {
        addRecord("appType", -1);
        return;
    }

    //read commanline args
    int apptype = 0;
    int boidsCountPower = 10;

    if (argc > 2){
        apptype = atoi(argv[2]);
        addRecord("appType", apptype);
    }

    if (argc > 3){
        boidsCountPower = max(1, atoi(argv[3]));
        addRecord("boidCount", boidsCountPower);
    }

    //setup grid
    glm::vec3 space = getVec("spaceHigh") - getVec("spaceLow");
    //float zone = 2 * getFloat("flockingZone");
    float zone = getFloat("flockingZone");
    glm::uvec3 grid = glm::max(space / zone, glm::vec3(1));
    cout << "grid: " << grid << endl;
    addRecord("grid", grid.x, grid.y, grid.z);
}


/*void Environment::addRecord(unsigned char type, const string & name, const string & initValue)
{
    switch (type)
    {
    case EType::TINT:
        mapInt[name] = atoi(initValue.c_str());
        break;
    case EType::TFLOAT:
        mapFloat[name] = atof(initValue.c_str());
        break;
    default:
        break;
    }
}*/

void Environment::addRecord(const string & name, const int32_t & initValue)
{
    mapInt[name] = initValue;
}


void Environment::addRecord(const string & name, const float & initValue)
{
    mapFloat[name] = initValue;
}

void Environment::addRecord(const string & name, const float & x, const float & y, const float & z)
{
    mapVec[name] = glm::vec3(x, y, z);
}

float Environment::getFloat(const char * value)
{
    auto it = mapFloat.find(string(value));
    if (it != mapFloat.end())
        return it->second;
    return 0.0f;
}

int32_t Environment::getInt(const char * value)
{
    auto it = mapInt.find(string(value));
    if (it != mapInt.end())
        return it->second;
    return 0;
}

glm::vec3 Environment::getVec(const char * value)
{
    auto it = mapVec.find(string(value));
    if (it != mapVec.end())
        return it->second;
    return glm::vec3(0.0f);
}

const string& Environment::getPath() const
{
	return path;
}