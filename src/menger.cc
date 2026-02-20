#include "menger.h"

namespace {
	const int kMinLevel = 0;
	const int kMaxLevel = 4;
};

Menger::Menger()
{
	// Add additional initialization if you like
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

// FIXME generate Menger sponge geometry
void
Menger::generate_geometry(std::vector<glm::vec4>& obj_vertices,
			  std::vector<glm::vec4>& vtx_normals,
                          std::vector<glm::uvec3>& obj_faces) const
{ 
	obj_vertices.clear();
	vtx_normals.clear();
	obj_faces.clear();
	double unit=1.0f;
	double start = -0.5f;

	double x = 1.0f;
	int bigI = 0;
	glm::vec3 startPoint (0.0f,0.0f,unit);
	if(nesting_level_ == 0){
		bigI = 36;
		// Front
		// Starting point
		obj_vertices.push_back(glm::vec4((start), (start), (start), x));
		obj_vertices.push_back(glm::vec4((start)+unit, (start), (start), x));
		obj_vertices.push_back(glm::vec4((start), (start)+unit, (start), x));

		obj_vertices.push_back(glm::vec4((start)+unit, (start), (start), x));
		obj_vertices.push_back(glm::vec4((start), (start)+unit, (start), x));
		obj_vertices.push_back(glm::vec4((start)+unit, (start)+unit, (start), x));
		for(int i =0 ; i<6;i++){
			vtx_normals.push_back(glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
		}

		//Left
		obj_vertices.push_back(glm::vec4((start), (start), (start), x));
		//TopFrontLeft
		obj_vertices.push_back(glm::vec4((start), (start)+unit, (start), x));
		obj_vertices.push_back(glm::vec4((start), (start), (start)+unit, x));

		obj_vertices.push_back(glm::vec4((start), (start)+unit, (start), x));
		obj_vertices.push_back(glm::vec4((start), (start)+unit, (start)+unit, x));
		obj_vertices.push_back(glm::vec4((start), (start), (start)+unit, x));
		for(int i =0 ; i<6;i++){
			vtx_normals.push_back(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
		}

		//Top
		obj_vertices.push_back(glm::vec4((start), (start)+unit, (start), x));
		obj_vertices.push_back(glm::vec4((start)+unit, (start)+unit, (start), x));
		//TopBackRight
		obj_vertices.push_back(glm::vec4((start)+unit, (start)+unit, (start)+unit, x));

		obj_vertices.push_back(glm::vec4((start), (start)+unit, (start), x));
		obj_vertices.push_back(glm::vec4((start), (start)+unit, (start)+unit, x));
		obj_vertices.push_back(glm::vec4((start)+unit, (start)+unit, (start)+unit, x));
		for(int i =0 ; i<6;i++){
			vtx_normals.push_back(glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
		}

		//Right 
		obj_vertices.push_back(glm::vec4((start)+unit, (start), (start), x));
		obj_vertices.push_back(glm::vec4((start)+unit, (start), (start)+unit, x));
		obj_vertices.push_back(glm::vec4((start)+unit, (start)+unit, (start)+unit, x));

		obj_vertices.push_back(glm::vec4((start)+unit, (start), (start), x));
		obj_vertices.push_back(glm::vec4((start)+unit, (start)+unit, (start), x));
		obj_vertices.push_back(glm::vec4((start)+unit, (start)+unit, (start)+unit, x));
		for(int i =0 ; i<6;i++){
			vtx_normals.push_back(glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f));
		}

		//Bottom
		obj_vertices.push_back(glm::vec4((start), (start), (start), x));
		obj_vertices.push_back(glm::vec4((start)+unit, (start), (start), x));
		obj_vertices.push_back(glm::vec4((start)+unit, (start), (start)+unit, x));

		obj_vertices.push_back(glm::vec4((start), (start), (start), x));
		obj_vertices.push_back(glm::vec4((start), (start), (start)+unit, x));
		obj_vertices.push_back(glm::vec4((start)+unit, (start), (start)+unit, x));
		for(int i =0 ; i<6;i++){
			vtx_normals.push_back(glm::vec4(0.0f, -1.0f, 0.0f, 0.0f));
		}

		// Back
		obj_vertices.push_back(glm::vec4((start), (start), (start)+unit, x));
		obj_vertices.push_back(glm::vec4((start)+unit, (start), (start)+unit, x));
		obj_vertices.push_back(glm::vec4((start)+unit, (start)+unit, (start)+unit, x));

		obj_vertices.push_back(glm::vec4((start), (start), (start)+unit, x));
		obj_vertices.push_back(glm::vec4((start), (start)+unit, (start)+unit, x));
		obj_vertices.push_back(glm::vec4((start)+unit, (start)+unit, (start)+unit, x));
		for(int i =0 ; i<6;i++){
			vtx_normals.push_back(glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
		}

	}
	else if(nesting_level_ == 1){
		l1Helper(obj_vertices,vtx_normals, start, unit, startPoint);
		bigI = 36 *20;
	}
	else if(nesting_level_ == 2){
		l2Helper(obj_vertices,vtx_normals, start, unit, startPoint);
		bigI = 36 *400;
	}
	else if(nesting_level_ == 3){
		l3Helper(obj_vertices,vtx_normals, start, unit, startPoint);
		bigI = 36 *8000;
	}
	else if(nesting_level_ == 4){
		l4Helper(obj_vertices,vtx_normals, start, unit, startPoint);
		bigI = 36 *160000;
	}




	// for(int i =0 ; i<(bigI);i++){
	// 	vtx_normals.push_back(glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
	// }

	int counter =0;
	while(counter<(bigI)){
		obj_faces.push_back(glm::uvec3(counter++, counter++, counter++));
	}


}
void 
Menger::gHelper(std::vector<glm::vec4>& obj_vertices,std::vector<glm::vec4>& vtx_normals,double start,double unit,glm::vec3 startPoint) const{
	//Scaling
	double x = 1.0f;
	//3d space transform
	double xx = startPoint.x;
	double y = startPoint.y;
	double z = startPoint.z;
	// Front
	// Starting point
	obj_vertices.push_back(glm::vec4((start+xx), (start+y), (start+z), x));
	obj_vertices.push_back(glm::vec4((start+xx)+unit, (start+y), (start+z), x));
	obj_vertices.push_back(glm::vec4((start+xx), (start+y)+unit, (start+z), x));

	obj_vertices.push_back(glm::vec4((start+xx)+unit, (start+y), (start+z), x));
	obj_vertices.push_back(glm::vec4((start+xx), (start+y)+unit, (start+z), x));
	obj_vertices.push_back(glm::vec4((start+xx)+unit, (start+y)+unit, (start+z), x));
	for(int i =0 ; i<6;i++){
		vtx_normals.push_back(glm::vec4(0.0f, 0.0f, 1.0f, 0.0f));
	}

	//Left
	obj_vertices.push_back(glm::vec4((start+xx), (start+y), (start+z), x));
	//TopFrontLeft
	obj_vertices.push_back(glm::vec4((start+xx), (start+y)+unit, (start+z), x));
	obj_vertices.push_back(glm::vec4((start+xx), (start+y), (start+z)+unit, x));

	obj_vertices.push_back(glm::vec4((start+xx), (start+y)+unit, (start+z), x));
	obj_vertices.push_back(glm::vec4((start+xx), (start+y)+unit, (start+z)+unit, x));
	obj_vertices.push_back(glm::vec4((start+xx), (start+y), (start+z)+unit, x));
	for(int i =0 ; i<6;i++){
		vtx_normals.push_back(glm::vec4(1.0f, 0.0f, 0.0f, 0.0f));
	}

	//Top
	obj_vertices.push_back(glm::vec4((start+xx), (start+y)+unit, (start+z), x));
	obj_vertices.push_back(glm::vec4((start+xx)+unit, (start+y)+unit, (start+z), x));
	//TopBackRight
	obj_vertices.push_back(glm::vec4((start+xx)+unit, (start+y)+unit, (start+z)+unit, x));

	obj_vertices.push_back(glm::vec4((start+xx), (start+y)+unit, (start+z), x));
	obj_vertices.push_back(glm::vec4((start+xx), (start+y)+unit, (start+z)+unit, x));
	obj_vertices.push_back(glm::vec4((start+xx)+unit, (start+y)+unit, (start+z)+unit, x));
	for(int i =0 ; i<6;i++){
		vtx_normals.push_back(glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
	}

	//Right 
	obj_vertices.push_back(glm::vec4((start+xx)+unit, (start+y), (start+z), x));
	obj_vertices.push_back(glm::vec4((start+xx)+unit, (start+y), (start+z)+unit, x));
	obj_vertices.push_back(glm::vec4((start+xx)+unit, (start+y)+unit, (start+z)+unit, x));

	obj_vertices.push_back(glm::vec4((start+xx)+unit, (start+y), (start+z), x));
	obj_vertices.push_back(glm::vec4((start+xx)+unit, (start+y)+unit, (start+z), x));
	obj_vertices.push_back(glm::vec4((start+xx)+unit, (start+y)+unit, (start+z)+unit, x));
	for(int i =0 ; i<6;i++){
		vtx_normals.push_back(glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f));
	}

	//Bottom
	obj_vertices.push_back(glm::vec4((start+xx), (start+y), (start+z), x));
	obj_vertices.push_back(glm::vec4((start+xx)+unit, (start+y), (start+z), x));
	obj_vertices.push_back(glm::vec4((start+xx)+unit, (start+y), (start+z)+unit, x));

	obj_vertices.push_back(glm::vec4((start+xx), (start+y), (start+z), x));
	obj_vertices.push_back(glm::vec4((start+xx), (start+y), (start+z)+unit, x));
	obj_vertices.push_back(glm::vec4((start+xx)+unit, (start+y), (start+z)+unit, x));
	for(int i =0 ; i<6;i++){
		vtx_normals.push_back(glm::vec4(0.0f, -1.0f, 0.0f, 0.0f));
	}

	// Back
	obj_vertices.push_back(glm::vec4((start+xx), (start+y), (start+z)+unit, x));
	obj_vertices.push_back(glm::vec4((start+xx)+unit, (start+y), (start+z)+unit, x));
	obj_vertices.push_back(glm::vec4((start+xx)+unit, (start+y)+unit, (start+z)+unit, x));

	obj_vertices.push_back(glm::vec4((start+xx), (start+y), (start+z)+unit, x));
	obj_vertices.push_back(glm::vec4((start+xx), (start+y)+unit, (start+z)+unit, x));
	obj_vertices.push_back(glm::vec4((start+xx)+unit, (start+y)+unit, (start+z)+unit, x));
	for(int i =0 ; i<6;i++){
		vtx_normals.push_back(glm::vec4(0.0f, 0.0f, -1.0f, 0.0f));
	}

}
void 
Menger::l1Helper(std::vector<glm::vec4>& obj_vertices, std::vector<glm::vec4>& vtx_normals,double start, double unit, glm::vec3 startPoint) const{
	double len = unit/3;
	double l1 = len;
	double l2 = len*2;
	startPoint.z-=len;

	//Front Slice - 8
	glm::vec3 startPoint1 (0.0f,0.0f,0.0f);
	startPoint1 = startPoint;
	gHelper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 += glm::vec3(l1,0.0f,0.0f);
	gHelper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,0.0f,0.0f);
	gHelper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(0.0f,l1,0.0f);
	gHelper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,l1,0.0f);
	gHelper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(0.0f,l2,0.0f);
	gHelper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l1,l2,0.0f);
	gHelper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,l2,0.0f);
	gHelper(obj_vertices,vtx_normals,start,len,startPoint1);

	//Middle Slice - 4
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(0.0f,0.0f,-l1);
	gHelper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,0.0f,-l1);
	gHelper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(0.0f,l2,-l1);
	gHelper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,l2,-l1);
	gHelper(obj_vertices,vtx_normals,start,len,startPoint1);

	//Back Slice - 8
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(0.0f,0.0f,-l2);
	gHelper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l1,0.0f,-l2);
	gHelper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,0.0f,-l2);
	gHelper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(0.0f,l1,-l2);
	gHelper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,l1,-l2);
	gHelper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(0.0f,l2,-l2);
	gHelper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l1,l2,-l2);
	gHelper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,l2,-l2);
	gHelper(obj_vertices,vtx_normals,start,len,startPoint1);
}
void 
Menger::l2Helper(std::vector<glm::vec4>& obj_vertices, std::vector<glm::vec4>& vtx_normals,double start, double unit, glm::vec3 startPoint) const{
	double len = unit/3;
	double l1 = len;
	double l2 = len*2;

	//Front Slice - 8
	glm::vec3 startPoint1 (0.0f,0.0f,0.0f);
	startPoint1 = startPoint;
	l1Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 += glm::vec3(l1,0.0f,0.0f);
	l1Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,0.0f,0.0f);
	l1Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(0.0f,l1,0.0f);
	l1Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,l1,0.0f);
	l1Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(0.0f,l2,0.0f);
	l1Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l1,l2,0.0f);
	l1Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,l2,0.0f);
	l1Helper(obj_vertices,vtx_normals,start,len,startPoint1);

	//Middle Slice - 4
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(0.0f,0.0f,-l1);
	l1Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,0.0f,-l1);
	l1Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(0.0f,l2,-l1);
	l1Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,l2,-l1);
	l1Helper(obj_vertices,vtx_normals,start,len,startPoint1);

	//Back Slice - 8
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(0.0f,0.0f,-l2);
	l1Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l1,0.0f,-l2);
	l1Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,0.0f,-l2);
	l1Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(0.0f,l1,-l2);
	l1Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,l1,-l2);
	l1Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(0.0f,l2,-l2);
	l1Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l1,l2,-l2);
	l1Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,l2,-l2);
	l1Helper(obj_vertices,vtx_normals,start,len,startPoint1);

}
void 
Menger::l3Helper(std::vector<glm::vec4>& obj_vertices, std::vector<glm::vec4>& vtx_normals,double start, double unit, glm::vec3 startPoint) const{
	double len = unit/3;
	double l1 = len;
	double l2 = len*2;

	//Front Slice - 8
	glm::vec3 startPoint1 (0.0f,0.0f,0.0f);
	startPoint1 = startPoint;
	l2Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 += glm::vec3(l1,0.0f,0.0f);
	l2Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,0.0f,0.0f);
	l2Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(0.0f,l1,0.0f);
	l2Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,l1,0.0f);
	l2Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(0.0f,l2,0.0f);
	l2Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l1,l2,0.0f);
	l2Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,l2,0.0f);
	l2Helper(obj_vertices,vtx_normals,start,len,startPoint1);

	//Middle Slice - 4
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(0.0f,0.0f,-l1);
	l2Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,0.0f,-l1);
	l2Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(0.0f,l2,-l1);
	l2Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,l2,-l1);
	l2Helper(obj_vertices,vtx_normals,start,len,startPoint1);

	//Back Slice - 8
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(0.0f,0.0f,-l2);
	l2Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l1,0.0f,-l2);
	l2Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,0.0f,-l2);
	l2Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(0.0f,l1,-l2);
	l2Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,l1,-l2);
	l2Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(0.0f,l2,-l2);
	l2Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l1,l2,-l2);
	l2Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,l2,-l2);
	l2Helper(obj_vertices,vtx_normals,start,len,startPoint1);

}
void 
Menger::l4Helper(std::vector<glm::vec4>& obj_vertices, std::vector<glm::vec4>& vtx_normals,double start, double unit, glm::vec3 startPoint) const{
	double len = unit/3;
	double l1 = len;
	double l2 = len*2;

	//Front Slice - 8
	glm::vec3 startPoint1 (0.0f,0.0f,0.0f);
	startPoint1 = startPoint;
	l3Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 += glm::vec3(l1,0.0f,0.0f);
	l3Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,0.0f,0.0f);
	l3Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(0.0f,l1,0.0f);
	l3Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,l1,0.0f);
	l3Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(0.0f,l2,0.0f);
	l3Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l1,l2,0.0f);
	l3Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,l2,0.0f);
	l3Helper(obj_vertices,vtx_normals,start,len,startPoint1);

	//Middle Slice - 4
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(0.0f,0.0f,-l1);
	l3Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,0.0f,-l1);
	l3Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(0.0f,l2,-l1);
	l3Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,l2,-l1);
	l3Helper(obj_vertices,vtx_normals,start,len,startPoint1);

	//Back Slice - 8
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(0.0f,0.0f,-l2);
	l3Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l1,0.0f,-l2);
	l3Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,0.0f,-l2);
	l3Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(0.0f,l1,-l2);
	l3Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,l1,-l2);
	l3Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(0.0f,l2,-l2);
	l3Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l1,l2,-l2);
	l3Helper(obj_vertices,vtx_normals,start,len,startPoint1);
	startPoint1 = startPoint;
	startPoint1 += glm::vec3(l2,l2,-l2);
	l3Helper(obj_vertices,vtx_normals,start,len,startPoint1);

}

