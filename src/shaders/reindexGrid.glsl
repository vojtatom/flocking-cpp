//#version 430 core

layout (local_size_x = 1024) in;

//STRUCTS
struct Boid{
    float pos[3];
    float vel[3];
    float acc[3];
    int   id;
    float countAround;
};

//BUFFERS AND UNIFORMS
layout(std430, binding = 0) buffer boidBuffer
{
    Boid boids[];
};

layout(std430, binding = 3) buffer indexBuffer
{
    uint indices[]; //buffer of indices for corresponding cells
};

//number of boids
layout(location = 1) uniform uint size;
layout(location = 2) uniform uvec3 gridRes;

void main(void) 
{
    uint boidId = gl_WorkGroupSize.x * gl_WorkGroupID.x + gl_LocalInvocationID.x;
   
    if (boidId < size)
    {
        //assign starts of the group
        uint cellID = boids[boidId].id;
        uint prevBoidCellID;
        
        if (boidId == 0)
        {
            prevBoidCellID = 0;
            indices[0] = 0;   
        } else 
            prevBoidCellID = boids[boidId - 1].id;

        
        if (cellID > prevBoidCellID)
        {
            //fill all previous unhabitated cells
            for(uint i = prevBoidCellID + 1; i <= cellID; ++i)
                indices[i] = boidId;           
        }

        //last element, fill calls after last cell
        if (boidId == size - 1)
        {
            uint lastCell = gridRes.x * gridRes.y * gridRes.z;
            for(uint i = cellID + 1; i < lastCell + 1; ++i)
                indices[i] = boidId + 1;  
        }
    }
}