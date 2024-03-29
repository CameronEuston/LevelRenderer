// "ModularColumn_bottom.h" generated by "Obj2Header.exe" [Version 1.9d] Author: L.Norri Fullsail University.
// Data is converted to left-handed coordinate system and UV data is V flipped for Direct3D/Vulkan.
// The companion file "ModularColumn_bottom.h2b" is a binary dump of this format(-padding) for more flexibility.
// Loading *.h2b: read version, sizes, handle strings(max len 260) by reading until null-terminated.
/***********************/
/*  Generator version  */
/***********************/
#ifndef _ModularColumn_bottom_version_
const char ModularColumn_bottom_version[4] = { '0','1','9','d' };
#define _ModularColumn_bottom_version_
#endif
/************************************************/
/*  This section contains the model's size data */
/************************************************/
#ifndef _ModularColumn_bottom_vertexcount_
const unsigned ModularColumn_bottom_vertexcount = 164;
#define _ModularColumn_bottom_vertexcount_
#endif
#ifndef _ModularColumn_bottom_indexcount_
const unsigned ModularColumn_bottom_indexcount = 276;
#define _ModularColumn_bottom_indexcount_
#endif
#ifndef _ModularColumn_bottom_materialcount_
const unsigned ModularColumn_bottom_materialcount = 2; // can be used for batched draws
#define _ModularColumn_bottom_materialcount_
#endif
#ifndef _ModularColumn_bottom_meshcount_
const unsigned ModularColumn_bottom_meshcount = 2;
#define _ModularColumn_bottom_meshcount_
#endif
/************************************************/
/*  This section contains the raw data to load  */
/************************************************/
#ifndef __OBJ_VEC3__
typedef struct _OBJ_VEC3_
{
	float x,y,z; // 3D Coordinate.
}OBJ_VEC3;
#define __OBJ_VEC3__
#endif
#ifndef __OBJ_VERT__
typedef struct _OBJ_VERT_
{
	OBJ_VEC3 pos; // Left-handed +Z forward coordinate w not provided, assumed to be 1.
	OBJ_VEC3 uvw; // D3D/Vulkan style top left 0,0 coordinate.
	OBJ_VEC3 nrm; // Provided direct from obj file, may or may not be normalized.
}OBJ_VERT;
#define __OBJ_VERT__
#endif
#ifndef _ModularColumn_bottom_vertices_
// Raw Vertex Data follows: Positions, Texture Coordinates and Normals.
const OBJ_VERT ModularColumn_bottom_vertices[164] =
{
	{	{ -0.415135f, 0.225874f, 0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -0.010300f, 0.999900f, -0.000000f }	},
	{	{ -0.415135f, 0.225874f, -0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -0.010300f, 0.999900f, -0.000000f }	},
	{	{ -0.456535f, 0.225447f, -0.455395f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -0.010300f, 0.999900f, -0.000000f }	},
	{	{ 0.454255f, -0.031408f, -0.455395f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -0.003800f, -1.000000f, 0.003800f }	},
	{	{ 0.454255f, -0.031408f, 0.455395f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -0.003800f, -1.000000f, -0.003800f }	},
	{	{ 0.412855f, -0.031097f, 0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -0.003800f, -1.000000f, -0.003800f }	},
	{	{ 0.412855f, 0.278139f, 0.206998f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.000000f, 1.000000f }	},
	{	{ 0.412855f, 0.225874f, 0.206998f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.000000f, 1.000000f }	},
	{	{ 0.454043f, 0.225874f, 0.206998f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.000000f, 1.000000f }	},
	{	{ -0.415135f, 0.225874f, -0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.999900f, -0.010300f }	},
	{	{ 0.412855f, 0.225874f, -0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.003900f, 1.000000f, -0.005900f }	},
	{	{ 0.454255f, 0.225447f, -0.455395f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.004300f, 1.000000f, -0.006000f }	},
	{	{ 0.412855f, 0.225874f, 0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.003900f, 1.000000f, 0.005900f }	},
	{	{ -0.415135f, 0.225874f, 0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.999900f, 0.010300f }	},
	{	{ -0.456535f, 0.225447f, 0.455395f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.999900f, 0.010300f }	},
	{	{ -0.456535f, -0.031408f, 0.455395f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -1.000000f, -0.000000f, -0.000000f }	},
	{	{ -0.456535f, 0.225447f, 0.455395f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -1.000000f, -0.000000f, -0.000000f }	},
	{	{ -0.456535f, 0.225447f, -0.455395f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -1.000000f, -0.000000f, -0.000000f }	},
	{	{ 0.454255f, 0.225447f, 0.455395f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.004300f, 1.000000f, 0.006000f }	},
	{	{ 0.454043f, 0.225874f, 0.206998f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.001100f, 1.000000f, 0.000200f }	},
	{	{ 0.454255f, -0.031408f, -0.455395f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 1.000000f, 0.000400f, 0.000400f }	},
	{	{ 0.454043f, 0.225874f, -0.206998f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 1.000000f, 0.000400f, 0.000100f }	},
	{	{ 0.454255f, -0.031408f, 0.455395f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 1.000000f, 0.000400f, -0.000400f }	},
	{	{ -0.415135f, -0.031097f, 0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.003800f, -1.000000f, -0.003800f }	},
	{	{ -0.415135f, -0.031097f, -0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.003800f, -1.000000f, 0.003800f }	},
	{	{ -0.456535f, -0.031408f, 0.455395f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.003800f, -1.000000f, -0.003800f }	},
	{	{ 0.454043f, 0.225874f, 0.206998f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 1.000000f, 0.000400f, -0.000100f }	},
	{	{ 0.454043f, 0.278139f, -0.206998f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 1.000000f, -0.000000f, -0.000000f }	},
	{	{ 0.349967f, 0.350641f, -0.175554f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 1.000000f, -0.000000f }	},
	{	{ 0.349967f, 0.350641f, 0.175554f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 1.000000f, -0.000000f }	},
	{	{ 0.391156f, 0.350641f, 0.175554f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 1.000000f, -0.000000f }	},
	{	{ -0.456535f, -0.031408f, 0.455395f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.000000f, 1.000000f }	},
	{	{ 0.454255f, -0.031408f, 0.455395f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.000000f, 1.000000f }	},
	{	{ 0.454255f, 0.225447f, 0.455395f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.000000f, 1.000000f }	},
	{	{ 0.454255f, -0.031408f, -0.455395f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, -0.000000f, -1.000000f }	},
	{	{ -0.456535f, -0.031408f, -0.455395f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, -0.000000f, -1.000000f }	},
	{	{ -0.456535f, 0.225447f, -0.455395f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, -0.000000f, -1.000000f }	},
	{	{ 0.412855f, 0.278139f, -0.206998f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.397900f, -0.917400f }	},
	{	{ 0.349967f, 0.350641f, -0.175554f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.397900f, -0.917400f }	},
	{	{ 0.391156f, 0.350641f, -0.175554f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.397900f, -0.917400f }	},
	{	{ 0.412855f, 0.225874f, -0.206998f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 1.000000f, -0.000000f }	},
	{	{ 0.454043f, 0.225874f, -0.206998f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.001100f, 1.000000f, -0.000200f }	},
	{	{ 0.412855f, -0.031097f, -0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -0.003800f, -1.000000f, 0.003800f }	},
	{	{ -0.456535f, -0.031408f, -0.455395f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.003800f, -1.000000f, 0.003800f }	},
	{	{ 0.412855f, 0.278139f, 0.206998f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.397900f, 0.917400f }	},
	{	{ 0.454043f, 0.278139f, 0.206998f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.397900f, 0.917400f }	},
	{	{ 0.391156f, 0.350641f, 0.175554f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.397900f, 0.917400f }	},
	{	{ 0.454043f, 0.278139f, -0.206998f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.755400f, 0.655200f, -0.000000f }	},
	{	{ 0.391156f, 0.350641f, -0.175554f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.755400f, 0.655200f, -0.000000f }	},
	{	{ 0.391156f, 0.350641f, 0.175554f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.755400f, 0.655200f, -0.000000f }	},
	{	{ 0.412855f, 0.225874f, -0.206998f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, -0.000000f, -1.000000f }	},
	{	{ 0.412855f, 0.278139f, -0.206998f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, -0.000000f, -1.000000f }	},
	{	{ 0.454043f, 0.278139f, -0.206998f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, -0.000000f, -1.000000f }	},
	{	{ -0.456535f, 0.225447f, 0.455395f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -0.010300f, 0.999900f, -0.000000f }	},
	{	{ 0.454043f, 0.278139f, 0.206998f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.000000f, 1.000000f }	},
	{	{ -0.456535f, 0.225447f, -0.455395f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.999900f, -0.010300f }	},
	{	{ -0.456535f, -0.031408f, -0.455395f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -1.000000f, -0.000000f, -0.000000f }	},
	{	{ 0.412855f, 0.225874f, 0.206998f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 1.000000f, -0.000000f }	},
	{	{ 0.454255f, 0.225447f, 0.455395f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 1.000000f, 0.000000f, -0.000900f }	},
	{	{ 0.454255f, 0.225447f, -0.455395f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 1.000000f, -0.000000f, 0.000900f }	},
	{	{ 0.454043f, 0.278139f, 0.206998f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 1.000000f, -0.000000f, -0.000000f }	},
	{	{ 0.391156f, 0.350641f, -0.175554f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 1.000000f, -0.000000f }	},
	{	{ -0.456535f, 0.225447f, 0.455395f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.000000f, 1.000000f }	},
	{	{ 0.454255f, 0.225447f, -0.455395f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, -0.000000f, -1.000000f }	},
	{	{ 0.454043f, 0.278139f, -0.206998f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.397900f, -0.917400f }	},
	{	{ 0.349967f, 0.350641f, 0.175554f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.397900f, 0.917400f }	},
	{	{ 0.454043f, 0.278139f, 0.206998f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.755400f, 0.655200f, -0.000000f }	},
	{	{ 0.454043f, 0.225874f, -0.206998f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, -0.000000f, -1.000000f }	},
	{	{ 0.481730f, -0.110850f, 0.482870f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.155200f, 0.987900f }	},
	{	{ -0.484010f, -0.110850f, 0.482870f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.155200f, 0.987900f }	},
	{	{ -0.508407f, -0.266168f, 0.507267f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.155200f, 0.987900f }	},
	{	{ -0.484010f, -0.110850f, -0.482870f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.155200f, -0.987900f }	},
	{	{ 0.481730f, -0.110850f, -0.482870f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.155200f, -0.987900f }	},
	{	{ 0.506127f, -0.266168f, -0.507267f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.155200f, -0.987900f }	},
	{	{ -0.508407f, -0.266168f, 0.507267f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -0.987900f, 0.155200f, -0.000000f }	},
	{	{ -0.484010f, -0.110850f, 0.482870f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -0.987900f, 0.155200f, -0.000000f }	},
	{	{ -0.484010f, -0.110850f, -0.482870f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -0.987900f, 0.155200f, -0.000000f }	},
	{	{ 0.481730f, -0.110850f, -0.482870f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.978800f, -0.204900f }	},
	{	{ -0.484010f, -0.110850f, -0.482870f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.978800f, -0.204900f }	},
	{	{ -0.415135f, -0.096429f, -0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.978800f, -0.204900f }	},
	{	{ -0.484010f, -0.110850f, 0.482870f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.978800f, 0.204900f }	},
	{	{ 0.481730f, -0.110850f, 0.482870f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.978800f, 0.204900f }	},
	{	{ 0.412855f, -0.096429f, 0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.978800f, 0.204900f }	},
	{	{ 0.412855f, 0.225874f, -0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, -0.000000f, -1.000000f }	},
	{	{ -0.415135f, 0.225874f, -0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, -0.000000f, -1.000000f }	},
	{	{ -0.415135f, 0.278139f, -0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, -0.000000f, -1.000000f }	},
	{	{ -0.415135f, 0.225874f, 0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.000000f, 1.000000f }	},
	{	{ 0.412855f, 0.225874f, 0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.000000f, 1.000000f }	},
	{	{ 0.412855f, 0.278139f, 0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.000000f, 1.000000f }	},
	{	{ 0.412855f, 0.225874f, 0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 1.000000f, -0.000000f, -0.000000f }	},
	{	{ 0.412855f, 0.225874f, 0.206998f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 1.000000f, -0.000000f, -0.000000f }	},
	{	{ 0.412855f, 0.278139f, 0.206998f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 1.000000f, -0.000000f, -0.000000f }	},
	{	{ 0.349967f, 0.350641f, 0.351107f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.655200f, 0.755400f }	},
	{	{ -0.352247f, 0.350641f, 0.351107f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.655200f, 0.755400f }	},
	{	{ -0.415135f, 0.278139f, 0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.655200f, 0.755400f }	},
	{	{ 0.349967f, 0.350641f, -0.351107f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.755400f, 0.655200f, -0.000000f }	},
	{	{ 0.349967f, 0.350641f, -0.175554f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.755400f, 0.655200f, -0.000000f }	},
	{	{ 0.412855f, 0.278139f, -0.206998f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.755400f, 0.655200f, -0.000000f }	},
	{	{ -0.352247f, 0.350641f, -0.351107f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.655200f, -0.755400f }	},
	{	{ 0.349967f, 0.350641f, -0.351107f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.655200f, -0.755400f }	},
	{	{ 0.412855f, 0.278139f, -0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.655200f, -0.755400f }	},
	{	{ -0.508407f, -0.266168f, 0.507267f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, -1.000000f, 0.000000f }	},
	{	{ -0.508407f, -0.266168f, -0.507267f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, -1.000000f, 0.000000f }	},
	{	{ 0.506127f, -0.266168f, -0.507267f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, -1.000000f, 0.000000f }	},
	{	{ -0.415135f, 0.225874f, -0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -1.000000f, -0.000000f, -0.000000f }	},
	{	{ -0.415135f, 0.225874f, 0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -1.000000f, -0.000000f, -0.000000f }	},
	{	{ -0.415135f, 0.278139f, 0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -1.000000f, -0.000000f, -0.000000f }	},
	{	{ 0.481730f, -0.110850f, 0.482870f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.204900f, 0.978800f, -0.000000f }	},
	{	{ 0.481730f, -0.110850f, -0.482870f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.204900f, 0.978800f, -0.000000f }	},
	{	{ 0.412855f, -0.096429f, -0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.204900f, 0.978800f, -0.000000f }	},
	{	{ -0.484010f, -0.110850f, -0.482870f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -0.204900f, 0.978800f, -0.000000f }	},
	{	{ -0.484010f, -0.110850f, 0.482870f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -0.204900f, 0.978800f, -0.000000f }	},
	{	{ -0.415135f, -0.096429f, 0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -0.204900f, 0.978800f, -0.000000f }	},
	{	{ 0.481730f, -0.110850f, -0.482870f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.987900f, 0.155200f, -0.000000f }	},
	{	{ 0.481730f, -0.110850f, 0.482870f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.987900f, 0.155200f, -0.000000f }	},
	{	{ 0.506127f, -0.266168f, 0.507267f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.987900f, 0.155200f, -0.000000f }	},
	{	{ 0.412855f, 0.225874f, -0.206998f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 1.000000f, -0.000000f, -0.000000f }	},
	{	{ 0.412855f, 0.225874f, -0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 1.000000f, -0.000000f, -0.000000f }	},
	{	{ 0.412855f, 0.278139f, -0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 1.000000f, -0.000000f, -0.000000f }	},
	{	{ 0.349967f, 0.350641f, 0.351107f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.755400f, 0.655200f, -0.000000f }	},
	{	{ 0.412855f, 0.278139f, 0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.755400f, 0.655200f, -0.000000f }	},
	{	{ 0.412855f, 0.278139f, 0.206998f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.755400f, 0.655200f, -0.000000f }	},
	{	{ -0.415135f, 0.278139f, 0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -0.755400f, 0.655200f, -0.000000f }	},
	{	{ -0.352247f, 0.350641f, 0.351107f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -0.755400f, 0.655200f, -0.000000f }	},
	{	{ -0.352247f, 0.350641f, -0.351107f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -0.755400f, 0.655200f, -0.000000f }	},
	{	{ 0.412855f, -0.096429f, -0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, -0.000000f, -1.000000f }	},
	{	{ -0.415135f, -0.096429f, -0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, -0.000000f, -1.000000f }	},
	{	{ -0.415135f, -0.031097f, -0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, -0.000000f, -1.000000f }	},
	{	{ -0.415135f, -0.096429f, 0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.000000f, 1.000000f }	},
	{	{ 0.412855f, -0.096429f, 0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.000000f, 1.000000f }	},
	{	{ 0.412855f, -0.031097f, 0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.000000f, 1.000000f }	},
	{	{ 0.412855f, -0.096429f, 0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 1.000000f, -0.000000f, -0.000000f }	},
	{	{ 0.412855f, -0.096429f, -0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 1.000000f, -0.000000f, -0.000000f }	},
	{	{ 0.412855f, -0.031097f, -0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 1.000000f, -0.000000f, -0.000000f }	},
	{	{ -0.352247f, 0.350641f, -0.351107f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 1.000000f, -0.000000f }	},
	{	{ 0.349967f, 0.350641f, -0.351107f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 1.000000f, -0.000000f }	},
	{	{ -0.415135f, -0.096429f, -0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -1.000000f, -0.000000f, -0.000000f }	},
	{	{ -0.415135f, -0.096429f, 0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -1.000000f, -0.000000f, -0.000000f }	},
	{	{ -0.415135f, -0.031097f, 0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -1.000000f, -0.000000f, -0.000000f }	},
	{	{ 0.506127f, -0.266168f, 0.507267f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.155200f, 0.987900f }	},
	{	{ -0.508407f, -0.266168f, -0.507267f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.155200f, -0.987900f }	},
	{	{ -0.508407f, -0.266168f, -0.507267f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -0.987900f, 0.155200f, -0.000000f }	},
	{	{ 0.412855f, -0.096429f, -0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.978800f, -0.204900f }	},
	{	{ -0.415135f, -0.096429f, 0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.978800f, 0.204900f }	},
	{	{ 0.412855f, 0.278139f, -0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, -0.000000f, -1.000000f }	},
	{	{ -0.415135f, 0.278139f, 0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.000000f, 1.000000f }	},
	{	{ 0.412855f, 0.278139f, 0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 1.000000f, -0.000000f, -0.000000f }	},
	{	{ 0.412855f, 0.278139f, 0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.655200f, 0.755400f }	},
	{	{ 0.412855f, 0.278139f, -0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.755400f, 0.655200f, -0.000000f }	},
	{	{ -0.415135f, 0.278139f, -0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.655200f, -0.755400f }	},
	{	{ 0.506127f, -0.266168f, 0.507267f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, -1.000000f, 0.000000f }	},
	{	{ -0.415135f, 0.278139f, -0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -1.000000f, -0.000000f, -0.000000f }	},
	{	{ 0.412855f, -0.096429f, 0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.204900f, 0.978800f, -0.000000f }	},
	{	{ -0.415135f, -0.096429f, -0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -0.204900f, 0.978800f, -0.000000f }	},
	{	{ 0.506127f, -0.266168f, -0.507267f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.987900f, 0.155200f, -0.000000f }	},
	{	{ 0.412855f, 0.278139f, -0.206998f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 1.000000f, -0.000000f, -0.000000f }	},
	{	{ 0.349967f, 0.350641f, 0.175554f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.755400f, 0.655200f, -0.000000f }	},
	{	{ -0.415135f, 0.278139f, -0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -0.755400f, 0.655200f, -0.000000f }	},
	{	{ 0.412855f, -0.031097f, -0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, -0.000000f, -1.000000f }	},
	{	{ -0.415135f, -0.031097f, 0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 0.000000f, 1.000000f }	},
	{	{ 0.412855f, -0.031097f, 0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 1.000000f, -0.000000f, -0.000000f }	},
	{	{ -0.352247f, 0.350641f, 0.351107f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 1.000000f, -0.000000f }	},
	{	{ 0.349967f, 0.350641f, 0.351107f },	{ 0.000000f, 0.000000f, 0.000000f },	{ 0.000000f, 1.000000f, -0.000000f }	},
	{	{ -0.415135f, -0.031097f, -0.413995f },	{ 0.000000f, 0.000000f, 0.000000f },	{ -1.000000f, -0.000000f, -0.000000f }	},
};
#define _ModularColumn_bottom_vertices_
#endif
#ifndef _ModularColumn_bottom_indices_
// Index Data follows: Sequential by mesh, Every Three Indices == One Triangle.
const unsigned int ModularColumn_bottom_indices[276] =
{
	 0, 1, 2,
	 3, 4, 5,
	 6, 7, 8,
	 9, 10, 11,
	 12, 13, 14,
	 15, 16, 17,
	 12, 18, 19,
	 20, 21, 22,
	 23, 5, 4,
	 24, 23, 25,
	 26, 21, 27,
	 28, 29, 30,
	 31, 32, 33,
	 34, 35, 36,
	 37, 38, 39,
	 10, 40, 41,
	 42, 24, 43,
	 44, 45, 46,
	 47, 48, 49,
	 50, 51, 52,
	 0, 2, 53,
	 3, 5, 42,
	 6, 8, 54,
	 9, 11, 55,
	 12, 14, 18,
	 15, 17, 56,
	 12, 19, 57,
	 58, 22, 26,
	 26, 22, 21,
	 21, 20, 59,
	 23, 4, 25,
	 24, 25, 43,
	 26, 27, 60,
	 28, 30, 61,
	 31, 33, 62,
	 34, 36, 63,
	 37, 39, 64,
	 10, 41, 11,
	 42, 43, 3,
	 44, 46, 65,
	 47, 49, 66,
	 50, 52, 67,
	 68, 69, 70,
	 71, 72, 73,
	 74, 75, 76,
	 77, 78, 79,
	 80, 81, 82,
	 83, 84, 85,
	 86, 87, 88,
	 89, 90, 91,
	 92, 93, 94,
	 95, 96, 97,
	 98, 99, 100,
	 101, 102, 103,
	 104, 105, 106,
	 107, 108, 109,
	 110, 111, 112,
	 113, 114, 115,
	 116, 117, 118,
	 119, 120, 121,
	 122, 123, 124,
	 125, 126, 127,
	 128, 129, 130,
	 131, 132, 133,
	 134, 28, 135,
	 136, 137, 138,
	 68, 70, 139,
	 71, 73, 140,
	 74, 76, 141,
	 77, 79, 142,
	 80, 82, 143,
	 83, 85, 144,
	 86, 88, 145,
	 89, 91, 146,
	 92, 94, 147,
	 95, 97, 148,
	 98, 100, 149,
	 101, 103, 150,
	 104, 106, 151,
	 107, 109, 152,
	 110, 112, 153,
	 113, 115, 154,
	 116, 118, 155,
	 119, 121, 156,
	 122, 124, 157,
	 125, 127, 158,
	 128, 130, 159,
	 131, 133, 160,
	 28, 134, 29,
	 29, 161, 162,
	 134, 161, 29,
	 136, 138, 163,
};
#define _ModularColumn_bottom_indices_
#endif
// Part of an OBJ_MATERIAL, the struct is 16 byte aligned so it is GPU register friendly.
#ifndef __OBJ_ATTRIBUTES__
typedef struct _OBJ_ATTRIBUTES_
{
	OBJ_VEC3    Kd; // diffuse reflectivity
	float	    d; // dissolve (transparency) 
	OBJ_VEC3    Ks; // specular reflectivity
	float       Ns; // specular exponent
	OBJ_VEC3    Ka; // ambient reflectivity
	float       sharpness; // local reflection map sharpness
	OBJ_VEC3    Tf; // transmission filter
	float       Ni; // optical density (index of refraction)
	OBJ_VEC3    Ke; // emissive reflectivity
	unsigned    illum; // illumination model
}OBJ_ATTRIBUTES;
#define __OBJ_ATTRIBUTES__
#endif
// This structure also has been made GPU register friendly so it can be transfered directly if desired.
// Note: Total struct size will vary depenedening on CPU processor architecture (string pointers).
#ifndef __OBJ_MATERIAL__
typedef struct _OBJ_MATERIAL_
{
	// The following items are typically used in a pixel/fragment shader, they are packed for GPU registers.
	OBJ_ATTRIBUTES attrib; // Surface shading characteristics & illumination model
	// All items below this line are not needed on the GPU and are generally only used during load time.
	const char* name; // the name of this material
	// If the model's materials contain any specific texture data it will be located below.
	const char* map_Kd; // diffuse texture
	const char* map_Ks; // specular texture
	const char* map_Ka; // ambient texture
	const char* map_Ke; // emissive texture
	const char* map_Ns; // specular exponent texture
	const char* map_d; // transparency texture
	const char* disp; // roughness map (displacement)
	const char* decal; // decal texture (lerps texture & material colors)
	const char* bump; // normal/bumpmap texture
	void* padding[2]; // 16 byte alignment on 32bit or 64bit
}OBJ_MATERIAL;
#define __OBJ_MATERIAL__
#endif
#ifndef _ModularColumn_bottom_materials_
// Material Data follows: pulled from a .mtl file of the same name if present.
const OBJ_MATERIAL ModularColumn_bottom_materials[2] =
{
	{
		{{ 0.287072f, 0.287072f, 0.287072f },
		1.000000f,
		{ 0.500000f, 0.500000f, 0.500000f },
		96.078423f,
		{ 1.000000f, 1.000000f, 1.000000f },
		60.000000f,
		{ 1.000000f, 1.000000f, 1.000000f },
		1.000000f,
		{ 0.000000f, 0.000000f, 0.000000f },
		2},
		"Rock.005",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
	},
	{
		{{ 0.510617f, 0.510617f, 0.510617f },
		1.000000f,
		{ 0.500000f, 0.500000f, 0.500000f },
		96.078423f,
		{ 1.000000f, 1.000000f, 1.000000f },
		60.000000f,
		{ 1.000000f, 1.000000f, 1.000000f },
		1.000000f,
		{ 0.000000f, 0.000000f, 0.000000f },
		2},
		"RockLight",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
	},
};
#define _ModularColumn_bottom_materials_
#endif
/************************************************/
/*  This section contains the model's structure */
/************************************************/
#ifndef _ModularColumn_bottom_batches_
// Use this conveinence array to batch render all geometry by like material.
// Each entry corresponds to the same entry in the materials array above.
// The two numbers provided are the IndexCount and the IndexOffset into the indices array.
// If you need more fine grained control(ex: for transformations) use the OBJ_MESH data below.
const unsigned int ModularColumn_bottom_batches[2][2] =
{
	{ 126, 0 },
	{ 150, 126 },
};
#define _ModularColumn_bottom_batches_
#endif
#ifndef __OBJ_MESH__
typedef struct _OBJ_MESH_
{
	const char* name;
	unsigned    indexCount;
	unsigned    indexOffset;
	unsigned    materialIndex;
}OBJ_MESH;
#define __OBJ_MESH__
#endif
#ifndef _ModularColumn_bottom_meshes_
// Mesh Data follows: Meshes are .obj groups sorted & split by material.
// Meshes are provided in sequential order, sorted by material first and name second.
const OBJ_MESH ModularColumn_bottom_meshes[2] =
{
	{
		"default",
		126,
		0,
		0,
	},
	{
		"default",
		150,
		126,
		1,
	},
};
#define _ModularColumn_bottom_meshes_
#endif
