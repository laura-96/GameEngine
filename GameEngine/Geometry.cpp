#include "Geometry.h"

#include "Devil/include/il.h"
#include "Devil/include/ilut.h"

#pragma comment( lib, "Devil/libx86/DevIL.lib" )
#pragma comment( lib, "Devil/libx86/ILU.lib" )
#pragma comment( lib, "Devil/libx86/ILUT.lib" )

Geometry::Geometry() : transform(IdentityMatrix), color(White), wire(false), axis(false), type(GeometryTypes::Point)
{

}

GeometryTypes Geometry::GetType() const
{
	return type;
}

void Geometry::Render() const
{
	glPushMatrix();
	glMultMatrixf(transform.M);

	if (axis == true)
	{
		// Draw Axis Grid
		glLineWidth(1.0f);

		glBegin(GL_LINES);

		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
		glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
		glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
		glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

		glEnd();

		glLineWidth(1.0f);
	}

	glColor3f(color.r, color.g, color.b);

	if (wire)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	InnerRender();

	glPopMatrix();
}

void Geometry::InnerRender() const
{
	glPointSize(5.0f);

	glBegin(GL_POINTS);

	glVertex3f(0.0f, 0.0f, 0.0f);

	glEnd();

	glPointSize(1.0f);
}

void Geometry::SetPos(float x, float y, float z)
{
	transform.translate(x, y, z);
}


void Geometry::SetRotation(float angle, const vec3 &u)
{
	transform.rotate(angle, u);
}

void Geometry::Scale(float x, float y, float z)
{
	transform.scale(x, y, z);
}

// Direct Cube

DirectCube::DirectCube() : Geometry(), size(1.0f, 1.0f, 1.0f)
{
	type = GeometryTypes::Direct_Cube;
	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ ((j & 0x8)) == 0)) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}
}

DirectCube::DirectCube(float sizeX, float sizeY, float sizeZ) : Geometry(), size(sizeX, sizeY, sizeZ)
{
	type = GeometryTypes::Direct_Cube;

	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ ((j & 0x8)) == 0)) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}
}

void DirectCube::InnerRender() const
{
	float sx = size.x * 0.5f;
	float sy = size.y * 0.5f;
	float sz = size.z * 0.5f;

	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	glGenTextures(1, (GLuint*) &imageName);
	glBindTexture(GL_TEXTURE_2D, imageName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_HEIGHT, CHECKERS_WIDTH, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, imageName);

	glBegin(GL_TRIANGLES);

	//FRONT FACE ------------------------------

	//1 triangle vx0, vx1, vx2
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(sx, sy, sz);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-sx, sy, sz);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-sx, -sy, sz);
	
	//2 triangle vx2, vx3, vx0
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-sx, -sy, sz);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(sx, -sy, sz);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(sx, sy, sz);

	//TOP FACE --------------------------------

	//1 triangle vx0, vx5, vx6
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(sx, sy, sz);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(sx, sy, -sz);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-sx, sy, -sz);

	//2 triangle vx6, vx1, vx0
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-sx, sy, -sz);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-sx, sy, sz);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(sx, sy, sz);

	//RIGHT FACE -------------------------------

	//1 triangle vx0, vx3, vx4
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(sx, sy, sz);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(sx, -sy, sz);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(sx, -sy, -sz);

	//2 triangle vx4, vx5, vx0
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(sx, -sy, -sz);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(sx, sy, -sz);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(sx, sy, sz);

	//BACK FACE -------------------------------

	//1 triangle vx7, vx6, vx5
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-sx, sy, -sz);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(sx, sy, -sz);

	//2 triangle vx5, vx4, vx7
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(sx, sy, -sz);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(sx, -sy, -sz);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);

	//BOTTOM FACE ----------------------------

	//1 triangle vx7, vx4, vx3
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-sx, -sy, -sz);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(sx, -sy, -sz);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(sx, -sy, sz);

	//2 triangle vx3, vx2, vx7
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(sx, -sy, sz);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-sx, -sy, sz);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-sx, -sy, -sz);

	//LEFT FACE ------------------------------

	//1 triangle vx7, vx2, vx1
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(-sx, -sy, sz);
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-sx, sy, sz);

	//2 triangle vx1, vx6, vx7
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(-sx, sy, sz);
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-sx, sy, -sz);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);

	glEnd();
	glDisable(GL_TEXTURE_2D);
}

// Vx Cube

VxCube::VxCube() : Geometry(), size(1.0f, 1.0f, 1.0f)
{
	type = GeometryTypes::Vx_Cube;

	float sx = size.x * 0.5f;
	float sy = size.y * 0.5f;
	float sz = size.z * 0.5f;

	glGenBuffers(1, (GLuint*) &(vertex));

	//FRONT FACE ------------------------
	vertices[0] = sx;
	vertices[1] = sy;
	vertices[2] = sz;
	
	vertices[3] = -sx;
	vertices[4] = sy;
	vertices[5] = sz;

	vertices[6] = -sx;
	vertices[7] = -sy;
	vertices[8] = sz;

	vertices[9] = -sx;
	vertices[10] = -sy;
	vertices[11] = sz;

	vertices[12] = sx;
	vertices[13] = -sy;
	vertices[14] = sz;

	vertices[15] = sx;
	vertices[16] = sy;
	vertices[17] = sz;

	//TOP FACE ------------------------
	vertices[18] = sx;
	vertices[19] = sy;
	vertices[20] = sz;

	vertices[21] = sx;
	vertices[22] = sy;
	vertices[23] = -sz;

	vertices[24] = -sx;
	vertices[25] = sy;
	vertices[26] = -sz;

	vertices[27] = -sx;
	vertices[28] = sy;
	vertices[29] = -sz;

	vertices[30] = -sx;
	vertices[31] = sy;
	vertices[32] = sz;

	vertices[33] = sx;
	vertices[34] = sy;
	vertices[35] = sz;
								
	//RIGHT FACE ------------------------
	vertices[36] = sx;
	vertices[37] = sy;
	vertices[38] = sz;

	vertices[39] = sx;
	vertices[40] = -sy;
	vertices[41] = sz;

	vertices[42] = sx;
	vertices[43] = -sy;
	vertices[44] = -sz;

	vertices[45] = sx;
	vertices[46] = -sy;
	vertices[47] = -sz;

	vertices[48] = sx;
	vertices[49] = sy;
	vertices[50] = -sz;

	vertices[51] = sx;
	vertices[52] = sy;
	vertices[53] = sz;
	
	//BACK FACE ------------------------
	vertices[54] = -sx;
	vertices[55] = -sy;
	vertices[56] = -sz;

	vertices[57] = -sx;
	vertices[58] = sy;
	vertices[59] = -sz;

	vertices[60] = sx;
	vertices[61] = sy;
	vertices[62] = -sz;

	vertices[63] = sx;
	vertices[64] = sy;
	vertices[65] = -sz;

	vertices[66] = sx;
	vertices[67] = -sy;
	vertices[68] = -sz;

	vertices[69] = -sx;
	vertices[70] = -sy;
	vertices[71] = -sz;

	//BOTTOM FACE ------------------------
	vertices[72] = -sx;
	vertices[73] = -sy;
	vertices[74] = -sz;

	vertices[75] = sx;
	vertices[76] = -sy;
	vertices[77] = -sz;

	vertices[78] = sx;
	vertices[79] = -sy;
	vertices[80] = sz;

	vertices[81] = sx;
	vertices[82] = -sy;
	vertices[83] = sz;

	vertices[84] = -sx;
	vertices[85] = -sy;
	vertices[86] = sz;

	vertices[87] = -sx;
	vertices[88] = -sy;
	vertices[89] = -sz;
		
	//LEFT FACE ------------------------
	vertices[90] = -sx;
	vertices[91] = -sy;
	vertices[92] = -sz;

	vertices[93] = -sx;
	vertices[94] = -sy;
	vertices[95] = sz;

	vertices[96] = -sx;
	vertices[97] = sy;
	vertices[98] = sz;

	vertices[99] = -sx;
	vertices[100] = sy;
	vertices[101] = sz;

	vertices[102] = -sx;
	vertices[103] = sy;
	vertices[104] = -sz;

	vertices[105] = -sx;
	vertices[106] = -sy;
	vertices[107] = -sz;	
					

	glBindBuffer(GL_ARRAY_BUFFER, vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36 * 3, vertices, GL_STATIC_DRAW);


}

VxCube::VxCube(float sizeX, float sizeY, float sizeZ) : Geometry(), size(sizeX, sizeY, sizeZ)
{
	type = GeometryTypes::Vx_Cube;

	float sx = size.x * 0.5f;
	float sy = size.y * 0.5f;
	float sz = size.z * 0.5f;

	glGenBuffers(1, (GLuint*) &(vertex));

	//FRONT FACE ------------------------
	vertices[0] = sx;
	vertices[1] = sy;
	vertices[2] = sz;

	vertices[3] = -sx;
	vertices[4] = sy;
	vertices[5] = sz;

	vertices[6] = -sx;
	vertices[7] = -sy;
	vertices[8] = sz;

	vertices[9] = -sx;
	vertices[10] = -sy;
	vertices[11] = sz;

	vertices[12] = sx;
	vertices[13] = -sy;
	vertices[14] = sz;

	vertices[15] = sx;
	vertices[16] = sy;
	vertices[17] = sz;

	//TOP FACE ------------------------
	vertices[18] = sx;
	vertices[19] = sy;
	vertices[20] = sz;

	vertices[21] = sx;
	vertices[22] = sy;
	vertices[23] = -sz;

	vertices[24] = -sx;
	vertices[25] = sy;
	vertices[26] = -sz;

	vertices[27] = -sx;
	vertices[28] = sy;
	vertices[29] = -sz;

	vertices[30] = -sx;
	vertices[31] = sy;
	vertices[32] = sz;

	vertices[33] = sx;
	vertices[34] = sy;
	vertices[35] = sz;

	//RIGHT FACE ------------------------
	vertices[36] = sx;
	vertices[37] = sy;
	vertices[38] = sz;

	vertices[39] = sx;
	vertices[40] = -sy;
	vertices[41] = sz;

	vertices[42] = sx;
	vertices[43] = -sy;
	vertices[44] = -sz;

	vertices[45] = sx;
	vertices[46] = -sy;
	vertices[47] = -sz;

	vertices[48] = sx;
	vertices[49] = sy;
	vertices[50] = -sz;

	vertices[51] = sx;
	vertices[52] = sy;
	vertices[53] = sz;

	//BACK FACE ------------------------
	vertices[54] = -sx;
	vertices[55] = -sy;
	vertices[56] = -sz;

	vertices[57] = -sx;
	vertices[58] = sy;
	vertices[59] = -sz;

	vertices[60] = sx;
	vertices[61] = sy;
	vertices[62] = -sz;

	vertices[63] = sx;
	vertices[64] = sy;
	vertices[65] = -sz;

	vertices[66] = sx;
	vertices[67] = -sy;
	vertices[68] = -sz;

	vertices[69] = -sx;
	vertices[70] = -sy;
	vertices[71] = -sz;

	//BOTTOM FACE ------------------------
	vertices[72] = -sx;
	vertices[73] = -sy;
	vertices[74] = -sz;

	vertices[75] = sx;
	vertices[76] = -sy;
	vertices[77] = -sz;

	vertices[78] = sx;
	vertices[79] = -sy;
	vertices[80] = sz;

	vertices[81] = sx;
	vertices[82] = -sy;
	vertices[83] = sz;

	vertices[84] = -sx;
	vertices[85] = -sy;
	vertices[86] = sz;

	vertices[87] = -sx;
	vertices[88] = -sy;
	vertices[89] = -sz;

	//LEFT FACE ------------------------
	vertices[90] = -sx;
	vertices[91] = -sy;
	vertices[92] = -sz;

	vertices[93] = -sx;
	vertices[94] = -sy;
	vertices[95] = sz;

	vertices[96] = -sx;
	vertices[97] = sy;
	vertices[98] = sz;

	vertices[99] = -sx;
	vertices[100] = sy;
	vertices[101] = sz;

	vertices[102] = -sx;
	vertices[103] = sy;
	vertices[104] = -sz;

	vertices[105] = -sx;
	vertices[106] = -sy;
	vertices[107] = -sz;


	glBindBuffer(GL_ARRAY_BUFFER, vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36 * 3, vertices, GL_STATIC_DRAW);
}

void VxCube::InnerRender() const
{
	// activate and specify pointer to vertex array
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glPushMatrix();
	// draw a cube
	glDrawArrays(GL_TRIANGLES, 0, 36 * 3);

	glPopMatrix();

	// deactivate vertex arrays after drawing
	glDisableClientState(GL_VERTEX_ARRAY);

}

// In Cube

InCube::InCube() : Geometry(), size(1.0f, 1.0f, 1.0f)
{
	type = GeometryTypes::In_Cube;

	LoadChecker();
	GenBuffers();
	
}

InCube::InCube(float sizeX, float sizeY, float sizeZ) : Geometry(), size(sizeX, sizeY, sizeZ)
{
	type = GeometryTypes::In_Cube;

	LoadChecker();
	GenBuffers();

	/*	   6 __________	5 			
		   /|		  /|			
		  /	|		 / |			
	   1 /__|_______/0 |
		 |  |		|  |
		 | 7|_______|__| 4
		 |  /		|  /
		 | /		| /	
	   2 |/_________|/3
	*/


}

InCube::InCube(const char* texture_path) : Geometry(), size(1.0f, 1.0f, 1.0f)
{
	type = GeometryTypes::In_Cube;
	
	path = texture_path;
	ilInit();
	ilutInit();

	ilutRenderer(ILUT_OPENGL);
	GenBuffers();

	LoadTexture(path.c_str());
	
}

void InCube::InnerRender() const
{

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glGenTextures(1, (GLuint*) &imageName);
	glBindTexture(GL_TEXTURE_2D, imageName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_HEIGHT, CHECKERS_WIDTH, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, imageName);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertex);

	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, uvs);
	glTexCoordPointer(3, GL_FLOAT, 0, NULL);

	glPushMatrix();

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, NULL);

	glPopMatrix();

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_TEXTURE_2D);
}

void InCube::LoadChecker()
{
	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ ((j & 0x8)) == 0)) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}
}

void InCube::GenBuffers()
{

	float sx = size.x * 0.5f;
	float sy = size.y * 0.5f;
	float sz = size.z * 0.5f;

	glGenBuffers(1, (GLuint*) &(index));
	glGenBuffers(1, (GLuint*) &(vertex));
	glGenBuffers(1, (GLuint*) &(uvs));

	//vx0
	vertices[0] = sx;
	vertices[1] = sy;
	vertices[2] = sz;

	//vx1
	vertices[3] = -sx;
	vertices[4] = sy;
	vertices[5] = sz;

	//vx2
	vertices[6] = -sx;
	vertices[7] = -sy;
	vertices[8] = sz;

	//vx3
	vertices[9] = sx;
	vertices[10] = -sy;
	vertices[11] = sz;

	//vx4
	vertices[12] = sx;
	vertices[13] = -sy;
	vertices[14] = -sz;

	//vx5
	vertices[15] = sx;
	vertices[16] = sy;
	vertices[17] = -sz;

	//vx6
	vertices[18] = -sx;
	vertices[19] = sy;
	vertices[20] = -sz;

	//vx7
	vertices[21] = -sx;
	vertices[22] = -sy;
	vertices[23] = -sz;

	//Should add repeated vertices for fitting UVs

	//vx8
	vertices[24] = sx;
	vertices[25] = sy;
	vertices[26] = sz;

	//vx9
	vertices[27] = sx;
	vertices[28] = sy;
	vertices[29] = -sz;

	//vx10
	vertices[30] = sx;
	vertices[31] = -sy;
	vertices[32] = -sz;

	//vx11
	vertices[33] = sx;
	vertices[34] = -sy;
	vertices[35] = sz;

	//vx12
	vertices[36] = -sx;
	vertices[37] = sy;
	vertices[38] = sz;

	//vx13
	vertices[39] = -sx;
	vertices[40] = sy;
	vertices[41] = -sz;

	//vx14
	vertices[42] = -sx;
	vertices[43] = -sy;
	vertices[44] = -sz;

	//vx15
	vertices[45] = -sx;
	vertices[46] = -sy;
	vertices[47] = sz;

	glBindBuffer(GL_ARRAY_BUFFER, vertex);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 48, vertices, GL_STATIC_DRAW);

	//FRONT FACE ---------------------------------
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 2;
	indices[4] = 3;
	indices[5] = 0;

	//TOP FACE ---------------------------------
	indices[6] = 1;
	indices[7] = 0;
	indices[8] = 6;
	indices[9] = 6;
	indices[10] = 0;
	indices[11] = 5;

	//RIGHT FACE ---------------------------------
	indices[12] = 8;
	indices[13] = 11;
	indices[14] = 10;
	indices[15] = 10;
	indices[16] = 9;
	indices[17] = 8;

	//BACK FACE ---------------------------------
	indices[18] = 6;
	indices[19] = 4;
	indices[20] = 7;
	indices[21] = 6;
	indices[22] = 5;
	indices[23] = 4;

	//BOTTOM FACE ---------------------------------
	indices[24] = 7;
	indices[25] = 3;
	indices[26] = 2;
	indices[27] = 7;
	indices[28] = 4;
	indices[29] = 3;

	//LEFT FACE ---------------------------------
	indices[30] = 12;
	indices[31] = 13;
	indices[32] = 14;
	indices[33] = 12;
	indices[34] = 14;
	indices[35] = 15;

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 36, indices, GL_STATIC_DRAW);


	UVs[0] = 1.0f;
	UVs[1] = 1.0f;
	UVs[2] = 0.0f;

	UVs[3] = 0.0f;
	UVs[4] = 1.0f;
	UVs[5] = 0.0f;

	UVs[6] = 0.0f;
	UVs[7] = 0.0f;
	UVs[8] = 0.0f;

	UVs[9] = 1.0f;
	UVs[10] = 0.0f;
	UVs[11] = 0.0f;

	UVs[12] = 1.0f;
	UVs[13] = 1.0f;
	UVs[14] = 0.0f;

	UVs[15] = 1.0f;
	UVs[16] = 0.0f;
	UVs[17] = 0.0f;

	UVs[18] = 0.0f;
	UVs[19] = 0.0f;
	UVs[20] = 0.0f;

	UVs[21] = 0.0f;
	UVs[22] = 1.0f;
	UVs[23] = 0.0f;

	UVs[24] = 0.0f;
	UVs[25] = 1.0f;
	UVs[26] = 0.0f;

	UVs[27] = 1.0f;
	UVs[28] = 1.0f;
	UVs[29] = 0.0f;

	UVs[30] = 1.0f;
	UVs[31] = 0.0f;
	UVs[32] = 0.0f;

	UVs[33] = 0.0f;
	UVs[34] = 0.0f;
	UVs[35] = 0.0f;

	UVs[36] = 1.0f;
	UVs[37] = 1.0f;
	UVs[38] = 0.0f;

	UVs[39] = 0.0f;
	UVs[40] = 1.0f;
	UVs[41] = 0.0f;

	UVs[42] = 0.0f;
	UVs[43] = 0.0f;
	UVs[44] = 0.0f;

	UVs[45] = 1.0f;
	UVs[46] = 0.0f;
	UVs[47] = 0.0f;

	glBindBuffer(GL_ARRAY_BUFFER, uvs);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 48, UVs, GL_STATIC_DRAW);
}


void InCube::RenderText()
{

	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	texture[0] = ilutGLBindTexImage();

	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glEnable(GL_TEXTURE_2D);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, vertex);

	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, uvs);
	glTexCoordPointer(3, GL_FLOAT, 0, NULL);

	glPushMatrix();

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, NULL);

	glPopMatrix();

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisable(GL_TEXTURE_2D);

	ilDisable(IL_ORIGIN_SET);
}


bool InCube::LoadTexture(const char* tex_file)
{
	ilGenImages(1, &imageId);
	ilBindImage(imageId);
	
	ilEnable(IL_ORIGIN_SET);
	//LOOK UP Not sure about what mode IL_ORIGIN_LOWER_LEFT is
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	if (!ilLoadImage(tex_file))
	{
		ilDeleteImages(1, &imageId);
		return false;
	}

	return true;
}