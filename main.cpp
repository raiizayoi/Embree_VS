#include "embree2/rtcore.h"
#include "embree2/rtcore_scene.h"
#include "embree2/rtcore_ray.h"
#include <limits>
#include <cstdio>

#pragma comment(lib,".\\lib\\embree.lib")

struct Vec3fa
{
	Vec3fa(float _x, float _y, float _z, float _w = 1.0f)
	: x(_x), y(_y), z(_z), w(_w) {}
	float x, y, z, w;
};
/* scene data */
RTCDevice g_device = nullptr;
RTCScene g_scene = nullptr;

float cube_vertices[8][4] =
{
	{ -1.0f, -1.0f, -1.0f, 0.0f },
	{ -1.0f, -1.0f, +1.0f, 0.0f },
	{ -1.0f, +1.0f, -1.0f, 0.0f },
	{ -1.0f, +1.0f, +1.0f, 0.0f },
	{ +1.0f, -1.0f, -1.0f, 0.0f },
	{ +1.0f, -1.0f, +1.0f, 0.0f },
	{ +1.0f, +1.0f, -1.0f, 0.0f },
	{ +1.0f, +1.0f, +1.0f, 0.0f }
};

unsigned int cube_quad_indices[24] = {
	0, 1, 3, 6,
	4, 5, 7, 6,
	0, 1, 5, 4,
	2, 3, 7, 6,
	0, 2, 6, 4,
	1, 3, 7, 5,
};
/* adds a cube to the scene */
unsigned int addCube(RTCScene scene_i)
{
	/* create a triangulated cube with 12 triangles and 8 vertices */
	unsigned int mesh = rtcNewQuadMesh(scene_i, RTC_GEOMETRY_STATIC, 6, 8);

	printf("mesh id: %u\n", mesh);

	/* set triangles and face colors */
	rtcSetBuffer(scene_i, 0, RTC_VERTEX_BUFFER, cube_vertices, 0, sizeof(float));
	rtcSetBuffer(scene_i, 0, RTC_INDEX_BUFFER, cube_quad_indices, 0, sizeof(unsigned int));
	return mesh;
}

/* called by the C++ code for initialization */
void device_init(char* cfg = nullptr)
{
	/* create new Embree device */
	g_device = rtcNewDevice(cfg);

	/* set error handler */
	/* create scene */
	g_scene = rtcDeviceNewScene(g_device, RTC_SCENE_STATIC, RTC_INTERSECT1);

	/* add cube */
	addCube(g_scene);

	/* commit changes to scene */
	rtcCommit(g_scene);
}

/* task that renders a single screen tile */
bool renderPixelStandard(float x, float y)
{
	/* initialize ray */
	RTCRay ray;
	ray.org[0] = x;
	ray.org[1] = y;
	ray.org[2] = -10;
	ray.dir[0] = 0;
	ray.dir[1] = 0;
	ray.dir[2] = 1;

	ray.tnear = 0.0f;
	ray.tfar = std::numeric_limits<float>::infinity();;
	ray.geomID = RTC_INVALID_GEOMETRY_ID;
	ray.primID = RTC_INVALID_GEOMETRY_ID;
	ray.mask = -1;
	ray.time = 0;

	printf("test intersect\n");

	/* intersect ray with scene */
	rtcIntersect(g_scene, ray);

	/* shade pixels */
	//Vec3fa color(0.0f, 0.0f, 0.0f, 0.0f);
	if (ray.geomID != RTC_INVALID_GEOMETRY_ID)
	{
		return true;// = Vec3fa(1.0f, 1.0f, 1.0f, 1.0f);
	}
	return false;
}

/*
void renderTileStandard()
{

	for (int y = y0; y < y1; y++) for (int x = x0; x < x1; x++)
	{
		Vec3fa color = renderPixelStandard(x, y);
	}
}

void renderTileTask(int taskIndex, int* pixels,
	const int width,
	const int height,
	const float time,
	const ISPCCamera& camera,
	const int numTilesX,
	const int numTilesY)
{
	renderTile(taskIndex, pixels, width, height, time, camera, numTilesX, numTilesY);
}

extern "C" void device_render(int* pixels,
	const int width,
	const int height,
	const float time,
	const ISPCCamera& camera)
{
	parallel_for(size_t(0), size_t(numTilesX*numTilesY), [&](const range<size_t>& range) {
		for (size_t i = range.begin(); i < range.end(); i++)
			renderTileTask(i, pixels, width, height, time, camera, numTilesX, numTilesY);
	});
}*/

/* called by the C++ code for cleanup */
void device_cleanup()
{
	rtcDeleteScene(g_scene); g_scene = nullptr;
	rtcDeleteDevice(g_device); g_device = nullptr;
}

int main()
{
	device_init();

	if (renderPixelStandard(1000,0))
	{
		printf("heheheheheh\n");
	}

	device_cleanup();
	getchar();
	return 0;
}