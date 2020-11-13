#include "ivypch.h"
#include "Cube.h"

Ivy::Cube::Cube(Entity * ent) : Ivy::Mesh::Mesh(ent)
{
	Vec4 position;
	Vec3 normal;
	Vec3 tangent;
	Vec3 bitangent;
	Vec2 texcoord;

	//mSubMesh.vertices = 
	//	// back face
	//	{{Vec4(-1.0f, -1.0f, -1.0f, 1.0f)}, {Vec3(-1.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(0.0f,  0.0f)}},		   // bottom-left
	//	{{Vec4( 1.0f,  1.0f, -1.0f, 1.0f)}, {Vec3(-1.0f, 1.0f, 1.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(0.0f,  0.0f)}},		   // top-right
	//	{{Vec4( 1.0f, -1.0f, -1.0f, 1.0f)}, {Vec3(-1.0f, 1.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(0.0f,  0.0f)}},		   // bottom-right         
	//	{{Vec4( 1.0f,  1.0f, -1.0f, 1.0f)}, {Vec3(-1.0f, 1.0f, 1.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(0.0f,  0.0f)}},		   // top-right
	//	{{Vec4(-1.0f, -1.0f, -1.0f, 1.0f)}, {Vec3(-1.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(0.0f,  0.0f)}},		   // bottom-left
	//	{{Vec4(-1.0f,  1.0f, -1.0f, 1.0f)}, {Vec3(-1.0f, 0.0f, 1.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(0.0f,  0.0f)}},		   // top-left
	//	// front face																														  
	//	{{Vec4(-1.0f, -1.0f,  1.0f, 1.0f)}, {Vec3( 1.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(0.0f,  0.0f)}}, 	   // bottom-left
	//	{{Vec4( 1.0f, -1.0f,  1.0f, 1.0f)}, {Vec3( 1.0f, 1.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(0.0f,  0.0f)}}, 	   // bottom-right
	//	{{Vec4( 1.0f,  1.0f,  1.0f, 1.0f)}, {Vec3( 1.0f, 1.0f, 1.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(0.0f,  0.0f)}}, 	   // top-right
	//	{{Vec4( 1.0f,  1.0f,  1.0f, 1.0f)}, {Vec3( 1.0f, 1.0f, 1.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(0.0f,  0.0f)}}, 	   // top-right
	//	{{Vec4(-1.0f,  1.0f,  1.0f, 1.0f)}, {Vec3( 1.0f, 0.0f, 1.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(0.0f,  0.0f)}}, 	   // top-left
	//	{{Vec4(-1.0f, -1.0f,  1.0f, 1.0f)}, {Vec3( 1.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(0.0f,  0.0f)}}, 	   // bottom-left
	//	// left face							     				   	  																	  
	//	{{Vec4(-1.0f,  1.0f,  1.0f, 1.0f)}, {Vec3( 0.0f, 1.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(1.0f,  0.0f)}}, 	   // top-right
	//	{{Vec4(-1.0f,  1.0f, -1.0f, 1.0f)}, {Vec3( 0.0f, 1.0f, 1.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(1.0f,  0.0f)}}, 	   // top-left
	//	{{Vec4(-1.0f, -1.0f, -1.0f, 1.0f)}, {Vec3( 0.0f, 0.0f, 1.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(1.0f,  0.0f)}}, 	   // bottom-left
	//	{{Vec4(-1.0f, -1.0f, -1.0f, 1.0f)}, {Vec3( 0.0f, 0.0f, 1.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(1.0f,  0.0f)}}, 	   // bottom-left
	//	{{Vec4(-1.0f, -1.0f,  1.0f, 1.0f)}, {Vec3( 0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(1.0f,  0.0f)}}, 	   // bottom-right
	//	{{Vec4(-1.0f,  1.0f,  1.0f, 1.0f)}, {Vec3( 0.0f, 1.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(1.0f,  0.0f)}}, 	   // top-right
	//	// right face			 				     				  	  																	  
	//	{{Vec4( 1.0f,  1.0f,  1.0f, 1.0f)}, {Vec3( 0.0f, 1.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(1.0f,  0.0f)}}, 	   // top-left
	//	{{Vec4( 1.0f, -1.0f, -1.0f, 1.0f)}, {Vec3( 0.0f, 0.0f, 1.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(1.0f,  0.0f)}}, 	   // bottom-right
	//	{{Vec4( 1.0f,  1.0f, -1.0f, 1.0f)}, {Vec3( 0.0f, 1.0f, 1.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(1.0f,  0.0f)}}, 	   // top-right         
	//	{{Vec4( 1.0f, -1.0f, -1.0f, 1.0f)}, {Vec3( 0.0f, 0.0f, 1.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(1.0f,  0.0f)}}, 	   // bottom-right
	//	{{Vec4( 1.0f,  1.0f,  1.0f, 1.0f)}, {Vec3( 0.0f, 1.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(1.0f,  0.0f)}}, 	   // top-left
	//	{{Vec4( 1.0f, -1.0f,  1.0f, 1.0f)}, {Vec3( 0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(1.0f,  0.0f)}}, 	   // bottom-left     
	//	// bottom face						        				      																	  
	//	{{Vec4(-1.0f, -1.0f, -1.0f, 1.0f)}, {Vec3( 0.0f, 0.0f, 1.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(0.0f, -1.0f)}}, 	   // top-right
	//	{{Vec4( 1.0f, -1.0f, -1.0f, 1.0f)}, {Vec3( 0.0f, 1.0f, 1.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(0.0f, -1.0f)}}, 	   // top-left
	//	{{Vec4( 1.0f, -1.0f,  1.0f, 1.0f)}, {Vec3( 0.0f, 1.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(0.0f, -1.0f)}}, 	   // bottom-left
	//	{{Vec4( 1.0f, -1.0f,  1.0f, 1.0f)}, {Vec3( 0.0f, 1.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(0.0f, -1.0f)}}, 	   // bottom-left
	//	{{Vec4(-1.0f, -1.0f,  1.0f, 1.0f)}, {Vec3( 0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(0.0f, -1.0f)}}, 	   // bottom-right
	//	{{Vec4(-1.0f, -1.0f, -1.0f, 1.0f)}, {Vec3( 0.0f, 0.0f, 1.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(0.0f, -1.0f)}}, 	   // top-right
	//	// top face								       				   	  																  
	//	{{Vec4(-1.0f,  1.0f, -1.0f, 1.0f)}, {Vec3( 0.0f, 0.0f, 1.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(0.0f,  1.0f)}}, 	   // top-left
	//	{{Vec4( 1.0f,  1.0f , 1.0f, 1.0f)}, {Vec3( 0.0f, 1.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(0.0f,  1.0f)}}, 	   // bottom-right
	//	{{Vec4( 1.0f,  1.0f, -1.0f, 1.0f)}, {Vec3( 0.0f, 1.0f, 1.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(0.0f,  1.0f)}}, 	   // top-right     
	//	{{Vec4( 1.0f,  1.0f,  1.0f, 1.0f)}, {Vec3( 0.0f, 1.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(0.0f,  1.0f)}}, 	   // bottom-right
	//	{{Vec4(-1.0f,  1.0f, -1.0f, 1.0f)}, {Vec3( 0.0f, 0.0f, 1.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(0.0f,  1.0f)}}, 	   // top-left
	//	{{Vec4(-1.0f,  1.0f,  1.0f, 1.0f)}, {Vec3( 0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec3(0.0f, 0.0f, 0.0f)}, {Vec2(0.0f,  1.0f)}};	   // bottom-left        
}
