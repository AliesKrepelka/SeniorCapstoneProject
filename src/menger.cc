#include "menger.h"

namespace {
	const int kMinLevel = 0;
	const int kMaxLevel = 4;
};

Menger::Menger()
{
}

Menger::~Menger()
{
}

void
Menger::set_nesting_level(int level)
{
	nesting_level_ = level;
	dirty_ = true;
}

bool
Menger::is_dirty() const
{
	return dirty_;
}

void
Menger::set_clean()
{
	dirty_ = false;
}

// Generates a single unit cube at world position (startPoint), with side length `unit`.
// `start` is the local half-offset (always -0.5 at top level, scaled recursively).
void
Menger::gHelper(std::vector<glm::vec4>& obj_vertices,
                std::vector<glm::vec4>& vtx_normals,
                double start, double unit, glm::vec3 startPoint) const
{
	double w = 1.0;
	double xx = startPoint.x;
	double yy = startPoint.y;
	double zz = startPoint.z;

	// We ignore `start` here and use startPoint directly as the cube origin.
	// Each cube goes from (xx, yy, zz) to (xx+unit, yy+unit, zz+unit).

	// Front face (normal +Z)
	obj_vertices.push_back(glm::vec4(xx,        yy,        zz+unit, w));
	obj_vertices.push_back(glm::vec4(xx+unit,   yy,        zz+unit, w));
	obj_vertices.push_back(glm::vec4(xx,        yy+unit,   zz+unit, w));
	obj_vertices.push_back(glm::vec4(xx+unit,   yy,        zz+unit, w));
	obj_vertices.push_back(glm::vec4(xx+unit,   yy+unit,   zz+unit, w));
	obj_vertices.push_back(glm::vec4(xx,        yy+unit,   zz+unit, w));
	for(int i = 0; i < 6; i++)
		vtx_normals.push_back(glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));

	// Back face (normal -Z)
	obj_vertices.push_back(glm::vec4(xx+unit,   yy,        zz, w));
	obj_vertices.push_back(glm::vec4(xx,        yy,        zz, w));
	obj_vertices.push_back(glm::vec4(xx+unit,   yy+unit,   zz, w));
	obj_vertices.push_back(glm::vec4(xx,        yy,        zz, w));
	obj_vertices.push_back(glm::vec4(xx,        yy+unit,   zz, w));
	obj_vertices.push_back(glm::vec4(xx+unit,   yy+unit,   zz, w));
	for(int i = 0; i < 6; i++)
		vtx_normals.push_back(glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));

	// Left face (normal -X)
	obj_vertices.push_back(glm::vec4(xx,        yy,        zz, w));
	obj_vertices.push_back(glm::vec4(xx,        yy,        zz+unit, w));
	obj_vertices.push_back(glm::vec4(xx,        yy+unit,   zz, w));
	obj_vertices.push_back(glm::vec4(xx,        yy,        zz+unit, w));
	obj_vertices.push_back(glm::vec4(xx,        yy+unit,   zz+unit, w));
	obj_vertices.push_back(glm::vec4(xx,        yy+unit,   zz, w));
	for(int i = 0; i < 6; i++)
		vtx_normals.push_back(glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f));

	// Right face (normal +X)
	obj_vertices.push_back(glm::vec4(xx+unit,   yy,        zz+unit, w));
	obj_vertices.push_back(glm::vec4(xx+unit,   yy,        zz, w));
	obj_vertices.push_back(glm::vec4(xx+unit,   yy+unit,   zz+unit, w));
	obj_vertices.push_back(glm::vec4(xx+unit,   yy,        zz, w));
	obj_vertices.push_back(glm::vec4(xx+unit,   yy+unit,   zz, w));
	obj_vertices.push_back(glm::vec4(xx+unit,   yy+unit,   zz+unit, w));
	for(int i = 0; i < 6; i++)
		vtx_normals.push_back(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));

	// Bottom face (normal -Y)
	obj_vertices.push_back(glm::vec4(xx,        yy,        zz, w));
	obj_vertices.push_back(glm::vec4(xx+unit,   yy,        zz, w));
	obj_vertices.push_back(glm::vec4(xx,        yy,        zz+unit, w));
	obj_vertices.push_back(glm::vec4(xx+unit,   yy,        zz, w));
	obj_vertices.push_back(glm::vec4(xx+unit,   yy,        zz+unit, w));
	obj_vertices.push_back(glm::vec4(xx,        yy,        zz+unit, w));
	for(int i = 0; i < 6; i++)
		vtx_normals.push_back(glm::vec4(0.0f, -1.0f, 0.0f, 0.0f));

	// Top face (normal +Y)
	obj_vertices.push_back(glm::vec4(xx,        yy+unit,   zz+unit, w));
	obj_vertices.push_back(glm::vec4(xx+unit,   yy+unit,   zz+unit, w));
	obj_vertices.push_back(glm::vec4(xx,        yy+unit,   zz, w));
	obj_vertices.push_back(glm::vec4(xx+unit,   yy+unit,   zz+unit, w));
	obj_vertices.push_back(glm::vec4(xx+unit,   yy+unit,   zz, w));
	obj_vertices.push_back(glm::vec4(xx,        yy+unit,   zz, w));
	for(int i = 0; i < 6; i++)
		vtx_normals.push_back(glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
}

// Checks if a 3x3x3 sub-cube position should be REMOVED (is a tunnel).
// Remove if 2 or more of the indices are the center index (1).
bool isTunnel(int xi, int yi, int zi) {
	int centerCount = (xi == 1) + (yi == 1) + (zi == 1);
	return centerCount >= 2;
}

void
Menger::l1Helper(std::vector<glm::vec4>& obj_vertices,
                 std::vector<glm::vec4>& vtx_normals,
                 double start, double unit, glm::vec3 startPoint) const
{
	double len = unit / 3.0;

	for (int xi = 0; xi < 3; xi++) {
		for (int yi = 0; yi < 3; yi++) {
			for (int zi = 0; zi < 3; zi++) {
				if (isTunnel(xi, yi, zi)) continue;

				glm::vec3 offset = startPoint + glm::vec3(xi * len, yi * len, zi * len);
				gHelper(obj_vertices, vtx_normals, start, len, offset);
			}
		}
	}
}

void
Menger::l2Helper(std::vector<glm::vec4>& obj_vertices,
                 std::vector<glm::vec4>& vtx_normals,
                 double start, double unit, glm::vec3 startPoint) const
{
	double len = unit / 3.0;

	for (int xi = 0; xi < 3; xi++) {
		for (int yi = 0; yi < 3; yi++) {
			for (int zi = 0; zi < 3; zi++) {
				if (isTunnel(xi, yi, zi)) continue;

				glm::vec3 offset = startPoint + glm::vec3(xi * len, yi * len, zi * len);
				l1Helper(obj_vertices, vtx_normals, start, len, offset);
			}
		}
	}
}

void
Menger::l3Helper(std::vector<glm::vec4>& obj_vertices,
                 std::vector<glm::vec4>& vtx_normals,
                 double start, double unit, glm::vec3 startPoint) const
{
	double len = unit / 3.0;

	for (int xi = 0; xi < 3; xi++) {
		for (int yi = 0; yi < 3; yi++) {
			for (int zi = 0; zi < 3; zi++) {
				if (isTunnel(xi, yi, zi)) continue;

				glm::vec3 offset = startPoint + glm::vec3(xi * len, yi * len, zi * len);
				l2Helper(obj_vertices, vtx_normals, start, len, offset);
			}
		}
	}
}

void
Menger::l4Helper(std::vector<glm::vec4>& obj_vertices,
                 std::vector<glm::vec4>& vtx_normals,
                 double start, double unit, glm::vec3 startPoint) const
{
	double len = unit / 3.0;

	for (int xi = 0; xi < 3; xi++) {
		for (int yi = 0; yi < 3; yi++) {
			for (int zi = 0; zi < 3; zi++) {
				if (isTunnel(xi, yi, zi)) continue;

				glm::vec3 offset = startPoint + glm::vec3(xi * len, yi * len, zi * len);
				l3Helper(obj_vertices, vtx_normals, start, len, offset);
			}
		}
	}
}

void
Menger::generate_geometry(std::vector<glm::vec4>& obj_vertices,
                          std::vector<glm::vec4>& vtx_normals,
                          std::vector<glm::uvec3>& obj_faces) const
{
	obj_vertices.clear();
	vtx_normals.clear();
	obj_faces.clear();

	// The sponge spans from (0,0,0) to (1,1,1), centered by starting at (-0.5,-0.5,-0.5)
	double unit = 1.0;
	glm::vec3 origin(-0.5f, -0.5f, -0.5f);

	if (nesting_level_ == 0) {
		gHelper(obj_vertices, vtx_normals, 0.0, unit, origin);
	} else if (nesting_level_ == 1) {
		l1Helper(obj_vertices, vtx_normals, 0.0, unit, origin);
	} else if (nesting_level_ == 2) {
		l2Helper(obj_vertices, vtx_normals, 0.0, unit, origin);
	} else if (nesting_level_ == 3) {
		l3Helper(obj_vertices, vtx_normals, 0.0, unit, origin);
	} else if (nesting_level_ == 4) {
		l4Helper(obj_vertices, vtx_normals, 0.0, unit, origin);
	}

	// Build face indices - each group of 3 vertices is one triangle
	int total = (int)obj_vertices.size();
	for (int i = 0; i < total; i += 3) {
		obj_faces.push_back(glm::uvec3(i, i + 1, i + 2));
	}
}